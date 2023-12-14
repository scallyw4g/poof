
#define poof(...)


poof(
  func foobaz(type) @omit_include
  {
    foobaZ!
  }
)

struct thing
{
  int foo;
  int baz;
};

poof(foobaz(thing))
// tests/integration/generated/functions/foobaz_.h

