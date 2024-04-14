# use 16 bit instructions, and mark the start of the text
.code16
.global _start

.data
msg:
    .ascii "Hello, World!\n"
    len = . - msg
.text



# reset routine
_start:
    movw    $0x500,%di
    jmp loop_init	# Jump to looping routine



loop_init:
    movb    (%di),%al
    cmpb    $0,%al
    je	    finish           #check if null character
    #UART START
    mov     %al,%bl          #moving the stored character into bl
    mov     $0x3fd,%dx
    in      (%dx),%al
    test    $0x20,%al       #start of comparisons to see if UART is busy
    jnz     print            #If THR is empty then print a character 
    jmp     loop_init        #If UART is busy then just re-do everything
    #UART END
 finish:
    jmp     finish
print:
    mov     %bl,%al         #put the stores character from bl into al
    mov     $0x3f8,%dx
    out     %al,(%dx)
    #mov    $0x0e,%ah       #prepare to print
    #int     $0x10
    addw    $1,%di
    jmp     loop_init

    


. = _start + 510		# Skip forward to byte 510
.word 0xaa55			# magic number that tells the BIOS that this device is bootable
