#define poof(...)


struct my_struct
{
  int MyStructInt;
};




// Let's define our first meta function!  We're going to make a new struct
// using `my_struct` we defined above.
//
poof(
  // Functions in `poof` are defined using the func keyword.  Here we define
  // metaprogram_another_struct, which takes a type argument.
  //
  func metaprogram_another_struct(Type)
  {
    // The Type.name statement below is substituted for the name of the type we
    // pass into this function.
    //
    struct my_metaprogrammed_struct
    {
      struct Type.name MetaStructMember;
    };
  }
)

// NOTE: Another way to write the metaprogram_another_struct function, without
// poof, would be the following:
//
// #define metaprogram_another_struct(Type) struct my_metaprogrammed_struct { struct Type whatever; };
//
// Poof does a lot of stuff that the C preprocessor can't, but for simple
// operations like this there are 1:1 mappings
//


// Here we call the meta func declared above, which poof executes and outputs
// to the include file immediately afterwards.
//
// Poof automatically generates and writes the include to your source files if
// you choose to not write it yourself.  It first generates a name for the file
// from the function name and type you pass it, then concatenates the value of
// the (--output-path|-o) arguemnt passed on the command line.
//
poof(metaprogram_another_struct(my_struct))
#include <generated/metaprogram_another_struct_my_struct.h>



int main()
{
  struct my_struct MyStructInstance = { .MyStructInt = 1 }; // <-- The original struct we defined

  struct my_metaprogrammed_struct MetaStruct = {}; // <-- The one we metaprogrammed!

  MetaStruct.MetaStructMember = MyStructInstance;
}


// NOTE(Jesse): Passing primitives such as:
//
// poof(metaprogram_another_struct(int))
// poof(metaprogram_another_struct(float))
// poof(metaprogram_another_struct(char))
// .. etc
//
// is currently unsupported for somewhat subtle reasons.  I've started down the
// path toward supporting this, but it involves writing a type-checker, so
// it'll continue to be unsupported for a while.  Thankfully, I've yet to run
// into a situation where I needed to do this.
//

