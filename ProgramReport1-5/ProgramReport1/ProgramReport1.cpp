
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
