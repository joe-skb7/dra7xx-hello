#include <common.h>

void ldelay(volatile unsigned int loops)
{
	while (loops--)
		;
}

/**
 * memset - Fill a region of memory with the given value
 * @s: Pointer to the start of the area.
 * @c: The byte to fill the area with
 * @count: The size of the area.
 *
 * Do not use memset() to access IO space, use memset_io() instead.
 */
void *memset(void *s, int c, size_t count)
{
	unsigned long *sl = (unsigned long *)s;
	unsigned long cl = 0;
	char *s8;
	int i;

	/* Do it one word at a time (32 bits or 64 bits) while possible */
	if (((unsigned long)s & (sizeof(*sl) - 1)) == 0) {
		for (i = 0; i < sizeof(*sl); i++) {
			cl <<= 8;
			cl |= c & 0xff;
		}
		while (count >= sizeof(*sl)) {
			*sl++ = cl;
			count -= sizeof(*sl);
		}
	}

	/* Fill 8 bits at a time */
	s8 = (char *)sl;
	while (count--)
		*s8++ = c;

	return s;
}
