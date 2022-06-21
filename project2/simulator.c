#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pipelines.c"

void initState(stateType *);
int convertNum(int);
void printState(stateType *);

int field0(int);
int field1(int);
int field2(int);
int opcode(int);
void printInstruction(int);

int main(int argc, char *argv[])
{    
    char line[MAXLINELENGTH];
    stateType state, newState;
    FILE *filePtr;
    int *mem;
    int halt = 0;

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

    initState(&state);
    initState(&newState);

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {
        if (sscanf(line, "%d", state.instrMem+state.numMemory) != 1 ||
            sscanf(line, "%d", state.dataMem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.instrMem[state.numMemory]);
    }

    printf("%d memory words\n", state.numMemory);
    printf("\tinstruction memory:\n");
    for(int i = 0; i < state.numMemory; i++){
        printf("\t\tinstrMem[ %d ] ", i);
        printInstruction(state.instrMem[i]);
    }

    while (!halt) {
        printState(&state);

        newState = state;
        newState.cycles++;

        /* --------------------- IF stage --------------------- */
        newState.IFID.instr = state.instrMem[state.pc];
        newState.IFID.pcPlus1 = state.pc + 1;

        /* --------------------- ID stage --------------------- */
        newState.IDEX.instr = state.IFID.instr;
        newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
        newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
        newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
        newState.IDEX.offset = field2(state.IFID.instr);

        /* --------------------- EX stage --------------------- */
        newState.EXMEM.instr = state.IDEX.instr;
        newState.EXMEM.branchTarget = convertNum(state.IDEX.offset) + state.IDEX.pcPlus1;

        switch (opcode(state.IDEX.instr)) {
        case ADD:
            newState.EXMEM.aluResult = state.IDEX.readRegA + state.IDEX.readRegB;
            break;

        case NOR:
            newState.EXMEM.aluResult = ~(state.IDEX.readRegA | state.IDEX.readRegB);
            break;

        case LW:
        case SW:
            newState.EXMEM.aluResult = convertNum(state.IDEX.offset) + state.IDEX.readRegA;
            break;
        case BEQ:
            if(state.IDEX.readRegA == state.IDEX.readRegB) {
                memset(&newState.IFID, 0, sizeof(newState.IFID));
                newState.IFID.instr = NOOP << 22;
                
                memset(&newState.IDEX, 0, sizeof(newState.IDEX));
                newState.IDEX.instr = NOOP << 22;

                newState.pc = newState.EXMEM.branchTarget - 1;
            }
            break;
        }
        newState.EXMEM.readRegB = state.IDEX.readRegB;

        /* --------------------- MEM stage --------------------- */
        newState.MEMWB.instr = state.EXMEM.instr;
        switch (opcode(state.EXMEM.instr)) {
        case ADD:
        case NOR:
            newState.MEMWB.writeData = state.EXMEM.aluResult;
            break;

        case LW:
            newState.MEMWB.writeData = newState.dataMem[state.EXMEM.aluResult];
            break;

        case SW:
            newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;
            break;

        case HALT:
            halt = 1;
        }

        /* --------------------- WB stage --------------------- */
        newState.WBEND.instr = state.MEMWB.instr;
        newState.WBEND.writeData = state.MEMWB.writeData;
        switch (opcode(state.MEMWB.instr)) {
        case ADD:
        case NOR:
            newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
            break;

        case LW:
            newState.reg[field1(state.MEMWB.instr)] = state.MEMWB.writeData;
            break;
        }

        newState.pc = newState.pc + 1;
        state = newState;
    }

    printState(&state);
    printf("machine halted\n");
    printf("total of %d cycles executed\n", state.cycles);
    exit(0);
}

void initState(stateType *state) {

    memset(state, 0, sizeof(state));
    state->IFID.instr = NOOP << 22;
    state->IDEX.instr = NOOP << 22;
    state->EXMEM.instr = NOOP << 22;
    state->MEMWB.instr = NOOP << 22;
    state->WBEND.instr = NOOP << 22;
}

int convertNum(int num) {
    if (num & (1<<15) ) num -= (1<<16);
    return(num);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);
    printf("\tdata memory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("\tIFID:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->IFID.instr);
    printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->IDEX.instr);
    printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
    printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
    printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
    printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->EXMEM.instr);
    printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
    printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
    printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
    printf("\tMEMWB:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->MEMWB.instr);
    printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->WBEND.instr);
    printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int
field0(int instruction)
{
    return( (instruction>>19) & 0x7);
}

int
field1(int instruction)
{
    return( (instruction>>16) & 0x7);
}

int
field2(int instruction)
{
    return(instruction & 0xFFFF);
}

int
opcode(int instruction)
{
    return(instruction>>22);
}

void
printInstruction(int instr)
{
    char opcodeString[10];
    if (opcode(instr) == ADD) {
        strcpy(opcodeString, "add");
    } else if (opcode(instr) == NOR) {
        strcpy(opcodeString, "nor");
    } else if (opcode(instr) == LW) {
        strcpy(opcodeString, "lw");
    } else if (opcode(instr) == SW) {
        strcpy(opcodeString, "sw");
    } else if (opcode(instr) == BEQ) {
        strcpy(opcodeString, "beq");
    } else if (opcode(instr) == JALR) {
        strcpy(opcodeString, "jalr");
    } else if (opcode(instr) == HALT) {
        strcpy(opcodeString, "halt");
    } else if (opcode(instr) == NOOP) {
        strcpy(opcodeString, "noop");
    } else {
        strcpy(opcodeString, "data");
    }
    printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
    field2(instr));
}
