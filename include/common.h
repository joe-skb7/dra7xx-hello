#ifndef COMMON_H
#define COMMON_H

#define ARRAY_SIZE(a)		(sizeof(a) / sizeof(a[0]))

#define writeb(v,a)		(*(volatile unsigned char *)(a) = (v))
#define writew(v,a)		(*(volatile unsigned short *)(a) = (v))
#define writel(v,a)		(*(volatile unsigned int *)(a) = (v))

#define readb(a)		(*(volatile unsigned char *)(a))
#define readw(a)		(*(volatile unsigned short *)(a))
#define readl(a)		(*(volatile unsigned int *)(a))

#define clrsetbits(addr, clear, set) \
	writel((readl(addr) & ~(clear)) | (set), (addr))

typedef unsigned int size_t;

void ldelay(volatile unsigned int loops);
void *memset(void *s, int c, size_t count);

#endif /* COMMON_H */
