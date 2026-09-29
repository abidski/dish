#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **parse(char *input) {
	int tok_bufsize = TOK_BUFSIZE;
	char **args = malloc(TOK_BUFSIZE * sizeof(char *));
	char *token = strtok(input, " ");
	int index = 0;
	while (token != NULL) {
		if (index > tok_bufsize) {
			tok_bufsize += tok_bufsize;
			args = realloc(args, tok_bufsize);
		}
		args[index] = token;
		token = strtok(NULL, " ");
		index++;
	}

	return args;
}

char *readline() {

	int ch;
	int i = 0;
	int buffer_size = READ_SIZE;
	char *buffer = malloc(buffer_size * (sizeof(char)));

	printf(" > ");

	while (1) {

		ch = getchar();

		if (i > buffer_size - 1) {
			buffer_size += READ_SIZE;
			buffer = realloc(buffer, buffer_size);
		}
		if (ch == EOF || ch == '\n') {
			break;
		}

		buffer[i] = ch;
		i++;
	}
	buffer[i] = '\0';
	return buffer;
}
void dish() {

	char *input;
	char **args;

	// get input
	input = readline();

	// parse input
	args = parse(input);
	/*
	for (int i = 0; args[i] != NULL; i++) {
		printf("args[%d] = %s\n", i, args[i]);
	}
	*/

	// exec
}
int main(int argc, char *argv[]) {

	while (1) {
		dish();
	}

	return EXIT_SUCCESS;
}
