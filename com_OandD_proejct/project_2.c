#define is_bigendian() ( (*(char*)&i) == 0 )
#include "stdio.h"
#define _CRT_SECURE_NO_WARNINGS

int main(void){

  FILE *pFile = fopen("input.bin", "rb");
  FILE *qFile = fopen("output.bin","wb");

  int count;
  unsigned int data;
  unsigned int exdata;
  unsigned int data1 = 0xAABBCCDD;
  unsigned int data2 = 0x11223344;
  if (pFile==NULL)
  {
    puts("input파일 오픈 실패!");
    return -1;
    }
  if (qFile==NULL)
  {
    puts("output파일 오픈 실패!");
    return -1;
    }
    while (1) {
    count = fread(&data, sizeof(data1), 1, pFile);
    if (count  != 1)
      break;
    printf("%8x\n", data);
  }

  while (1) {
    count = fread(&data, sizeof(data1), 1, qFile);
    if (count  != 1)
      break;
    printf("%8x\n", invertEndian(data));
  }
  fwrite(&data, sizeof(data1), 1, qFile);
  fclose(qFile);
  fclose(pFile);
  return 0;
}
const int i = 1;
int invertEndian (int inputValue) {
    unsigned char c1, c2, c3, c4;
    if (is_bigendian()) {
        return inputValue;
        } else {
            c1 = inputValue & 255;
            c2 = (inputValue >> 8) & 255;
            c3 = (inputValue >> 16) & 255;
            c4 = (inputValue >> 24) & 255;
            return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}
}
