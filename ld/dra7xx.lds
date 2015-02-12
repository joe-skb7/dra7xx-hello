#include <config.h>

MEMORY { .sram : ORIGIN = CONFIG_SRAM_BASE, LENGTH = CONFIG_SRAM_SIZE }
MEMORY { .sdram : ORIGIN = CONFIG_SDRAM_BASE, LENGTH = CONFIG_SDRAM_SIZE }

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
	.text :
	{
		__start = .;
		src/start.o	(.text*)
		*(.text*)
	} >.sram

	. = ALIGN(4);
	.rodata : { *(SORT_BY_ALIGNMENT(.rodata*)) } >.sram

	. = ALIGN(4);
	.data : { *(SORT_BY_ALIGNMENT(.data*)) } >.sram

	.bss :
	{
		. = ALIGN(4);
		__bss_start = .;
		*(.bss*)
		. = ALIGN(4);
		__bss_end = .;
	} >.sdram
}
