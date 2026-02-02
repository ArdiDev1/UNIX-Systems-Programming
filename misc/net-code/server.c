/*
 ** server.c -- a stream socket server demo
 */

#include "myftp.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define PORT "3103" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int create_handler(int new_fd);

int main(void) {
  int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  int yes = 1;
  char s[INET6_ADDRSTRLEN];
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and bind to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo); // all done with this structure

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  printf("server: waiting for connections on port %s...\n", PORT);

  while (1) { // main accept() loop
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
              s, sizeof s);

    // call a function to create a thread to deal with the client
    fprintf(stderr, "server: got connection from %s. fd is %d.\n", s, new_fd);
    if (create_handler(new_fd)) {
      close(new_fd);
    }
  }

  return 0;
}

#define BUF_SIZE 1024

// Use a structure, although we hvae only one field
// It is easier to add more fields in other programs
typedef struct thread_arg_tag {
  int sockfd;
  char *filelist;
} thread_arg_t;

void thread_die(thread_arg_t *arg) {
  printf("thread using socket fd %d exiting ...\n", arg->sockfd);
  close(arg->sockfd);
  free(arg->filelist);
  free(arg);
  pthread_exit(NULL);
}

#define my_assert(a)                                                           \
  do {                                                                         \
    if (!(a)) {                                                                \
      fprintf(stderr, "Error: line %d\n", __LINE__);                           \
      thread_die(arg);                                                         \
    }                                                                          \
  } while (0)

/* The main function of the thread handling the client.
 *
 * This is a long function. Could divide into smaller functions.
 * */
void *thread_main(void *arg_in) {
  thread_arg_t *arg = arg_in;
  int sockfd = arg->sockfd;

  arg->filelist = NULL;

  int done = 0;
  do {
    Command c;
    RHeader p;
    char *errmsg = NULL;

    int status = recv_all(sockfd, &c, sizeof(Command));
    my_assert(status == 0);

    c.arg[MAX_FILENAME_LEN] = 0; // client may be malicious
    c.code = ntohl(c.code);      // adjust endianness

    switch (c.code) {
    case CC_LS: {
      fprintf(stderr, "socket %d: Received LS.\n", sockfd);
      char *msg = makeFileList(".");
      if (msg) {
        arg->filelist = msg;
        int msg_len = strlen(msg);
        p.code = htonl(RC_TXT);
        p.length = htonl(msg_len);
        status = send_all(sockfd, &p, sizeof(p));
        my_assert(status == 0);
        status = send_all(sockfd, msg, msg_len);
        my_assert(status == 0);
        free(msg);
        arg->filelist = NULL;
      } else
        errmsg = "LS: Error: Cannot get file list.";
    } break;
    case CC_GET: { // send the named file back to client
      fprintf(stderr, "socket %d: Received GET '%s'.\n", sockfd, c.arg);
      long fileSize = (check_filename(c.arg) < 0) ? -1 : getFileSize(c.arg);
      if (fileSize >= 0) {
        p.code = htonl(RC_FILE);
        p.length = htonl(fileSize);
        status = send_all(sockfd, &p, sizeof(p));
        my_assert(status == 0);
        fprintf(stderr, "socket %d: header was sent.\n", sockfd);
        if (fileSize) {
          status = sendFileOverSocket(c.arg, sockfd);
          my_assert(status == 0);
          fprintf(stderr, "socket %d: file '%s' was sent.\n", sockfd, c.arg);
        }
      } else
        errmsg = "GET: Error: Invalid file name.";
    } break;
    case CC_EXIT:
      fprintf(stderr, "socket %d: Received EXIT.\n", sockfd);
      done = 1;
      break;
    default:
      // no response to error command code
      fprintf(stderr, "socket %d: Unknown command.\n", sockfd);
      break;
    }
    if (errmsg) {
      fprintf(stderr, "socket %d: %s\n", sockfd, errmsg);

      int len = strlen(errmsg);

      p.code = htonl(RC_ERROR);
      p.length = htonl(len);
      status = send_all(sockfd, &p, sizeof(p));
      my_assert(status == 0);
      if (p.length > 0) {
        status = send_all(sockfd, errmsg, len);
        my_assert(status == 0);
      }
    }
  } while (!done);

  thread_die(arg);
  return NULL;
}

/* create a thread to talk to the client.
 *
 * Return values:
 * 0:   success
 * -1:  error. new_fd is not closed.
 * */
int create_handler(int new_fd) {
  pthread_t tid;
  int rv;

  thread_arg_t *arg = malloc(sizeof(thread_arg_t));

  if (arg == NULL)
    return -1;

  arg->sockfd = new_fd;

  rv = pthread_create(&tid, NULL, thread_main, arg);
  if (rv) {
    free(arg);
    return -1;
  }

  // not much we can do if it fails. could just kill the process.
  pthread_detach(tid);
  return 0;
}
