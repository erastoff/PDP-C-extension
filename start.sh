#!/bin/sh
set -xe

#yum install -y  gcc \
#				make \
#				zlib-devel \
#				protobuf \
#				protobuf-c \
#				protobuf-c-compiler \
#				protobuf-c-devel \
#				gdb

ulimit -c unlimited
#cd /tmp/otus/
protoc-c --c_out=. deviceapps.proto
python3 setup.py build_ext --inplace --debug
python3 setup.py install
python3 -m unittest discover
