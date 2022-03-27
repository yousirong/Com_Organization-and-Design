#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

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

int main()
{
    char b[9];
    int x;
    int i;

    // 16진수 입력
    printf("16진수: ");
    scanf("%x", &x);

    // 문자열로 변환
    sprintf(b, "%08x", x);

    // 출력
    printf("%s", xtob(b[0]));
    for(i=1; i < 8; i++) {
        printf(" %s", xtob(b[i]));
    }
    printf("\n");

    return 0;
}