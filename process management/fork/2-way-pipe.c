#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
  int x = 5;
  // Declare two pipe arrays
  int pipe1[2], pipe2[2];

  // Create the first pipe
  if (pipe(pipe1) == -1) {
    perror("cannot create pipe 1");
    exit(1);
  }

  if (pipe(pipe2) == -1) {
    perror("cannot create pipe 2");
    exit(1);
  }

  // Fork a child process
  int p;
  p = fork();

  if(p == 0) {
    int z;
    close(pipe1[1]);
    close(pipe2[0]);

    read(pipe1[0], &z, sizeof(int));

    z++;

    write(pipe2[1], &z, sizeof(int));

    close(pipe1[0]);
    close(pipe2[1]);

  }
  else {
    int y;

    close(pipe1[0]);
    close(pipe2[1]);

    write(pipe1[1], &x, sizeof(int));
    read(pipe2[0], &y, sizeof(int));

    printf("y is equal to: %d\n", y);

    close(pipe1[1]);
    close(pipe2[0]);
  }

  return 0;
}