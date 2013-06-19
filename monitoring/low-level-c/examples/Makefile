###############
# Definitions #
###############

#=======
# Tools
#=======
# All inherited from parent make

#=====================
# Flags and Libraries
#=====================
HDF5FLAGS := -I$(HDF5DIR)/include -L$(HDF5DIR)/lib
HDF5LIBS := -lhdf5 -lhdf5_hl
MUFSFLAGS := -I$(EXAMPLEDIR) -Wl,-rpath=$(EXAMPLEDIR) -L$(EXAMPLEDIR)
MUFSLIBS := -lmufs
# All others inherited from parent make

#=========
# Targets
#=========
TARGETS := hdf5-example mockupfs-example
MUFSLIB := libmufs.so
OWNLIBS := $(MUFSLIB)


#########
# Rules #
#########

#==============
# Main Targets
#==============

.PHONY: examples
examples: $(TARGETS) Makefile
#	@echo --------------------------------------------------

hdf5-example: hdf5-example.c Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LDFLAGS) $(LLFLAGS) $(HDF5FLAGS) $(GLIBFLAGS) -o $@ $< $(HDF5LIBS) $(LLLIBS) $(GLIBLIBS)
	@echo done!

mockupfs-example: mockupfs-example.c $(MUFSLIB) Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LDFLAGS) $(LLFLAGS) $(MUFSFLAGS) -o $@ $< $(MUFSLIBS) $(LLLIBS)
	@echo done!

#===============
# Debug Version
#===============

.PHONY: debug
debug: CFLAGS += -O0 -ggdb -DDEBUG
debug: LIBFLAGS += -O0 -ggdb -DDEBUG
debug: LDFLAGS += -ggdb
debug: all Makefile

#======================
# Intermediate Targets
#======================

libmufs.so: mufs.c mufs.h Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LIBFLAGS) $(LLFLAGS) -o $@ $< $(LLLIBS)
	@echo done!

#==================
# Implicit Targets
#==================

%.o : %.c Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo done!

% : %.c

##################
# Pseudo Targets #
##################

.PHONY: clean
clean:
	@echo --------------------------------------------------
	@echo -n Cleaning Examples...
	@$(RM) $(TARGETS) $(OWNLIBS) *.o *.log *~ *.mufs *.h5
	@echo done!

.PHONY: test
test:
	@echo --------------------------------------------------
	@echo -n Running tests on Examples...
	@./mockupfs-example >mockupfs-example.log 2>mockupfs-example.err
	@./hdf5-example >hdf5-example.log 2>hdf5-example.err
	@echo passed!
