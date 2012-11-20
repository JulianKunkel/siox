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
TARGETS := hdf5-example mockupfs-example
MUFSLIBS := libmufs.so
HDF5FLAGS := -I$(HDF5DIR)/include -L$(HDF5DIR)/lib
HDF5LIBS := -lhdf5 -lhdf5_hl
OWNLIBS := $(MUFSLIBS)


##########
# Regeln #
##########

#==========
# Endziele
#==========

.PHONY: examples
examples: $(TARGETS) Makefile
#	@echo --------------------------------------------------

hdf5-example: hdf5-example.c Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LDFLAGS) $(LLFLAGS) $(HDF5FLAGS) $(GLIBFLAGS) -o $@ $< $(HDF5LIBS) $(LLLIBS) $(GLIBLIBS)
	@echo done!

mockupfs-example: mockupfs-example.c $(MUFSLIBS) Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LDFLAGS) $(LLFLAGS) -o $@ $< $(MUFSLIBS) $(LLLIBS)
	@echo done!

#===============
# Debug-Version
#===============

.PHONY: debug
debug: CFLAGS += -O0 -ggdb -DDEBUG
debug: LIBFLAGS += -O0 -ggdb -DDEBUG
debug: LDFLAGS += -ggdb
debug: all Makefile

libmufs.so: mufs.c mufs.h Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LIBFLAGS) $(LLFLAGS) -o $@ $< $(LLLIBS)
	@echo done!

#=================
# implizite Ziele
#=================
%.o : %.c Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo done!

% : %.c

###############
# Pseudoziele #
###############

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
