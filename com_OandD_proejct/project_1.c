#include <stdio.h>

void testOperator(int x, int y){

}

int main(void)
{
	int a,b,c;
	a = 0x01;
	b = 0x03;
	c = 0x00;

	c = a&b;

	printf("0x%08x & 0x%08x = 0x%08x \n",a,b,c);

	return 0;
}