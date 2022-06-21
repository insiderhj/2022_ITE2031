        lw  0   1   res
        lw  0   2   minus
        noop
        noop
        noop
        lw  2   3   minus
        noop
        noop
        noop
start   add 1   3   1
        add 3   2   3
        noop
        noop
        noop
        beq 3   0   done
        noop
        noop
        beq 0   0   start
        noop
        noop
done    sw  0   1   res
        halt
res     .fill   0
count   .fill   10
minus   .fill   -1
