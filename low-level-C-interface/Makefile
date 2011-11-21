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
CFLAGS := -std=c99 -pedantic -Wall -I/usr/local/hdf5/include
LIBFLAGS := $(CFLAGS) -shared -fPIC
LDFLAGS := $(CFLAGS) -L/usr/local/hdf5/lib

#=========
# Targets
#=========
TARGET := siox
OBJECTS := siox.o
OWNLIBS := libsiox-ll.so
LIBS := $(OWNLIBS) -lhdf5 -lhdf5_hl


##########
# Regeln #
##########

#=========
# Endziel
#=========
all: $(OBJECTS) $(OWNLIBS) Makefile
	@echo --------------------------------------------------
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)
	@echo ==================================================
	
#===============
# Debug-Version
#===============
debug: CFLAGS += -ggdb
debug: LIBFLAGS += -ggdb
debug: $(OBJECTS) $(OWNLIBS) Makefile
	@echo --------------------------------------------------
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)
	@echo ==================================================

#=========================
# explizite Zwischenziele
#=========================
libsiox-ll.so: siox-ll.c siox-ll.h
	@echo --------------------------------------------------
	$(CC) -c $(LIBFLAGS) -o $@ siox-ll.c
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
	$(RM) $(TARGET) $(OBJECTS) $(OWNLIBS) *~
	@echo ==================================================
	
.PHONY: docs
docs:
	@echo ==================================================
	doxygen Doxyfile
	@echo ==================================================
	
