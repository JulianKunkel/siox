all: test

test: ../ActivityBuilder.cpp ../../helpers/siox_time.c activity_builder-test.cpp ../../../include/monitoring/datatypes/Activity.hpp
	g++ -I ../../../include/ -I /opt/boost/1.53/include/ -std=c++11 $^ -lrt -o $@
