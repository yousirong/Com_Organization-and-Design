#define is_bigendian() ( (*(char*)&i) == 0 )
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
const int M_SIZE =1024;
unsigned char MEM[1024];
const int i = 1;
int memoryRead (int addr) {
    unsigned char i1, i2, i3, i4;
    if (is_bigendian()) {
        return addr;
        } else {
            i1 = addr & 255;
            i2 = (addr >> 8) & 255;
            i3 = (addr >> 16) & 255;
            i4 = (addr >> 24) & 255;
            return ((int)i1 << 24) + ((int)i2 << 16) + ((int)i3 << 8) + i4;
}
}

void memoryWrite(unsigned int addr, unsigned int data){

}
int main(void){

  FILE *pFile = fopen("input.bin", "rb");


  int count;
  unsigned int data;
  unsigned int exdata;
  unsigned int resdata; // 출력파일에 저장할 19개의 데이터를 모두 XOR하여 그 값을 Hexa로 출력하는 변수
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

    exdata = memoryRead(data);
    memoryWrite(MEM ,exdata);
    printf("%08x\n", exdata);
  }

  fclose(pFile);
  return 0;
}
