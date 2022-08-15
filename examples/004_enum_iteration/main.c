#include <stdio.h>

#define meta(...)

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
  meta
  (
    func (my_enum Enum)
    {
      // The `map_values` keyword iterates over each value in the my_enum type
      // Each value is available as `EnumValue` in the next scoped block {}
      (Enum.map_values (EnumValue)
       {
        printf("(EnumValue.name)\n");
      })
    }
  )
#include <generated/anonymous_function_my_enum_fd8fda1m.h>
}
