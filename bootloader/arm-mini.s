.global _start
.section .data
output_char:    .asciz "A"
.text 0x000
@trap table vector (instruction executed on trap)
_start:
_trap_table:
    @ Each trap instruction loads the address of its trap handler.
    ldr pc, =0x34	@ Reset
    ldr pc, $handlers + 4	@ Supervisor Call
    ldr pc, $handlers + 8	@ Supervisor Call
    ldr pc, $handlers + 12	@ Prefetch Abort
    ldr pc, $handlers + 16	@ Data Abort
    nop				@ Reserved; no-op.
    ldr pc, $handlers + 20	@ Regular Interrupt
    ldr pc, $handlers + 24	@ Fast Interrupt

@pointers to trap handler routines
handlers:
    .int reset, reset, reset, reset, reset, reset, reset

@reset routine
.text 0x34
reset:
    mrs r0, cpsr	@ Get status register
    bic r0, r0, #0xff	@ Clear status register bits 0-7
    orr r0, r0, #0xd3	@ Mask with 0x11010011
    msr cpsr_fc, r0	@ Load value into status register
    b loop_init		@ Jump to looping routine


loop_init:
    mov r0, #0		@ Set limitcounter to 64
    LDR R4, =0xA0000000 @ Address of the string
loop_body:
    mov  r3, #0x40       @storing the address of UART in r3    -------
    lsl  r3,#8
    orr  r3,r3,#0x10
    lsl  r3,#16
    add  r5,r3,#20
    ldr  r6,[r5]
    mov  r7,#0x20
    and  r7,r7,r6
    cmp  r7,#0
    beq  loop_body
    mov R2,#0           @we want to move zero into R2
    LDRB R2, [R4]        @ Load the first byte of the string into R2
    strb r2,[r3]        @sending the character to the address  --------
    add  R4,#1
    cmp  R2,#0
    beq  post_loop
    b loop_body

post_loop:
    b post_loop
