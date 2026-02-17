# Plugin build system for GyverLampCpp
# Requires xtensa-esp32-elf toolchain (installed with ESP-IDF or PlatformIO)

# Try to find the toolchain from PlatformIO packages
TOOLCHAIN_PREFIX ?= xtensa-esp32-elf-
CC      = $(TOOLCHAIN_PREFIX)gcc
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
NM      = $(TOOLCHAIN_PREFIX)nm
READELF = $(TOOLCHAIN_PREFIX)readelf

PLUGIN_API_DIR = ../src/plugin

CFLAGS = -Os -ffreestanding -fno-builtin -nostdlib -nostartfiles \
         -fno-exceptions -ffunction-sections -fdata-sections \
         -mtext-section-literals -I$(PLUGIN_API_DIR)

LDFLAGS = -nostdlib -nostartfiles -T plugin.ld

BUILDDIR = build
OUTDIR   = ../data/plugins

PLUGINS = sparkles_plugin color_plugin

.PHONY: all clean validate

all: $(PLUGINS:%=$(OUTDIR)/%.bin)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(OUTDIR):
	mkdir -p $(OUTDIR)

# Compile .c -> .o
$(BUILDDIR)/%.o: %.c $(PLUGIN_API_DIR)/plugin_api.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link .o -> .elf
$(BUILDDIR)/%.elf: $(BUILDDIR)/%.o | $(BUILDDIR)
	$(CC) $(LDFLAGS) $< -o $@

# Convert .elf -> .bin, then patch header
$(BUILDDIR)/%.bin: $(BUILDDIR)/%.elf | $(BUILDDIR)
	$(OBJCOPY) -O binary $< $@
	python3 patch_header.py $< $@ $(TOOLCHAIN_PREFIX)

# Copy to data/plugins/
$(OUTDIR)/%.bin: $(BUILDDIR)/%.bin | $(OUTDIR)
	cp $< $@

# Validate: check for no relocations and no undefined symbols
validate: $(PLUGINS:%=$(BUILDDIR)/%.elf)
	@echo "=== Validation ==="
	@for elf in $^; do \
		echo "--- $$elf ---"; \
		echo "Relocations (should be empty):"; \
		$(READELF) -r $$elf || true; \
		echo "Undefined symbols (should be empty):"; \
		$(NM) -u $$elf || true; \
		echo ""; \
	done

clean:
	rm -rf $(BUILDDIR)
	rm -f $(OUTDIR)/*.bin
