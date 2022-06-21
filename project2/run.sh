gcc ../project1/assembler/assemble.c -o ../project1/assembler/assemble.out
gcc simulator.c -o simulate

../project1/assembler/assemble.out $1.as $1.mc
./simulate $1.mc > output

rm $1.mc
