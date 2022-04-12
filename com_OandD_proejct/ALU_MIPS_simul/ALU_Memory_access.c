
//ALU
int logicOperation(int X, int Y, int C)
{
	if (C < 0 || C > 3)
	{
		printf("error in logic operation\n");
		exit(1);
	}

	if (C == 0) //AND
		return X & Y;
	else if (C == 1) //OR
		return X | Y;
	else if (C == 2) //XOR
		return X ^ Y;
	else //NOR
		return ~(X | Y);
}

int addSubtract(int X, int Y, int C)
{
	int ret;

	C %= 2; //c0
	if (C < 0 || C > 1)
	{
		printf("error in add / subract operation\n");
		exit(1);
	}

	if (C == 0) //add
		ret = X + Y;
	else //subtract
		ret = X - Y;

	return ret;
}

int shiftOperation(int V, int Y, int C)
{
	int ret;

	if (C < 0 || C > 3)
	{
		printf("error in shift operation\n");
		exit(1);
	}

	if (C == 0) //No shift
		ret = V;
	else if (C == 1) //Shift left logical
		ret = (unsigned int)V << Y;
	else if (C == 2) //Shift right logical
		ret = (unsigned int)V >> Y;
	else  //Shift right arithmetic
		ret = V >> Y;

	return ret;
}

int checkZero(int S)
{
	return S == 0;
}

int checkSetLess(int X, int Y)
{
	return X < Y;
}

int ALU(int X, int Y, int C, int *Z)
{
	int c32 = (C >> 2) & 3, c10 = C & 3, ret;

	if (c32 == 0) //shift
		ret = shiftOperation(X, Y, c10);
	else if (c32 == 1) //set less
		ret = checkSetLess(X, Y);
	else if (c32 == 2) //add/subtract
	{
		ret = addSubtract(X, Y, c10);
		*Z = checkZero(ret);
	}
	else //logic
		ret = logicOperation(X, Y, c10);

	return ret;
}


//MEMORY ACCESS
unsigned char progMEM[0x100000], dataMEM[0x100000], stakMEM[0x100000];

int MEM(unsigned int A, int V, int nRW, int S)
{
	unsigned int sel, offset;
	unsigned char *pM;
	sel = A >> 20; offset = A & 0xFFFFF;
	if (sel == 0x004) //program memory
		pM = progMEM;
	else if (sel == 0x100) //data memory
		pM = dataMEM;
	else if (sel == 0x7FF) //stack
		pM = stakMEM;
	else
	{
		printf("No memory\n");
		return 1;
	}

	pM += offset; //읽거나 쓸 메모리 주소 설정
	if (S == 0) //byte
	{
		if (nRW == 0) //read
		{
			return (char)pM[0];
		}
		else if (nRW == 1) //write
		{
			pM[0] = V;

			return 0;
		}
	}
	else if (S == 1) //half word
	{
		if (offset % 2 != 0)
		{
			printf("Not alignment address\n");
			return 1;
		}

		if (nRW == 0) //read
		{
			return (short)(pM[0] << 8) + pM[1]; //Big Endian 방식으로 읽기
		}
		else if (nRW == 1) //write
		{
			pM[0] = V >> 8; pM[1] = V; //Big Endian 방식으로 쓰기

			return 0;
		}
	}
	else if (S == 2) //word
	{
		if (offset % 4 != 0)
		{
			printf("Not alignment address\n");
			return 1;
		}

		if (nRW == 0) //read
		{
			return (pM[0] << 24) + (pM[1] << 16) + (pM[2] << 8) + pM[3];
		}
		else if (nRW == 1) //write
		{
			pM[0] = V >> 24; pM[1] = V >> 16; pM[2] = V >> 8; pM[3] = V;

			return 0;
		}
	}
	else //S가 유효하지 않은 값일 경우 오류
	{
		printf("Invalid S value\n");

		return 1;
	}
}