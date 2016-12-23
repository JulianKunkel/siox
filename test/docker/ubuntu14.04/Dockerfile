FROM ubuntu:14.04

WORKDIR /data
RUN apt-get update
RUN apt-get install -y libopenmpi-dev openmpi-bin mpich git cmake pkg-config gcc-4.7 nano wget
RUN wget https://cmake.org/files/v3.0/cmake-3.0.0-1-src.tar.bz2
RUN tar -xf cmake-3.0.0-1-src.tar.bz2
RUN bash -c "./cmake-3.0.0-1.sh all ; cd cmake-3.0.0/.build && make install"

RUN useradd -d /data -u 1000 test

# GLIB and Boost
RUN apt-get install -y libboost-dev libboost-program-options-dev libboost-serialization-dev libboost-regex-dev libboost-system-dev libboost-random-dev libboost-thread-dev libglib2.0-dev libpqxx-dev postgresql-common
