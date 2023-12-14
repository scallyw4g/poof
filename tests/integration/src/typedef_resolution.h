#define poof(...)


struct s0 {
  int foo;
};

struct s1 {
  s0 S0;
};


typedef s1 s2;
typedef s2 s3;

poof(
  func f1(t)
  {
    t.map (tmem)
    {
      tmem.type tmem.name;
    }
  }
)

typedef struct {
  int foo;
} s4;


/* typedef struct s5 { */
/*   int foo; */
/* } s6; */



poof(f1(s0))
#include <tests/integration/generated/typedef_resolution/f1_s0.h>

poof(f1(s1))
#include <tests/integration/generated/typedef_resolution/f1_s1.h>

poof(f1(s2))
#include <tests/integration/generated/typedef_resolution/f1_s2.h>

poof(f1(s3))
#include <tests/integration/generated/typedef_resolution/f1_s3.h>

poof(f1(s4))
#include <tests/integration/generated/typedef_resolution/f1_s4.h>

/* poof(f1(s5)) */

/* poof(f1(s6)) */




typedef enum e0 {
  foo,
  bar,
  baz
} e0;


/* typedef enum e1 { */
/*   foo, */
/*   bar, */
/*   baz */
/* } e2; */

typedef enum {
  foo,
  bar,
  baz
} e3;


poof(f1(e0))
#include <tests/integration/generated/typedef_resolution/f1_e0.h>

/* poof(f1(e1)) */

/* poof(f1(e2)) */

poof(f1(e3))
#include <tests/integration/generated/typedef_resolution/f1_e3.h>

/* poof(f1(e1)) */
/* #include <tests/integration/generated/typedef_resolution/f1_e1.h> */
