#include "sys.h"
#include <stdio.h>

#define for_(i, n) for (int i = 0; i < n; i++)

static short dir;
static short dt;
static short a;
static short b;
static short ft;
static short grid[10][9];
static short head[2];
static short len;
static short weight;

static void clear(void) {
  for_(i, 10) for_(j, 9) {
    draw(i, j, 0);
    grid[i][j] = 0;
  }
}

static void flash(void) {
  for_(i, 3) {
    for_(j, 10) for_(k, 9) {
      draw(j, k, 4 * (grid[j][k] == 0));
    }
    next(20);
    for_(j, 10) for_(k, 9) {
      draw(j, k, 4 * (grid[j][k] != 0));
    }
    next(20);
  }
}

static void init(void) {
  dir = 2;
  dt = 20;
  a = 0;
  b = 0;
  draw(4, 4, 4);
  draw(5, 4, 4);
  draw(6, 4, 4);
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
      if (grid[i][j] > 0) {
        grid[i][j]--;
        if (!grid[i][j]) {
          len--;
          draw(i, j, 0);
        }
      }
    }
    if (dir == 2 || dir == 8) {
      if (held(4) && !held(6)) dir = 4;
      if (held(6) && !held(4)) dir = 6;
    } else {
      if (held(2) && !held(8)) dir = 2;
      if (held(8) && !held(2)) dir = 8;
    }
    if      (dir == 2) head[0]--;
    else if (dir == 4) head[1]--;
    else if (dir == 6) head[1]++;
    else if (dir == 8) head[0]++;
    if (head[0] < 0 || head[0] >= 10) return;
    if (head[1] < 0 || head[1] >= 9)  return;
    if (grid[head[0]][head[1]] == -1) {
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
    draw(head[0], head[1], 4);
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
        grid[0][i] = -1;
        draw(i / 9, i % 9, 7);
      }
    }
    next(dt);
  }
}

void run(void) {
  for (;;) {
    init();
    next(dt);
    loop();
    printf("score: %d\n", weight - 3);
    flash();
    clear();
  }
}
