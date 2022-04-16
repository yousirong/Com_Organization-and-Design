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