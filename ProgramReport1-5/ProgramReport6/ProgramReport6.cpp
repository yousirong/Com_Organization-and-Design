/*
MIPS processor has 32 bits address bus  4 GiB address space
Aligned and Big-endian
Implement a memory system for MIPS processor
Assume the physical memory has 3 MiB totally
Text: 1 MiB (0x00400000~0x004FFFFF)
Data: 1 MiB (0x10000000~0x100FFFFF)
Stack: 1 MiB (0x7FF00000~0x7FFFFFFF)
Write a program that simulate the physical memory
int MEM(unsigned int A, int V, int nRW, int S);
32bit  nrw 1bitsignal 1 invert 0 active
A: memory address
V: write value
nRW: 0 Read, 1 Write
S(Size): 0Byte, 1Half word, 2 Word
3 static char arrays of 1 MiB internally
Each array corresponds to each physical memory
Using MEM() with address, each S size data can be accessed
When Read: ignore V, use return data
When Write: V is written to address, ignore return

메모리 동작
바이트 단위로 주소를 가지고 있으며 메모리는 물리적으로 4 개의 바이트 단위로 접근된다.


구현에 있어 내부적으로 주소를 변환해서 접근함
워드 접근: A & 0xffff_fffc 로 새로 주소 구함
반워드 접근: A & 0xffff_fffe 로 주소 변환
바이트: A 사용
메모리는 물리적으로 하위 두 비트는 사용 안함
반워드/바이트 경우 프로세서가 하위 비트 사용
MIPS는 aligned access를 하기 때문에
lw/sw에서 만들어 지는 주소: 하위 두 비트가00 임
lh/sh에서 만들어 지는 주소: 하위 한 비트가0 임

Read/Write의 각 경우를 검증할 수 있는 검사 프로그램을 만들어  MEM()을 검증한다.
lb/lbu/sb, lh/lhu/sh, lw/sw
memory boundary access
0x0040000, 0x004FFFFF
0x1000000, 0x100FFFFF
0x7FF0000, 0x7FFFFFFF

*/

// ProgramReport6.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "stdio.h"
#include "stdlib.h"

#define PROG_START	0x00400000
#define PROG_END	0x00500000
#define DATA_START	0x10000000
#define DATA_END	0x10100000
#define STAK_START	0x7FF00000
#define STAK_END	0x80000000

enum accessType { RD, WR };
enum accessSize { BYTE, HALF, WORD };

const int M_SIZE = 0x100000;
unsigned char progMEM[M_SIZE], dataMEM[M_SIZE], stakMEM[M_SIZE];

int MEM(unsigned int A, int V, int nRW, int S) {

	unsigned int memSelect, offset; // A = memSelect << 20 + offset
	unsigned char* pM;
	//	unsigned char cdata;
	//	unsigned short int sdata;
	//	unsigned int idata;

	if (nRW > 1 || S > 2) {
		printf("Error: Invalid range of nRW or S"); fflush(stdout);
		exit(1);
	}

	memSelect = A >> 20;
	offset = A & 0xFFFFF;
	if (memSelect == 0x004) pM = progMEM;         // program memory
	else if (memSelect == 0x100) pM = dataMEM;  // data memory
	else if (memSelect == 0x7FF) pM = stakMEM;  // stack
	else {
		printf("Error: No memory with address: %08x\n", A); fflush(stdout);
		exit(1);
	}

	if (S == 0) {  // byte
		if (nRW == 0) {	// read
			return pM[offset];
		}
		else {	// write
			pM[offset] = (unsigned char)V;
			return 1;
		}
	}
	else if (S == 1) { // half word
		offset = offset & 0xfffffffe;	// for aligned access
		if (nRW == 0) {	// read
			return (pM[offset] << 8) + pM[offset + 1];
		}
		else {	// write
			pM[offset] = (unsigned char)((V >> 8) & 0xff);
			pM[offset + 1] = (unsigned char)(V & 0xff);
			return 1;
		}
	}
	else if (S == 2) { // word
		offset = offset & 0xfffffffc;	// for aligned access
		if (nRW == 0) {	// read
			return (pM[offset] << 24) + (pM[offset + 1] << 16) + (pM[offset + 2] << 8) + pM[offset + 3];
		}
		else {	// write
			pM[offset] = (unsigned char)((V >> 24) & 0xff);
			pM[offset + 1] = (unsigned char)((V >> 16) & 0xff);
			pM[offset + 2] = (unsigned char)((V >> 8) & 0xff);
			pM[offset + 3] = (unsigned char)(V & 0xff);
			return 1;
		}
	}
	return 0;
}

void resetMem(void)
{
	int i;
	for (i = 0; i < M_SIZE; i++) {
		progMEM[i] = 0;
		dataMEM[i] = 0;
		stakMEM[i] = 0;
	}
}

int main() {
	unsigned int addr;
	unsigned int val;

	int ret;

	resetMem();	// reset memory to all zero

	// program memory test
	val = 0xAABBCCDD;
	addr = PROG_START;

	ret = MEM(addr, val, WR, BYTE);
	printf("PROG BYTE WR: A=%08x, %02x\n", addr, val);
	ret = MEM(addr, val, RD, BYTE); // 0xdd
	printf("PROG BYTE RD: A=%08x, %02x\n", addr, ret);

	ret = MEM(addr, val, WR, HALF);
	printf("PROG HALF WR: A=%08x, %02x\n", addr, val);
	ret = MEM(addr, val, RD, HALF); // 0xccdd
	printf("PROG HALF RD: A=%08x, %04x\n", addr, ret);

	ret = MEM(addr, val, RD, BYTE); // 0xcc
	printf("PROG BYTE RD: A=%08x, %02x\n", addr, val);
	ret = MEM(addr + 1, val, RD, BYTE); // 0xdd
	printf("PROG BYTE RD: A=%08x, %02x\n", addr + 1, ret);

	ret = MEM(addr, val, WR, WORD);
	printf("PROG WORD WR: A=%08x, %08x\n", addr, val);
	ret = MEM(addr, val, RD, WORD); // 0xaabbccdd
	printf("PROG WORD RD: A=%08x, %08x\n", addr, ret);

	addr = PROG_END - 1;

	ret = MEM(addr, val, WR, BYTE);
	printf("PROG BYTE WR: A=%08x, %02x\n", addr, val);
	ret = MEM(addr, val, RD, BYTE); // 0xdd
	printf("PROG BYTE RD: A=%08x, %02x\n", addr, ret);

	addr = PROG_END - 2;

	ret = MEM(addr, val, WR, HALF);
	printf("PROG HALF WR: A=%08x, %02x\n", addr, val);
	ret = MEM(addr, val, RD, HALF); // 0xccdd
	printf("PROG HALF RD: A=%08x, %04x\n", addr, ret);

	addr = PROG_END - 4;

	ret = MEM(addr, val, WR, WORD);
	printf("PROG WORD WR: A=%08x, %08x\n", addr, val);
	ret = MEM(addr, val, RD, WORD); // 0xaabbccdd
	printf("PROG WORD RD: A=%08x, %08x\n", addr, ret);

	// data memory test
	val = 0xAABBCCDD;
	addr = DATA_START;

	ret = MEM(addr, val, WR, BYTE);
	printf("\nDATA BYTE WR: A=%08x, %02x\n", addr, val);
	ret = MEM(addr, val, RD, BYTE); // 0xdd
	printf("DATA BYTE RD: A=%08x, %02x\n", addr, ret);

	ret = MEM(addr, val, WR, HALF);
	printf("DATA HALF WR: A=%08x, %02x\n", addr, val);
	ret = MEM(addr, val, RD, HALF); // 0xccdd
	printf("DATA HALF RD: A=%08x, %04x\n", addr, ret);

	addr = DATA_END - 1;

	ret = MEM(addr, val, WR, BYTE);
	printf("DATA BYTE WR: A=%08x, %02x\n", addr, val);
	ret = MEM(addr, val, RD, BYTE); // 0xdd
	printf("DATA BYTE RD: A=%08x, %02x\n", addr, ret);

	// stack memory test
	val = 0xAABBCCDD;
	addr = STAK_START;

	ret = MEM(addr, val, WR, BYTE);
	printf("\nSTAK BYTE WR: A=%08x, %02x\n", addr, val);
	ret = MEM(addr, val, RD, BYTE); // 0xdd
	printf("STAK BYTE RD: A=%08x, %02x\n", addr, ret);

	ret = MEM(addr, val, WR, HALF);
	printf("STAK HALF WR: A=%08x, %02x\n", addr, val);
	ret = MEM(addr, val, RD, HALF); // 0xccdd
	printf("STAK HALF RD: A=%08x, %04x\n", addr, ret);

	addr = STAK_END - 1;

	ret = MEM(addr, val, WR, BYTE);
	printf("STAK BYTE WR: A=%08x, %02x\n", addr, val);
	ret = MEM(addr, val, RD, BYTE); // 0xdd
	printf("STAK BYTE RD: A=%08x, %02x\n", addr, ret);

	return 0;
}
