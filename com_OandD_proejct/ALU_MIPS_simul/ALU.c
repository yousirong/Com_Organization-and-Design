// ALU 201904458 이준용
#include <stdio.h>
#include <stdlib.h>

int ALU(int X, int Y, int C, int* Z);
int checkZero(int Oa);
int checkSetLess(int X, int Y);
int logicOperation(int X, int Y, int s1s0);
int shiftOperation(int V, int Y, int s1s0);
int addSubstract(int X, int Y, int s0);

int ALU(int X, int Y, int C, int *Z)
{
    int c32, c10;
    int ret;

    c32 = (C >> 2) & 3; //2보다 크고 11이랑 and 연산한 값을 c32에다 저장
    c10 = C & 3;
    // printf("%8x  %8x  ",c32,c10);
    if (c32 == 0) {
        // printf("shiftOperation");
        ret = shiftOperation(X, Y, c10);
    }
    else if (c32 == 1 && c10 ==1) {
        // printf("checkSetLess");

        int Oa = addSubstract(X, Y, c10);
        int Zero = checkZero(Oa);
        Z = &Zero;
            // printf("%d    %d\n", Zero,Z);
        ret = checkSetLess(X, Y);

    }
    else if (c32 == 2 && c10 < 2) {

        // printf("addSubstract");
        ret = addSubstract(X, Y, c10);
    }
    else if(c32 == 3){
        // printf("logicOperation");
        ret = logicOperation(X, Y, c10);
    }
    else{
        ret= -1;
    }
    return ret;
}

int logicOperation(int X, int Y, int s1s0)  // 0 and 1 or  2 xor 3 nor
{
    if (s1s0 < 0 || s1s0 > 3) {
        printf("error in logic operation\n");
        exit(1);
    }
    if (s1s0 == 0) //AND
        return X & Y;
    else if (s1s0 == 1)//OR
        return X | Y;
    else if (s1s0 == 2)//XOR
        return X ^ Y;
    else//NOR
        return ~(X | Y);
}

int shiftOperation(int V, int Y, int s1s0) { //s1s0에 2비트 값이 들어감(0,1,2,3)
    int ret;
    if (s1s0 < 0 || s1s0 > 3) {
        printf("error in shift operation\n");
        exit(1);
    }
    if (s1s0 == 0) {//No shift
        // printf("no shift");
        ret = Y; //shift가 일어나지 않으므로 비트 이동 없이 Y값 그대로 출력
    }
    else if (s1s0 == 1) {//Shift Logical left
        //printf("sll");
        Y = Y << V;
        ret = Y; //Y를 V비트 만큼 왼쪽으로 이동
    }
    else if (s1s0 == 2) {//Shift Logical right
        // printf("slr");
        Y = Y >> V;
        ret = Y; //Y를 V비트 만큼 오른쪽으로 이동
    }
    else { //Shift Logical arithmetic
        // printf("sra");
        for(int i=0; i<V;i++){
            Y = Y >> 1;
            Y = Y | 0x80000000;
        }
        ret = Y;
    }
    return ret;
}
int addSubstract(int X, int Y, int s0) {
    int ret;
    // printf("Add/sub");
    if (s0 < 0 || s0 > 1) {
        printf("error in add/substract operation\n");
        exit(1);
    }
    if (s0 == 0) { //add  -> 0
        ret = X | Y;

    }
    else if(s0 == 1 ) {//substract -> 1

        // ret = (X | (~Y) );
        ret = X - Y;
    }
    return ret;
}
int checkZero(int Z) {
    int Zero;
    // printf("checkZero");
    int res = Z >> 31;
    //printf("MSB == %d\n ", res);
    if (res == 0) { // add/sub결과가 0이면 1아니면 0
        Zero =1;
    }
    else {
        Zero =0;
    }
    return Zero;
}

int checkSetLess(int X, int Y) {
    int ret;
    // printf("checksetless");
    if (X < Y) {
        ret =1;
    }
    else {
        ret =0;
    }
    return ret;
}
void test(){
    int x, y, s, z;

    x = 00110110;
    y = 01101101;

    printf("x : %8x, y : %8x\n", x, y);
    for (int i = 0; i < 16; i++) {
        s = ALU(x, y, i, &z);
        if(s!= (-1)){
            printf("s : %8x, z : %8x\n", s, z);
        }
    }

}

int main() {
    test();

    return 0;
}
