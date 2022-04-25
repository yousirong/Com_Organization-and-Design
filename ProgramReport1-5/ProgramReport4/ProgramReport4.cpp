/*
4. Executable binary file (xxx.bin) 파일에서 명령어를 읽어 메모리에 저장하고 각 명령어를 해석하여 어셈블리어로 출력한다.
MIPS 명령어 코딩 포맷 (R, I, J)에 대해 상세하게 이해한다.
Binary file format은 Report3과 동일하다.
Big-endian, 첫 4Byte는 명령어 개수, 다음 4Byte는 데이터 개수
데이터는 무시하고 명령어만 메모리에 로드하여 사용
메모리에 저장되어 있는 명령어를 하나씩 읽어서 명령어를 해석하여 출력한다.
Report3에서 작성한 코드에 완전한 형태의 명령어 출력이 되도록 코드 추가한다.
C의 union과 bit field를 쓰면 명령어 각 field 추출이 편리해짐. (뒤에 sample)
출력되는 명령어는 어셈블리어 형태를 가져야 한다: Disassemble
프로그램의 검증은 주어진 어셈블리 프로그램을 이용한다. (xxx.s)
의사 명령어로 인하여 출력되는 명령어들과 다를 수 있다.
QtSpim에 어셈블리 프로그램을 load하면 실제 명령어들을 확인할 수 있다.
주어진 4개의 테스트 파일로 프로그램을 검증한다.
다음을 하나의 파일(B학번성명.xxx)로 만들어 제출한다.
소스 프로그램, 4개의 출력 파일

*/


// ProgramReport4.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "stdio.h"
#include "defines.h"
#include <iostream>
#include "disassemble.cpp"
extern void printShift(unsigned int op, unsigned int rd, unsigned int rt, unsigned int sht);
extern void printJr(unsigned int op, unsigned int rs);
extern void printSyscall(unsigned int op);
extern void printMfHiLo(unsigned int op, unsigned int rd);
extern void printMul(unsigned int op, unsigned int rs, unsigned int rt);
extern void printALU(unsigned int op, unsigned int rd, unsigned int rs, unsigned int rt);
extern void printALUI(unsigned int op, unsigned int rt, unsigned int rs, unsigned int immediate);
extern void printBranch(unsigned int op, unsigned int rs, unsigned int rt, int offset);
extern void printJump(unsigned int op, unsigned int offset);
extern void printLoadStore(unsigned int op, unsigned int rt, unsigned int rs, int offset);

union instructionRegister IR;

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

	data = *(unsigned int*)& MEM[addr];
	data = invertEndian(data);

	return data;
}

void memoryWrite(unsigned int addr, unsigned int data) {
	if ((addr & 3) != 0) { // check unaligned access?
		printf("memoryWrite(): Unaligned access error!\n");
		addr &= 0xFFFFFFFC; // ignore the least 2 bits
	}

	*(unsigned int*)& MEM[addr] = invertEndian(data);

	return;
}


// R-type instructin decoding
void decodeRtype(unsigned int fct)
{
	unsigned int fcth, fctl;

	fctl = fct & 0x7;
	fcth = (fct & 0x38) >> 3;

	if (fcth == 0) {
		if (fctl == 0)      {printShift(SLL, IR.RI.rd, IR.RI.rt, IR.RI.sht);}
		else if (fctl == 2){ printShift(SRL, IR.RI.rd, IR.RI.rt, IR.RI.sht);}
		else if (fctl == 3) {printShift(SRA, IR.RI.rd, IR.RI.rt, IR.RI.sht);}
		else {
			printf("Undefined instruction\n");
			return;
		}
	}
	else if (fcth == 1) {
		if (fctl == 0)      printJr(JR, IR.RI.rs);
		else if (fctl == 4) printSyscall(SYSCALL);
		else printf("Undefined instruction\n");
	}
	else if (fcth == 2) {
		if (fctl == 0)      printMfHiLo(MFHI, IR.RI.rd);
		else if (fctl == 2) printMfHiLo(MFLO, IR.RI.rd);
		else printf("Undefined instruction\n");
	}
	else if (fcth == 3) {
		if (fctl == 0)      printMul(MUL, IR.RI.rs, IR.RI.rt);
		else printf("Undefined instruction\n");
	}
	else if (fcth == 4) {
		if (fctl == 0)      printALU(ADD, IR.RI.rd, IR.RI.rs, IR.RI.rt);
		else if (fctl == 2) printALU(SUB, IR.RI.rd, IR.RI.rs, IR.RI.rt);
		else if (fctl == 4) printALU(AND, IR.RI.rd, IR.RI.rs, IR.RI.rt);
		else if (fctl == 5) printALU(OR, IR.RI.rd, IR.RI.rs, IR.RI.rt);
		else if (fctl == 6) printALU(XOR, IR.RI.rd, IR.RI.rs, IR.RI.rt);
		else if (fctl == 7) printALU(NOR, IR.RI.rd, IR.RI.rs, IR.RI.rt);
		else printf("Undefined instruction\n");
	}
	else if (fcth == 5) {
		if (fctl == 2)      printALU(SLT, IR.RI.rd, IR.RI.rs, IR.RI.rt);
		else printf("Undefined instruction\n");
	}
	else printf("Undefined instruction\n");
}

// instruction decoding
void instructionDecode(void)
{
	unsigned int opc, fct;

	unsigned int opch, opcl;

	opc = IR.RI.opc;
	fct = IR.RI.fct;

	printf("Opc: %2x, Fct: %2x, Inst: ", opc, fct);

	opcl = opc & 0x7;
	opch = (opc & 0x38) >> 3;

	if (opch == 0) {
		if (opcl == 0) {

			decodeRtype(fct);
		}
		else if (opcl == 1) printBranch(BLTZ, IR.II.rs, IR.II.rt, IR.II.offset);
		else if (opcl == 2) printJump(J, IR.JI.jval);
		else if (opcl == 3) printJump(JAL, IR.JI.jval);
		else if (opcl == 4) printBranch(BEQ, IR.II.rs, IR.II.rt, IR.II.offset);
		else if (opcl == 5) printBranch(BNE, IR.II.rs, IR.II.rt, IR.II.offset);
		else printf("Undefined instruction\n");
	}
	else if (opch == 1) {
		if (opcl == 0)      printALUI(ADDI, IR.II.rt, IR.II.rs, IR.II.offset);
		else if (opcl == 2) printALUI(SLTI, IR.II.rt, IR.II.rs, IR.II.offset);
		else if (opcl == 4) printALUI(ANDI, IR.II.rt, IR.II.rs, IR.II.offset);
		else if (opcl == 5) printALUI(ORI, IR.II.rt, IR.II.rs, IR.II.offset);
		else if (opcl == 6) printALUI(XORI, IR.II.rt, IR.II.rs, IR.II.offset);
		else if (opcl == 7) printALUI(LUI, IR.II.rt, IR.II.rs, IR.II.offset);
		else printf("Undefined instruction\n");
	}
	else if (opch == 4) {
		if (opcl == 0)      printLoadStore(LB, IR.II.rt, IR.II.rs, IR.II.offset);
		else if (opcl == 3) printLoadStore(LW, IR.II.rt, IR.II.rs, IR.II.offset);
		else if (opcl == 4) printLoadStore(LBU, IR.II.rt, IR.II.rs, IR.II.offset);
		else printf("Undefined instruction\n");
	}
	else if (opch == 5) {
		if (opcl == 0)      printLoadStore(SB, IR.II.rt, IR.II.rs, IR.II.offset);
		else if (opcl == 3) printLoadStore(SW, IR.II.rt, IR.II.rs, IR.II.offset);
		else printf("Undefined instruction\n");
	}
	else printf("Undefined instruction\n");
}

int main()
{
	int i;
	// errno_t err;
	//const char* fileName = "as_ex01_arith.bin";
	//const char* fileName = "as_ex02_logic.bin";
	//const char* fileName = "as_ex03_ifelse.bin";
	const char* fileName = "as_ex01_arith.bin";
	FILE* pFile = fopen(fileName, "rb");

	unsigned int data;
	unsigned int addr;

	unsigned int iCount;	// # of instructions
	unsigned int dCount;	// # of data

	// err = fopen_s(&pFile, fileName, "rb");
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
		IR.I = memoryRead(PC); // instruction fetch
		PC += 4;
		instructionDecode(); // instruction decode
	}

	fclose(pFile);

	return 0;
}