        lw  0   1   two
        lw  0   3   minus
        lw  3   4   minus
start   add 1   1   1
        add 3   4   4
        beq 0   4   done
        beq 0   0   start
done    add 1   3   1
        halt
two     .fill   2
count   .fill   5
minus   .fill   -1
