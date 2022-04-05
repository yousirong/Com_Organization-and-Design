#include <stdio.h>
#include <stdlib.h>

int ALU(int X, int Y, int C, int* Z);
int logicOperation(int X, int Y, int C);
int addSubstract(int X, int Y, int C);
int shiftOperation(int V, int Y, int C);
int checkZero(int S);
int checkSetLess(int X, int Y);

int main() {
    void test();
}

void test(){
    int x, y, s, z;

    x = 01010101; //예시
    y = 11110000;

    printf("x : %8x, y : %8x\n", x, y);
    for (int i = 0; i < 16; i++) {
        s = ALU(x, y, i, &z);
        printf("s : %8x, z : %8x\n", s, z);
    }
}

int ALU(int X, int Y, int C, int *Z)
{
    int c32, c10;
    int ret;

    c32 = (C >> 2) & 3; //2보다 크고 11이랑 and 연산한 값을 c32에다 저장
    c10 = C & 3;
    if (c32 == 0) {
        ret = shiftOperation(X, Y, c10);
    }
    else if (c32 == 1) {
        ret = checkSetLess(X, Y);
    }
    else if (c32 == 2) {
        *Z = 1;
        ret = addSubstract(X, Y, C);
    }
    else {
        ret = logicOperation(X, Y, c10);
    }
    return ret;
}

int logicOperation(int X, int Y, int C)
{
    if (C < 0 || C > 3) {
        printf("error in logic operation\n");
        exit(1);
    }
    if (C == 0) //AND
        return X & Y;
    else if (C == 1)//OR
        return X | Y;
    else if (C == 2)//XOR
        return X ^ Y;
    else//NOR
        return ~(X | Y);
}

int addSubstract(int X, int Y, int C) {
    int ret;
    if (C < 0 || C > 1) {
        printf("error in add/substract operation\n");
        exit(1);
    }
    if (C == 0) { //add

        ret = X | Y;
    }
    else {//substract

        ret = (X | (~Y + 1));
    }
    return ret;
}

int shiftOperation(int V, int Y, int C) { //C에 2비트 값이 들어감(0,1,2,3)
    int ret;
    if (C < 0 || C > 3) {
        printf("error in shift operation\n");
        exit(1);
    }
    if (C == 0) {//No shift
        ret = Y; //shift가 일어나지 않으므로 비트 이동 없이 Y값 그대로 출력
    }
    else if (C == 1) {//Logical left
        Y = Y >> V;
        ret = Y; //Y를 V비트 만큼 왼쪽으로 이동
    }
    else if (C == 2) {//Logical right
        Y = Y << V;
        ret = Y; //Y를 V비트 만큼 오른쪽으로 이동
    }
    else {//Arith right 부호있는 상수도
        Y = Y << V; //MSB를 비교하여 0이나 1을 추가해주어야 하는 부분을 모르겠습니다.
        ret = Y;
    }
    return ret;
}

int checkZero(int S) {
    int ret;
    if (S == 0) {
        return 1;
    }
    else {
        return 0;
    }
    return ret;
}

int checkSetLess(int X, int Y) {
    int ret;
    if (X < Y) {
        return 1;
    }
    else {
        return 0;
    }
    return ret;
}
