################
# Definitionen #
################

#=======
# Tools
#=======
CC := gcc
RM := -rm -f
MAKE := make

#=======
# Flags
#=======
CFLAGS := -std=c99 -pedantic -Wall
LIBFLAGS := $(CFLAGS) -shared -fPIC
HDF5FLAGS := -I/usr/local/hdf5/include -L/usr/local/hdf5/lib
LDFLAGS := $(CFLAGS)

#=========
# Targets
#=========
TARGETS := siox-hdf5-example siox-mockupfs-example
OBJECTS := siox-hdf5-example.o siox-mockupfs-example.o
SIOXLIBS := libsiox-ll.so
MUFSLIBS := libmufs.so
HDF5LIBS := -lhdf5 -lhdf5_hl
OWNLIBS := $(SIOXLIBS) $(MUFSLIBS)
#LIBS := $(OWNLIBS) 


##########
# Regeln #
##########

#=========
# Endziel
#=========
all: $(TARGETS)
	@echo ==================================================
	
siox-hdf5-example: siox-hdf5-example.c $(SIOXLIBS) Makefile
	@echo Making $@...
	$(CC) $(LDFLAGS) $(HDF5FLAGS) -o $@ $< $(SIOXLIBS) $(HDF5LIBS)
	@echo ...done!
	@echo ==================================================
	
siox-mockupfs-example: siox-mockupfs-example.c $(SIOXLIBS) $(MUFSLIBS) Makefile
	@echo Making $@...
	$(CC) $(LDFLAGS) -o $@ $< $(SIOXLIBS) $(MUFSLIBS)
	@echo ...done!
	@echo ==================================================
	
#===============
# Debug-Version
#===============
debug: CFLAGS += -ggdb
debug: LIBFLAGS += -ggdb
debug: LDFLAGS += -ggdb
debug: all
#	$(OBJECTS) $(OWNLIBS) Makefile
#	@echo --------------------------------------------------
#	@echo Making $@...
#	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)
#	@echo ...done!
#	@echo ==================================================

#=========================
# explizite Zwischenziele
#=========================
libsiox-ll.so: siox-ll.c siox-ll.h
	@echo Making $@...
	$(CC) -c $(LIBFLAGS) -o $@ siox-ll.c
	@echo ...done!
	@echo --------------------------------------------------

libmufs.so: mufs.c mufs.h
	@echo Making $@...
	$(CC) -c $(LIBFLAGS) -o $@ mufs.c
	@echo ...done!
	@echo --------------------------------------------------

#=================
# implizite Ziele
#=================
.c.o:
	@echo --------------------------------------------------
	@echo Making $@...
	$(CC) -c $(CFLAGS) -o $@ $*.c
	@echo ...done!
	@echo --------------------------------------------------


###############
# Pseudoziele #
###############

.PHONY: clean
clean:
	@echo ==================================================
	@echo Cleaning directory...
	@$(RM) $(TARGETS) $(OBJECTS) $(OWNLIBS) *.log *~
	@echo ...done!
	@echo ==================================================
	
.PHONY: docs
docs:
	@echo ==================================================
	@echo Making documentation...
	@doxygen Doxyfile
	@$(MAKE) --silent --directory=latex >makePDF.log
	@echo ...done!
	@echo ==================================================
	
