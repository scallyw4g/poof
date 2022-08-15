#include <stdio.h> // We need printf for this example

#define meta(...)


// Here's a pretty useless enum for you.
//
enum my_enum
{
  MyEnum_Foo,
  MyEnum_Bar,
  MyEnum_Baz,
};


//
// Here's a meta-function that creates a new enum augmented with values
// generated from an old enum.
//
// I can't say I can imagine why one would want to do this, but it illustrates
// enum value iteration in a nice way.
//
meta
(
  func make_fancy_new_enum(EnumType)
  {
    enum augmented_(EnumType.name) {

      (EnumType.map_values (EnumValue)
       {
          Augmented_(EnumValue.name),
       })

      Agumented_Voo,
      Agumented_Doo,
      Agumented_Boo,
    };
  }
)

//
// Here's a meta-function that iterates over each value of an enum and outputs
// a call to printf with it's name embedded in a c-string.
meta
(
  func print_enum_values(EnumType)
  {
    (EnumType.map_values (EnumValue) {
      printf("(EnumValue.name)\n");
    })
    printf("\n"); // This isn't necessary, but it'll make the output formatting slightly nicer.
  }
)


// Here we're going to generate a new enum called augmented_my_enum
meta(make_fancy_new_enum(my_enum))
#include <generated/make_fancy_new_enum_my_enum.h>


// We could call it again on the generated enum .. but let's not get too crazy
// meta(make_fancy_new_enum(augmented_my_enum))
// #include <generated/make_fancy_new_enum_augmented_my_enum.h>

int main()
{
  printf("-- test program begin\n\n");


  printf("-- print out my_enum values \n");
  meta(print_enum_values(my_enum))
#include <generated/print_enum_values_my_enum.h>


  printf("-- print out augmented_enum values \n");
  meta(print_enum_values(augmented_my_enum))
#include <generated/print_enum_values_augmented_my_enum.h>


  printf("-- test program end\n");
  return 0;
}


// NOTE(Jesse): The includes are normally generated and written to your source
// files automatically by `poof`.  This functionality is currently broken,
// however it's on the top of my priority list to fix this feature.  Manually
// typing those include names is super annoying.
//
// NOTE(Jesse): `poof` currently only supports absolute includes using angle
// brackets <>.  Supporting relative includes is a small amount of work and
// is also high on my priority list.
//
// NOTE(Jesse): Passing primitives such at:
//
// meta(derive_useless_struct_from(int))
// meta(derive_useless_struct_from(float))
// meta(derive_useless_struct_from(char))
// .. etc
//
// is currently unsupported for somewhat subtle reasons.  It may be worthwhile
// to implement a subset of `poof` functionality for primitives, though doing
// so is not high-priority at this moment.  I've encountered a need to do this
// a few times.
//

