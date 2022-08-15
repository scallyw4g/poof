#include <stdio.h>

#define meta(...)

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


meta
(
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
  meta
  (
    // Using the `containing` constraint allows us to filter members which
    // contain a specified type.
    //
    // This is incredibly useful in rare cases.  I expect to add more of these
    // constraints in the future, or a more generic system for creating them
    // on-the-fly.
    func (my_discriminated_union StructType)
    {
      (StructType.map_members(Member).containing(nested_struct)
      {
         printf("(Member.type) (Member.name)\n");
      })
    }
  )
#include <generated/print_struct_members_my_struct.h>
}
