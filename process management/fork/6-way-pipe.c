#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int pipes[6][2];

    // create all pipes first
    for (int i = 0; i < 6; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Pipe failed");
            exit(1);
        }
    }

    // --- Child 1 ---
    pid_t id1 = fork();
    if (id1 == 0) {
        int x = 0;
        close(pipes[0][0]); // close read end of first pipe
        write(pipes[0][1], &x, sizeof(int));
        close(pipes[0][1]);
        exit(0);
    }
    close(pipes[0][1]); // parent keeps read side open

    // --- Child 2 ---
    pid_t id2 = fork();
    if (id2 == 0) {
        int x;
        close(pipes[1][0]);
        read(pipes[0][0], &x, sizeof(int));
        close(pipes[0][0]);
        x++;
        write(pipes[1][1], &x, sizeof(int));
        close(pipes[1][1]);
        exit(0);
    }
    close(pipes[1][1]);
    close(pipes[0][0]);

    // --- Child 3 ---
    pid_t id3 = fork();
    if (id3 == 0) {
        int x;
        close(pipes[2][0]);
        read(pipes[1][0], &x, sizeof(int));
        close(pipes[1][0]);
        x++;
        write(pipes[2][1], &x, sizeof(int));
        close(pipes[2][1]);
        exit(0);
    }
    close(pipes[2][1]);
    close(pipes[1][0]);

    // --- Child 4 ---
    pid_t id4 = fork();
    if (id4 == 0) {
        int x;
        close(pipes[3][0]);
        read(pipes[2][0], &x, sizeof(int));
        close(pipes[2][0]);
        x++;
        write(pipes[3][1], &x, sizeof(int));
        close(pipes[3][1]);
        exit(0);
    }
    close(pipes[3][1]);
    close(pipes[2][0]);

    // --- Child 5 ---
    pid_t id5 = fork();
    if (id5 == 0) {
        int x;
        close(pipes[4][0]);
        read(pipes[3][0], &x, sizeof(int));
        close(pipes[3][0]);
        x++;
        write(pipes[4][1], &x, sizeof(int));
        close(pipes[4][1]);
        exit(0);
    }
    close(pipes[4][1]);
    close(pipes[3][0]);

    // --- Child 6 ---
    pid_t id6 = fork();
    if (id6 == 0) {
        int x;
        close(pipes[5][0]);
        read(pipes[4][0], &x, sizeof(int));
        close(pipes[4][0]);
        x++;
        write(pipes[5][1], &x, sizeof(int));
        close(pipes[5][1]);
        exit(0);
    }
    close(pipes[5][1]);
    close(pipes[4][0]);

    // --- Parent reads final value ---
    int result;
    read(pipes[5][0], &result, sizeof(int));
    close(pipes[5][0]);

    printf("Final value after 6 processes: %d\n", result);

    // Wait for all children
    for (int i = 0; i < 6; i++)
        wait(NULL);

    return 0;
}
