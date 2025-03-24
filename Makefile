# Makefile for C++ project
VERSIONS_DIR=versions
VERSIONS_COMPILATION=$(wildcard $(VERSIONS_DIR)/*)
# We remove the versions/ prefix
VERS=$(subst versions/,,$(VERSIONS_COMPILATION))
# version specific targets
NO_VISUAL=0

# We print the list of available versions 
# Only print the version name
versions: $(VERSIONS_COMPILATION)
	@echo "Specifying LOCAL_LIBS=1 will build the version with local libraries."
	@echo "Specifying NO_VISUAL=1 will build the version without visualisation."
	@echo "Available versions:"
	@echo $(VERSIONS_COMPILATION) | tr ' ' '\n' | sed 's|^|  - |; s|versions/||'

# Targets
all: $(VERS)

# We build the versions
$(VERS):
	@echo "Building $@..."
	$(MAKE) -C $(VERSIONS_DIR)/$@ $(if $(filter 1,$(NO_VISUAL)),novisual)
	@echo "Building $@ done."

# We clean the versions
clean:
	@echo "Cleaning..."
	@for version in $(VERS); do \
		$(MAKE) -s -C $(VERSIONS_DIR)/$$version clean; \
	done
	@echo "Cleaning done."

.PHONY: all clean
