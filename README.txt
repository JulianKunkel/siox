Requirements
============

The communication code requires the components especified in the CMakeLists.txt 
file, that is:

* Google protocol buffers
* Boost library (threads, system and unit_test_framework)
* YAML-CPP
* PostgreSQL devel packages and libpq

During development the following versions were used:

* Google protocol buffers 2.5.0
* Boost 1.53
* GCC 4.7 (for C++0x support)
* YAML-CPP 0.3.0
* PostgreSQL 9.2

Compilation
===========

To compile create a "build" directory and run cmake followed by make:

mkdir build
cd build
cmake ..
make

To execute the unit tests, do:
make test
