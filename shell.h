#ifndef SHELL_H
#define SHELL_H

#define READ_SIZE 1024
#define TOK_BUFSIZE 64
#include <stdio.h>
#include <stdlib.h>


void dish();
char * readline(void);
char ** parse(char*  );




#endif
