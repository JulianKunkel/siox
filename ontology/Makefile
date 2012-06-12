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
TARGETS := testontology
ONTLIBS := libsioxont.so
OWNLIBS := $(ONTLIBS)
# LIBS := $(OWNLIBS)


##########
# Regeln #
##########

#==========
# Endziele
#==========
.PHONY: all
all: $(TARGETS) Makefile
#	@echo --------------------------------------------------

testontology: testontology.o $(ONTLIBS) Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LDFLAGS) $(ONTFLAGS) -o $@ $< $(ONTLIBS)
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
libsioxont.so: ontology.c ontology.h datatype.c datatype.h metric.c metric.h Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LIBFLAGS) -o $@ $< datatype.c metric.c
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
