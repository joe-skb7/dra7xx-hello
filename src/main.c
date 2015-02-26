#include <stdint.h>
#include <board.h>
#include <common.h>
#include <config.h>
#include <serial.h>

/* Linker symbols */
extern char __bss_start[], __bss_end[];

static void sys_init(void)
{
#ifndef CONFIG_NOR_BOOT
	/* Clear the BSS */
	memset(__bss_start, 0, __bss_end - __bss_start);
#endif
}

static void init(void)
{
	sys_init();
	board_init();
	serial_init();
}

static void loop(void)
{
	serial_puts("Hello, DRA7xx!\n");
	for (;;)
		;
}

int main(void)
{
	init();
	loop();

	return 0;
}
