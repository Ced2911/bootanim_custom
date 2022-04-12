.section .ctr0_text

!
! Entry point
!
.global start
start:

    !
    ! Clear BSS
    mov.l   bss_start,r0
    mov.l   bss_end,r1
    mov     #0,r2
lbss:
    cmp/ge  r1,r0
    bt      lbss_end
    mov.b   r2,@r0
    add     #1,r0
    bra     lbss
    nop
lbss_end:
    ! Set initial stack pointer
    mov.l    stack_ptr,r15

    ! Jump to _main()
    mov.l   main_ptr,r0
    jsr @r0
    nop

    !
    ! Once _main() has terminated, disable interrupts and loop infinitely
    !
    mov     #0xf,r0
    shll2   r0
    shll2   r0
    ldc     r0,sr
end:
    bra     end
    nop
.align 2
start_ptr:  .long start
stack_ptr:  .long 0x06002000 
bss_start:  .long __bss_start
bss_end:    .long __bss_end
main_ptr:   .long _main

.global _atexit
_atexit:
   bra end
   nop

