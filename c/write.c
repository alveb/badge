#include "sys.h"

char msg[] = "Flygande b�ckasiner s�ka whila p� mjuka tufvor.";

void run(void) {
  for (;;) {
    write(msg, sizeof(msg) - 1);
  }
}
