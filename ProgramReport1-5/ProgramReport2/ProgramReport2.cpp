/*
프로그램 big-endian 형태로 데이터가 저장된 binary file (이진파일)에서 4-byte 정수들을 읽고 이를 little-endian 형태로 파일에 저장하는 프로그램을 작성한다.
이진 파일에 대해 이해하고 파일 접근 라이브러리 함수를 이해한다.
Little-endian과 Big-endian에 대해 이해한다.
방법
과제를 위하여 주어진 이진 파일을 사용한다. (input.bin)
입력 파일에는 4-byte 데이터가 모두 19개 있다.
입력파일을 읽기 위하여 open하고 결과를 저장할 출력파일(output.bin)을 쓰기 위하여 새로 만들며 open한다.
4-byte 정수를 입력 받아서 endian이 바뀐 데이터를 반환하는 함수를 작성한다.
unsigned int invertEndian(unsigned int inputValue)
bitwise 논리 연산자 (&, |, ^, ~)와 shift 연산자 (>>, <<)를 이용하여 구현
입력 이진파일에서 4-byte 데이터를 모두 읽고 Endian을 바꾸어 출력파일에 모두 저장하고 파일을 닫으며 종료한다.
출력파일에 저장할 19개의 데이터를 모두 XOR하여 그 값을 Hexa로 출력한다.

*/

// ProgramReport2.cpp :
//

#include <stdio.h>
#include <iostream>
#include <errno.h>
using namespace std;

unsigned int invertEndian(unsigned int data)
{
	unsigned char c[4];

	c[3] = (unsigned char)data; data >>= 8;
	c[2] = (unsigned char)data; data >>= 8;
	c[1] = (unsigned char)data; data >>= 8;
	c[0] = (unsigned char)data;

	return *(unsigned int*)c;
}

int main()
{
	FILE* pInFile = fopen("input.bin", "rb");;  // input file pointer
	FILE* pOutFile = fopen("output.bin","wb"); // output file pointer
	// errno_t err;

	unsigned int inData, outData;
	unsigned int valueEOR = 0;

	// open a bianry file to read its contents
	// err = fopen_s(&pInFile, "input.bin", "rb");
	if (pInFile==NULL) {
		printf("Cannot open input file\n");
		return 1;
	}
	// open a bianry file to write the inverted endian contents
	// err = fopen_s(&pOutFile, "output.bin", "wb");
	if (pOutFile==NULL) {
		printf("Cannot open output file\n");
		return 1;
	}

	while (1) {
		int count = fread(&inData, sizeof(inData), 1, pInFile); // read 4 byte data
		if (count != 1) // if cannot read integer anymore, break
			break;
		outData = invertEndian(inData);
		fwrite(&outData, sizeof(unsigned int), 1, pOutFile);

		valueEOR ^= outData;	// valueEOR was initialzed to zero, then the first ^ produce the same data
		printf("%08X %08X\n", inData, outData);
	}

	printf("The EOR value for output file is %08X\n", valueEOR);

	fclose(pInFile);
	fclose(pOutFile);

	return 0;
}