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
# Only set this if placed somewhere really unusual

#=====================
# Flags and Libraries
#=====================
CFLAGS := -std=gnu1x -pedantic -Wall -Wextra
LIBFLAGS := $(CFLAGS) -shared -fPIC
LDFLAGS := $(CFLAGS)
GLIBFLAGS := $(shell pkg-config --cflags glib-2.0)
GLIBLIBS := $(shell pkg-config --libs glib-2.0)
#ONTFLAGS := -iquote$(ONTDIR) -Wl,-rpath=$(ONTDIR)
ONTFLAGS := -I$(ONTDIR) -Wl,-rpath=$(ONTDIR)
ONTLIBS := -lsioxont -L$(ONTDIR)
LLFLAGS := -I$(LLDIR) -Wl,-rpath=$(LLDIR) $(GLIBFLAGS)
LLLIBS := -lsiox-ll -L$(LLDIR) $(GLIBLIBS)

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
REFMAN := SIOX-Reference
# Don't hand these variables down to sub-makes
unexport DOXYDIRS LOGS REFMAN


#########
# Rules #
#########

# Replace the default suffixes by our own suffix list
.SUFFIXES:
.SUFFIXES: .c .o .h

#==============
# Main Targets
#==============

.PHONY: all
all: SUBTARGET := all
all: $(TARGETS) Makefile
	@echo ==================================================

#===============
# Debug Version
#===============

.PHONY: debug
debug: SUBTARGET := debug
debug: $(TARGETS) Makefile
	@echo ==================================================

#======================
# Intermediate Targets
#======================

.PHONY: ont
ont: Makefile
	@echo ==================================================
	@echo Making Ontology...
	@$(MAKE) --silent --directory=$(ONTDIR) $(SUBTARGET)
	@echo ...done!

.PHONY: ll
ll: ont Makefile
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
	@$(MAKE) --silent --directory=$(EXAMPLEDIR) clean
	@echo --------------------------------------------------
	@echo -n Cleaning Main Directory...
	@$(RM) $(LOGS) *~
	@$(RM) -r $(DOXYDIRS) $(REFMAN).pdf $(REFMAN).html
	@echo done!
	@echo --------------------------------------------------
	@echo ...done!
	@echo ==================================================

.PHONY: docs
docs: Doxyfile Makefile
	@echo ==================================================
	@echo Making documentation...
	@doxygen Doxyfile
	@$(MAKE) --silent --directory=doc/latex > doc/latex/makePDF.log
	@mv doc/latex/refman.pdf ./$(REFMAN).pdf
	@ln -s doc/html/index.html ./$(REFMAN).html
	@$(RM) -r doc/latex
	@echo ...done!
	@echo ==================================================

.PHONY: examples
examples: all Makefile
	@echo Making examples...
	@$(MAKE) --silent --directory=$(EXAMPLEDIR) $(SUBTARGET)
	@echo ...done!
	@echo ==================================================

.PHONY: test
test: all Makefile
	@echo Running tests...
	@$(MAKE) --silent --directory=$(ONTDIR) test
	@$(MAKE) --silent --directory=$(LLDIR) test
	@echo --------------------------------------------------
	@echo ...done!
	@echo ==================================================
