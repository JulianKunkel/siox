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
SIOXLIBS := $(LLDIR)/libsiox-ll.so
ONTLIBS := $(ONTDIR)/libsioxont.so
MUFSLIBS := libmufs.so
HDF5LIBS := -lhdf5 -lhdf5_hl
OWNLIBS := $(SIOXLIBS) $(MUFSLIBS)
#LIBS := $(OWNLIBS) $(ONTLIBS)


##########
# Regeln #
##########

#==========
# Endziele
#==========

.PHONY: examples
examples: $(TARGETS) Makefile
#	@echo --------------------------------------------------

hdf5-example: hdf5-example.c $(SIOXLIBS) Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LDFLAGS) $(LLFLAGS) $(HDF5FLAGS) -o $@ $< $(HDF5LIBS) $(SIOXLIBS)
	@echo done!

mockupfs-example: mockupfs-example.c $(SIOXLIBS) $(MUFSLIBS) Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LDFLAGS) $(LLFLAGS) -o $@ $< $(MUFSLIBS) $(SIOXLIBS)
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
# libsiox-ll.so: siox-ll.c siox-ll.h Makefile
# #	@echo --------------------------------------------------
# 	@echo -n Making $@...
# 	$(CC) $(LIBFLAGS) $(ONTFLAGS) -o $@ $< $(ONTLIBS)
# 	@echo done!

libmufs.so: mufs.c mufs.h Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LIBFLAGS) $(LLFLAGS) -o $@ $< $(SIOXLIBS)
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
	@echo -n Cleaning Low-Level-C-Interface...
	@$(RM) $(TARGETS) $(OWNLIBS) *.o *.log *~ *.mufs *.h5
	@echo done!

.PHONY: test
test:
	@echo --------------------------------------------------
	@echo -n Running tests on Low-Level-C-Interface...
	@./mockupfs-example >mockupfs-example.log 2>mockupfs-example.err
	@./hdf5-example >hdf5-example.log 2>hdf5-example.err
	@echo passed!
