#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <zlib.h>
#include "deviceapps.pb-c.h"

#define MAGIC  0xFFFFFFFF
#define DEVICE_APPS_TYPE 1

typedef struct pbheader_s {
    uint32_t magic;
    uint16_t type;
    uint16_t length;
} pbheader_t;
#define PBHEADER_INIT {MAGIC, 0, 0}


// https://github.com/protobuf-c/protobuf-c/wiki/Examples
void example() {
    DeviceApps msg = DEVICE_APPS__INIT;
    DeviceApps__Device device = DEVICE_APPS__DEVICE__INIT;
    void *buf;
    unsigned len;

    char *device_id = "e7e1a50c0ec2747ca56cd9e1558c0d7c";
    char *device_type = "idfa";
    device.has_id = 1;
    device.id.data = (uint8_t*)device_id;
    device.id.len = strlen(device_id);
    device.has_type = 1;
    device.type.data = (uint8_t*)device_type;
    device.type.len = strlen(device_type);
    msg.device = &device;

    msg.has_lat = 1;
    msg.lat = 67.7835424444;
    msg.has_lon = 1;
    msg.lon = -22.8044005471;

    msg.n_apps = 3;
    msg.apps = malloc(sizeof(uint32_t) * msg.n_apps);
    msg.apps[0] = 42;
    msg.apps[1] = 43;
    msg.apps[2] = 44;
    len = device_apps__get_packed_size(&msg);

    buf = malloc(len);
    device_apps__pack(&msg, buf);

    fprintf(stderr,"Writing %d serialized bytes\n",len); // See the length of message
    fwrite(buf, len, 1, stdout); // Write to stdout to allow direct command line piping

    free(msg.apps);
    free(buf);
}

// Read iterator of Python dicts
// Pack them to DeviceApps protobuf and write to file with appropriate header
// Return number of written bytes as Python integer
static PyObject* py_deviceapps_xwrite_pb(PyObject* self, PyObject* args) {
    const char* path;
    PyObject* iterable;
    pbheader_t header = PBHEADER_INIT;

    if (!PyArg_ParseTuple(args, "Os", &iterable, &path))
        return NULL;

    FILE *file = fopen(path, "wb");
    if (!file) {
        PyErr_SetString(PyExc_IOError, "Could not open file for writing");
        return NULL;
    }

    gzFile gzfile = gzdopen(fileno(file), "wb");
    if (!gzfile) {
        fclose(file);
        PyErr_SetString(PyExc_IOError, "Could not open file for gzip writing");
        return NULL;
    }

    PyObject *iterator = PyObject_GetIter(iterable);
    if (!iterator) {
        gzclose(gzfile);
        fclose(file);
        PyErr_SetString(PyExc_TypeError, "Argument is not iterable");
        return NULL;
    }

    unsigned int total_bytes_written = 0;

    // Iterate over Python dictionaries
    PyObject *item;
    while ((item = PyIter_Next(iterator)) != NULL) {
        DeviceApps msg = DEVICE_APPS__INIT;
        DeviceApps__Device device = DEVICE_APPS__DEVICE__INIT;

        // Extract data from Python dictionary
        // Example: PyObject_GetAttrString(item, "key");

        // Pack data into DeviceApps protobuf
        // Example: msg.attribute = data;

        // Get serialized message length
        unsigned len = device_apps__get_packed_size(&msg);

        // Create header
        header.type = DEVICE_APPS_TYPE;
        header.length = len;

        // Write header to file
        fwrite(&header, sizeof(pbheader_t), 1, gzfile);

        // Allocate buffer for serialized message
        void *buf = malloc(len);
        if (!buf) {
            gzclose(gzfile);
            fclose(file);
            Py_DECREF(item);
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory");
            return NULL;
        }

        // Pack message into buffer
        device_apps__pack(&msg, buf);

        // Write serialized message to file
        if (gzwrite(gzfile, buf, len) != len) {
            free(buf);
            gzclose(gzfile);
            fclose(file);
            Py_DECREF(item);
            PyErr_SetString(PyExc_IOError, "Failed to write to file");
            return NULL;
        }

        // Update total bytes written
        total_bytes_written += sizeof(pbheader_t) + len;

        // Clean up
        free(buf);
        Py_DECREF(item);
    }

    // Clean up
    Py_DECREF(iterator);
    gzclose(gzfile);
    fclose(file);

    // Return total bytes written as Python integer
    return PyLong_FromUnsignedLong(total_bytes_written);
}

// Unpack only messages with type == DEVICE_APPS_TYPE
// Return iterator of Python dicts
static PyObject* py_deviceapps_xread_pb(PyObject* self, PyObject* args) {
    const char* path;

    if (!PyArg_ParseTuple(args, "s", &path))
        return NULL;

    // Open file for reading
    FILE *file = fopen(path, "rb");
    if (!file) {
        PyErr_SetString(PyExc_IOError, "Could not open file for reading");
        return NULL;
    }

    // Open gzipped file
    gzFile gzfile = gzdopen(fileno(file), "rb");
    if (!gzfile) {
        fclose(file);
        PyErr_SetString(PyExc_IOError, "Could not open file for gzip reading");
        return NULL;
    }

    // Define buffer and header
    pbheader_t header;
    void *buf;

    while (gzread(gzfile, &header, sizeof(pbheader_t)) > 0) {
        if (header.magic != MAGIC) {
            gzclose(gzfile);
            fclose(file);
            PyErr_SetString(PyExc_IOError, "Invalid header magic number");
            return NULL;
        }
        if (header.type != DEVICE_APPS_TYPE) {
            // Skip message if type is not DEVICE_APPS_TYPE
            gzseek(gzfile, header.length, SEEK_CUR);
            continue;
        }

        // Allocate buffer for serialized message
        buf = malloc(header.length);
        if (!buf) {
            gzclose(gzfile);
            fclose(file);
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory");
            return NULL;
        }

        // Read serialized message from file
        if (gzread(gzfile, buf, header.length) != header.length) {
            free(buf);
            gzclose(gzfile);
            fclose(file);
            PyErr_SetString(PyExc_IOError, "Failed to read from file");
            return NULL;
        }

        // Unpack serialized message
        DeviceApps *msg = device_apps__unpack(NULL, header.length, buf);
        if (!msg) {
            free(buf);
            gzclose(gzfile);
            fclose(file);
            PyErr_SetString(PyExc_IOError, "Failed to unpack message");
            return NULL;
        }

        // Create Python dictionary from message
        // Example: PyObject *dict = PyDict_New();

        // Add message attributes to dictionary
        // Example: PyDict_SetItemString(dict, "key", Py_BuildValue("value"));

        // Return dictionary as Python object
        // Example: return dict;

        // Clean up
        device_apps__free_unpacked(msg, NULL);
        free(buf);
    }

    // Clean up
    gzclose(gzfile);
    fclose(file);

    Py_RETURN_NONE;
}


static PyMethodDef PBMethods[] = {
     {"deviceapps_xwrite_pb", py_deviceapps_xwrite_pb, METH_VARARGS, "Write serialized protobuf to file fro iterator"},
     {"deviceapps_xread_pb", py_deviceapps_xread_pb, METH_VARARGS, "Deserialize protobuf from file, return iterator"},
     {NULL, NULL, 0, NULL}
};

//PyMODINIT_FUNC initpb(void) {
//     (void) Py_InitModule("pb", PBMethods);
//}
static struct PyModuleDef pbmodule = {
    PyModuleDef_HEAD_INIT,
    "pb",   /* name of module */
    NULL,    /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    PBMethods
};

PyMODINIT_FUNC PyInit_pb(void) {
     return PyModule_Create(&pbmodule);
}