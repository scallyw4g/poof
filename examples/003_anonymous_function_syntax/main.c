#include <stdio.h>

#define meta(...)

struct my_struct
{
  int foo;
};

int main()
{
  // A meta function that does not specify a name is an anonymous function and
  // is executed immediately.  It's required that an anonymous function
  // specifies a type for its type argument.
  //
  // This is particularly useful when paired with type constraints for avoiding
  // a certain class of bugs.
  //
  meta
  (
    func (my_struct StructType)
    {
      printf("(StructType.name)");
    }
  )
#include <generated/print_struct_members_my_struct.h>
}
