#include <stdio.h>


int main() {
    int buf[100][10] ={0};

    int (*buf2)[10] = buf;

    printf("%d\n", buf[0][0]);
    printf("%d\n", buf2[0][0]);
}