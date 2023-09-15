
#define poof(...)

struct s0 {
  int foo;
};

typedef s0 s1;

poof(
  func f1(t)
  {
    t.is_defined?
    {
      // defined
    }
    {
      // undefined
    }
  }
)

poof(f1(s0))
#include <tests/integration/generated/typedef_resolution/f1_s0.h>

poof(f1(s1))
#include <tests/integration/generated/typedef_resolution/f1_s1.h>
