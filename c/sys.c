#define _POSIX_C_SOURCE 199309L

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "extra.h"
#include "sys.h"

static short bv; /* button vector */
static short pv; /* press vector */
static short hv; /* held vector */
static short (*fb)[10][9]; /* frame buffer */
static long fs; /* frame start */

/* X stuff */
static Display *disp;
static Window win;
static GC gc;
static Atom del;
static long pal[8];

static long gettime(void) {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return 100 * t.tv_sec + t.tv_nsec / 1e7;
}

static void initpal(void)
{
  int s = DefaultScreen(disp);
  Colormap m = DefaultColormap(disp, s);
  XColor c = { 0 };
  c.flags = DoRed;
  for (int i = 0; i < 8; i++) {
    c.red = 0x2222 * i;
    int r = XAllocColor(disp, m, &c);
    if (!r) {
      printf("failed to allocate color\n");
      exit(1);
    }
    pal[i] = c.pixel;
  }
}

static void updisp(void)
{
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 9; j++) {
      XSetForeground(disp, gc, pal[fb ? (*fb)[i][j] : 0]);
      XFillRectangle(disp, win, gc, 40 * j, 40 * i, 40, 40);
    }
  }
  XFlush(disp);
}

static void press_(int n) {
  bv |= 1 << n;
  pv |= 1 << n;
  hv |= 1 << n;
}

static void rel(int n) {
  bv &= ~(1 << n);
}

static void hand(void) {
  XEvent e;
  KeySym s;
  while (XPending(disp)) {
    XNextEvent(disp, &e);
    switch (e.type) {
    case KeyPress:
      s = XLookupKeysym(&e.xkey, 0);
      switch (s) {
      case XK_1: case XK_KP_1:             press_( 1); break;
      case XK_2: case XK_KP_2: case XK_w:  press_( 2); break;
      case XK_3: case XK_KP_3:             press_( 3); break;
      case XK_4: case XK_KP_4: case XK_a:  press_( 4); break;
      case XK_5: case XK_KP_5:             press_( 5); break;
      case XK_6: case XK_KP_6: case XK_d:  press_( 6); break;
      case XK_7: case XK_KP_7:             press_( 7); break;
      case XK_8: case XK_KP_8: case XK_s:  press_( 8); break;
      case XK_9: case XK_KP_9:             press_( 9); break;
      case XK_0: case XK_KP_0:             press_( 0); break;
      case XK_minus:           case XK_j:  press_(10); break;
      case XK_equal:           case XK_i:  press_(11); break;
      case XK_q: printf("exit sys\n"); exit(0);
      }
      break;
    case KeyRelease:
      s = XLookupKeysym(&e.xkey, 0);
      switch (s) {
      case XK_1: case XK_KP_1:             rel( 1); break;
      case XK_2: case XK_KP_2: case XK_w:  rel( 2); break;
      case XK_3: case XK_KP_3:             rel( 3); break;
      case XK_4: case XK_KP_4: case XK_a:  rel( 4); break;
      case XK_5: case XK_KP_5:             rel( 5); break;
      case XK_6: case XK_KP_6: case XK_d:  rel( 6); break;
      case XK_7: case XK_KP_7:             rel( 7); break;
      case XK_8: case XK_KP_8: case XK_s:  rel( 8); break;
      case XK_9: case XK_KP_9:             rel( 9); break;
      case XK_0: case XK_KP_0:             rel( 0); break;
      case XK_minus:           case XK_j:  rel(10); break;
      case XK_equal:           case XK_i:  rel(11); break;
      }
      break;
    case Expose:
      if (e.xexpose.count == 0) updisp();
      break;
    case ClientMessage:
      if ((Atom) e.xclient.data.l[0] == del) {
        printf("exit sys\n"); exit(0);
      }
      break;
    }
  }
}

static void dispose(void) {
  if (gc) XFreeGC(disp, gc);
  if (win) XDestroyWindow(disp, win);
  if (disp) XCloseDisplay(disp);
}

void init(void) {
  atexit(dispose);
  disp = XOpenDisplay(0);
  if (!disp) {
    printf("failed to open display\n");
    exit(1);
  }
  initpal();
  win = XCreateSimpleWindow(disp,
    RootWindow(disp, DefaultScreen(disp)),
    0, 0, 40 * 9, 40 * 10, 1,
    pal[0], pal[0]
  );
  XSelectInput(disp, win,
    ExposureMask | KeyPressMask | KeyReleaseMask
  );
  del = XInternAtom(disp, "WM_DELETE_WINDOW", 0);
  XSetWMProtocols(disp, win, &del, 1);
  XMapWindow(disp, win);
  gc = XCreateGC(disp, win, 0, 0);
  fs = gettime();
  bv = pv = hv = 0;
}

static void sel(long to) {
  int fd = ConnectionNumber(disp);
  struct timeval t = { to / 100, 1e4 * (to % 100) };
  fd_set rfds; FD_ZERO(&rfds); FD_SET(fd, &rfds);
  select(fd + 1, &rfds, 0, 0, &t);
}

void next(short dt) {
  pv = 0;
  hv = bv;
  updisp();
  sel(0);
  hand();
  for (;;) {
    long now, rem;
    now = gettime();
    rem = dt - (now - fs);
    if (rem <= 0) break;
    sel(rem);
    hand();
  }
  fs += dt;
}

short rand_(short n) {
  return (rand() & 0xffff) % n;
}

void draw(short (*p)[10][9]) { fb = p; }
short press(void) { return pv; }
short down(void) { return hv; }
