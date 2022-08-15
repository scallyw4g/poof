#include <stdio.h>

#define meta(...)

struct my_struct
{
  int foo;
  float bar;
};


int main()
{
  printf("-- test program begin\n\n");


// A meta function that does not specify a name is an anonymous function and is
// executed immediately.  It's required that an anonymous function specifies a
// type for its type argument.
//
// This is particularly useful when paired with type constraints for avoiding
// a certain class of bug.
//
meta
(
  func (my_struct StructType)
  {
    (StructType.map_members (Member)
    {
       printf("(Member.type) (Member.name)\n");
    })
    printf("\n");
  }
)
#include <generated/print_struct_members_my_struct.h>


  printf("-- test program end\n");
  return 0;
}
