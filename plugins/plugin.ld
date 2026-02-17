SECTIONS {
    . = 0;
    .plugin_header : { *(.plugin_header) }
    .text          : { *(.text .text.* .literal .literal.*) }
    .rodata        : { *(.rodata .rodata.*) }
    /DISCARD/      : { *(.comment .note.* .eh_frame* .xtensa.info) }
}
