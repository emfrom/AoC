# Top-level Makefile
SUBDIRS := $(foreach dir, $(shell seq 1 25), \
             $(if $(wildcard $(dir)), $(dir)))

.PHONY: all clean $(SUBDIRS)

# Default target: build all subdirectories
all: $(SUBDIRS)

# Target for cleaning
clean: $(SUBDIRS)

# Propagate the targets to subdirectories
$(SUBDIRS):
	@echo "Entering directory '$@'"
	$(MAKE) -C $@ $(MAKECMDGOALS)
	@echo "Leaving directory '$@'"
