################
# Definitionen #
################

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
HDF5DIR :=
#/usr/include

#=======
# Flags
#=======
CFLAGS := -std=c1x -pedantic -Wall
LIBFLAGS := $(CFLAGS) -shared -fPIC
LDFLAGS := $(CFLAGS)
HDF5FLAGS := -I$(HDF5DIR)/include -L$(HDF5DIR)/lib
#ONTFLAGS := -iquote$(ONTDIR) -Wl,-rpath=$(ONTDIR)
ONTFLAGS := -I$(ONTDIR) -Wl,-rpath=$(ONTDIR)
LLFLAGS := -I$(LLDIR) -Wl,-rpath=$(LLDIR)

#=========
# Targets
#=========
TARGETS := ont ll
# Don't hand these variables down to sub-makes
unexport TARGETS

#=============
# für Doxygen
#=============
DOXYDIRS := html latex
LOGS := *.log
REFMAN := SIOX-Reference.pdf
# Don't hand these variables down to sub-makes
unexport DOXYDIRS LOGS REFMAN


##########
# Regeln #
##########

# Replace the default suffixes by our own suffix list
.SUFFIXES:
.SUFFIXES: .c .o .h

#=========
# Endziel
#=========

.PHONY: all
all: SUBTARGET := all
all: $(TARGETS)
	@echo ==================================================

#===============
# Debug-Version
#===============

.PHONY: debug
debug: SUBTARGET := debug
debug: $(TARGETS)
	@echo ==================================================

#=========================
# explizite Zwischenziele
#=========================

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

###############
# Pseudoziele #
###############

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
	@$(MAKE) --silent --directory=latex > latex/makePDF.log
	@mv latex/refman.pdf ./$(REFMAN)
	#@$(RM) -r latex
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
