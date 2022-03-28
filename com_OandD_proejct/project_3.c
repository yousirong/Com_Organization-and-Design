#define is_bigendian() ( (*(char*)&i) == 0 )
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
const int M_SIZE =1024;
unsigned int MEM[1024];
const int i = 1;
unsigned int IR=0;  // instruction register
struct  OPTAB {
    char name[8];
    char len[6];
    char pt[2];
} optab[] = { {"0", "000000","0"}, {"bltz", "000001","1"}, {"j", "000010","2"},
{"jal", "000011","3"}, {"beq", "000100","4"},{"bne", "000101","5"}, {"addi", "001000","8"},
{"slti", "001010","a"}, {"andi", "001100","c"}, {"ori", "001101","d"}, {"xori", "001110","e"},
{"lui", "001111","f"}, {"lb", "100000","20"},{"lw", "100011","23"}, {"lbu", "100100","24"}, {"sb", "101000","28"},{"sw", "101011","2b"}};
struct  FuncTAB {
    char name[8];
    char len[6];
    char pt[2];
} functab[] = { {"sll", "000000","0"}, {"srl", "000010","2"}, {"sra", "000011","3"},
{"jr", "001000","8"}, {"syscall", "001100","c"},{"mfhi", "010000","10"}, {"mflo", "010010","12"},
{"mul", "011000","18"}, {"add", "100000","20"}, {"sub", "100010","22"}, {"and", "100100","24"}, {"or", "100101","25"}, {"xor","100110","26"},
{"nor", "100111","27"}, {"slt", "101010","2a"}};
const char *xtob(int x)
{
static const char *B[] = {
"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
"1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111",
};
if ('A' <= x && x <= 'F') x = x - 'A' + 10;
else if ('a' <= x && x <= 'f') x = x - 'a' + 10;
else x -= '0';

return (0 <= x && x <= 15) ? B[x] : NULL;
}

int invertEndian (int inputValue) {  //endian이 바뀐 데이터를 반환하는 함수
    unsigned char i1, i2, i3, i4;
    if (is_bigendian()) {
        return inputValue;
        } else {
            i1 = inputValue & 255;
            i2 = (inputValue >> 8) & 255;
            i3 = (inputValue >> 16) & 255;
            i4 = (inputValue >> 24) & 255;
            return ((int)i1 << 24) + ((int)i2 << 16) + ((int)i3 << 8) + i4;
}
}
void memoryRead(unsigned int data)
{
    char b[9];
    char res[32] ="";
    char opcode[6];
    char funccode[6];
    // 문자열로 변환
    sprintf(b, "%08x", data);
    // 출력
    // printf("%s", xtob(b[0]));
    strcat(res,xtob(b[0]));
    for(int i=1; i < 8; i++) {
        // printf(" %s", xtob(b[i]));
        strcat(res,xtob(b[i]));
    }
    // printf("%s\n",res);
    //opcode
    strncpy(opcode,&res[0],6);
    opcode[6] = '\0';
    //func
    strncpy(funccode,&res[26],6);
    funccode[6] = '\0';
    if(strcmp(opcode, optab[0].len)==0){  // opcode == 000000
        for(int k=0; k<sizeof(functab); k++){
        if(strcmp(funccode,functab[k].len)==0){
            printf("Opc: %s, Fct: %s, Inst: %s ", optab[0].pt,functab[k].pt,functab[k].name);
            }

    }
    }else if(strcmp(opcode, optab[0].len)!=0){
        for(int k=0; k<sizeof(optab); k++){
        if(strcmp(opcode,optab[k].len)==0){ //opcode == optab
            printf("Opc: %s, Fct: %02x, Inst: %s ",  optab[k].pt,(unsigned char)funccode & 255,functab[k].name);
        }
    }
    }else{
        printf("");
    }
    printf("\n");

}
void memoryWrite(unsigned int addr, unsigned int data)
{
    unsigned char i1, i2, i3, i4;
    if (is_bigendian()) {
        return data;
        } else {
            i1 = data & 255;
            i2 = (data >> 8) & 255;
            i3 = (data >> 16) & 255;
            i4 = (data >> 24) & 255;
            // printf("%02x %02x %02x %02x\n",i1, i2,i3,i4);
            MEM[addr] = i1;
            MEM[addr+1] = i2;
            MEM[addr+2] = i3;
            MEM[addr+3] =i4;
            // printf("%02x %02x %02x %02x\n",MEM[addr], MEM[addr+1], MEM[addr+2], MEM[addr+3]);
}
}

int main(void){
  FILE *pFile = fopen("as_ex01_arith.bin", "rb");
  int count;
  unsigned int data;
  unsigned int exdata;
  int cnt=0;

  unsigned int data1 = 0xAABBCCDD;
  unsigned int data2 = 0x11223344;
  if (pFile==NULL)
  {
    puts("input파일 오픈 실패!");
    return -1;
    }

    while (1) {
    count = fread(&data, sizeof(data1), 1, pFile);
    if (count != 1)
      break;

    exdata = invertEndian(data);
    // printf("%08x\n", exdata);
    memoryWrite(cnt,data);
    cnt+=4;
    if(cnt==0){
        // char chHex[]=MEM[4];
        unsigned int nResult = 0;
        nResult = strtol(MEM[3], NULL, 16);
        printf("Number of instructions: %d",nResult);
    }else if(cnt==4){
        // char chHex[]=MEM[8];
        unsigned int nResult = 0;
        nResult = strtol(MEM[7], NULL, 16);
        printf("Number of data: %d",nResult);
    }else{
        memoryRead(exdata);
    }
  }

  fclose(pFile);
  return 0;
}
