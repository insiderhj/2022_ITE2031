        lw	0	1	eight
        lw	0	2	minus
        lw  0   3   minus
start   add	1	2	1
        add 3   2   3
        beq 0   1   done
        beq 0   0   start
	    noop
done	halt
six     .fill   6
minus   .fill   -1
stAddr	.fill   start
