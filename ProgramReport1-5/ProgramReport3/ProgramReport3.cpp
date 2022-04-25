/*
2. Binary file (xxx.bin) 파일에서 명령어를 읽어 메모리에 저장하고 각각이 어떤 명령어인지를 하나씩 해석하는 프로그램을 작성한다.
이진 파일에 대해 이해하고 파일 접근 라이브러리 함수를 이해한다.
프로그램의 메모리 로드에 대해 이해한다.
Little-endian과 Big-endian에 대해 이해한다.
명령어 코딩 포맷에 대해 이해한다.

프로그램 작성을 위한 필요 지식
이진 파일 및 실행 이미지 파일 구조 이해
이진 파일에 대한 이해는 과제2를 참조, 실행 이미지 구조는 뒤에서 설명
메모리 정렬(Alignment)과 Endian 이해
2주차 강의와 과제 참조
C로 메모리 모델링하는 방법 이해
뒤에서 설명
명령어 코딩에 대한 이해
2주차 강의 자료


C로 메모리를 모델링하기 위하여 unsinged char의 배열로 선언한다.
메모리의 논리적 형태를 모델링
Ex) unsigned char MEM[M_SIZE]; 2^10이므로 10bit쓰고 나머지는 0000으로 저장
M_SIZE: 제공되는 input file의 명령어를 모두 저장할 수 있게 크게 한다.
ex) const int M_SIZE = 1024;
배열의 인덱스가 MEM을 접근하기 위한 주소가 된다.
메모리를 word 단위로 읽고 쓰는 함수를 작성한다: 물리적 메모리 접근 모델링
unsigned int memoryRead(unsigned int addr);
void memoryWrite(unsigned int addr, unsigned int data);
각 함수에서 입력 받은 addr이 aligned access 인지 검사하고 aligned access가 아닐 경우에는 오류 메시지를 출력하고 addr을 aligned access가 되도록 주소를 교정하여 MEM을 access한다.
# 4의배수인지 아닌지
addr이 4의 배수가 되도록 주소를 truncate 시킨다: & 와 상수 사용
MEM 상에서 4-byte 데이터는 big-endian으로 저장되어 접근되어야 한다.
Ex) 0xAABBCCDD 가 메모리 8번지에 저장된다면
MEM[8]: 0xAA, MEM[9]: 0xBB, MEM[10]: 0xCC, MEM[11]: 0xDD 가 저장

*/

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
