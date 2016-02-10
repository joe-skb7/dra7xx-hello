/* Driver for NS16550 UART */

#include <serial.h>
#include <common.h>

#define SERIAL_CLK_HZ		48000000
#define BAUDRATE		115200

#define UART1_BASE		0x4806a000
#define UART6_BASE		0x48068000

#if (CONSOLE == 0)
# define UART_BASE		UART1_BASE
#elif (CONSOLE == 5)
# define UART_BASE		UART6_BASE
#else
# error Error: Wrong console number
#endif

#define OFF_RBR			0x00
#define OFF_THR			0x00
#define OFF_DLL			0x00
#define OFF_IER			0x04
#define OFF_DLM			0x04
#define OFF_FCR			0x08
#define OFF_IIR			0x08
#define OFF_LCR			0x0c
#define OFF_MCR			0x10
#define OFF_LSR			0x14
#define OFF_MSR			0x18
#define OFF_SCR			0x1c
#define OFF_MDR1		0x20

#define IER_DIS			0x00 /* disable all UART interrupts */
#define MDR1_RESET		0x07
#define MDR1_RUN		0x00
#define LSR_THRE		0x20
#define LSR_TEMT		0x40
#define LCR_BKSE		0x80 /* bank sel */
#define LCR_8N1			0x03
#define MCR_DTR			0x01 /* DTR complement */
#define MCR_RTS			0x02 /* RTS complement */
#define FCR_FIFO_EN		0x01 /* FIFO enable */
#define FCR_RXSR		0x02 /* Receiver soft reset */
#define FCR_TXSR		0x04 /* Transmitter soft reset */

#define WR(val, addr)		writeb(val, UART_BASE + OFF_##addr)
#define RD(addr)		readb(UART_BASE + OFF_##addr)

void serial_init(void)
{
	unsigned divisor = SERIAL_CLK_HZ / 16 / BAUDRATE;

	while (!(RD(LSR) & LSR_TEMT))
		;

	WR(IER_DIS, IER);
	WR(MDR1_RESET, MDR1);				/* reset */
	WR(MCR_DTR | MCR_RTS, MCR);
	WR(FCR_FIFO_EN | FCR_RXSR | FCR_TXSR, FCR);

	/* NS16550_setbrg() */
	WR(LCR_BKSE | LCR_8N1, LCR);
	WR(divisor & 0xff, DLL);
	WR(divisor >> 8, DLM);
	WR(LCR_8N1, LCR);

	WR(MDR1_RUN, MDR1);				/* run */
}

static inline void _serial_putc(char c)
{
	while (!(RD(LSR) & LSR_THRE))
		;
	WR(c, THR);
}

void serial_putc(char c)
{
	if (c == '\n')
		_serial_putc('\r');
	_serial_putc(c);
}

void serial_puts(const char *s)
{
	while (*s)
		serial_putc(*s++);
}
