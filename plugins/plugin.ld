MEMORY {
    plugin (rwx) : ORIGIN = 0x0, LENGTH = 16K
}

SECTIONS {
    .plugin_header : { KEEP(*(.plugin_header)) } > plugin
    .text          : { *(.literal .literal.* .text .text.*) } > plugin
    .rodata        : { *(.rodata .rodata.*) } > plugin
    .data          : { *(.data .data.*) } > plugin
    .bss           : { *(.bss .bss.*) } > plugin
    /DISCARD/      : { *(.comment .note.* .eh_frame* .xtensa.info .xt.prop .xt.lit .xt.insn) }
}
