#include <stdio.h>
#include "sys.h"

#define for_(i, n) for (int i = 0; i < n; i++)

/* IOJLSTZ */

static short size[] = { 4, 2, 3, 3,  3,  3,  3 };
static short len[]  = { 1, 0, 2, 2,  1,  2,  1 };
static short off[]  = { 0, 2, 3, 7, 11, 13, 17 };
static short pt[]   = {
  3840, 17476,  15, 312, 210, 57, 150, 120, 147, 60,
   402,   240, 306, 184, 154, 58, 178, 408, 180,
};
static short pos[2];
static short grid[11];
static short score;

static void flash(short n) {
  ;
}

static void init(void) {
  pos[0] = 4;
  pos[1] = 2;
  for_(i, 10) grid[i] = 1025;
  grid[10] = 2047;
  score = 0;
}

static short check(void) {
  ;
}

static void loop(void) {
  short rot = 0;
  short p = 0;
  for (;;) {
    for_(i, 30) {
      if (press(2)) { p += 1; p %= 7; }
      if (press(8)) { p -= 1; p %= 7; }
      if (press(4)) pos[1] -= 1;
      if (press(6)) pos[1] += 1;
      if (press(10)) { rot -= 1; rot &= ((1 << len[p]) - 1); }
      if (press(11)) { rot += 1; rot &= ((1 << len[p]) - 1); }
      for_(i, 10) for_(j, 9) {
        paint(i, j, 5 * (grid[i] >> (j + 1) & 1));
      }
      for_(i, size[p]) for_(j, size[p]) {
        paint(i + pos[0], j + pos[1],
          7 * (pt[off[p] + rot] >> (size[p] * i) >> j & 1));
      }
      next(5);
    }
  }
}

void run(void) {
  init();
  loop();
}
