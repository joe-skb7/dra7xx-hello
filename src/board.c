#include <stdint.h>
#include <board.h>
#include <common.h>

/* UART1 (ttyS0) */
#define UART1_RX		0x4a0037e0	/* pad name: uart1_rxd */
#define UART1_TX		0x4a0037e4	/* pad name: uart1_txd */
#define PM_L4PER_PWRSTCTRL	0x4ae07400
#define PM_L4PER_PWRSTST	0x4ae07404
#define CM_L4PER_CLKSTCTRL	0x4a009700
#define CM_L4PER_UART1_CLKCTRL	0x4a009840

/* UART6 (ttyS5) */
#define UART6_RX		0x4a00376c	/* pad name: MMC1_SDCD */
#define UART6_TX		0x4a003770	/* pad name: MMC1_SDWP */
#define PM_IPU_PWRSTCTRL	0x4ae06500
#define PM_IPU_PWRSTST		0x4ae06504
#define CM_IPU_CLKSTCTRL	0x4a005540
#define CM_IPU_UART6_CLKCTRL	0x4a005580

#define MODE(n)			(n & 0xf)
#define PEN			(0 << 16)	/* Pull Up/Down: Enable */
#define PDIS			(1 << 16)	/* Pull Up/Down: Disable */
#define PTU			(1 << 17)	/* Pull Up/Down Type: Up */
#define PTD			(0 << 17)	/* Pull Up/Down Type: Down */
#define IN			(1 << 18)
#define OUT			(0 << 18)
#define FSC			(1 << 19)	/* Fast Slew Control */
#define SSC			(0 << 19)	/* Slow Slew Control */

/* CM_<clock_domain>_CLKCTRL */
#define CD_CLKCTRL_CLKTRCTRL_SHIFT			0
#define CD_CLKCTRL_CLKTRCTRL_MASK			3
#define CD_CLKCTRL_CLKTRCTRL_NO_SLEEP			0
#define CD_CLKCTRL_CLKTRCTRL_SW_SLEEP			1
#define CD_CLKCTRL_CLKTRCTRL_SW_WKUP			2
#define CD_CLKCTRL_CLKTRCTRL_HW_AUTO			3

/* CM_<clock_domain>_<module>_CLKCTRL */
#define MODULE_CLKCTRL_MODULEMODE_SHIFT			0
#define MODULE_CLKCTRL_MODULEMODE_MASK			3
#define MODULE_CLKCTRL_IDLEST_SHIFT			16
#define MODULE_CLKCTRL_IDLEST_MASK			(3 << 16)
#define MODULE_CLKCTRL_MODULEMODE_SW_DISABLE		0
#define MODULE_CLKCTRL_MODULEMODE_HW_AUTO		1
#define MODULE_CLKCTRL_MODULEMODE_SW_EXPLICIT_EN	2

/* PM_<power_domain>_PWRSTST */
#define PD_PWRSTST_POWERSTATEST_SHIFT			0
#define PD_PWRSTST_POWERSTATEST_MASK			3
#define PD_PWRSTST_POWERSTATEST_ON_ACTIVE		0x3

#if (CONSOLE == 0)
# define UART_RX		UART1_RX
# define UART_TX		UART1_TX
# define UART_RX_MODE		(MODE(0) | IN | PEN | PTU | FSC)
# define UART_TX_MODE		(MODE(0) | IN | PEN | PTU | FSC)
# define UART_PWRSTCTRL		PM_L4PER_PWRSTCTRL
# define UART_PWRSTST		PM_L4PER_PWRSTST
# define UART_CLKSTCTRL		CM_L4PER_CLKSTCTRL
# define UART_CLKCTRL		CM_L4PER_UART1_CLKCTRL
#elif (CONSOLE == 5)
# define UART_RX		UART6_RX
# define UART_TX		UART6_TX
# define UART_RX_MODE		(MODE(3) | IN | PEN | PTU)
# define UART_TX_MODE		(MODE(3) | OUT | PDIS)
# define UART_PWRSTCTRL		PM_IPU_PWRSTCTRL
# define UART_PWRSTST		PM_IPU_PWRSTST
# define UART_CLKSTCTRL		CM_IPU_CLKSTCTRL
# define UART_CLKCTRL		CM_IPU_UART6_CLKCTRL
#else
# error Error: Wrong console number
#endif

struct pin {
	uint32_t addr;
	uint32_t mode;
};

static struct pin pins[] = {
	{
		.addr = UART_TX,
		.mode = UART_TX_MODE,
	},
	{
		.addr = UART_RX,
		.mode = UART_RX_MODE,
	},
};

static void board_mux_pins(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(pins); ++i)
		writel(pins[i].mode, pins[i].addr);
}

static void board_enable_clock(uint32_t cm_src, uint32_t cm_dest,
		uint32_t pm_dest)
{
	/* Force destination domain to be awake (SW_WKUP) */
	clrsetbits(cm_dest, CD_CLKCTRL_CLKTRCTRL_MASK,
			CD_CLKCTRL_CLKTRCTRL_SW_WKUP <<
			CD_CLKCTRL_CLKTRCTRL_SHIFT);

	/* Wait until power domain that encloses the destination domain is ON */
	while ((readl(pm_dest) & PD_PWRSTST_POWERSTATEST_MASK) !=
			PD_PWRSTST_POWERSTATEST_ON_ACTIVE)
		;

	/* Explicitly enable src module (functional clocks and power) */
	clrsetbits(cm_src, MODULE_CLKCTRL_MODULEMODE_MASK,
			MODULE_CLKCTRL_MODULEMODE_SW_EXPLICIT_EN <<
			MODULE_CLKCTRL_MODULEMODE_SHIFT);
}

static void board_setup_clocks(void)
{
	/* Enable UART clock */
	board_enable_clock(UART_CLKCTRL, UART_CLKSTCTRL, UART_PWRSTST);
}

void board_init(void)
{
	board_setup_clocks();
	board_mux_pins();
}
