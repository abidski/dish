#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *builtins[] = {"cd"};
int (*builtins_func[])(char **) = {&cd};

int num_builtins() { return sizeof(builtins) / sizeof(char *); }
int cd(char **args) {

	if (args[1] == NULL) {
		fprintf(stderr, "expected argument to \"cd\"\n");
	}

	int res = chdir(args[1]);
	if (res != 0) {
		perror("dish");
	}
	return 1;
}
int run(char **args) {

	for (int i = 0; i < num_builtins(); i++) {
		if (strcmp(args[0], builtins[i]) == 0) {
			return (*builtins_func[i])(args);
		}
	}
	return exec(args);
}

int exec(char **args) {

	pid_t pid = fork();
	pid_t wpid;
	int status;
	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror("exec");
		}
		exit(EXIT_FAILURE);

	} else if (pid > 0) {
		// Parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	} else {
		perror("dish");
	}
	return 1;
}

char **parse(char *input) {
	int tok_bufsize = TOK_BUFSIZE;
	char **args = malloc(TOK_BUFSIZE * sizeof(char *));
	char *token = strtok(input, TOK_DELIM);
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

	input = readline();

	args = parse(input);

	run(args);

	free(input);
	free(args);
}

int main(int argc, char *argv[]) {

	while (1) {
		dish();
	}

	return EXIT_SUCCESS;
}
