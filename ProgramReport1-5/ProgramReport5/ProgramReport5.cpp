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
