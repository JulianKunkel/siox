Requirements
============

The communication code requires the components especified in the CMakeLists.txt 
file, that is:

* Google protocol buffers
* Boost library (threads and system)

During development the following versions were used:

* Google protocol buffers 2.5.0
* Boost 1.53

Compilation
===========

To compile create a "build" directory and run the following commands inside it:

cmake ..
make

To execute the unit tests, do:
make test
