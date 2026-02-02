#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>

#define MAX 10240
#define MAX_WORD_COUNT 60000                //we have less than 60000 words
#define MAX_WORD_LENGTH 80                  //each word is less than 80 letters

char words[MAX_WORD_COUNT][MAX_WORD_LENGTH];        //2-d array to hold all the words
int word_count = 0;                                 //number of words, initilized to 0

void read_file_to_array(char *filename)
{
    FILE *fp;

    fp = fopen(filename, "r");
    if(fp==NULL)
    {
        printf("Cannot open file %s.\n", filename);
        exit(-1);
    }
 
    while(!feof(fp))
        fscanf(fp, "%s\n", words[word_count++]);
    fclose(fp);
}

int in_dict(char* msg) {
    int res, l, r = word_count;

    l = 0;
    while (l <= r) {
        int mid = (l + r) / 2;

        res = strcmp(msg, words[mid]);
        if(res == 0)
            return 1;

        if(res > 0) {
            l = mid + 1;
        }
        else {
            r = mid - 1;
        }
    }
    
    return 0;
}

int main(int argc, char** argv) {
    read_file_to_array("dict.txt");
    char* msg = "a";
    
    int i, len = strlen(msg);

    i = 0;
    while (i < len) {

        if(isalpha(msg[i])) {
            char buff[80];
            int j = i;
        
            while (isalpha(msg[j]) && j < len) {
                buff[j - i] = msg[j];
                j++;
            }

            buff[j - i] = '\0';
            i = j;

            printf("%s \n", buff);
        }

        i += 1;
    }
    
    return 0;
}