
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

poof(f1(s0))
#include <tests/integration/generated/typedef_resolution/f1_s0.h>

poof(f1(s1))
#include <tests/integration/generated/typedef_resolution/f1_s1.h>

poof(f1(s2))
#include <tests/integration/generated/typedef_resolution/f1_s2.h>

poof(f1(s3))
#include <tests/integration/generated/typedef_resolution/f1_s3.h>
