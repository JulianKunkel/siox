#!/bin/bash
g++ systeminfo_containers.cpp systeminfo_persistency_interface.cpp test/systeminfo_test.cpp -lpqxx -lpq
./a.out
