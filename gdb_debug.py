import pb

data = [
    {
        "device": {"type": "idfa", "id": "e7e1a50c0ec2747ca56cd9e1558c0d7c"},
        "lat": 67.7835424444,
        "lon": -22.8044005471,
        "apps": [1, 2, 3, 4],
    },
    {
        "device": {"type": "gaid", "id": "e7e1a50c0ec2747ca56cd9e1558c0d7d"},
        "lat": 42,
        "lon": -42,
        "apps": [1, 2],
    },
    {
        "device": {"type": "gaid", "id": "e7e1a50c0ec2747ca56cd9e1558c0d7d"},
        "lat": 42,
        "lon": -42,
        "apps": [],
    },
    {"device": {"type": "gaid", "id": "e7e1a50c0ec2747ca56cd9e1558c0d7d"}, "apps": [1]},
]
path = "test_gdb_output.pb.gz"

bytes_written = pb.deviceapps_xwrite_pb(data, path)

print(f"Bytes written: {bytes_written}")
