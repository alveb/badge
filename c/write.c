#include "sys.h"

char msg[] = "Flygande bäckasiner söka whila på mjuka tufvor.";

void run(void) {
  for (;;) {
    write(msg, sizeof(msg) - 1);
  }
}
