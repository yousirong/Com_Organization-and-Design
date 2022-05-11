#define _CRT_SECURE_NO_WARNINGS
#define M_SIZE 1000
#define REG_SIZE 32
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

const int check = 1;

static FILE* pFile = NULL;
//static errno_t err;
static int continueTask = 1;

/*각 format에 따른 구조체 형식이다. 강의자료 참고함.
RI = r-format 구조체
II = i-format 구조체
JI = j-format 구조체*/
union itype {
	unsigned int I;
	struct rFormat {
		unsigned int opcode : 6;
		unsigned int rs : 5;
		unsigned int rt : 5;
		unsigned int rd : 5;
		unsigned int funct : 6;
	}RI;
	struct iFormat {
		unsigned int opcode : 6;
		unsigned int rs : 5;
		unsigned int rt : 5;
		unsigned int offset : 16;
	}II;
	struct jFormat {
		unsigned int opcode : 6;
		unsigned int jumpAddr : 26;
	}JI;
}IR;

void openBinaryFile(char* filePath);   // l 명령어 실행시 filePath를 받아서 바이너리 파일 여는 함수
unsigned int To_BigEndian(unsigned int x);  // 빅엔디안 변경 함수 => hex값
unsigned char getOp(int opc);  // opcode 확인 함수
// binary to decimal 한 값을 int값으로 저장함
unsigned char* getInstName(int opc, int fct, int* isImmediate);   // debugging 함수
void instExecute(int opc, int fct, int* isImmediate);   // instruction 실행함수
int MEM(unsigned int A, int V, int nRW, int S); // memory access함수
// ALU
int logicOperation(int X, int Y, int C);
int addSubtract(int X, int Y, int C);
int shiftOperation(int V, int Y, int C);
int checkZero(int S);
int checkSetLess(int X, int Y);
int ALU(int X, int Y, int C, int* Z);   // R-format 명령어에서 ALU함수 필요

static unsigned char progMEM[0x100000], dataMEM[0x100000], stakMEM[0x100000];

static unsigned int R[32], PC;

char* regArr[32] = { "$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3",
					"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
					"$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
					"$t8","$t9","$k0","$k1","$gp","$sp","$s8","$ra" };

//Memory Access 부분이다.
int MEM(unsigned int A, int V, int nRW, int S) {
	unsigned int sel, offset;
	unsigned char* pM;
	sel = A >> 20;
	offset = A & 0xFFFFF;

	if (sel == 0x004) pM = progMEM;			// Program memory
	else if (sel == 0x100) pM = dataMEM;	// Data memory
	else if (sel == 0x7FF) pM = stakMEM;	// Stack
	else {

		printf("No memory\n");
		// 에러 케이스 테스트를 위해 전체 프로그램을 종료하지 않고
		// 함수만 종료한다
		return 1;
	}

	if (S == 0) {
		// Byte
		if (nRW == 0) {
			// Read
			return pM[offset];
		}
		else if (nRW == 1) {
			// Write
			pM[offset] = V;
		}
		else {
			printf("nRW input error\n");
			return 1;
			//exit(1);
		}
	}
	else if (S == 1) {
		// Half word
		if (offset % 2 != 0)	// Half-word-aligned Check
		{
			printf("Not an half-word-aligned address input!\n");
			return 1;
			//exit(1);
		}
		if (nRW == 0) {
			// Read
			int result = (pM[offset] << 8) + (pM[offset + 1]);
			return result;
		}
		else if (nRW == 1) {
			// Write
			pM[offset] = (V >> 8) & 0xFF;
			pM[offset + 1] = V & 0xFF;
		}
		else {
			printf("nRW input error\n");
			return 1;
			//exit(1)
		}
	}
	else if (S == 2) {
		// Word
		if (offset % 4 != 0)	// Word-aligned Check
		{
			printf("Not an word-aligned address input!\n");
			return 1;
			//exit(1)
		}
		if (nRW == 0) {
			// Read
			int result = (pM[offset] << 24) + (pM[offset + 1] << 16) + (pM[offset + 2] << 8) + (pM[offset + 3]);
			return result;
		}
		else if (nRW == 1) {
			// Write
			pM[offset] = (V >> 24) & 0xFF;
			pM[offset + 1] = (V >> 16) & 0xFF;
			pM[offset + 2] = (V >> 8) & 0xFF;
			pM[offset + 3] = V & 0xFF;
		}
		else {
			printf("nRW input error\n");
			return 1;
			//exit(1)
		}
	}
	else {//S가 유효하지 않은 값일 경우 오류처리
		printf("Size input error\n");
		return 1;
		//exit(1)
	}
}


void initializeRegister();//레지스터 초기화
void setRegister(unsigned int regNum, unsigned int val);//원하는 레지스터 값을 변경할 수 있는 함수.
void setMemory(char* offset, char* val);//원하는 값으로 해당 메모리에 접근하여 값을 변경하는 함수.

void updatePC(unsigned int addr);//현재 pc값을 원하는 값으로 변경하는 함수이다.
void loadInitTask();//바이너리 파일을 로드하고 메모리에  적재하는 작업을 담당하는 함수.
void showRegister();//인터페이스 'r'실행시 반환되는 함수
void startGoTask();//인터페이스 'g'실행시 반환되는 함수
void startStepTask();//인터페이스 's'실행시 반환되는 함수

int main()
/*위 소스코드는 simulator 인터페이스 만드는 내용이다.
simulator명령은 I/j/g/s/m/r/x/sr/sm으로 구성된다.*/
{
	char cmdLine[50];
	int lenCode = 0;
	// Initialize
	initializeRegister();
	// General 모드 와 Debug 모드 구분
	while (1) {
		lenCode = 0;
		printf("Type Command : ");
		gets(cmdLine);

		// 공백 문자를 기준으로 문자열을 자르고 포인터 반환
		char* ptr = strtok(cmdLine, " ");
		// 명령어 코드
		char* cmdCode = ptr;
		if (ptr != NULL) {
			// 명령어 코드 글자 수
			lenCode = strlen(cmdCode);
		}
		if (lenCode == 1) {
			switch (*cmdCode) {
			case 'l':
				// load program
				// ptr은 Filename 문자열을 가리킨다.
				ptr = strtok(NULL, " ");
				char* filePath = NULL;
				if (ptr == NULL) {
					printf("Error: Not enough arguments.\n");
					printf("ex) l C:\\pub\\as_ex01_arith.bin\n");
				}
				else {
					filePath = ptr;
					openBinaryFile(filePath);
				}
				break;
			case 'j':
				// jump, 입력한 위치에서 simulator 실행을 준비한다.
				// ptr은 프로그램 시작 위치 문자열을 가리킨다.
				ptr = strtok(NULL, " ");
				char* newAddr = NULL;
				if (ptr == NULL) {
					printf("Error: Not enough arguments.\n");
					printf("ex) j 0x40000000\n");
				}
				else {
					newAddr = ptr;
					updatePC(strtol(newAddr, NULL, 16));
				}
				break;
			case 'g':
				/*Go program, 현재PC위치에서 simulator가 명령어를 끝까지 처리한다.
				이때 사용되는 함수는 startGoTask()이다.*/
				if (pFile != NULL) {
					startGoTask();
				}
				else {
					printf("Error: Load Binary File in advance!\n");
				}
				break;
			case 's': {
				/* Step, 명령어에 의하여 변경된 레지스터, 메모리 정보를 출력한다.
				이때 사용되는 함수는 startStepTask()이다.*/
				if (pFile != NULL) {
					startStepTask();
					showRegister();
				}
				else {
					printf("Error: Load Binary File in advance!\n");
				}
				break; }
			case 'm':
				// View memory
				// ptr은 start 문자열을 가리킨다., startAddr~endAddr범위의 메모리 내용 출력
				ptr = strtok(NULL, " ");

				if (ptr == NULL) {
					printf("Error: Not enough arguments.\n");
					printf("ex) m 0x10000000 0x1000FFFF\n");
				}
				else {
					char* start = ptr;

					// ptr은 end 문자열을 가리킨다.
					ptr = strtok(NULL, " ");
					if (ptr == NULL) {
						printf("Error: Not enough arguments.\n");
						printf("ex) m 0x10000000 0x1000FFFF\n");
					}
					else {
						char* end = ptr;

						unsigned int startAddr = strtol(start, NULL, 16);
						unsigned int endAddr = strtol(end, NULL, 16);

						for (unsigned int i = startAddr; i <= endAddr; i = i + 4) {
							printf("[0x%08x] => 0x%x\n", i, MEM(i, NULL, 0, 2));
						}
					}
				}
				break;
			case 'r':
				/* View register, 현재 레지스터 내용 출력
				이때 사용되는 함수는 showRegister()이다.*/
				showRegister();
				break;
			case 'x':
				// Program exit, 종료
				exit(1);
				break;
			default:
				printf("Error: Invalid command arguments.\n");
				break;
			}
		}
		else if (lenCode == 2) {
			if (strcmp(cmdCode, "sr") == 0) {
				// 특정 레지스터의 값 설정
				char* regNum = NULL;
				char* regVal = NULL;
				// ptr은 register number 문자열을 가리킨다.
				ptr = strtok(NULL, " ");

				if (ptr == NULL) {
					printf("Error: Not enough arguments.\n");
					printf("ex) sr 1 20\n");
				}
				else {
					regNum = ptr;
					// ptr은 value 문자열을 가리킨다.
					ptr = strtok(NULL, " ");
					if (ptr == NULL) {
						printf("Error: Not enough arguments.\n");
						printf("ex) sr 1 20\n");
					}
					else {
						regVal = ptr;
						setRegister(atoi(regNum), strtol(regVal, NULL, 16));
					}
				}
			}
			else if (strcmp(cmdCode, "sm") == 0) {
				// 메모리 특정 주소의 값 설정
				// ptr은 start 문자열을 가리킨다.
				ptr = strtok(NULL, " ");

				if (ptr == NULL) {
					printf("Error: Not enough arguments.\n");
					printf("ex) sm 0xFFFFFFFF 20\n");
				}
				else {
					printf("OK\n");
					char* memLoc = ptr;

					// ptr은 end 문자열을 가리킨다.
					ptr = strtok(NULL, " ");
					if (ptr == NULL) {
						printf("Error: Not enough arguments.\n");
						printf("ex) sm 0xFFFFFFFF 20\n");
					}
					else {
						printf("OK\n");
						char* memVal = ptr;
					}
				}
			}
			else {
				printf("Error: Invalid command arguments.\n");
			}
		}
		else if (lenCode > 2) {
			printf("Error: Invalid command arguments.\n");
		}
	}
	return 0;
}

// 바이너리 파일 여는 함수
void openBinaryFile(char* filePath) {
	//err = fopen_s(&pFile, "as_ex01_arith.bin", "rb");
	//err = fopen_s(&pFile, "as_ex02_logic.bin", "rb");
	//err = fopen_s(&pFile, "as_ex03_ifelse.bin", "rb");

	// File Validation TEST

    // FILE* testFile = NULL;
    //--------------------------------------------------------이부분 고치기 file 못읽음
	FILE* testFile = fopen( filePath, "rb");
	if (testFile == NULL) {
		printf("Cannot open file\n");
		return 1;
	}
	unsigned int data;
	unsigned int data1 = 0xAABBCCDD;
	if (fread(&data, sizeof(data1), 1, testFile) != 1)
		exit(1);
	fclose(testFile);

	// Load Real File
	fopen_s(&pFile, filePath, "rb");
	printf("The Binary File Has Been Loaded Successfully.\n");

	// Load Init Task (메모리 적재)
	loadInitTask();
}

//인터페이스 's'실행시 반환되는 함수
void startStepTask() {
	printf("current value : %x\n", MEM(PC, NULL, 0, 2));
	unsigned instBinary = MEM(PC, NULL, 0, 2);
	PC = PC + 4;
	/* Instruction Decode */
	// 명령어 타입(R, I, J) 체크 및
	// 명령어 type에 따라 분기하여 추출
	switch (getOp((instBinary >> 26) & 0x3F))
	{
	case 'R':
		// R-Format 기준, opcode 추출
		IR.RI.opcode = (instBinary >> 26) & 0x3F;
		// rs 추출
		IR.RI.rs = (instBinary >> 21) & 0x1F;
		// rt 추출
		IR.RI.rt = (instBinary >> 16) & 0x1F;
		// rd 추출
		IR.RI.rd = (instBinary >> 11) & 0x1F;
		// funct 추출
		IR.RI.funct = instBinary & 0x3F;

		instExecute(IR.RI.opcode, IR.RI.funct, NULL);

		// 명령어 구분에 따른 결과 출력 변화
		if (IR.RI.opcode == 0 && IR.RI.funct == 12) {
			// syscall 명령어 case
			printf("%s\n\n", getInstName(IR.RI.opcode, IR.RI.funct, NULL));
			continueTask = 0;
		}
		else if (IR.RI.opcode == 0 && IR.RI.funct == 8) {
			// jr 명령어 case
			printf("%s %s\n\n", getInstName(IR.RI.opcode, IR.RI.funct, NULL), regArr[IR.RI.rs]);
		}
		else {
			printf("%s %s %s %s\n\n", getInstName(IR.RI.opcode, IR.RI.funct, NULL), regArr[IR.RI.rd], regArr[IR.RI.rs], regArr[IR.RI.rt]);
		}
		break;
	case 'I':
		// I-Format 기준, opcode 추출
		IR.II.opcode = (instBinary >> 26) & 0x3F;
		int isImmediate = 0; // immediate 값이면 1로 바꿈
		// rs 추출
		IR.II.rs = (instBinary >> 21) & 0x1F;
		// rt 추출
		IR.II.rt = (instBinary >> 16) & 0x1F;
		// offset/immediate value 추출
		IR.II.offset = instBinary & 0xFFFF;

		instExecute(IR.II.opcode, NULL, &isImmediate);

		// offset인지 immediate value 인지에 따른 결과 출력 변화
		printf("%s", getInstName(IR.II.opcode, NULL, &isImmediate));
		if (isImmediate == 1) {
			printf(" %s %s %d\n\n", regArr[IR.II.rt], regArr[IR.II.rs], IR.II.offset);
		}
		else {
			printf(" %s %d(%s)\n\n", regArr[IR.II.rt], IR.II.offset, regArr[IR.II.rs]);
		}
		break;
	case 'J':
		// J-Format 기준, opcode 추출
		IR.JI.opcode = (instBinary >> 26) & 0x3F;
		// jump target address 추출
		IR.JI.jumpAddr = instBinary & 0x3FFFFFF;

		instExecute(IR.JI.opcode, NULL, NULL);

		// 결과 출력
		printf("%s %d\n\n", getInstName(IR.JI.opcode, NULL, NULL), IR.JI.jumpAddr);
		break;
	default:
		break;
	}
}

//인터페이스 'g'실행시 반환되는 함수
void startGoTask() {

	while (continueTask) {
		/* Instruction Fetch */
		printf("current value : %x\n", MEM(PC, NULL, 0, 2));
		unsigned instBinary = MEM(PC, NULL, 0, 2);
		PC = PC + 4;
		/* Instruction Decode */
		// 명령어 타입(R, I, J) 체크 및
		// 명령어 type에 따라 분기하여 추출
		switch (getOp((instBinary >> 26) & 0x3F))
		{
		case 'R':
			// R-Format 기준, opcode 추출
			IR.RI.opcode = (instBinary >> 26) & 0x3F;
			// rs 추출
			IR.RI.rs = (instBinary >> 21) & 0x1F;
			// rt 추출
			IR.RI.rt = (instBinary >> 16) & 0x1F;
			// rd 추출
			IR.RI.rd = (instBinary >> 11) & 0x1F;
			// funct 추출
			IR.RI.funct = instBinary & 0x3F;

			instExecute(IR.RI.opcode, IR.RI.funct, NULL);


			// 명령어 구분에 따른 결과 출력 변화 (For Debugging)
			if (IR.RI.opcode == 0 && IR.RI.funct == 12) {
				// syscall 명령어 case
				printf("%s\n\n", getInstName(IR.RI.opcode, IR.RI.funct, NULL));
				continueTask = 0;
			}
			else if (IR.RI.opcode == 0 && IR.RI.funct == 8) {
				// jr 명령어 case
				printf("%s %s\n\n", getInstName(IR.RI.opcode, IR.RI.funct, NULL), regArr[IR.RI.rs]);
			}
			else {
				printf("%s %s %s %s\n\n", getInstName(IR.RI.opcode, IR.RI.funct, NULL), regArr[IR.RI.rd], regArr[IR.RI.rs], regArr[IR.RI.rt]);
			}

			break;
		case 'I':
			// I-Format 기준, opcode 추출
			IR.II.opcode = (instBinary >> 26) & 0x3F;
			int isImmediate = 0; // immediate 값이면 1로 바꿈
			// rs 추출
			IR.II.rs = (instBinary >> 21) & 0x1F;
			// rt 추출
			IR.II.rt = (instBinary >> 16) & 0x1F;
			// offset/immediate value 추출
			IR.II.offset = instBinary & 0xFFFF;

			instExecute(IR.II.opcode, NULL, &isImmediate);


			// offset인지 immediate value 인지에 따른 결과 출력 변화 (For Debugging)
			printf("%s", getInstName(IR.II.opcode, NULL, &isImmediate));
			if (isImmediate == 1) {
				printf(" %s %s %d\n\n", regArr[IR.II.rt], regArr[IR.II.rs], IR.II.offset);
			}
			else {
				printf(" %s %d(%s)\n\n", regArr[IR.II.rt], IR.II.offset, regArr[IR.II.rs]);
			}

			break;
		case 'J':
			// J-Format 기준, opcode 추출
			IR.JI.opcode = (instBinary >> 26) & 0x3F;
			// jump target address 추출
			IR.JI.jumpAddr = instBinary & 0x3FFFFFF;

			instExecute(IR.JI.opcode, NULL, NULL);


			// 결과 출력 (For Debugging)
			printf("%s %d\n\n", getInstName(IR.JI.opcode, NULL, NULL), IR.JI.jumpAddr);

			break;
		default:
			break;
		}
	}
}

//인터페이스 'r'실행시 반환되는 함수
void showRegister() {
	// 16진수로 출력
	printf("[REGISTER]\n");
	for (int i = 0; i < REG_SIZE; i++) {
		printf("$%d=\t0x%x\n", i, R[i]);
	}
	printf("PC=\t0x%x\n", PC);
}

//레지스터 초기화
void initializeRegister() {
	for (int i = 0; i < REG_SIZE; i++) {
		// 32bit
		R[i] = 0x00000000;
	}
	// PC 초기값 설정
	PC = 0x00400000;
	// SP 초기값 설정
	R[29] = 0x80000000;
}

//원하는 레지스터 값을 변경할 수 있는 함수.
void setRegister(unsigned int regNum, unsigned int val) {

	R[regNum] = val;
}

//원하는 값으로 해당 메모리에 접근하여 값을 변경하는 함수.
void setMemory(char* offset, char* val) {

	R[atoi(offset)] = strtol(val, NULL, 16);
}

//현재 pc값을 원하는 값으로 변경하는 함수이다.
void updatePC(unsigned int addr) {
	PC = addr;
}

/*Instruction Fetch단계 =>loadInintTask() = 바이너리 파일을 load하고 메모리에 적재하는 작업을 담당하는 함수*/
void loadInitTask() {
	updatePC(0x400000);
	setRegister(29, 0x80000000);

	//printf("\n%s\n", loadedFilePath);
	unsigned int data;
	unsigned int data1 = 0xAABBCCDD;
	unsigned int numInst;
	unsigned int numData;

	// Read the number of Instructions
	fread(&numInst, sizeof(data1), 1, pFile);
	numInst = To_BigEndian(numInst);
	// Read the number of Datas
	fread(&numData, sizeof(data1), 1, pFile);
	numData = To_BigEndian(numData);

	printf("size of Instructions : %d\n", numInst);
	printf("size of Datas : %d\n", numData);

	unsigned int memAddr = 0x00400000;
	unsigned int dataAddr = 0x10000000;

	for (int i = 0; i < numInst; i++) {
		if (fread(&data, sizeof(data1), 1, pFile) != 1)
			exit(1);
		// 명령어 메모리 적재
		data = To_BigEndian(data);
		printf("Instruction = %08x\n", data);

		MEM(memAddr, data, 1, 2);
		memAddr = memAddr + 4;
	}

	for (int i = 0; i < numData; i++) {
		if (fread(&data, sizeof(data1), 1, pFile) != 1)
			exit(1);
		data = To_BigEndian(data);
		// 데이터 메모리 적재
		printf("Data = %08x\n", data);

		MEM(dataAddr, data, 1, 2);
		dataAddr = dataAddr + 4;
	}
}
/*To_BigEndian = 데이터가 있을때 큰 단위가 앞으로 나오게 만드는 함수.
이진수에서는 상위비트로 갈 수록 값이 커지기 때문에 앞쪽으로 갈 수록 단위가 커진다.*/
unsigned int To_BigEndian(unsigned int x)
{
	unsigned int result = (x & 0xFF) << 24;

	result |= ((x >> 8) & 0xFF) << 16;
	result |= ((x >> 16) & 0xFF) << 8;
	result |= ((x >> 24) & 0xFF);

	return result;
}

/*Instruction Decode단계 => getOp() = instruction의 Op code, 즉 operation의 종류를 반환하는 함수
0이면 R-type, 2또는3이면 J-type, 그 외는 I-type으로 처리함.
instExecute() = op, function code에 따라 명령어를 분류하고 해당되는 연산을 실행하는 함수이다.*/
unsigned char getOp(int opc) {
	char format;
	// R-Format instruction
	if (opc == 0) {
		format = 'R';
	}
	// J-Format instruction
	else if ((opc == 2) || (opc == 3)) {
		format = 'J';
	}
	// I-Format instruction
	else {
		format = 'I';
	}
	return format;
}

/*R-format + (I-format 또는 J-format)라는 2가지 format으로 나누었다.
switch문을 사용해 case마다 명령어 처리했다.
각 instruction은 MIPS simulator 강의자료 참고함*/
void instExecute(int opc, int fct, int* isImmediate) {
	if (opc != 0) {
		// I-Format 또는 J-Format 인 경우
		switch (opc) {
		case 1: {
			// bltz
			int Z;
			if (ALU(R[IR.II.rs], 0, 4, &Z) == 1) {
				updatePC(PC + IR.II.offset * 4);	// PC = PC + 4 + 4 * offset
			}
			break; }
		case 2:
			// j
			updatePC(IR.JI.jumpAddr);	// go to L
			break;
		case 3:
			// jal
			setRegister(31, PC + 4);	// $ra = PC + 4
			updatePC(IR.JI.jumpAddr);	// go to L
			break;
		case 4: {
			// beq
			int Z;
			if (ALU(R[IR.II.rs], R[IR.II.rt], 8, &Z) == 0) {
				updatePC(PC + IR.II.offset * 4);	// PC = PC + 4 + 4 * offset
			}
			*isImmediate = 1;
			break; }
		case 5: {
			// bne
			int Z;
			if (ALU(R[IR.II.rs], R[IR.II.rt], 8, &Z) != 0) {
				updatePC(PC + IR.II.offset * 4);	// PC = PC + 4 + 4 * offset
			}
			*isImmediate = 1;
			break; }
		case 8: {
			// addi
			int Z;
			R[IR.II.rt] = ALU(R[IR.II.rs], IR.II.offset, 8, &Z);
			*isImmediate = 1;
			break; }
		case 10: {
			// slti
			int Z;
			R[IR.II.rt] = ALU(R[IR.II.rs], IR.II.offset, 4, &Z);
			*isImmediate = 1;
			break; }
		case 12: {
			// andi
			int Z;
			R[IR.II.rt] = ALU(R[IR.II.rs], IR.II.offset, 12, &Z);
			*isImmediate = 1;
			break; }
		case 13: {
			// ori
			int Z;
			R[IR.II.rt] = ALU(R[IR.II.rs], IR.II.offset, 13, &Z);
			*isImmediate = 1;
			break; }
		case 14: {
			// xori
			int Z;
			R[IR.II.rt] = ALU(R[IR.II.rs], IR.II.offset, 14, &Z);
			*isImmediate = 1;
			break; }
		case 15: {
			// lui
			R[IR.II.rt] = IR.II.offset << 16;
			*isImmediate = 1;
			break; }
		case 32:
			// lb
			R[IR.II.rt] = MEM(R[IR.II.rs] + IR.II.offset, NULL, 0, 0);
			break;
		case 35:
			// lw
			R[IR.II.rt] = MEM(R[IR.II.rs] + IR.II.offset, NULL, 0, 2);
			break;
		case 36:
			// lbu
			R[IR.II.rt] = MEM(R[IR.II.rs] + IR.II.offset, NULL, 0, 0);
			break;
		case 40:
			// sb
			MEM(R[IR.II.rs] + IR.II.offset, R[IR.II.rt], 1, 0);
			break;
		case 43:
			// sw
			MEM(R[IR.II.rs] + IR.II.offset, R[IR.II.rt], 1, 2);
			break;
		default:
			// NOT FOUND!
			break;
		}
	}
	else {
		// R-Format 인 경우
		switch (fct) {
		case 0: {
			// sll
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 1, &Z);
			break; }
		case 2: {
			// srl
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 2, &Z);
			break; }
		case 3: {
			// sra
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 3, &Z);
			break; }
		case 8:
			// jr
			updatePC(R[31]);	// go to $ra
			break;
		case 12:
			// syscall
			continueTask = 0;
			break;
		case 16:
			// mfhi
			break;
		case 18:
			// mflo
			break;
		case 24:
			// mul
			break;
		case 32: {
			// add
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 8, &Z);
			break; }
		case 34: {
			// sub
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 9, &Z);
			break; }
		case 36: {
			// and
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 12, &Z);
			break; }
		case 37: {
			// or
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 13, &Z);
			break; }
		case 38: {
			// xor
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 14, &Z);
			break; }
		case 39: {
			// nor
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 15, &Z);
			break; }
		case 42: {
			// slt
			int Z;
			R[IR.RI.rd] = ALU(R[IR.RI.rs], R[IR.RI.rt], 4, &Z);
			break; }
		default:
			// NOT FOUND!
			break;
		}
	}
}

unsigned char* getInstName(int opc, int fct, int* isImmediate) {
	if (opc != 0) {
		// I-Format 또는 J-Format 인 경우
		switch (opc) {
		case 1:
			return "bltz";
		case 2:
			return "j";
		case 3:
			return "jal";
		case 4:
			*isImmediate = 1;
			return "beq";
		case 5:
			*isImmediate = 1;
			return "bne";
		case 8:
			*isImmediate = 1;
			return "addi";
		case 10:
			*isImmediate = 1;
			return "slti";
		case 12:
			*isImmediate = 1;
			return "andi";
		case 13:
			*isImmediate = 1;
			return "ori";
		case 14:
			*isImmediate = 1;
			return "xori";
		case 15:
			*isImmediate = 1;
			return "lui";
		case 32:
			return "lb";
		case 35:
			return "lw";
		case 36:
			return "lbu";
		case 40:
			return "sb";
		case 43:
			return "sw";
		default:
			return "NOT FOUND!";
		}
	}
	else {
		// R-Format 인 경우
		switch (fct) {
		case 0:
			return "sll";
		case 2:
			return "srl";
		case 3:
			return "sra";
		case 8:
			return "jr";
		case 12:
			return "syscall";
		case 16:
			return "mfhi";
		case 18:
			return "mflo";
		case 24:
			return "mul";
		case 32:
			return "add";
		case 34:
			return "sub";
		case 36:
			return "and";
		case 37:
			return "or";
		case 38:
			return "xor";
		case 39:
			return "nor";
		case 42:
			return "slt";
		default:
			return "NOT FOUND!";
		}
	}
}
/*ALU함수 부분이다.
1. logicalOperation() = 비트 연산자 수행하는 함수
2. addSubtract() = 덧셈,뺄셈 수행하는 함수이다.
3. shiftOperation() = 쉬프트 연산 함수이다.
4. checkZero() = 0인지 확인하는 함수이다.
5. checkSetLess() = x보다 작은지 판별하는 함수이다.*/
int logicOperation(int X, int Y, int C) {
	if (C < 0 || C > 3) {
		printf("error in logic operation\n");
		exit(1);
	}
	if (C == 0) {
		// AND
		return X & Y;
	}
	else if (C == 1) {
		// OR
		return X | Y;
	}
	else if (C == 2) {
		// XOR
		return X ^ Y;
	}
	else {
		// NOR
		return ~(X | Y);
	}
}

int addSubtract(int X, int Y, int C) {
	int ret;
	if (C < 0 || C > 1) {
		printf("error in add/subtract operation\n");
		exit(1);
	}
	if (C == 0) {
		// add
		ret = X + Y;
	}
	else {
		// subtract
		ret = X - Y;
	}
	return ret;
}

// V is 5 bit shift amount
int shiftOperation(int V, int Y, int C) {
	int ret;
	if (C < 0 || C > 3) {
		printf("error in shift operation\n");
		exit(1);
	}
	if (C == 0) {
		// No shift : 그대로 반환
		ret = V;
	}
	else if (C == 1) {
		// Logical left
		ret = V << Y;
	}
	else if (C == 2) {
		// Logical right
		ret = V >> Y;
	}
	else {
		// Arith right
		ret = V >> Y;
	}
	return ret;
}

// 이함수는 add 또는 subtract 수행 시만
// 사용하여 Z값을 설정한다.
int checkZero(int S) {
	int ret = 0;
	// check if S is zero,
	// and return 1 if it is zero
	// else return 0
	if (S == 0) {
		ret = 1;
	}
	return ret;
}

int checkSetLess(int X, int Y) {
	int ret;

	// check if X < Y,
	// and return 1 if it is true
	// else return 0
	if (Y > X) {
		ret = 1;
	}
	else {
		ret = 0;
	}
	return ret;
}

int ALU(int X, int Y, int C, int* Z) {
	int c32, c10;
	int ret;

	c32 = (C >> 2) & 3;
	c10 = C & 3;
	if (c32 == 0) {
		//shift
		ret = shiftOperation(X, Y, c10);
	}
	else if (c32 == 1) {
		// set less
		ret = checkSetLess(X, Y);
	}
	else if (c32 == 2) {
		// addsubtract
		ret = addSubtract(X, Y, c10);
		*Z = checkZero(ret);
	}
	else {
		// logic
		ret = logicOperation(X, Y, c10);
	}
	return ret;
}