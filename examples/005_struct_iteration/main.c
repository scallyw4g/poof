#include <stdio.h> // We need printf for this example

#define meta(...)


// Here's a pretty useless datatype for you.
//
struct my_struct
{
  int foo;
  float bar;

  struct
  {
    struct my_struct * my_foo;
    struct my_struct ** my_bar;
  } baz;
};




int main()
{
  // TODO(Jesse, immediate): The output of this is actually quite broken.
  // Correct output is a printf call for each member of my_struct, and
  // my_struct::baz.  Something like:
  //
  // printf("int foo");
  // printf("float bar");
  // printf("struct baz");
  // printf("struct my_struct * my_foo");
  // printf("struct my_struct ** my_bar");
  //
  //


  // Structure iteration is similar to enum value iteration, however we use
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
