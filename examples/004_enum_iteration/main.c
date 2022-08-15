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
  func make_fancy_new_enum(Enum)
  {
    enum augmented_(Enum.name) {

      (Enum.map_values (EnumValue)
       {
          Augmented_(EnumValue.name),
       })

      Agumented_Voo,
      Agumented_Doo,
      Agumented_Boo,
    };
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
  printf("> my_enum values \n");

  // Here we generate code to print my_enum values
  meta
  (
    func (my_enum Enum)
    {
      (Enum.map_values (EnumValue) {
        printf("(EnumValue.name)\n");
      })
    }
  )
#include <generated/anonymous_function_my_enum_fd8fda1m.h>

  printf("\n> augmented_enum values \n");


  // Here we generate code to print the augmented_my_enum values
  meta
  (
    func (augmented_my_enum Enum)
    {
      (Enum.map_values (EnumValue) {
        printf("(EnumValue.name)\n");
      })
    }
  )
#include <generated/anonymous_function_augmented_my_enum_qta832h1.h>
}
