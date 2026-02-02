#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

int main() {
    int fd[2];

    if(pipe(fd) < 0) {
        perror("pipe()");
        return -1;
    } 

    pid_t proc_id = fork();
    if(proc_id == 0) {
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);

        execlp("echo", "echo", "Hello world", NULL);
        exit(-1);
    }

    close(fd[1]);

    pid_t proc_id1 = fork();
    if(proc_id1 == 0) {
        dup2(fd[0], 0);
        close(fd[0]);

        execlp("wc", "wc", NULL);
        exit(-1);
    }

    close(fd[0]);

    waitpid(proc_id, NULL, 0);
    waitpid(proc_id1, NULL, 0);
    return 0;
}