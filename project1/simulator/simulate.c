#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define NUMMEMORY 65536
#define NUMREGS 8
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
    int totalInst;
} stateType;

void initState(struct stateStruct *);
int convertNum(int);
void printState(stateType *);

int main(int argc, char *argv[])
{    
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;
    int *mem;

    int opcode, regA, regB, dest;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    initState(&state);
    for(; state.pc < state.numMemory; state.pc++) {

        printState(&state);
        state.totalInst++;

        opcode = state.mem[state.pc] >> 22 & 7;
        regA = state.mem[state.pc] >> 19 & 7;
        regB = state.mem[state.pc] >> 16 & 7;

        switch (opcode) {
        case 0:
            dest = state.mem[state.pc] & 7;
            state.reg[dest] = state.reg[regA] + state.reg[regB];
            break;

        case 1:
            dest = state.mem[state.pc] & 7;
            state.reg[dest] = ~(state.reg[regA] | state.reg[regB]);
            break;

        case 2:
            dest = convertNum(state.mem[state.pc] & 65535);
            state.reg[regB] = state.mem[state.reg[regA] + dest];
            break;

        case 3:
            dest = convertNum(state.mem[state.pc] & 65535);
            state.mem[state.reg[regA] + dest] = state.reg[regB];
            break;

        case 4:
            dest = convertNum(state.mem[state.pc] & 65535);
            if (state.reg[regA] == state.reg[regB]) state.pc = state.pc + dest;
            break;

        case 5:
            state.reg[regB] = state.pc + 1;
            state.pc = state.reg[regA] - 1;
            break;

        case 6:
            printf("machine halted\n");
            state.pc++;
            goto halt;
        }
    }

halt:
    printf("total of %d instructions executed\n", state.totalInst);
    printf("final state of machine:\n");
    printState(&state);
    return(0);
}

void initState(struct stateStruct *state) {
    int *reg;
    
    state->pc = 0;
    state->totalInst = 0;
    for(reg = state->reg; reg < &state->reg[NUMREGS]; reg++) {
        *reg = 0;
    }
}

int convertNum(int num) {
    if (num & (1<<15) ) num -= (1<<16);
    return(num);
}

void printState(stateType *statePtr)
{    
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}
