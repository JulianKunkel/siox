CC = g++
CCFLAGS = -Wall -std=c++11 -g -D_GLIBCXX_USE_NANOSLEEP
LDFLAGS = -pthread 
binary = test_01_basic-functionality

# object and source files
#########################

# get all cpps from all dirs and generate corresponding out files
#SRC = $(shell find . -type f -iname "*.cpp")
#OBJ1 = $(shell find . -type f -iname "*.cpp" | sed "s\#.*/\#\#" )
#OBJ = $(OBJ1:%.cpp=%.o)


SRC = ../ActivityMultiplexerListener_Impl1.cpp ../ActivityMultiplexer_Impl1.cpp Activity.cpp main_test_01_basic-functionality.cpp
OBJ = ActivityMultiplexerListener_Impl1.o ActivityMultiplexer_Impl1.o Activity.o main_test_01_basic-functionality.o

# likely you are already done 
#############################

# build things
$(binary): $(OBJ)
	$(CC) $(CCFLAGS) -o $(binary) $(OBJ) $(LDFLAGS)

$(OBJ): $(SRC)
	$(CC) $(CCFLAGS) -c $(SRC) 

clean:
	rm -rf $(OBJ) $(binary)

test:
	@echo $(SRC)
	@echo $(OBJ)
