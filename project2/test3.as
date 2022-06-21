        lw  0   1   first
        lw  0   2   second
        lw  0   5   mul
        lw  0   6   minus
        noop
        noop
        nor 1   2   3
        noop
        noop
        noop
        add 0   3   4
        noop
        noop
        noop
start   add 3   4   3
        add 5   6   5
        noop
        noop
        noop
        beq 0   5   done
        noop
        noop
        beq 0   0   start
        noop
        noop
done    sw  0   3   res
        halt
first   .fill   -7
second  .fill   -15
mul     .fill   3
minus   .fill   -1
res     .fill   0
