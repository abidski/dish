#include "shell.h"
#include <stdio.h>
#include <stdlib.h>

void readline() {

  int ch;
  char *buffer = malloc(READ_SIZE * (sizeof(char *)));

  printf(" > ");
  ch = getchar();
}
void dish() {

  // get input
  readline();

  // parse input
  // exec
}
int main(int argc, char *argv[]) {

  while (1) {
    dish();
  }

  return EXIT_SUCCESS;
}
