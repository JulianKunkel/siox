###############
# Definitions #
###############

# Prepare to hand all variables down to sub-makes
export

#=======
# Tools
#=======
CC := gcc
RM := -rm -f
# MAKE is automagically set to the command used to call us

#=============
# Directories
#=============
LLDIR := $(CURDIR)/low-level-C-interface
ONTDIR := $(CURDIR)/ontology
EXAMPLEDIR := $(CURDIR)/examples
HDF5DIR :=
#/usr/include

#=======
# Flags
#=======
CFLAGS := -std=c1x -pedantic -Wall
LIBFLAGS := $(CFLAGS) -shared -fPIC
LDFLAGS := $(CFLAGS)
GLIBFLAGS := `pkg-config --cflags glib-2.0` `pkg-config --libs glib-2.0`
HDF5FLAGS := -I$(HDF5DIR)/include -L$(HDF5DIR)/lib
#ONTFLAGS := -iquote$(ONTDIR) -Wl,-rpath=$(ONTDIR)
ONTFLAGS := $(GLIBFLAGS) -I$(ONTDIR) -Wl,-rpath=$(ONTDIR)
LLFLAGS := $(GLIBFLAGS) -I$(LLDIR) -Wl,-rpath=$(LLDIR)

#=========
# Targets
#=========
TARGETS := ont ll
# Don't hand these variables down to sub-makes
unexport TARGETS

#=============
# for Doxygen
#=============
DOXYDIRS := html latex
LOGS := *.log
REFMAN := SIOX-Reference.pdf
# Don't hand these variables down to sub-makes
unexport DOXYDIRS LOGS REFMAN


#########
# Rules #
#########

# Replace the default suffixes by our own suffix list
.SUFFIXES:
.SUFFIXES: .c .o .h

#=============
# Main Target
#=============

.PHONY: all
all: SUBTARGET := all
all: $(TARGETS)
	@echo ==================================================

#===============
# Debug Version
#===============

.PHONY: debug
debug: SUBTARGET := debug
debug: $(TARGETS)
	@echo ==================================================

#==========================
# Explicit Partial Targets
#==========================

.PHONY: ont
ont:
	@echo ==================================================
	@echo Making Ontology...
	@$(MAKE) --silent --directory=$(ONTDIR) $(SUBTARGET)
	@echo ...done!

.PHONY: ll
ll: ont
	@echo ==================================================
	@echo Making Low-Level-C-Interface...
	@$(MAKE) --silent --directory=$(LLDIR) $(SUBTARGET)
	@echo ...done!

##################
# Pseudo Targets #
##################

.PHONY: clean
clean:
	@echo ==================================================
	@echo Cleaning project...
	@$(MAKE) --silent --directory=$(ONTDIR) clean
	@$(MAKE) --silent --directory=$(LLDIR) clean
	@echo --------------------------------------------------
	@echo -n Cleaning Main Directory...
	@$(RM) $(LOGS) *~
	@$(RM) -r $(DOXYDIRS) $(REFMAN)
	@echo done!
	@echo --------------------------------------------------
	@echo ...done!
	@echo ==================================================

.PHONY: docs
docs:
	@echo ==================================================
	@echo Making documentation...
	@doxygen Doxyfile
	@$(MAKE) --silent --directory=doc/latex > doc/latex/makePDF.log
	@mv doc/latex/refman.pdf ./$(REFMAN)
	@$(RM) -r doc/latex
	@echo ...done!
	@echo ==================================================

.PHONY: examples
examples: ll ont
	@echo ==================================================
	@echo Making examples...
	@$(MAKE) --silent --directory=$(EXAMPLEDIR) $(SUBTARGET)
	@echo ...done!
	@echo ==================================================

.PHONY: test
test:
	@echo ==================================================
	@echo Running tests...
	@$(MAKE) --silent --directory=$(ONTDIR) test
	@$(MAKE) --silent --directory=$(LLDIR) test
	@echo --------------------------------------------------
	@echo ...done!
	@echo ==================================================
