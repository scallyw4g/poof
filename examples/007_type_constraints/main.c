#include <stdio.h>

#define poof(...)

//
// In this example we'll learn how to use the .contains_type() constraint to
// filter a set of members.  Let's start off by generating the d_union from
// the previous example.
//

struct nested_struct
{
  int NestedInt;
};

struct foo_struct
{
  struct nested_struct FooNested;
};

struct bar_struct
{
  int BarInt;
};

struct baz_struct
{
  int BazInt;
};


poof(
  d_union my_discriminated_union
  {
    foo_struct
    bar_struct
    baz_struct

    flazz       enum_only
  }
)
#include <generated/d_union_my_discriminated_union.h>

int main()
{
  poof(
    //
    // Using the `contains_type` constraint allows us to filter members which
    // contain a specified type.
    //
    // This is incredibly useful in rare cases.  I expect to add more of these
    // constraints in the future, or a more generic system for creating them
    // on-the-fly.
    //
    func (my_discriminated_union StructType)
    {
      printf(" -- Union Members \n");
      StructType.map_members(M)
      {
        M.is_union?
        {
          M.map_members(UM)
          {
            printf("UM.type UM.name\n");
          }
        }
      }

      printf("\n -- Union Members containing a nested_struct\n");
      StructType.map_members(M)
      {
        M.is_union?
        {
          M.map_members(UM)
          {
            UM.contains_type(nested_struct)?
            {
              printf("UM.type UM.name\n");
            }
          }
        }
      }
    }
  )
#include <generated/print_struct_members_my_struct.h>
}
