#ifndef CONFIG_H
#define CONFIG_H

/* Two choices:
 *
 * 0: ttyS0 = UART1
 * 5: ttyS5 = UART6
 */
#define CONSOLE			0

#define CONFIG_STACK_ADDR	0x40380000 /* Non-secure SRAM end */
#define CONFIG_SRAM_BASE	0x40300350
#define CONFIG_FLASH_BASE	0x08000000

#ifdef CONFIG_NOR_BOOT
# define CONFIG_TEXT_BASE	CONFIG_FLASH_BASE
#else
# define CONFIG_TEXT_BASE	CONFIG_SRAM_BASE
#endif

#endif /* CONFIG_H */
