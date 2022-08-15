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


// Structure iteration is similar to enum value iteration, however we use
// `map_members` instead of `map_values`.
//
// For the sake of consistency, those APIs may merge these into just `map` in
// the future.  TBD.
meta
(
  func print_struct_members(StructType)
  {
      (StructType.map_members (Member)
       {
         printf("(Member.type) (Member.name)\n");
         (Member.is_struct? {
            (print_struct_members(Member))
          })
       })
   printf("\n");
  }
)




int main()
{
  printf("-- test program begin\n\n");

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

  printf("-- print out my_struct values \n");
  meta(print_struct_members(my_struct))
#include <generated/print_struct_members_my_struct.h>


  printf("-- test program end\n");
  return 0;
}
