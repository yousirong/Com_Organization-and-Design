#include <stdio.h>
static char progMEM[0x100000], dataMEM[0x100000], stackMEM[0x100000];

int MEM(unsigned int A, int V, int nRW, int S)
{
	unsigned int sel, offset;
	char* pM = NULL;
	int returnVal = 0;

	sel = A >> 20;
	printf("Area : 0x%03x", sel);
	offset = A & 0xFFFFF;
// Text: 1 MiB (0x00400000~0x004FFFFF)
	if (sel == 0x004)
	{
		pM = progMEM;
		printf(" -> Text Area");
	}
//Data: 1 MiB (0x10000000~0x100FFFFF)
	else if (sel == 0x100)
	{
		pM = dataMEM;
		printf(" -> Data Area");
	}
//Stack: 1 MiB (0x7FF00000~0x7FFFFFFF)
	else if (sel == 0x7FF)
	{
		pM = stackMEM;
		printf(" -> Stack Area");
	}
// 이외의 메모리는 address bus에 해당 안됨.
	else
	{
		printf(" - No Pyysical memory \n");
		return 0;
	}
// ************S(Size): 0->Byte, 1->Half word, 2-> Word******************
	if (S == 0)
	{
		printf(" | byte(8bits)\n");
		V = V & 0xFF;
	}
	else if (S == 1)
	{
		printf(" | half word(16bits)\n");
		if (A % 2 != 0) printf("경고: 시작 주소가 2의 배수가 아닙니다.\n");
		V = V & 0xFFFF;
	}
	else if (S == 2)
	{
		printf(" | word(32bits)\n");
		if (A % 4 != 0) printf("경고: 시작 주소가 4의 배수가 아닙니다.\n");
		V = V & 0xFFFFFFFF;
	}
	else
	{
		printf("\n Unit size를 다시 설정해주세요. \n");
		return 0;
	}
//nRW: 0-> Read, 1-> Write
	if (nRW == 0)
	{
		printf("Read Data : ");

		if (S == 0)	// Read 8bits
		{
			returnVal = pM[offset] & 0x000000ff;
			printf("%02x / ", (pM[offset] & 0x000000ff));
		}
		else if (S == 1)	// Read 16bits
		{
			printf("%02x %02x / ", pM[offset] & 0x000000ff, pM[offset + 1] & 0x000000ff);
			returnVal = ((pM[offset] << 8) & 0x0000ff00) | (pM[offset + 1] & 0x000000ff);
		}
		else if (S == 2)	// Read 32bits
		{
			printf("%02x ", (pM[offset] & 0x000000ff));
			printf("%02x ", (pM[offset + 1] & 0x000000ff));
			printf("%02x ", (pM[offset + 2] & 0x000000ff));
			printf("%02x / ", (pM[offset + 3] & 0x000000ff));
			returnVal += pM[offset + 3];
			returnVal += (pM[offset + 2] << 8);
			returnVal += (pM[offset + 1] << 16);
			returnVal += (pM[offset] << 24);
		}
		else
		{
			printf("프로그램 오류!\n");
			return 0;
		}
	}
	else if (nRW == 1)
	{
		if (S == 0) // Write 8bits
		{
			printf("Write Data : %02x / ", V);
			pM[offset] = V & 0x000000ff;
		}
		else if (S == 1)	// Write 16bits
		{
			printf("Write Data : ");
			pM[offset] = V >> 8;
			pM[offset + 1] = V & 0x000000ff;
			printf("%02x %02x / ", pM[offset] & 0x000000ff, pM[offset + 1] & 0x000000ff);
		}
		else if (S == 2)	// Write 32bits
		{
			printf("Write Data : ");
			pM[offset] = (V >> 24) & 0x000000ff;
			printf("%02x ", pM[offset] & 0x000000ff);
			pM[offset + 1] = (V >> 16) & 0x000000ff;
			printf("%02x ", pM[offset + 1] & 0x000000ff);
			pM[offset + 2] = (V >> 8) & 0x000000ff;
			printf("%02x ", pM[offset + 2] & 0x000000ff);
			pM[offset + 3] = V & 0x000000ff;
			printf("%02x / ", pM[offset + 3] & 0x000000ff);
		}
		else
		{
			printf("프로그램 오류!\n");
			return 0;
		}
	}
	else
	{
		printf("nRW(0or1) is error \n");
		return 0;
	}

	return returnVal;
}

int main()
{

	// MEM(unsigned int A, int V, int nRW, int S)
	printf("Case1 : %08x | ", 0x00300116);
	printf("Return Value : %08x\n\n", MEM(0x00300116, 0x99, 1, 0));
	printf("Case2 : %08x | ", 0x00400116);
	printf("Return Value : %08x\n\n", MEM(0x00400116, 0x99, 1, 0));
	printf("Case3 : %08x | ", 0x00400116);
	printf("Return Value : %08x\n\n", MEM(0x00400116, 0x00, 0, 0));
	printf("Case4 : %08x | ", 0x00400124);
	printf("Return Value : %08x\n\n", MEM(0x00400124, 0x9876, 1, 1));
	printf("Case5 : %08x | ", 0x00400124);
	printf("Return Value : %08x\n\n", MEM(0x00400124, 0x00, 0, 0));
	printf("Case6 : %08x | ", 0x00400125);
	printf("Return Value : %08x\n\n", MEM(0x00400125, 0x00, 0, 0));
	printf("Case7 : %08x | ", 0x00400124);
	printf("Return Value : %08x\n\n", MEM(0x00400124, 0x00, 0, 1));
	printf("Case8 : %08x | ", 0x00400125);
	printf("Return Value : %08x\n\n", MEM(0x00400125, 0x00, 0, 1));
	printf("Case9 : %08x | ", 0x00400126);
	printf("Return Value : %08x\n\n", MEM(0x00400126, 0x11111111, 1, 2));
	printf("Case10 : %08x | ", 0x00400180);
	printf("Return Value : %08x\n\n", MEM(0x00400180, 0x11111111, 0, 2));
	printf("Case11 : %08x | ", 0x00400180);
	printf("Return Value : %08x\n\n", MEM(0x00400180, 0x11111111, 0, 2));
	printf("Case12 : %08x | ", 0x00400180);
	printf("Return  Value : %08x\n\n", MEM(0x00400180, 0x11111111, 0, 3));
	printf("Case13 : %08x | ", 0x10000360);
	printf("Return Value : %08x\n\n", MEM(0x10000360, 0x98765432, 1, 2));
	printf("Case14 : %08x | ", 0x10000360);
	printf("Return Value : %08x\n\n", MEM(0x10000360, 0x00, 0, 2));
	printf("Case15 : %08x | ", 0x10000360);
	printf("Return Value : %08x\n\n", MEM(0x10000360, 0x00, 0, 2));
	printf("Case16 : %08x | ", 0x7FF00384);
	printf("Return Value : %08x\n\n", MEM(0x7FF00384, 0x00004444, 1, 2));
	printf("Case17 : %08x | ", 0x7FF00384);
	printf("Return Value : %08x\n\n", MEM(0x7FF00384, 0x00, 0, 2));
	printf("Case18 : %08x | ", 0x7FF00404);
	printf("Return Value : %08x\n\n", MEM(0x7FF00404, 0x00, 0, 2));
	printf("Case19 : %08x | ", 0x7FF00504);
	printf("Return Value : %08x\n\n", MEM(0x7FF00504, 0x4444, 1, 1));







	return 0;
}