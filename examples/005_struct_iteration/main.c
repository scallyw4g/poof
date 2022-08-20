#include <stdio.h>

#define meta(...)


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
  // For the sake of consistency, those APIs may merge into just `map` in the
  // future.  TBD.
  meta
  (
    func (my_struct Type)
    {
      (Type.map_members (Member)
       {
         printf("(Member.type) (Member.name)\n");
       })
    }
  )
#include <generated/anonymous_function_my_struct_f8p25x1m.h>
}

