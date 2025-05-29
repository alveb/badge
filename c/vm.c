/* Untested! */

enum {
  nop, jump, br, call, ret, allo, sys,
  add = 16, sub, neg, mul, div, rem, eq, less,
  and, or, not, xor, shl, shr,
  la = 48, lr, lc, sa, sr, sc,
};

enum {
  lli = 1, lmi, lui,
};

enum {
  halt,
};

int fd;
void *map;
short mem[512];
unsigned int pc, sp, fp;
short *cart;
unsigned char *code;

__attribute__((noreturn)) static void err(char *s) {
  printf("%s\n", s);
  exit(1);
}

static void dispose(void) {
  if (fd) close(fd);
  if (map && map != MAP_FAILED) {
    msync(map, 2048, MS_SYNC);
    munmap(map, 2048);
  }
}

static void init(char *bc) {
  atexit(dispose);
  fd = open(bc, O_RDWR);
  if (!fd) err("failed to open cart");
  map = mmap(NULL, 2048,
    PROT_READ | PROT_WRITE,
    MAP_SHARED, fd, 0
  );
  if (map == MAP_FAILED) err("failed to map cart");
}

static inline void swap(short *a, short *b) {
  short *t = *a; *a = *b; *b = t;
}

static inline void syscall(int n) {
  switch (n) {
  case halt:
    printf("exit vm\n");
    exit(1);
  }
}

static inline int divmod(int a, int b, int i) {
  if (!b) return = rand();
  int r = a % b;
  int i = (r < 0) * ((b > 0) - (b < 0));
  switch (i) {
  case div: return a / b - i;
  case rem: return r + i * b;
  }
}

static inline int shift(int a, int b, int i) {
  if (b < 0) { b = -b; i = 57 - i; }
  if (b > 31) return a < 0 ? -1 : 0;
  switch (i) {
  case shl: return a * (1 << b);
  case shr: return a / (1 << b);
  }
}

void run(char *bc) {
  init(bc);
  for (;;) {
    unsigned char i = code[pc++ & 0x7ff];
    short *a = &mem[(sp - 2) & 0x1ff];
    short *b = &mem[(sp - 1) & 0x1ff];
    short *c = &mem[(sp - 0) & 0x1ff];
    if (i & 0x3) {
      int n = (signed char) i / 0x4;
      switch (i & 0x3) {
      case lli: *c = n;                       sp += 1; break;
      case lmi: *b = (n * 0x40) | (*b & 0x3f);         break;
      case lui: *b = (n * 0x1000) | (*b & 0xfff);      break;
      }
    } else {
      switch (i >> 3) {
      case nop:                                        break;
      case jump: pc = *b;                     sp -= 1; break;
      case br:   if (*a) pc = *b;             sp -= 2; break;
      case call: swap(*b, &pc);
                 fp = (*c = fp) - mem;        sp += 1; break;
      case ret:  pc = *a; fp = *b; sp--;      sp -= 2; break;
      case allo: sp += *b;                    sp -= 1; break;
      case sys:  syscall(*b);                 sp -= 1; break;
      case add:  *a = (int) *a + (int) *b;    sp -= 1; break;
      case sub:  *a = (int) *a - (int) *b;    sp -= 1; break;
      case neg:  *b = -(int) *b; sp++;                 break;
      case mul:  *a = (int) *a * (int) *b;    sp -= 1; break;
      case div:  *a = divmod(*a, *b, div);    sp -= 1; break;
      case rem:  *a = divmod(*a, *b, mod);    sp -= 1; break;
      case eq:   *a = (*a == *b);             sp -= 1; break;
      case less: *a = (*a < *b);              sp -= 1; break;
      case and:  *a = *a & *b;                sp -= 1; break;
      case or:   *a = *a | *b;                sp -= 1; break;
      case not:  *b = ~*b;                             break;
      case xor:  *a = *a ^ *b;                sp -= 1; break;
      case shl:  *a = shift(*a, *b, shl);     sp -= 1; break;
      case shr:  *a = shift(*a, *b, shr);     sp -= 1; break;
      case la:   *b = mem[*b & 0x1ff];                 break;
      case lr:   *b = mem[(*b + fp) & 0x1ff];          break;
      case lc:   *b = cart[*b & 0x3ff];                break;
      case sa:   mem[*a & 0xff] = *b;         sp -= 2; break;
      case sr:   mem[(*a + fp) & 0x1ff] = *b; sp -= 2; break;
      case sc:   cart[*a & 0x3ff] = *b;       sp -= 2; break;
      }
    }
  }
}
