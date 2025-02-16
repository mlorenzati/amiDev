# Main Makefile

SUBFOLDERS := nolibs muiTest sdlTest  # Space-separated list of subdirectories

# Detect OS
ifeq ($(OS),Windows_NT)
    # Windows-specific syntax
    LOOP_CMD = for /d %%d in ($(SUBFOLDERS)) do ( $(MAKE) -C %%d clean )
else
    # Unix-like (Linux, macOS) syntax
    LOOP_CMD = for dir in $(SUBFOLDERS); do $(MAKE) -C $$dir clean; done
endif

# Clean target to clean subfolders
clean:
	@echo "Cleaning subfolders..."
	$(LOOP_CMD)
	@echo "Cleaning all completed..."