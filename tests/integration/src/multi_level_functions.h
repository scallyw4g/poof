
#define poof(...)

poof(
  func f1(t)
  {
    // f1
    t.map_members(t_mem)
    {
      // f1 map
      t_mem.type t_mem.name;
    }
  }
)

poof(
  func f2(t)
  {
    // f2
    t.map_members(t_mem)
    {
      // f2 map
      f1(t_mem)
    }
  }
)

struct child0
{
  int foo;
};

struct child1
{
  float bar;
};

struct parent
{
  child0 C0;
  child1 C1;
};

poof( f2(parent) )
#include <tests/integration/generated/multi_level_functions/f2_foo.h>
