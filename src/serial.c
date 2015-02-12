#include <serial.h>
#include <common.h>

#define SERIAL_CLK_HZ		48000000
#define BAUDRATE		115200

#define UART6_BASE		0x48068000

#define OFF_RBR			0x00
#define OFF_THR			0x00
#define OFF_DLL			0x00
#define OFF_IER			0x04
#define OFF_DLM			0x04
#define OFF_FCR			0x08
#define OFF_IIR			0x08
#define OFF_LCR			0x0C
#define OFF_MCR			0x10
#define OFF_LSR			0x14
#define OFF_MSR			0x18
#define OFF_SCR			0x1C
#define OFF_MDR1		0x20

#define WR(val, addr)		writeb(val, UART6_BASE + OFF_##addr)
#define RD(addr)		readb(UART6_BASE + OFF_##addr)

void serial_init(void)
{
	unsigned divisor = SERIAL_CLK_HZ / 16 / BAUDRATE;

	WR(0x00, IER);
	WR(0x07, MDR1);			/* reset */
	WR(0x83, LCR);			/* 8N1 + banksel */
	WR(divisor & 0xFF, DLL);
	WR(divisor >> 8, DLM);
	WR(0x03, LCR);			/* 8N1 */
	WR(0x03, MCR);			/* DTR, RTS */
	WR(0x07, FCR);			/* reset and enable FIFO */
	WR(0x00, MDR1);			/* run */
}

static inline void _serial_putc(char c)
{
	while (!(RD(LSR) & 0x20))
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
