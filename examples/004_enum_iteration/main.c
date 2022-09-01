#include <stdio.h>

#define poof(...)

enum my_enum
{
  MyEnum_Foo,
  MyEnum_Bar,
  MyEnum_Baz,
};


int main()
{
  printf("> my_enum values \n");

  // Here we generate code to print my_enum values using an anonymous function
  //
  poof(
    func (my_enum TEnum)
    {
      // The `map_values` keyword iterates over each value in the my_enum type
      // Each value is available as `TEnumValue` in the next scoped block
      TEnum.map_values (TEnumValue)
      {
        printf("TEnumValue.name\n");
      }
    }
  )
#include <generated/anonymous_function_my_enum_fd8fda1m.h>
}
