#include <stdio.h>

#define poof(...)


struct my_struct
{
  int foo;
  float bar;
  long baz;
};

int main()
{
  // Struct member iteration is similar to enum value iteration, however we use
  // `map_members` instead of `map_values`.
  //
  // For the sake of consistency, those APIs will likely merge into a more
  // singular `map` in the future.  TBD.
  //
  poof(
    func (my_struct Type)
    {
      Type.map_members (Member)
      {
        printf("Member.type Member.name\n");
      }
    }
  )
#include <generated/anonymous_function_my_struct_f8p25x1m.h>
}

