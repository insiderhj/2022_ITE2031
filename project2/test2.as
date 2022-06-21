        lw	0	1	five
        lw  0   2   here
        lw  0   3   stAddr
        beq 0   0   jump
        noop
        noop
next    lw  0   5   here
done	halt
jump    add 1   2   1 
        noop
        noop
        noop
        sw  0   1   here
        beq 0   0   next
        noop
        noop
five	.fill   5
here	.fill   2
stAddr	.fill   next
