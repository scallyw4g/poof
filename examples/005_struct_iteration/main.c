#include <stdio.h>

#define meta(...)


struct my_struct
{
  int foo;
  float bar;
  struct my_struct *baz;
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

//
// TODO(Jesse, immediate): The output of this is actually quite broken.
// Correct output is a printf call for each member of my_struct.  Something like:
//
// printf("int foo");
// printf("float bar");
// printf("struct my_struct * baz");
//


