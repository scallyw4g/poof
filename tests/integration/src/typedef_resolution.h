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
  func f1(t) @omit_include
  {
    t.map (tmem)
    {
      tmem.type tmem.name;
    }
  }
)

typedef struct {
  int foo;
} s4, *s5;


/* typedef struct s5 { */
/*   int foo; */
/* } s6; */



poof(f1(s0))
// tests/integration/generated/typedef_resolution/f1$s0$D3EZG5Gm.h

poof(f1(s1))
// tests/integration/generated/typedef_resolution/f1$s1$KhdvuiPp.h

poof(f1(s2))
// tests/integration/generated/typedef_resolution/f1$s2$51uquobE.h

poof(f1(s3))
// tests/integration/generated/typedef_resolution/f1$s3$mRdWYNyZ.h

poof(f1(s4))
// tests/integration/generated/typedef_resolution/f1$s4$FSuWQjoZ.h

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
// tests/integration/generated/typedef_resolution/f1$e0$PeyUXVBq.h

/* poof(f1(e1)) */

/* poof(f1(e2)) */

poof(f1(e3))
// tests/integration/generated/typedef_resolution/f1$e3$tsw2X4D5.h

/* poof(f1(e1)) */
/* #include <tests/integration/generated/typedef_resolution/f1_e1.h> */

poof(
  func f2(t) @omit_include { t }
)

poof(f2(s0))
// tests/integration/generated/typedef_resolution/f2$s0$gB9VmuCJ.h

poof(f2(s1))
// tests/integration/generated/typedef_resolution/f2$s1$dHlAEku6.h

poof(f2(s2))
// tests/integration/generated/typedef_resolution/f2$s2$NmSZX8vO.h

poof(f2(s3))
// tests/integration/generated/typedef_resolution/f2$s3$2MyrmQ2B.h

poof(f2(s4))
// tests/integration/generated/typedef_resolution/f2$s4$ZxFo6rFI.h

/* poof(f2(s5)) */





poof(f2(e0))
// tests/integration/generated/typedef_resolution/f2$e0$sy7jnXsx.h

poof(f2(e3))
// tests/integration/generated/typedef_resolution/f2$e3$WnwIe0c4.h
