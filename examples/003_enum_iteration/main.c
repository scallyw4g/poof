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
// generated from another enum.
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


  // Here we generate code to print my_enum values
  printf("-- print out my_enum values \n");
  meta(print_enum_values(my_enum))
#include <generated/print_enum_values_my_enum.h>


  // Here we generate code to print augmented_my_enum values
  printf("-- print out augmented_enum values \n");
  meta(print_enum_values(augmented_my_enum))
#include <generated/print_enum_values_augmented_my_enum.h>


  printf("-- test program end\n");
  return 0;
}
