import os
import struct
import unittest
from _ctypes import sizeof

import pb
MAGIC = 0xFFFFFFFF
DEVICE_APPS_TYPE = 1
TEST_FILE = "test.pb.gz"


class TestPB(unittest.TestCase):
    deviceapps = [
        {"device": {"type": "idfa", "id": "e7e1a50c0ec2747ca56cd9e1558c0d7c"},
         "lat": 67.7835424444, "lon": -22.8044005471, "apps": [1, 2, 3, 4]},
        {"device": {"type": "gaid", "id": "e7e1a50c0ec2747ca56cd9e1558c0d7d"}, "lat": 42, "lon": -42, "apps": [1, 2]},
        {"device": {"type": "gaid", "id": "e7e1a50c0ec2747ca56cd9e1558c0d7d"}, "lat": 42, "lon": -42, "apps": []},
        {"device": {"type": "gaid", "id": "e7e1a50c0ec2747ca56cd9e1558c0d7d"}, "apps": [1]},
    ]

    # def tearDown(self):
    #     os.remove(TEST_FILE)

    def test_write(self):
        print("TEST WRITE")
        bytes_written = pb.deviceapps_xwrite_pb(self.deviceapps, TEST_FILE)
        self.assertTrue(bytes_written > 0)

        # with open(TEST_FILE, "rb") as f:
        #     header = f.read(sizeof(pbheader_t))
        #     magic_value = struct.unpack('I', header[0:4])[0]  # Первые 4 байта - значение magic
        # self.assertEqual(magic_value, MAGIC)
        # check magic, type, etc.
        # Чтение и вывод данных из файла
        with open(TEST_FILE, "rb") as f:
            while True:
                # Чтение заголовка
                # header = f.read(struct.calcsize("IHH"))
                header = f.read(8)
                if len(header) == 0:
                    break

                # Распаковка заголовка
                magic, type_dev, length = struct.unpack("IHH", header)
                print(f"Magic: {magic}, Type: {type_dev}, Length: {length}")

                # Чтение тела сообщения
                body = f.read(length)
                print(f"Body: {body.hex()}")

                # Проверка на конец файла
                if len(body) < length:
                    break

    # @unittest.skip("Optional problem")
    # def test_read(self):
    #     print("TEST READ")
    #     pb.deviceapps_xwrite_pb(self.deviceapps, TEST_FILE)
    #     for i, d in enumerate(pb.deviceapps_xread_pb(TEST_FILE)):
    #         self.assertEqual(d, self.deviceapps[i])
    #
    #     try:
    #         iterator = pb.deviceapps_xread_pb(TEST_FILE)
    #         for item in iterator:
    #             print(item)
    #             print('TEST')
    #     except Exception as e:
    #         print(f"Error reading from file: {e}")


    # def test_deviceapps_xwrite_pb(self):
    #     data = [{"id": 1, "name": "app1"}, {"id": 2, "name": "app2"}]
    #     written_bytes = pb.deviceapps_xwrite_pb(data, 'test_output.pb.gz')
    #     self.assertGreater(written_bytes, 0)
    #
    # def test_deviceapps_xread_pb(self):
    #     data = [{"id": 1, "name": "app1"}, {"id": 2, "name": "app2"}]
    #     pb.deviceapps_xwrite_pb(data, 'test_output.pb.gz')
    #     result = pb.deviceapps_xread_pb('test_output.pb.gz')
    #     self.assertIsInstance(result, list)
    #     self.assertEqual(len(result), 2)