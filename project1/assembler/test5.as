        lw  0   1   res
        lw  0   2   minus
        lw  2   3   minus
start   add 1   3   1
        add 3   2   3
        beq 3   0   done
        beq 0   0   start
done    sw  0   1   res
        halt
res     .fill   0
count   .fill   10
minus   .fill   -1
