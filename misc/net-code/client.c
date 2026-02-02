/*
 ** client.c -- a stream socket client demo
 */

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "myftp.h"

#define DEFAULT_PORT "3103" // the port client will be connecting to

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void die(char *s) {
  if (errno)
    perror(s);
  else
    fprintf(stderr, "Error: %s\n", s);
  exit(EXIT_FAILURE);
}

#define BUF_SIZE (MAX_FILENAME_LEN + 1)
#define CMD_SIZE 20

void print_help(void);

#define my_assert(a)                                                           \
  do {                                                                         \
    if (!(a)) {                                                                \
      fprintf(stderr, "Error: line %d: assert failed.\n", __LINE__);           \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

// skip to the end of the line
// return 0 on success or EOF on EOF
int skip_to_EOL() {
  int c;
  if (feof(stdin))
    return EOF;

  do {
    c = getchar();
  } while (c != EOF && c != '\n');

  if (c == EOF)
    return EOF;
  return 0;
}

// cmd has CMD_SIZE bytes
// return 0 on success or EOF on error
int get_command(char *cmd) {
  if (feof(stdin))
    return EOF;
  // the longest command has 8 or fewer characters
  if (scanf("%8s", cmd) == 1)
    return 0;
  return EOF;
}

// buf should have at least (MAX_FILENAME_LEN + 1) bytes
// > 0: length of the filename
// 0: no file name
// -1: EOF
// -2: too long
int get_filename(char *buf) {
  int c;

  if (feof(stdin))
    return -1;

  // skip the spaces or tabs
  do {
    c = getchar();
  } while (c == ' ' || c == '\t');

  if (c == EOF)
    return -1;

  if (c == '\n') {
    ungetc(c, stdin);
    return 0;
  }

  ungetc(c, stdin);
  if (scanf("%251s", buf) != 1) {
    return -1;
  }
  return 1;
}

void display_error(int sid, RHeader *p) {
  if (p->code != RC_ERROR) {
    fprintf(stderr, "Fatal Error: invalid payload.\n");
    exit(1);
  }
  int len = p->length;
  if (len) {
    char *buf = malloc(len + 1);
    my_assert(buf != NULL);

    int status = recv_all(sid, buf, len);
    my_assert(status == 0);
    buf[len] = 0;
    fprintf(stderr, "%s\n", buf);
    free(buf);
  } else {
    fprintf(stderr, "Error Code. No error message.\n");
  }
}

void doLSCommand(int sid);
void doExitCommand(int sid);
void doGETCommand(int sid);

int main(int argc, char *argv[]) {
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  char *hostname = NULL;
  char port[10] = DEFAULT_PORT; // initialize it with string literals

  for (int i = 1; i < argc; i++) {
    if (!strncmp(argv[i], "-i", 2)) {
    } else if (!strncmp(argv[i], "-p", 2)) {
      int v = atoi(&argv[i][2]);
      if (v < 0 || v >= 65535)
        die("port number must be between 0 and 65535.");
      snprintf(port, sizeof(port), "%d", v);
    } else if (!strcmp(argv[i], "-h")) {
      print_help();
    } else {
      hostname = argv[i];
    }
  }

  if (hostname == NULL)
    print_help();

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(argv[1], port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and connect to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("client: connect");
      close(sockfd);
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
            sizeof s);
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure

  int done = 0;
  while (!done) {
    char opcode[CMD_SIZE];
    int rv = get_command(opcode);
    if (rv < 0) {
      done = 1;
      continue;
    } else if (strcasecmp(opcode, "exit") == 0) {
      doExitCommand(sockfd);
      done = 1;
    } else if (strcasecmp(opcode, "ls") == 0) {
      doLSCommand(sockfd);
    } else if (strcasecmp(opcode, "get") == 0) {
      doGETCommand(sockfd);
    } else {
      fprintf(stderr, "Invalid command.\n");
    }
    if (skip_to_EOL())
      done = 1;
  }
  close(sockfd);

  return 0;
}

void print_help(void) {
  char *helpmsg =
      "Usage: ./client localhost [options]\n"
      "-p<N>    port to connect to (default:3102).\n"
      "-s<N>    time in ms to wait after sending a line (default:0).\n"
      "-c       do not check bye message (default:1).\n"
      "-h       print help message and exit.\n";
  fprintf(stderr, "%s", helpmsg);
  exit(EXIT_FAILURE);
}

void doLSCommand(int sid) {
  Command c = {0};
  RHeader p;
  int status;

  c.code = htonl(CC_LS);

  status = send_all(sid, &c, sizeof(c));
  my_assert(status == 0);
  status = recv_all(sid, &p, sizeof(p));
  my_assert(status == 0);

  p.code = ntohl(p.code);
  p.length = ntohl(p.length);

  if (p.code == RC_TXT) {
    char *buf = malloc(sizeof(char) * p.length + 1);
    my_assert(buf != NULL);
    status = recv_all(sid, buf, p.length);
    my_assert(status == 0);
    buf[p.length] = 0;
    printf("List of files:\n%s\n", buf);
    free(buf);
  } else
    display_error(sid, &p);
}

void doGETCommand(int sid) {
  Command c = {0};
  RHeader p;
  int status;

  c.code = htonl(CC_GET);
  status = get_filename(c.arg);
  if (status <= 0) {
    fprintf(stderr, "No filename was found.\n");
    return;
  }
  fprintf(stderr, "GET: downloading %s ...\n", c.arg);

  status = send_all(sid, &c, sizeof(c));
  my_assert(status == 0);
  status = recv_all(sid, &p, sizeof(p));
  my_assert(status == 0);
  p.code = ntohl(p.code);
  p.length = ntohl(p.length);
  if (p.code == RC_FILE) {
    receiveFileOverSocket(sid, c.arg, ".download", p.length);
    fprintf(stderr, "File %s has been downloaded.\n", c.arg);
  } else
    display_error(sid, &p);
}

void doExitCommand(int sid) {
  Command c = {0};
  int status;
  c.code = htonl(CC_EXIT);
  status = send_all(sid, &c, sizeof(c));
  my_assert(status == 0);
}
