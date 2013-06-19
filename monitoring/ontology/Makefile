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
TARGETS := testontology
ONTLIB := libsioxont.so
OWNLIBS := $(ONTLIB)


#########
# Rules #
#########

#==============
# Main Targets
#==============

.PHONY: all
all: $(TARGETS) Makefile
#	@echo --------------------------------------------------

testontology: testontology.o $(ONTLIB) Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LDFLAGS) $(ONTFLAGS) -o $@ $< $(ONTLIBS)
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

libsioxont.so: ontology.c ontology.h datatype.c datatype.h metric.c metric.h Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LIBFLAGS) -o $@ $< datatype.c metric.c
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

.PHONY: test
test:
	@echo --------------------------------------------------
	@echo -n Running tests on Ontology...
	@./testontology >testontology.log 2>testontology.err
	@echo passed!

.PHONY: clean
clean:
	@echo --------------------------------------------------
	@echo -n Cleaning Ontology...
	@$(RM) $(TARGETS) $(OWNLIBS) *.o *.log *.err *~
	@echo done!
