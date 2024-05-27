FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    gcc \
    make \
    zlib1g-dev \
    protobuf-compiler \
    protobuf-c-compiler \
    libprotobuf-c-dev \
    python3 \
    python3-dev \
    python3-setuptools \
    python3-pip \
    gdb \
    && apt-get clean

COPY . /tmp/otus/

WORKDIR /tmp/otus/

CMD ["./start.sh"]
CMD ["bash"]
