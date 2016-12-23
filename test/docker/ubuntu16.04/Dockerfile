FROM ubuntu:16.04

WORKDIR /data
RUN apt-get update
RUN apt-get install -y libopenmpi-dev openmpi-bin mpich git cmake pkg-config gcc-5 gcc-4.8 nano wget

# POSTGRES TESTS
RUN apt-get install -y postgresql libpq-dev
# Sleep is neccesary as the DB starts in the background
RUN /etc/init.d/postgresql start; sleep 5; /etc/init.d/postgresql status ; su - postgres -c "createuser root -d" ; su - postgres -c "echo CREATE DATABASE ROOT OWNER ROOT | psql"

RUN useradd -d /data -u 1000 test

# GLIB and Boost
RUN apt-get install -y libboost-dev libboost-program-options-dev libboost-serialization-dev libboost-regex-dev libboost-system-dev libboost-random-dev libboost-thread-dev libglib2.0-dev libpqxx-dev postgresql-common
