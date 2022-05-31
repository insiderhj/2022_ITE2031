#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAXLINELENGTH 1000
#define LABELNAME 7
#define MAXLABELS 60

struct label {

    int used;
    char name[LABELNAME];
    int line;
};

int initLabels();
int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int isInt(char *);
int isBetween(char *, int, int);
int getOpcode(char *);
int addLabel(char *name, int line);
struct label getLabel(char *name);

struct label labels[MAXLABELS];

int main(int argc, char *argv[])
{    
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    
    int parseRes = 0;
    int num0, num1, num2;
    int opcodeNum;

    int pc = -1;
    int dest, offset;
    char resultStr[12];
    struct label tempLabel;

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];
    inFilePtr = fopen(inFileString, "r");

    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        
        pc++;
        if (!strlen(label)) {
            continue;
        }
        if (isNumber(label)) {
            printf("error: label starts with number\n");
            exit(1);
        }
        if (strlen(label) > 6) {
            printf("error: label length too long\n");
            exit(1);
        }
        if (addLabel(label, pc)) {
            printf("error: duplicate label name\n");
            exit(1);
        }
    }

    rewind(inFilePtr);

    pc = -1;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        
        pc++;
        opcodeNum = getOpcode(opcode);
        switch(opcodeNum) {
        //add, nor
        case 0:
        case 1:
            if (!isBetween(arg0, 0, 7) || !isBetween(arg1, 0, 7) || !isBetween(arg2, 0, 7)) {
                printf("error: invalid argument\n");
                exit(1);
            }
            parseRes = opcodeNum << 22 | atoi(arg0) << 19 | atoi(arg1) << 16 | atoi(arg2);
            break;

        //lw, sw
        case 2:
        case 3:
            if (!isBetween(arg0, 0, 7) || !isBetween(arg1, 0, 7)) {
                printf("error: invalid argument\n");
                exit(1);
            }
            
            if (!isInt(arg2)) {
                tempLabel = getLabel(arg2);
                if (!tempLabel.used) {
                    printf("error: undefined label\n");
                    exit(1);
                }

                offset = tempLabel.line;
            } else {
                if (!isBetween(arg2, -32768, 32767)) {
                    printf("error: invalid argument\n");
                    exit(1);
                }
                offset = atoi(arg2);
            }

            parseRes = opcodeNum << 22 | atoi(arg0) << 19 | atoi(arg1) << 16 | offset & 65535;
            break;
        
        //beq
        case 4:
            if (!isBetween(arg0, 0, 7) || !isBetween(arg1, 0, 7)) {
                printf("error: invalid argument\n");
                exit(1);
            }
            if (!isInt(arg2)) {
                tempLabel = getLabel(arg2);
                if (!tempLabel.used) {
                    printf("error: undefined label\n");
                    exit(1);
                }

                dest = tempLabel.line;
                offset = dest - pc - 1;
            } else {
                if (!isBetween(arg2, -32768, 32767)) {
                    printf("error: invalid argument\n");
                    exit(1);
                }
                offset = atoi(arg2);
            }

            parseRes = opcodeNum << 22 | atoi(arg0) << 19 | atoi(arg1) << 16 | offset & 65535;
            break;

        //jalr
        case 5:
            if (!isInt(arg0) || !isInt(arg1)) {
                printf("error: argument is not a number\n");
                exit(1);
            }
            parseRes = opcodeNum << 22 | atoi(arg0) << 19 | atoi(arg1) << 16;
            break;

        //halt, noop
        case 6:
        case 7:
            parseRes = opcodeNum << 22;
            break;

        case 8:
            if (!isInt(arg0)) {
                tempLabel = getLabel(arg0);
                if (!tempLabel.used) {
                    printf("error: undefined label\n");
                    exit(1);
                }
                parseRes = tempLabel.line;
            } else parseRes = atoi(arg0);
            break;
        //else
        default:
            printf("error: invalid opcode\n");
            exit(1);
        }

        fprintf(outFilePtr, "%d\n", parseRes);
    }
    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
    return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
        char *arg1, char *arg2)
{    
    char line[MAXLINELENGTH];
    char *ptr = line;
    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
        /* reached end of file */
        return(0);
    }
    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
        printf("error: line too long\n");
        exit(1);
    }
    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n\r ]", label)) {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }
    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);
    return(1);
}

int isNumber(char *string)
{    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

int isInt(char *string)
{
    char string2[11];
    sprintf(string2, "%d", atoi(string));
    return(!strcmp(string, string2));
}

int isBetween(char *string, int min, int max) {

    if (!isInt(string)) return 0;
    return atoi(string) >= min && atoi(string) <= max;
}

int getOpcode(char *opcode) {

    if (!strcmp(opcode, "add")) return 0;
    if (!strcmp(opcode, "nor")) return 1;
    if (!strcmp(opcode, "lw")) return 2;
    if (!strcmp(opcode, "sw")) return 3;
    if (!strcmp(opcode, "beq")) return 4;
    if (!strcmp(opcode, "jalr")) return 5;
    if (!strcmp(opcode, "halt")) return 6;
    if (!strcmp(opcode, "noop")) return 7;
    if (!strcmp(opcode, ".fill")) return 8;
    return -1;
}

int addLabel(char *name, int line) {
    for (int i = 0; i < MAXLABELS; i++) {
        if (labels[i].used && !strcmp(labels[i].name, name)) {
            return -1;
        }
        if (!labels[i].used) {
            
            labels[i].used = 1;
            strcpy(labels[i].name, name);
            labels[i].line = line;
            return 0;
        }
    }
}

struct label getLabel(char *name) {

    struct label res;
    res.used = 0;
    for (int i = 0; i < MAXLABELS; i++) {
        if (!labels[i].used)  return res;
        if (!strcmp(labels[i].name, name)) {
            res.used = 1;
            strcpy(res.name, name);
            res.line = labels[i].line;
            return res;
        }
    }
}
