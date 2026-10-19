.include "vc4.qinc"

.set output, ra1

        ldi     r0, 0          # a = F(0)
        ldi     r1, 1          # b = F(1)

        # Repeat: (a, b) = (b, a + b)
        add     r2, r0, r1
        mov     r0, r1
        mov     r1, r2

        add     r2, r0, r1
        mov     r0, r1
        mov     r1, r2

        add     r2, r0, r1
        mov     r0, r1
        mov     r1, r2

        add     r2, r0, r1
        mov     r0, r1
        mov     r1, r2

        add     r2, r0, r1
        mov     r0, r1
        mov     r1, r2

        add     r2, r0, r1
        mov     r0, r1
        mov     r1, r2

        add     r2, r0, r1
        mov     r0, r1
        mov     r1, r2

        add     r2, r0, r1
        mov     r0, r1
        mov     r1, r2

        add     r2, r0, r1
        mov     r0, r1
        mov     r1, r2

        add     r2, r0, r1
        mov     r0, r1
        mov     r1, r2

        # r1 now contains F(10) = 55. Write the 16 SIMD lanes to output.
        mov     output, unif
        mov     vw_setup, vpm_setup(0, 1, h32(0))
        mov     vpm, r1
        mov     -, vw_wait
        mov     vw_setup, vdw_setup_0(1, 16, dma_h32(0, 0))
        mov     vw_setup, vdw_setup_1(0)
        mov     vw_addr, output
        mov     -, vw_wait
        thrend