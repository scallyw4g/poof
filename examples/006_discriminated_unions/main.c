#include <stdio.h>

#define poof(...)

struct foo_struct
{
  int foo_int;
};

struct bar_struct
{
  int bar_int;
};

struct baz_struct
{
  int baz_int;
};

//
// Here's where things start to get a little more flavorful.
//
// The d_union keyword asks `poof` to generate a discriminated union structure
// for us, from the specified structs.
//
// The enum_only keyword specifies that an additional enum value should be
// generated with the given name (in this case, flazz), but there is not an
// accompanying struct definition.
//
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
  printf(" -- Enum Values\n");

  // Here, we generate code to print the enum tag values
  poof(
    func (my_discriminated_union_type Enum)
    {
      Enum.map_values (Val)
      {
        printf("Val.name\n");
      }
    }
  )
#include <generated/print_enum_values_fd9ff432f.h>

  printf("\n -- Struct Members\n");

  // Here, we generate code to print the discriminated union members from the struct we generated
  poof(
    func (my_discriminated_union StructType)
    {
      StructType.map_members (Member)
      {
        printf("Member.type Member.name\n");
      }
    }
  )
#include <generated/print_struct_members_my_struct.h>
}
