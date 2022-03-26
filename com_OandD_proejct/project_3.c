#define is_bigendian() ( (*(char*)&i) == 0 )
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
const int M_SIZE =1024;
unsigned int MEM[1024];
const int i = 1;


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

unsigned int memoryRead(unsigned int addr)
{

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
            // return ((int)i1 << 24) + ((int)i2 << 16) + ((int)i3 << 8) + i4;
            // return addr[0], addr[2], addr[4], addr[6];
}
}

int main(void){
  FILE *pFile = fopen("input.bin", "rb");
  int count;
  unsigned int data;
  unsigned int exdata;
  int cnt=0;
//   unsigned int resdata; // 출력파일에 저장할 19개의 데이터를 모두 XOR하여 그 값을 Hexa로 출력하는 변수
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
    printf("%08x  %d\n", exdata, cnt);
    memoryWrite(cnt,data);
    cnt++;
  }

  fclose(pFile);
  return 0;
}
