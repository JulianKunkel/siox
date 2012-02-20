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
OBJECTS := testontology.o
OWNLIBS := libsioxont.so
LIBS := $(OWNLIBS)


##########
# Regeln #
##########

#==========
# Endziele
#==========
.PHONY: all
all: $(TARGETS) Makefile
#	@echo --------------------------------------------------

testontology: testontology.o $(LIBS) Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)
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
libsioxont.so: ontology.c ontology.h Makefile
#	@echo --------------------------------------------------
	@echo -n Making $@...
	$(CC) -c $(LIBFLAGS) -o $@ ontology.c
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

.PHONY: test
test:
	@echo --------------------------------------------------
	@echo -n Running tests on Ontology...
	@./testontology >testontology.log
	@echo passed!

.PHONY: clean
clean:
	@echo --------------------------------------------------
	@echo -n Cleaning Ontology...
	@$(RM) $(TARGETS) $(OBJECTS) $(OWNLIBS) *.log *~
	@echo done!
