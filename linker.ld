OUTPUT_FORMAT("elf64-x86-64")
ENTRY(kernel_main)

SECTIONS {
    . = 0x100000;

    .text : {
        *(.text)
        *(.text.*)
    } :text

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    } :data

    .data : {
        *(.data)
        *(.data.*)
    } :data

    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    } :bss

    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
        *(.gcc_except_table*)
    }
}