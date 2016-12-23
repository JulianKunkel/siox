FROM ubuntu:17.04

WORKDIR /data
RUN apt-get update
RUN apt-get install -y libopenmpi-dev openmpi-bin mpich git cmake pkg-config gcc-6 nano

RUN useradd -d /data -u 1000 test

# GLIB and Boost
RUN apt-get install -y libboost-dev libboost-program-options-dev libboost-serialization-dev libboost-regex-dev libboost-system-dev libboost-random-dev libboost-thread-dev libglib2.0-dev libpqxx-dev postgresql-common

RUN apt-get install -y python
