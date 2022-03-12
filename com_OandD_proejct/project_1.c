#include <stdio.h>
#include <stdlib.h> // rand() 함수 포함 라이브러리

void testOperator(int x, int y){
    int ans = x&y;
    printf("0x%08x & 0x%08x = 0x%08x \n",x,y,ans);
    int res = ans << 2;
    printf("0x%08x << 2 = 0x%08x",ans,res);
}

int main(void)
{
	int x,y;
    for(int i =0; i<5; i++){ //5번 반복
        x = rand()%9;
        y = rand()%9;
    }
    testOperator(x,y);
	return 0;
}