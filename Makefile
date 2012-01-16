#
# This make file is for testing the tree.
# Please cd into a project subdir and compile from there
#

# don't build these due to issues
PROJECT_DIRS_EXCLUDE = projects/Stokerbot-NG.1284

PROJECT_DIRS = $(filter-out ${PROJECT_DIRS_EXCLUDE}, $(wildcard projects/*))



buildbot :: clean all

clean ::
	echo "LIST: ${PROJECT_DIRS}"
	$(MAKE) $(PROJECT_DIRS:%=%.clean)

all ::
	echo "LIST: ${PROJECT_DIRS}"
	$(MAKE) $(PROJECT_DIRS:%=%.all)

# suffix rules to build target

%.all :: 
	@echo ""
	@echo "*****************************"
	@echo "Building: $*"
	@echo ""
	$(MAKE) -C $* all
	@echo ""
	@echo "Finished building: $*"
	@echo "*****************************"
	@echo ""

%.clean ::
	$(MAKE) -C $* clean
