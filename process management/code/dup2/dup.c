#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
    if(argc < 3){
        printf("usage: <filename> <msg>...\n");
        return -1;
    }

    int fd = open(argv[1], O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if(fd < 0) {
        perror("Opening file");
        return -1;
    }
    
    dup2(fd, 1);
    close(fd);

    printf("You can see this in %s", argv[1]);
    return 0;
}