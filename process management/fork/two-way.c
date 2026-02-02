#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LEN 1024

#define P_READ 0
#define P_WRITE 1

int main() {

    int pipe_1[2];

    if(pipe(pipe_1) == 1)
        return -1;

    int pipe_2[2];
    
    if(pipe(pipe_2) == 1)
        return -1;

    pid_t pid = fork();


    if(pid == 0) {
        //close unused descriptors
        close(pipe_1[P_WRITE]);
        close(pipe_2[P_READ]);

        char* msg = "World";

        char buf[MAX_LEN];
        int i = 0;

        while (read(pipe_1[P_READ], &buf[i++], sizeof(char)) > 0);
        buf[i-1] = '\0';

        printf("Read '%s' from parent\n", buf);
        close(pipe_1[P_READ]);

        write(pipe_2[P_WRITE], msg, strlen(msg));
        printf("Wrote '%s' to parent\n", msg);

        close(pipe_2[P_WRITE]);
        return 0;
    }

    // close unused file descriptors
    close(pipe_1[P_READ]);
    close(pipe_2[P_WRITE]);

    char* msg = "Hello";

    // write message to parent
    write(pipe_1[P_WRITE], msg, strlen(msg));
    printf("Wrote '%s' to child\n", msg);

    close(pipe_1[P_WRITE]);

    char buf[MAX_LEN];
    int i = 0;

    while (read(pipe_2[P_READ], &buf[i++], sizeof(char)) > 0);
    buf[i-1] = '\0';

    printf("Read '%s' from child\n", buf);

    wait(NULL);
    return 0;
}