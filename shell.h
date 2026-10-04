#ifndef SHELL_H
#define SHELL_H

#define READ_SIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


void dish();
char * readline(void);
char ** parse(char*,int * num );
int exec (char **);
int run (char **);
int cd (char **);
int num_builtins();
int handle_pipe(char **, int * num);




#endif
