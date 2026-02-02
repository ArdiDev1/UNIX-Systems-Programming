#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#define MAX 4096

int check_char(char ch) {
    return ispunct(ch) || isspace(ch);
}

int calc_score(char* msg, int len) {
    int score = 0;

    for(int i = 0; i < len / sizeof(int); i++) {
        if(!isalnum(msg[i]) && !check_char(msg[i]) && !isalpha(msg[i]))
            return 0;
    }
    
    return 1;
}

void zip_up(int shift, int key, char* buff, int* msg, int len) {
    if(len == 0)
        return;

    int i;
    for(i = 0; i < len / sizeof(int); i++) 
        buff[i] = (char) ((msg[i] ^ key) >> shift);        

    buff[i] = '\0';
}

void decrypt(char* buff, int* msg, int len) {
    int max_score = 0;

    for(size_t shift = 0; shift <= 255; shift++) {
        for(size_t key = 0; key <= 24; key++) {

            zip_up(shift, key, buff, msg, len);
            int score = calc_score(buff, len);
            
            if(score == 1) {
                printf("%s, score: %d\n", buff, score);
                return;
            }

        }
    }
}

int main(int argc, char* argv[]) { 
    if(argc < 2) {
        printf("Usage: %s <shift>, <key>\n", argv[0]);
        return -1;
    }

    int fd = open("out", O_RDWR, 0644);
    if(fd < 0) {
        perror("Could not find");
        return -1;
    }

    int msg[MAX];
    int len = read(fd, msg, MAX);

    char* buff = malloc(sizeof(char) * MAX);
    decrypt(buff, msg, len);

    free(buff);
    close(fd);
    return 0;
}