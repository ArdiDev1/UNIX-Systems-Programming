#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    int pipe_fd[2];

    if(pipe(pipe_fd) == -1) {
        perror("Pipe");
        return -1;
    }

    int p_read = pipe_fd[0];
    int p_write = pipe_fd[1];

    pid_t id = fork();

    if(id == 0) {
        int x = 5;
        close(p_read);

        //write to pipe_fd[1]
        write(pipe_fd[p_write], &x, sizeof(int));
        printf("Wrote to %d to pipe\n", x);

        fflush(stdout);
        close(p_write);
        return 0;
    }
    else {
        int y;
        close(p_write);

        read(pipe_fd[p_read], &y, sizeof(int));
        close(p_read);

        wait(NULL);
        printf("Read %d from pipe\n", y);
    }

    printf("hello\n");

    return 0;
}