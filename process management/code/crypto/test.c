#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    const char* msg = "hello";

    for(int i = 0; i < strlen(msg); i++) {

        printf("%s \n", msg + i);
    }

    return 0;
}