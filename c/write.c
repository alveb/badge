#include "sys.h"

char msg[] = "Flygande b�ckasiner s�ka whila p� mjuka tufvor.";

void run(void) {
  for (;;) {
    write_(msg, sizeof(msg) - 1);
  }
}
