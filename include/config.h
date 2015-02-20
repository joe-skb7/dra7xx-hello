#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_STACK_ADDR		0x40380000 /* Non-secure SRAM end */

#if (CONFIG_BOOT == 1)		/* SD card */

#define CONFIG_SRAM_BASE		0x40300350
#define CONFIG_SDRAM_BASE		0x84000000

#elif (CONFIG_BOOT == 2)	/* NOR (XIP) */

#define CONFIG_FLASH_BASE		0x08000000

#else

#error Invalid CONFIG_BOOT value!

#endif /* CONFIG_BOOT */

#define CONFIG_SRAM_SIZE		0x20000		/* 128 KiB */
#define CONFIG_SDRAM_SIZE		0x100000	/* 1 MiB */

#endif /* CONFIG_H */
