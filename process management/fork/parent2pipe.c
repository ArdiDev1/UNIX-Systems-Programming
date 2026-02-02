#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#define READ_END 0
#define WRITE_END 1

int main() {
    int pipe_to_child[6][2];
    int pipe_to_parent[6][2];

    // create all pipes
    for (int i = 0; i < 6; i++) {
        if (pipe(pipe_to_child[i]) == -1 || pipe(pipe_to_parent[i]) == -1) {
            perror("pipe failed");
            exit(1);
        }
    }

    // ---- CHILD 1 ----
    pid_t id1 = fork();
    if (id1 == 0) {
        close(pipe_to_child[0][WRITE_END]);
        close(pipe_to_parent[0][READ_END]);

        int x;
        read(pipe_to_child[0][READ_END], &x, sizeof(int));
        x++;
        write(pipe_to_parent[0][WRITE_END], &x, sizeof(int));

        close(pipe_to_child[0][READ_END]);
        close(pipe_to_parent[0][WRITE_END]);
        exit(0);
    }

    // ---- CHILD 2 ----
    pid_t id2 = fork();
    if (id2 == 0) {
        close(pipe_to_child[1][WRITE_END]);
        close(pipe_to_parent[1][READ_END]);

        int x;
        read(pipe_to_child[1][READ_END], &x, sizeof(int));
        x++;
        write(pipe_to_parent[1][WRITE_END], &x, sizeof(int));

        close(pipe_to_child[1][READ_END]);
        close(pipe_to_parent[1][WRITE_END]);
        exit(0);
    }

    // ---- CHILD 3 ----
    pid_t id3 = fork();
    if (id3 == 0) {
        close(pipe_to_child[2][WRITE_END]);
        close(pipe_to_parent[2][READ_END]);

        int x;
        read(pipe_to_child[2][READ_END], &x, sizeof(int));
        x++;
        write(pipe_to_parent[2][WRITE_END], &x, sizeof(int));

        close(pipe_to_child[2][READ_END]);
        close(pipe_to_parent[2][WRITE_END]);
        exit(0);
    }

    // ---- CHILD 4 ----
    pid_t id4 = fork();
    if (id4 == 0) {
        close(pipe_to_child[3][WRITE_END]);
        close(pipe_to_parent[3][READ_END]);

        int x;
        read(pipe_to_child[3][READ_END], &x, sizeof(int));
        x++;
        write(pipe_to_parent[3][WRITE_END], &x, sizeof(int));

        close(pipe_to_child[3][READ_END]);
        close(pipe_to_parent[3][WRITE_END]);
        exit(0);
    }

    // ---- CHILD 5 ----
    pid_t id5 = fork();
    if (id5 == 0) {
        close(pipe_to_child[4][WRITE_END]);
        close(pipe_to_parent[4][READ_END]);

        int x;
        read(pipe_to_child[4][READ_END], &x, sizeof(int));
        x++;
        write(pipe_to_parent[4][WRITE_END], &x, sizeof(int));

        close(pipe_to_child[4][READ_END]);
        close(pipe_to_parent[4][WRITE_END]);
        exit(0);
    }

    // ---- CHILD 6 ----
    pid_t id6 = fork();
    if (id6 == 0) {
        close(pipe_to_child[5][WRITE_END]);
        close(pipe_to_parent[5][READ_END]);

        int x;
        read(pipe_to_child[5][READ_END], &x, sizeof(int));
        x++;
        write(pipe_to_parent[5][WRITE_END], &x, sizeof(int));

        close(pipe_to_child[5][READ_END]);
        close(pipe_to_parent[5][WRITE_END]);
        exit(0);
    }

    // ---- PARENT ----
    for (int i = 0; i < 6; i++) {
        close(pipe_to_child[i][READ_END]);
        close(pipe_to_parent[i][WRITE_END]);
    }

    // talk to each child
    for (int i = 0; i < 6; i++) {
        int val = i;
        write(pipe_to_child[i][WRITE_END], &val, sizeof(int));
        close(pipe_to_child[i][WRITE_END]); // done writing to this child

        int result;
        read(pipe_to_parent[i][READ_END], &result, sizeof(int));
        close(pipe_to_parent[i][READ_END]);

        printf("Parent sent %d to child %d, got back %d\n", val, i + 1, result);
    }

    // wait for all children
    for (int i = 0; i < 6; i++)
        wait(NULL);

    return 0;
}
