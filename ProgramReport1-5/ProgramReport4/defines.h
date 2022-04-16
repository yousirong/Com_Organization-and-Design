#pragma once

union instructionRegister {
	unsigned int I; // for fetch
	struct RType {
		unsigned int fct : 6;
		unsigned int sht : 5;
		unsigned int rd : 5;
		unsigned int rt : 5;
		unsigned int rs : 5;
		unsigned int opc : 6;
	} RI;
	struct IType {
		int offset : 16;		// Note that this field is signed
		unsigned int rt : 5;
		unsigned int rs : 5;
		unsigned int opc : 6;
	} II;
	struct JType {
		unsigned int jval : 26;
		unsigned int opc : 6;
	} JI;
};

#define NOP		0x00	// 00000

#define SLL		0x01	// 00001
#define SRL		0x02	// 00010
#define SRA		0x03	// 00011

#define SLT		0x04	// 00100

#define MFHI	0x05	// 00101
#define MFLO	0x06	// 00110
#define MUL     0x07	// 00111

#define ADD		0x08	// 01000
#define SUB		0x09	// 01001

#define JR      0x0A	// 01010
#define SYSCALL 0x0B	// 01011

#define AND		0x0C	// 01100
#define OR 		0x0D	// 01101
#define XOR		0x0E	// 01110
#define NOR		0x0F	// 01111

#define LB      0x10	// 10000
#define SB      0x11	// 10001
#define LW      0x12	// 10010
#define SW      0x13	// 10011

#define SLTI	0x14	// 10100
#define BLTZ	0x15	// 10101

#define J		0x16	// 10110  
#define JAL		0x17	// 10111

#define ADDI	0x18	// 11000

#define LBU     0x19	// 11001

#define BEQ		0x1A	// 11010
#define BNE		0x1B	// 11011

#define ANDI	0x1C	// 11100
#define ORI 	0x1D	// 11101
#define XORI	0x1E	// 11110
#define LUI  	0x1F	// 11111

