#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "extra.h"

int main(__attribute__((unused)) int argc, char *argv[]) {
  srand(time(0));
  init();
  run(argv[1]);
  printf("exit main\n");
}
