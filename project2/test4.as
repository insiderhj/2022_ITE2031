        lw  0   1   two
        lw  0   3   minus
        noop
        noop
        noop
        lw  3   4   minus
        noop
        noop
start   add 1   1   1
        add 3   4   4
        noop
        noop
        noop
        beq 0   4   done
        noop
        noop
        beq 0   0   start
        noop
        noop
done    add 1   3   1
        halt
two     .fill   2
count   .fill   5
minus   .fill   -1
