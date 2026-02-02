#ifndef __MYFTP_H_
#define __MYFTP_H_

#include<stddef.h>

#define CC_LS 0
#define CC_GET 1
#define CC_PUT 2
#define CC_EXIT 3
#define CC_SIZE 4

#define RC_TXT 0
#define RC_FILE 1
#define RC_SIZE 2
#define RC_ERROR 9

#define MAX_FILENAME_LEN 251

typedef struct CommantTag {
  int code;                       // one of CC_*
  char arg[MAX_FILENAME_LEN + 1]; // the last byte of arg must be 0
} Command;

typedef struct RHeaderTag {
  int code; // one of RC_*
  int length;
} RHeader;

char *makeFileList(char *path);
long getFileSize(char *fName);
int sendFileOverSocket(char *fName, int chatSocket);
int receiveFileOverSocket(int sid, char *fname, char *ext, int fSize);

int check_filename(char *fn);

int send_all(int sid, void *buf, size_t n);
int send_str(int sid, char *s);
int recv_all(int sid, void *buf, size_t n);

#endif
