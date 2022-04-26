/*
두개의 정수를 받아 연산자를 테스트하는 함수를 작성한다.
void testOperator(int x, int y)
인수로 받은 두 정수를 hexa 값으로 표시하고  논리 연산과 shift 연산을 수행하여 그 결과 값을 표시한다.
main 함수에서 정수 데이터 x, y를 선언한다.
5번의 루프를 돌면서 x, y를 random 값으로 설정한다.
설정한 값으로 testOperator(x, y)를 호출한다.

*/

#include <stdio.h>
#include <stdlib.h>

// To make a 32-bit random number,
// call rand() four times cause rand() generates a random number from 0 to 2^15-1
unsigned int rand32bit() {
	unsigned int t;
	t = rand() % 256;	// make 0 <= t < 256
	t = (t << 8) | (rand() % 256); // 16 bit random number
	t = (t << 8) | (rand() % 256); // 24 bit random number
	t = (t << 8) | (rand() % 256); // 32 bit random number
	return t;
}

// Print an 32-bit unsigned integer to the binary representation
void printBinary(unsigned int d) {
	for (int i = 0; i < 32; i++, d <<= 1)
		if (d & 0x80000000) printf("1");
		else        		printf("0");
}

void printResults(const char* s, unsigned int d) {
	printf("%s %08X ", s, d); printBinary(d); printf("\n");
}

void testOperator(unsigned int x, unsigned int y) {
	printResults("Input x:", x);
	printResults("Input y:", y);

	// logic operator
	printResults("    x&y:", x & y);
	printResults("    x|y:", x | y);
	printResults("    x^y:", x ^ y);
	printResults("     ~x:", ~x);
	printResults("     ~y:", ~y);

	// shift operator
	printResults("   x<<4:", x << 4);
	printResults("   y<<4:", y << 4);
	printResults("   x>>4:", x >> 4);
	printResults("   y>>4:", y >> 4);
}

int main() {
	unsigned int x, y;

	for (int i = 0; i < 5; i++) {
		x = rand32bit();
		y = rand32bit();
		testOperator(x, y);
		printf("\n");
	}
}
