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
# All inherited from parent make

#=========
# Targets
#=========
TARGETS := libsiox-ll.so


#########
# Rules #
#########

#==============
# Main Targets
#==============

.PHONY: all
all: $(TARGETS) Makefile
#	@echo --------------------------------------------------

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

libsiox-ll.so: siox-ll.c siox-ll.h Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LIBFLAGS) $(ONTFLAGS) $(GLIBFLAGS) -o $@ $< $(ONTLIBS) $(GLIBLIBS) $(PAPILIBS)
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
	@echo -n Cleaning Low-Level-C-Interface...
	@$(RM) $(TARGETS) *.o *.log *~
	@echo done!

.PHONY: test
test:
	@echo --------------------------------------------------
	@echo -n Running tests on Low-Level-C-Interface...
	@echo -n "(no tests as yet)..."
	#@./siox-mockupfs-example >siox-mockupfs-example.log 2>siox-mockupfs-example.err
	#@./siox-hdf5-example >siox-hdf5-example.log 2>siox-hdf5-example.err
	@echo passed!
