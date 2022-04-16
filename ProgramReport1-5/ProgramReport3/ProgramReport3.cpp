// ProgramReport3.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "stdio.h"

unsigned int invertEndian(unsigned int data)
{
	unsigned char c[4];

	c[3] = (unsigned char)data; data = data >> 8;
	c[2] = (unsigned char)data; data = data >> 8;
	c[1] = (unsigned char)data; data = data >> 8;
	c[0] = (unsigned char)data;

	return *(unsigned int*)c;
}

const int M_SIZE = 1024;
unsigned char MEM[M_SIZE];

unsigned int memoryRead(unsigned int addr) {
	unsigned int data;

	if ((addr & 3) != 0) { // check unaligned access?
		printf("memoryRead(): Unaligned access error!\n");
		addr &= 0xFFFFFFFC; // make aligned address
	}
	/*
	data = (unsigned int)MEM[addr];
	data |= (unsigned int)MEM[addr + 1] << 8;
	data |= (unsigned int)MEM[addr + 2] << 16;
	data |= (unsigned int)MEM[addr + 3] << 24;
	*/
	data = *(unsigned int*)&MEM[addr];
	data = invertEndian(data);

	return data;
}

void memoryWrite(unsigned int addr, unsigned int data) {
	if ((addr & 3) != 0) { // check unaligned access?
		printf("memoryWrite(): Unaligned access error!\n");
		addr &= 0xFFFFFFFC; // ignore the least 2 bits
	}
	/*
	MEM[addr + 3] = (unsigned char)data; data >>= 8;
	MEM[addr + 2] = (unsigned char)data; data >>= 8;
	MEM[addr + 1] = (unsigned char)data; data >>= 8;
	MEM[addr] = (unsigned char)data;
	*/
	*(unsigned int*)& MEM[addr] = invertEndian(data);

	return;
}

unsigned int IR;	// Instruction Register

// R-type instructin decoding
void decodeRtype(unsigned int fct)
{
	unsigned int fcth, fctl;

	fctl = fct & 0x7;
	fcth = (fct & 0x38) >> 3;

	if (fcth == 0) {
		if (fctl == 0) printf("sll\n");
		else if (fctl == 2) printf("srl\n");
		else if (fctl == 4) printf("sra\n");
		else printf("Undefined instruction\n");
	}
	else if (fcth == 1) {
		if (fctl == 0) printf("jr\n");
		else if (fctl == 4) printf("syscall\n");
		else printf("Undefined instruction\n");
	}
	else if (fcth == 2) {
		if (fctl == 0) printf("mfhi\n");
		else if (fctl == 2) printf("mflo\n");
		else printf("Undefined instruction\n");
	}
	else if (fcth == 3) {
		if (fctl == 0) printf("mul\n");
		else printf("Undefined instruction\n");
	}
	else if (fcth == 4) {
		if (fctl == 0) printf("add\n");
		else if (fctl == 2) printf("sub\n");
		else if (fctl == 4) printf("and\n");
		else if (fctl == 5) printf("or\n");
		else if (fctl == 6) printf("xor\n");
		else if (fctl == 7) printf("nor\n");
		else printf("Undefined instruction\n");
	}
	else if (fcth == 5) {
		if (fctl == 2) printf("slt\n");
		else printf("Undefined instruction\n");
	}
	else printf("Undefined instruction\n");
}

// instruction decoding
void instructionDecode(void)
{
	unsigned int opc, fct;

	unsigned int opch, opcl;

	opc = IR >> 26;
	fct = IR & 0x3f;

	printf("Opc: %2x, Fct: %2x, Inst: ", opc, fct);

	opcl = opc & 0x7;
	opch = (opc & 0x38) >> 3;

	if (opch == 0) {
		if (opcl == 0) {
			decodeRtype(fct);
		}
		else if (opcl == 1) printf("bltz\n");
		else if (opcl == 2) printf("j\n");
		else if (opcl == 3) printf("jal\n");
		else if (opcl == 4) printf("beq\n");
		else if (opcl == 5) printf("bne\n");
		else printf("Undefined instruction\n");
	}
	else if (opch == 1) {
		if (opcl == 0) printf("addi\n");
		else if (opcl == 2) printf("slti\n");
		else if (opcl == 4) printf("andi\n");
		else if (opcl == 5) printf("ori\n");
		else if (opcl == 6) printf("xori\n");
		else if (opcl == 7) printf("lui\n");
		else printf("Undefined instruction\n");
	}
	else if (opch == 4) {
		if (opcl == 0) printf("lb\n");
		else if (opcl == 3) printf("lw\n");
		else if (opcl == 4) printf("lbu\n");
		else printf("Undefined instruction\n");
	}
	else if (opch == 5) {
		if (opcl == 0) printf("sb\n");
		else if (opcl == 3) printf("sw\n");
		else printf("Undefined instruction\n");
	}
	else printf("Undefined instruction\n");
}

int main()
{
	int i;

	// errno_t err;
	const char* fileName = "as_ex01_arith.bin";
	//const char* fileName = "as_ex02_logic.bin";
	//const char* fileName = "as_ex03_ifelse.bin";
	//const char* fileName = "as_ex04_fct.bin";
    FILE* pFile = fopen(fileName, "rb");
	unsigned int data;
	unsigned int addr;

	unsigned int iCount;	// # of instructions
	unsigned int dCount;	// # of data


	if (pFile==NULL) {
		printf("Cannot open file: %s\n", fileName);
		return 1;
	}

	// read instruction and data numbers
	fread(&data, sizeof(data), 1, pFile);
	iCount = invertEndian(data);
	fread(&data, sizeof(data), 1, pFile);
	dCount = invertEndian(data);
	printf("Number of Instructions: %d, Number of Data: %d\n", iCount, dCount);

	// Load to memory
	addr = 0;
	for (i = 0; i < (int)iCount; i++) {
		fread(&data, sizeof(unsigned int), 1, pFile);
		data = invertEndian(data);
		memoryWrite(addr, data);
		addr += 4;
	}

	// Decoding
	unsigned int PC = 0; // program counter
	for (i = 0; i < (int)iCount; i++) {
		IR = memoryRead(PC); // instruction fetch
		PC += 4;
		instructionDecode(); // instruction decode
	}

	fclose(pFile);

	return 0;
}
