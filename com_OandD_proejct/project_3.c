#define is_bigendian() ( (*(char*)&i) == 0 )
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
const int M_SIZE =1024;
unsigned int MEM[1024];
const int i = 1;
unsigned int IR=0;  // instruction register
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
    // char b[9];
    // char res[64] ="";
    // // 문자열로 변환
    // sprintf(b, "%08x", data);
    // // 출력
    // // printf("%s", xtob(b[0]));
    // strcpy(res,xtob(b[0]));
    // for(int i=1; i < 8; i++) {
    //     // printf(" %s", xtob(b[i]));
    //     strcpy(res,xtob(b[i]));
    // }
    // printf("%s",xtob(b[1]));
    // printf("\n");
    unsigned char i1, i2, i3, i4;
    if (is_bigendian()) {
        return data;
        } else {
            i4 = data & 255;
            printf("%d\n",i4);
            }
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
            printf("%02x %02x %02x %02x\n",MEM[addr], MEM[addr+1], MEM[addr+2], MEM[addr+3]);
}
}

int main(void){
  FILE *pFile = fopen("as_ex02_logic.bin", "rb");
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
    printf("%08x\n", exdata);
    memoryWrite(cnt,data);
    memoryRead(exdata);

    cnt+=4;
  }

  fclose(pFile);
  return 0;
}
