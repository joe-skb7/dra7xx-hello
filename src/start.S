#include <config.h>

.globl _start
_start:
	/* Disable interrupts (FIQ and IRQ), also set the cpu to SVC32 mode,
	 * except if in HYP mode already
	 */
	mrs	r0, cpsr
	and	r1, r0, #0x1f		@ mask mode bits
	teq	r1, #0x1a		@ test for HYP mode
	bicne	r0, r0, #0x1f		@ clear all mode bits
	orrne	r0, r0, #0x13		@ set SVC mode
	orr	r0, r0, #0xc0		@ disable FIQ and IRQ
	msr	cpsr,r0

	/* Configure stack */
	ldr	sp, =(CONFIG_STACK_ADDR)
	bic	sp, sp, #7	/* 8-byte alignment for ABI compliance */

	/* Jump to C code */
	bl	main
