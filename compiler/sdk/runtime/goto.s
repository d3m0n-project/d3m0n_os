.syntax unified
.arm

.global _setjmp
.type _setjmp, %function

.global _longjmp
.type _longjmp, %function

_setjmp:
	// r0 = jmp_buf env
	stmia   r0, {r4-r11}

	str	 sp, [r0, #32]
	str	 lr, [r0, #36]

	// returns 0
	mov	 r0, #0
	bx	  lr

.size _setjmp, .-_setjmp

_longjmp:
    /*
     * r0 = jmp_buf env
     * r1 = value
     */

    // longjmp(env, 0) must return 1
    cmp     r1, #0
    moveq   r1, #1

    // restore r4-r11
    ldmia   r0, {r4-r11}

    // restore stack pointer
    ldr     sp, [r0, #32]

    // restore return address
    ldr     lr, [r0, #36]

    // setjmp() will appear to return val
    mov     r0, r1

    // continue execution
    bx      lr

.size _longjmp, .-_longjmp
