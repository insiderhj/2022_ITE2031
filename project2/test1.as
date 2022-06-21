        lw	0	1	six
        lw	0	2	minus
        lw  0   3   minus
start   add	1	2	1       $data hazard on reg1
        add 3   2   3                   $data hazard on reg2
        noop
        noop
        beq 0   1   done
        noop
        noop
        beq 0   0   start
        noop
        noop
done	halt
six     .fill   6
minus   .fill   -1
stAddr	.fill   start
