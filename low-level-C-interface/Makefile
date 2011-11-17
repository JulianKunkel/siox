################
# Definitionen #
################

#=======
# Tools
#=======
CC := gcc
RM := -rm -f

#=======
# Flags
#=======
CFLAGS := -ansi -pedantic -Wall
LIBFLAGS := $(CFLAGS) -shared -fPIC

#=========
# Targets
#=========
TARGET := siox
OBJECTS := siox.o
LIBS := libsiox-ll.sa
HDF5PATH := /usr/lib/hdf5
HDF5LIBS := $(HDF5PATH)/libhdf5.a $(HDF5PATH)/libhdf5_hl.a

##########
# Regeln #
##########

#=========
# Endziel
#=========
all: $(OBJECTS) $(LIBS) Makefile
	@echo --------------------------------------------------
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS) $(HDF5LIBS)
	@echo ==================================================
	
#===============
# Debug-Version
#===============
debug: CFLAGS += -ggdb
debug: LIBFLAGS += -ggdb
debug: $(OBJECTS) $(LIBS) Makefile
	@echo --------------------------------------------------
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS) $(HDF5LIBS)
	@echo ==================================================

#=========================
# explizite Zwischenziele
#=========================
libsiox-ll.sa: siox-ll.c siox-ll.h
	@echo --------------------------------------------------
	$(CC) -c $(LIBFLAGS) -o libsiox-ll.sa siox-ll.c
	@echo --------------------------------------------------

#=================
# implizite Ziele
#=================
.c.o:
	@echo --------------------------------------------------
	$(CC) -c $(CFLAGS) -o $@ $*.c
	@echo --------------------------------------------------

###############
# Pseudoziele #
###############

.PHONY: clean
clean:
	@echo ==================================================
	$(RM) $(TARGET) $(OBJECTS) $(LIBS) *~
	@echo ==================================================
	
.PHONY: docs
docs:
	@echo ==================================================
	doxygen Doxyfile
	@echo ==================================================
	
