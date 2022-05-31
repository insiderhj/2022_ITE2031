        lw	0	1	five
        lw  0   2   here
        lw  0   3   stAddr
        beq 0   0   jump
next    lw  0   5   here
	    noop
done	halt
jump    add 1   2   1 
        sw  0   1   here
        jalr    3   4
five	.fill   5
here	.fill   2
stAddr	.fill   next
