#include <stdio.h>

#include "sys.h"

static short dir;
static short dt;
static short a;
static short b;
static short dv;
static short fb[10][9];
static short ft;
static short grid[10][9];
static short head[2];
static short len;
static short weight;

static void clear(void) {
  for_(i, 10) for_(j, 9) {
    fb[i][j] = 0;
    grid[i][j] = 0;
  }
}

static void flash(void) {
  for_(i, 6) {
    for_(j, 10) for_(k, 9) {
      fb[j][k] = 3 - fb[j][k];
    }
    next(20);
  }
}

static void init(void) {
  dir = 2;
  dt = 20;
  a = 0;
  b = 0;
  fb[4][4] = 4;
  fb[5][4] = 4;
  fb[6][4] = 4;
  ft = 3;
  grid[6][4] = 3;
  grid[7][4] = 2;
  grid[8][4] = 1;
  head[0] = 6;
  head[1] = 4;
  len = 3;
  weight = 3;
}

static void loop(void) {
  for (;;) {
    for_(i, 10) for_(j, 9) {
      if (grid[i][j] > 0 && grid[i][j] < 255) {
        grid[i][j]--;
        if (!grid[i][j]) {
          len--;
          fb[i][j] = 0;
        }
      }
    }
    dv = down();
    if (dir == 2 || dir == 8) {
      switch (dv & ((1 << 4) | (1 << 6))) {
      case 1 << 4: dir = 4; break;
      case 1 << 6: dir = 6; break;
      }
    } else {
      switch (dv & ((1 << 2) | (1 << 8))) {
      case 1 << 2: dir = 2; break;
      case 1 << 8: dir = 8; break;
      }
    }
    switch (dir) {
    case 2: head[0]--; break;
    case 4: head[1]--; break;
    case 6: head[1]++; break;
    case 8: head[0]++; break;
    }
    if (head[0] < 0 || head[0] >= 10) return;
    if (head[1] < 0 || head[1] >= 9)  return;
    if (grid[head[0]][head[1]] == 255) {
      weight++;
      if (!b--) {
        b = ++a;
        dt--;
      }
      ft = rand_(10) | 1;
    } else if (grid[head[0]][head[1]]) {
      return;
    }
    grid[head[0]][head[1]] = weight;
    fb[head[0]][head[1]] = 4;
    len++;
    if (ft) {
      ft--;
      if (!ft) {
        short i, n;
        i = 0;
        n = rand_(90 - len);
        for (;;) {
          while (grid[0][i]) i++;
          if (!n) break;
          i++;
          n--;
        }
        grid[0][i] = 255;
        fb[0][i] = 7;
      }
    }
    next(dt);
  }
}

void run(void) {
  for (;;) {
    init();
    draw(&fb);
    next(dt);
    loop();
    printf("score: %d\n", weight - 3);
    flash();
    draw(0);
    clear();
  }
}
