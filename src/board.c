#include <stdint.h>
#include <board.h>
#include <common.h>

#define MODE(n)			(n & 0xf)
#define PEN			(0 << 16)	/* Pull Up/Down: Enable */
#define PDIS			(1 << 16)	/* Pull Up/Down: Disable */
#define PTU			(1 << 17)	/* Pull Up/Down Type: Up */
#define PTD			(0 << 17)	/* Pull Up/Down Type: Down */
#define IN			(1 << 18)
#define OUT			(0 << 18)

#define UART6_TX		0x4a003770	/* pad name: MMC1_SDWP */
#define UART6_RX		0x4a00376c	/* pad name: MMC1_SDCD */

#define PM_IPU_PWRSTCTRL				0x4ae06500
#define PM_IPU_PWRSTST					0x4ae06504
/* ipu.ckgen */
#define CM_IPU_CLKSTCTRL				0x4a005540
#define CM_IPU_UART6_CLKCTRL				0x4a005580

/* CM_<clock_domain>__CLKCTRL */
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

struct pin {
	uint32_t addr;
	uint32_t mode;
};

static struct pin pins[] = {
	{
		.addr = UART6_TX,
		.mode = MODE(3) | OUT | PDIS,
	},
	{
		.addr = UART6_RX,
		.mode = MODE(3) | IN | PEN | PTU,
	},
};

static void board_mux_pins(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(pins); ++i)
		writel(pins[i].mode, pins[i].addr);
}

/* Do not add any prints in this function */
static void board_setup_clocks(void)
{
	/* Enable UART6 */

	/* Turn IPU power on for UART6.
	 *
	 * POWERSTATE:       PM_IPU_PWRSTCTRL[0:1] = 0x3 (ON State)
	 * AESSMEM_RETSTATE: PM_IPU_PWRSTCTRL[8] = 0x1 (Memory bank is retained
	 *                   when domain is in RETENTION state)
	 */
	writel(0x103, PM_IPU_PWRSTCTRL);

	clrsetbits(CM_IPU_CLKSTCTRL, CD_CLKCTRL_CLKTRCTRL_MASK,
			CD_CLKCTRL_CLKTRCTRL_SW_WKUP <<
			CD_CLKCTRL_CLKTRCTRL_SHIFT);
	/* Wait until IPU power domain is ON-ACTIVE */
	while ((readl(PM_IPU_PWRSTST) & 0x3) != 0x3);

	clrsetbits(CM_IPU_UART6_CLKCTRL,
			MODULE_CLKCTRL_MODULEMODE_MASK,
			MODULE_CLKCTRL_MODULEMODE_SW_EXPLICIT_EN <<
			MODULE_CLKCTRL_MODULEMODE_SHIFT);
}

void board_init(void)
{
	board_setup_clocks();
	board_mux_pins();
}
