#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Helper function to close all pipe fds except the ones to keep open
void close_all_pipes_except(int pipes[][2], int num_pipes, int keep_read_fd, int keep_write_fd) {
    for (int i = 0; i < num_pipes; i++) {
        if (pipes[i][0] != keep_read_fd)
            close(pipes[i][0]);
        if (pipes[i][1] != keep_write_fd)
            close(pipes[i][1]);
    }
}

int main() {
    int pipes[4][2]; // 4 pipes: p->c1, c1->c2, c2->c3, c3->p
    int num_pipes = 4;

    // Create pipes
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    pid_t pid1 = fork();
    if (pid1 == -1) { perror("fork"); exit(1); }

    if (pid1 == 0) {
        // Child1
        // Keep only read end of p->c1 and write end of c1->c2 open
        close_all_pipes_except(pipes, num_pipes, pipes[0][0], pipes[1][1]);

        int num;
        read(pipes[0][0], &num, sizeof(int));
        close(pipes[0][0]);

        num++;
        printf("Child1 incremented number to %d\n", num);

        write(pipes[1][1], &num, sizeof(int));
        close(pipes[1][1]);

        _exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == -1) { perror("fork"); exit(1); }

    if (pid2 == 0) {
        // Child2
        // Keep only read end of c1->c2 and write end of c2->c3 open
        close_all_pipes_except(pipes, num_pipes, pipes[1][0], pipes[2][1]);

        int num;
        read(pipes[1][0], &num, sizeof(int));
        close(pipes[1][0]);

        num++;
        printf("Child2 incremented number to %d\n", num);

        write(pipes[2][1], &num, sizeof(int));
        close(pipes[2][1]);

        _exit(0);
    }

    pid_t pid3 = fork();
    if (pid3 == -1) { perror("fork"); exit(1); }

    if (pid3 == 0) {
        // Child3
        // Keep only read end of c2->c3 and write end of c3->p open
        close_all_pipes_except(pipes, num_pipes, pipes[2][0], pipes[3][1]);

        int num;
        read(pipes[2][0], &num, sizeof(int));
        close(pipes[2][0]);

        num++;
        printf("Child3 incremented number to %d\n", num);

        write(pipes[3][1], &num, sizeof(int));
        close(pipes[3][1]);

        _exit(0);
    }

    // Parent process
    // Keep only write end of p->c1 and read end of c3->p open
    close_all_pipes_except(pipes, num_pipes, pipes[3][0], pipes[0][1]);

    int start_num = 0;
    write(pipes[0][1], &start_num, sizeof(int));
    close(pipes[0][1]);

    int final_num;
    read(pipes[3][0], &final_num, sizeof(int));
    close(pipes[3][0]);

    printf("Parent received final number: %d\n", final_num);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);

    return 0;
}