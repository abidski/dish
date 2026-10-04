#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* builtins[] = { "cd" };
int (*builtins_func[])(char**) = { &cd };

int num_builtins() { return sizeof(builtins) / sizeof(char*); }
int cd(char** args) {

	if (args[1] == NULL) {
		fprintf(stderr, "expected argument to \"cd\"\n");
	}

	int res = chdir(args[1]);
	if (res != 0) {
		perror("dish");
	}
	return 1;
}
int run(char** args) {

	for (int i = 0; i < num_builtins(); i++) {
		if (strcmp(args[0], builtins[i]) == 0) {
			return (*builtins_func[i])(args);
		}
	}
	return exec(args);
}

int exec(char** args) {

	pid_t pid = fork();
	pid_t wpid;
	int status;
	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror("dish");
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

int handle_pipe(char** args, int* size) {

	char*** commands = malloc(*size * sizeof(char**));
	for (int k = 0; k < *size; k++) {
		commands[k] = malloc((*size + 1) * sizeof(char*)); // room for *size words + NULL
	}

	int* pipes = malloc(0);
	int splits_found = 0;
	char* pipe_output;

	for (int i = 0; i < *size; i++) {

		if (strcmp(args[i], "|") == 0) {
			splits_found += 1;
			pipes = realloc(pipes, splits_found * sizeof(int));
			pipes[i - 1] = i;

			int fd[2];
			int status;
			pid_t wpid;
			pid_t pid;

			if (pipe(fd) == -1) {
				perror("dish");
			}

			pid = fork();

			if (splits_found <= 1) {
				if (pid == 0) {
					dup2(fd[1], STDOUT_FILENO);
					close(fd[0]);
					close(fd[1]);
					execvp(commands[splits_found - 1][0], commands[splits_found - 1]);
				} else if (pid > 0) {
					close(fd[1]);
					close(fd[0]);

					do {
						wpid = waitpid(pid, &status, WUNTRACED);
					} while (!WIFEXITED(status) && !WIFSIGNALED(status));

					char* output;
					char buf[256];
					ssize_t prev_size;
					ssize_t n;
					while ((n = read(fd[0], buf, sizeof buf)) > 0) {

						output = realloc(output, n + prev_size);
						memcpy(output + n + prev_size, buf, n);
						prev_size += n;
					}
				}
				continue;
			} else {

				if (pid == 0) {
					dup2(fd[1], STDOUT_FILENO);
					close(fd[0]);
					close(fd[1]);
					execvp(commands[splits_found - 1][0], commands[splits_found - 1]);
				} else if (pid > 0) {
					close(fd[1]);
					close(fd[0]);

					do {
						wpid = waitpid(pid, &status, WUNTRACED);
					} while (!WIFEXITED(status) && !WIFSIGNALED(status));
				}
				continue;
			}
		}
		commands[splits_found][i] = args[i];
	}
	if (splits_found == 0) {
		return 1;
	}
	return 0;
}

char** parse(char* input, int* size) {
	int tok_bufsize = TOK_BUFSIZE;
	char** args = malloc(TOK_BUFSIZE * sizeof(char*));
	char* token = strtok(input, TOK_DELIM);
	int index = 0;

	while (token != NULL) {
		if (index > tok_bufsize) {
			tok_bufsize += tok_bufsize;
			args = realloc(args, tok_bufsize * sizeof(char*));
		}
		args[index] = token;
		token = strtok(NULL, " ");
		index++;
	}

	*size = index;

	return args;
}

char* readline() {

	int ch;
	int i = 0;
	int buffer_size = READ_SIZE;
	char* buffer = malloc(buffer_size * (sizeof(char)));

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

	char* input;
	char** args;
	int args_num;

	input = readline();

	args = parse(input, &args_num);

	if (handle_pipe(args, &args_num) == 1) {
		run(args);
	}

	free(input);
	free(args);
}

int main(int argc, char* argv[]) {

	while (1) {
		dish();
	}

	return EXIT_SUCCESS;
}
