#include <stdio.h>

#define poof(...)

struct my_struct
{
  int foo;
};

int main()
{
  // A poof function that does not specify a name is an anonymous function and
  // is executed immediately.  It's required that an anonymous function
  // specifies a type for its type argument.
  //
  // Anonymous functions are particularly useful when paired with type
  // constraints to generate code that avoids certain classes of bugs.  We'll
  // get to that in a later example.
  //
  poof(
    func (my_struct StructType)
    {
      printf("StructType.name");
    }
  )
#include <generated/print_struct_members_my_struct.h>
}
