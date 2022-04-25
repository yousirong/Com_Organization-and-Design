/*
CPU의 ALU를 시뮬레이션 하는 ALU 함수를 프로그램으로 구현한다.
ALU 동작
Arithmetic + Logic + Shift 연산에 대한 결과를 출력
피연산자(operand)와 연산자(operator)를 입력 받아 해당 연산의 결과 출력
연산 결과에 대한 상태 정보(N, Z, C, V)를 출력
ALU 함수 구현: int ALU(int X, int Y, int S, int *Z);
Input: 32-bit X, Y, 4-bit S(selection)
입력은 두개의 피연산자 데이터와 연산자를 나타내는 제어 값으로 구성
입력은 32비트 정수값 X, Y, 그리고 제어를 위한 S는 4 비트이지만 정수로 구현
Output: 32-bit O, 1-bit Z(ero)
연산자 종류에 따른 출력 값과 add/sub 연산 결과가 0인지를 나타내는 Z 값
Z = 1 if O == 0, Z = 0 else
HW는 1 비트이지만 32비트에 저장
연산 결과 값 O는 ALU 함수에서 정수로 반환하고 Z 값은 포인터를 이용하여 반환

구현 절차
ALU 하드웨어 구성과 유사하게 모듈 단위로 함수를 구현하고 전체를 구현
int logicOperation(int X, int Y, int s1s0);
논리 연산을 담당하는 함수 if S3S2 == 0b11
s1s0에 따라서 AND, OR, XOR, NOR
int shiftOperation(int X, int Y, int s1s0);
시프트 연산을 담당하는 함수 if S3S2 == 0b00
s1s0에 따라서 No shift, SLL, SRL, SRA
int addSubtract(int X, int Y, int s0);
산술연산을 담당하는 함수 if S3S2 == 0b10 or S3S2 == 0b01
s0에 따라서 ADD, SUB
int checkSetLess (int X, int Y);
less than을 검사하는 함수 if S3S2 == 0b01
LT(<) = 1 if X < Y: addSubtract() 함수를 이용하여 빼기를 수행하고 반환값의 MSB를 반환한다.
int checkZero(int Oa);
Add/Sub 모듈의 연산 결과가 0인지 검사하는 함수
ZERO = 1 if Oa == 0, else ZERO = 0
구현된 모듈 함수들을 이용하여 ALU 전체 함수를 구현한다.
int ALU(int X, int Y, int S, int *Z);

*/

// ProgramReport5.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "stdio.h"
//#include "defines.h"

// Arithmetic unit
int addSubtract(int X, int Y, int C) {
	if (C < 0 || C > 1) {
		printf("error in addSubtract()...\n");
		return 0;
	}
	if (C == 0)
		return X + Y;
	else
		return X - Y;
}

// Logic unit
int logicOperation(int X, int Y, int C)
{
	if (C < 0 || C > 3) {
		printf("error in logicOperation()...\n");
		return 0;
	}

	if (C == 0)
		return X & Y;	// bitwise-AND
	else if (C == 1)
		return X | Y;   // bitwise-OR
	else if (C == 2)
		return X ^ Y;   // bitwise-XOR
	else
		return ~(X | Y); // bitwise-NOR
}

// Shift unit
// V: 5 bit shift amount
int shiftOperation(int V, int Y, int C)
{
	int ret;

	if (C < 0 || C > 3 || V > 31) {
		printf("error in shiftOperation()...\n");
		return 0;
	}

	if (C == 0) {	// No shift
		ret = Y;
	}
	else if (C == 1) {	// Logical left
		ret = Y << V;
	}
	else if (C == 2) {	// Logical right
		ret = (unsigned int)Y >> V;
	}
	else {	// Arithmetic right
		ret = Y >> V;
	}

	return ret;
}

int checkSetLess(int X, int Y)
{
	if (X < Y) return 1;
	else return 0;
}

int checkZero(int S)
{
	if (S == 0) return 1;
	else return 0;
}

// ALU

int ALU(int X, int Y, int C, int* Z) {
	int c32, c10;
	int ret;

	c32 = (C >> 2) & 3;	// 3, 2 bit of Control input
	c10 = C & 3;		// 1, 0 bit of Control input

	if (c32 == 0) { // shift

		ret = shiftOperation(X, Y, c10);
	}
	else if (c32 == 1) {  // set less
		ret = checkSetLess(X, Y);
	}
	else if (c32 == 2) {  // addsubtract
		ret = addSubtract(X, Y, c10 & 1);
		*Z = checkZero(ret);
	}
	else {  // logic
		ret = logicOperation(X, Y, c10);
	}

	return ret;
}

int main() {
	int i, x, y, s, z = 0;

	// Shift test
	x = 0x6;
	y = 0xAAAAAAAA;
	printf("Shift test ->  X: %08X, Y: %08X\n", x, y);

	for (i = 0; i < 4; i++) {
		s = ALU(x, y, i, &z);
		printf("S: %08X, Z: %08X\n", s, z);
	}

	// Set less test
	x = 0x55555555;
	y = 0xAAAAAAAA;
	printf("\nSet less test -> X: %08X, Y: %08X\n", x, y);
	s = ALU(x, y, 4, &z);
	printf("X<Y: %08X, Z: %08X\n", s, z);
	s = ALU(y, x, 4, &z);
	printf("Y<X: %08X, Z: %08X\n", s, z);

	// Arithmetic test
	x = 0xAAAAAAAA;
	y = 0xAAAAAAAA;
	printf("\nArithmetic test -> X: %08X, Y: %08X\n", x, y);
	s = ALU(x, y, 8, &z); // add
	printf("X+Y: %08X, Z: %08X\n", s, z);
	s = ALU(x, y, 9, &z); // sub
	printf("X-Y: %08X, Z: %08X\n", s, z);

	x = 0x55555555;
	y = 0x33333333;
	printf("\nArithmetic test -> X: %08X, Y: %08X\n", x, y);
	s = ALU(x, y, 8, &z); // add
	printf("X+Y: %08X, Z: %08X\n", s, z);
	s = ALU(x, y, 9, &z); // sub
	printf("X-Y: %08X, Z: %08X\n", s, z);

	// Logic test
	x = 0x5555AAAA;
	y = 0xAAAAAAAA;
	printf("\nLogic test -> X: %08X, Y: %08X\n", x, y);

	for (i = 12; i < 16; i++) {
		s = ALU(x, y, i, &z);
		printf("S: %08X, Z: %0X\n", s, z);
	}

	return 0;
}
