################
# Definitionen #
################

#=======
# Tools
#=======
# All inherited from parent make

#=======
# Flags
#=======
# All inherited from parent make

#=========
# Targets
#=========
TARGETS := siox-hdf5-example siox-mockupfs-example
OBJECTS := siox-hdf5-example.o siox-mockupfs-example.o
SIOXLIBS := libsiox-ll.so
MUFSLIBS := libmufs.so
HDF5LIBS := -lhdf5 -lhdf5_hl
ONTLIBS := $(ONTDIR)/libsioxont.so
OWNLIBS := $(SIOXLIBS) $(MUFSLIBS)
#LIBS := $(OWNLIBS) $(ONTLIBS)


##########
# Regeln #
##########

#==========
# Endziele
#==========

.PHONY: all
all: $(TARGETS) Makefile
#	@echo --------------------------------------------------

siox-hdf5-example: siox-hdf5-example.c $(SIOXLIBS) $(ONTLIBS) Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LDFLAGS) $(HDF5FLAGS) -o $@ $< $(SIOXLIBS) $(ONTLIBS) $(HDF5LIBS)
	@echo done!

siox-mockupfs-example: siox-mockupfs-example.c $(SIOXLIBS) $(ONTLIBS) $(MUFSLIBS) Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LDFLAGS) -o $@ $< $(SIOXLIBS) $(ONTLIBS) $(MUFSLIBS)
	@echo done!

#===============
# Debug-Version
#===============

.PHONY: debug
debug: CFLAGS += -O0 -ggdb -DDEBUG
debug: LIBFLAGS += -O0 -ggdb -DDEBUG
debug: LDFLAGS += -ggdb
debug: all

#=========================
# explizite Zwischenziele
#=========================
libsiox-ll.so: siox-ll.c siox-ll.h Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) -c $(LIBFLAGS) -o $@ siox-ll.c
	@echo done!

libmufs.so: mufs.c mufs.h Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) -c $(LIBFLAGS) -o $@ mufs.c
	@echo done!

#=================
# implizite Ziele
#=================
%.o : %.c Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) -c $(CFLAGS) $< -o $@
	@echo done!

% : %.c

###############
# Pseudoziele #
###############

.PHONY: clean
clean:
	@echo --------------------------------------------------
	@echo -n Cleaning Low-Level-C-Interface...
	@$(RM) $(TARGETS) $(OBJECTS) $(OWNLIBS) *.log *~ *.mufs *.h5
	@echo done!

.PHONY: test
test:
	@echo --------------------------------------------------
	@echo -n Running tests on Low-Level-C-Interface...
	@./siox-mockupfs-example >siox-mockupfs-example.log
	@./siox-hdf5-example >siox-hdf5-example.log
	@echo ...passed!
