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
#HDF5FLAGS := -I/usr/local/hdf5/include -L/usr/local/hdf5/lib
LDFLAGS := $(CFLAGS)

#=========
# Targets
#=========
TARGETS := testontology
OBJECTS := testontology.o
ONTOLOGYLIBS := libsioxont.so
#SIOXLIBS := libsiox-ll.so
#MUFSLIBS := libmufs.so
#HDF5LIBS := -lhdf5 -lhdf5_hl
OWNLIBS := $(ONTOLOGYLIBS)
DOXYDIRS := html latex
#LIBS := $(OWNLIBS) 


##########
# Regeln #
##########

#=========
# Endziel
#=========
all: $(TARGETS)
	@echo ==================================================
	
testontology: testontology.o $(ONTOLOGYLIBS) Makefile
	@echo Making $@...
	$(CC) $(LDFLAGS) -o $@ $< $(ONTOLOGYLIBS)
	@echo ...done!
	@echo ==================================================
	
#===============
# Debug-Version
#===============
debug: CFLAGS += -O0 -ggdb -DDEBUG
debug: LIBFLAGS += -O0 -ggdb -DDEBUG
debug: LDFLAGS += -ggdb
debug: all

#=========================
# explizite Zwischenziele
#=========================
libsioxont.so: ontology.c ontology.h
	@echo Making $@...
	$(CC) -c $(LIBFLAGS) -o $@ ontology.c
	@echo ...done!
	@echo --------------------------------------------------

#=================
# implizite Ziele
#=================
%.o : %.c
	@echo --------------------------------------------------
	@echo Making $@...
	@echo CPPFLAGS: $(CPPFLAGS)
	$(CC) -c $(CFLAGS) $< -o $@
	@echo ...done!
	@echo --------------------------------------------------

% : %.c

###############
# Pseudoziele #
###############

.PHONY: clean
clean:
	@echo ==================================================
	@echo Cleaning directory...
	@$(RM) $(TARGETS) $(OBJECTS) $(OWNLIBS) *.log *~
	@$(RM) -r $(DOXYDIRS)
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
	
