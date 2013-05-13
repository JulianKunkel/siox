CC = g++
CCFLAGS = -Wall -std=c++11
LDFLAGS = -L/opt/local/lib -lboost_system-mt -lboost_thread-mt
# final binary name 
binary = test_mpl_async

# object and source files
#########################

# get all cpps from all dirs and generate corresponding out files
SRC = $(shell find . -type f -iname "*.cpp")
OBJ1 = $(shell find . -type f -iname "*.cpp" | sed "s\#.*/\#\#" )
OBJ = $(OBJ1:%.cpp=%.o)


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
