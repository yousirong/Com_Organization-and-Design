#include "stdio.h"
#include "defines.h"

const char* instName[] = { "nop", "sll", "srl", "sra", "slt", "mfhi", "mflo", "mul",
					"add", "sub", "jr", "syscall", "and", "or", "xor", "nor",
					"lb", "sb", "lw", "sw", "slti", "bltz", "j", "jal",
					"addi", "lbu", "beq", "bne", "andi", "ori", "xori", "lui" };

void printShift(unsigned int op, unsigned int rd, unsigned int rt, unsigned int sht)
{
	printf("%s  $%d, $%d, %d\n", instName[op], rd, rt, sht);
}

void printJr(unsigned int op, unsigned int rs)
{
	printf("%s  $%d\n", instName[op], rs);
}

void printSyscall(unsigned int op)
{
	printf("%s\n", instName[op]);
}

void printMfHiLo(unsigned int op, unsigned int rd)
{
	printf("%s $%d\n", instName[op], rd);
}

void printMul(unsigned int op, unsigned int rs, unsigned int rt)
{
	printf("%s  $%d, $%d\n", instName[op], rs, rt);
}

void printALU(unsigned int op, unsigned int rd, unsigned int rs, unsigned int rt)
{
	printf("%s  $%d, $%d, $%d\n", instName[op], rd, rs, rt);
}

void printALUI(unsigned int op, unsigned int rt, unsigned int rs, unsigned int immediate)
{
	printf("%s  $%d, $%d, %d\n", instName[op], rt, rs, immediate);
}

void printBranch(unsigned int op, unsigned int rs, unsigned int rt, int offset)
{
	if (op == BLTZ)
		printf("%s  $%d, %d\n", instName[op], rs, offset<<2); // print shifted offset
	else
		printf("%s  $%d, $%d, %d\n", instName[op], rs, rt, offset<<2); // print shifted offset
}

void printJump(unsigned int op, unsigned int offset)
{

	printf("%s 0x%08X\n", instName[op], offset<<2);	// print shifted offset
}

void printLoadStore(unsigned int op, unsigned int rt, unsigned int rs, int offset)
{
	printf("%s  $%d, %d($%d)\n", instName[op], rt, offset, rs);
}

