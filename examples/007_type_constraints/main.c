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

// The d_union keyword asks `poof` to please generate a discriminated union structure
// for us from the specified structures.
//
// The enum_only keyword specifies that an additional `type` enum value should
// be generated with the given name (in this case, flazz), but there is not an
// accompanying struct definition.
//
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
  printf("-- test program begin\n\n");


  meta
  (

    // Using the `containing` constraint allows us to filter members which
    // contain a specified type.
    func (my_discriminated_union StructType)
    {
      (StructType.map_members(Member).containing(nested_struct)
      {
         printf("(Member.type) (Member.name)\n");
      })
    }
  )
#include <generated/print_struct_members_my_struct.h>


  printf("\n-- test program end\n");
  return 0;
}
