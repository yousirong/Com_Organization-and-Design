#define is_bigendian() ( (*(char*)&i) == 0 )
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
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

int main(void){

  FILE *pFile = fopen("input.bin", "rb");
  FILE *qFile = fopen("output.bin","wb");

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
  if (qFile==NULL)
  {
    puts("output파일 오픈 실패!");
    return -1;
    }

    while (1) {
    count = fread(&data, sizeof(data1), 1, pFile);
    if (count != 1)
      break;
    //printf("%08x\n", data);
    exdata = invertEndian(data);   // input.bin에서 읽은 데이터를 endian이 바뀐 데이터를 반환하는 함수를 사용
    resdata = invertEndian(data)^resdata; //출력파일에 저장할 19개의 데이터를 모두 XOR
    fwrite(&exdata, sizeof(data1), 1, qFile);  //output.bin에 fwrite
  }
  printf("%08x\n",resdata); //출력파일에 저장할 19개의 데이터를 모두 XOR하여 그 값을 Hexa로 출력
  fclose(qFile);
  fclose(pFile);
  return 0;
}
