#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define MAX 32

char* msg = "This is a message that was encrpyted using some key\n"
            "This is another message";

void zip_up(char* message, int shift, int key, int fd) {
    size_t len = strlen(message); 
    int msg[len];

    for(int i = 0; i < len; i++) {
        msg[i] = (message[i] << shift) ^ key;        
    }

    write(fd, msg, len * sizeof(int));
}

int main(int argc, char* argv[]) { 
    if(argc < 2) {
        printf("Usage: %s <shift>, <key>..\n", argv[0]);
        return -1;
    }

    int fd = open("out", O_CREAT | O_RDWR, 0644);
    if(fd < 0) {
        perror("Coud not open file");
        return -1;
    }

    zip_up(msg, atoi(argv[1]), atoi(argv[2]), fd);
    
    close(fd);
    return 0;
}