#!/usr/bin/env python3
"""
Patch plugin binary header with function offsets.

Reads an ELF file to find *_activate, *_tick, *_deactivate symbol addresses,
then patches those as little-endian uint32 offsets into the corresponding
fields of the plugin_header_t at the start of the .bin file.

Usage: python3 patch_header.py <elf_file> <bin_file> [toolchain_prefix]
"""

import struct
import subprocess
import sys
import os


NM = "xtensa-esp32-elf-nm"


def get_symbol_address(elf_path, suffix):
    """Find a symbol ending with the given suffix using nm."""
    result = subprocess.run(
        [NM, elf_path],
        capture_output=True, text=True, check=True
    )
    for line in result.stdout.splitlines():
        parts = line.strip().split()
        if len(parts) == 3:
            addr, _typ, name = parts
            if name.endswith(suffix):
                return int(addr, 16)
    return None


def main():
    global NM
    if len(sys.argv) < 3 or len(sys.argv) > 4:
        print(f"Usage: {sys.argv[0]} <elf_file> <bin_file> [toolchain_prefix]")
        sys.exit(1)

    elf_path = sys.argv[1]
    bin_path = sys.argv[2]
    if len(sys.argv) == 4:
        NM = sys.argv[3] + "nm"

    activate_addr = get_symbol_address(elf_path, "_activate")
    tick_addr = get_symbol_address(elf_path, "_tick")
    deactivate_addr = get_symbol_address(elf_path, "_deactivate")

    if activate_addr is None or tick_addr is None or deactivate_addr is None:
        print("ERROR: Could not find all three function symbols (*_activate, *_tick, *_deactivate)")
        print(f"  activate:   {activate_addr}")
        print(f"  tick:        {tick_addr}")
        print(f"  deactivate: {deactivate_addr}")
        sys.exit(1)

    # plugin_header_t layout:
    #   uint32_t magic;              offset 0
    #   uint32_t abi_version;        offset 4
    #   uint32_t header_size;        offset 8
    #   char     uuid[37];           offset 12
    #   uint8_t  _pad_uuid[3];       offset 49
    #   uint32_t version;            offset 52
    #   char     name[48];           offset 56
    #   uint32_t activate_offset;    offset 104
    #   uint32_t tick_offset;        offset 108
    #   uint32_t deactivate_offset;  offset 112
    activate_field_offset = 104
    tick_field_offset = 108
    deactivate_field_offset = 112

    with open(bin_path, "r+b") as f:
        f.seek(activate_field_offset)
        f.write(struct.pack("<I", activate_addr))
        f.seek(tick_field_offset)
        f.write(struct.pack("<I", tick_addr))
        f.seek(deactivate_field_offset)
        f.write(struct.pack("<I", deactivate_addr))

    print(f"Patched {os.path.basename(bin_path)}:")
    print(f"  activate:   0x{activate_addr:08x}")
    print(f"  tick:        0x{tick_addr:08x}")
    print(f"  deactivate: 0x{deactivate_addr:08x}")


if __name__ == "__main__":
    main()
