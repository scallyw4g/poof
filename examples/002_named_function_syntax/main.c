#define poof(...)


struct my_struct
{
  int trevor;
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
    // And here we define a new type derived from whatever type we passed in
    //
    // The Type.name statement below is substituted by poof for the name of
    // the type we pass into this function.
    //
    struct my_metaprogrammed_struct
    {
      struct Type.name whatever;
    };
  }
)



// Here we call the meta func declared above, which poof executes and outputs
// the result of to the include file immediately afterwards.
//
poof(metaprogram_another_struct(my_struct))
#include <generated/metaprogram_another_struct_my_struct.h>



int main()
{
  struct my_struct foo = { .trevor = 1 }; // <-- The original struct we defined

  struct my_metaprogrammed_struct bar = { .whatever = { .trevor = 2 } }; // <-- The one we metaprogrammed!
}


// NOTE(Jesse): The includes are normally generated and written to your source
// files automatically by `poof`.  This functionality is currently broken,
// however it's on the top of my priority list to fix this feature.  Manually
// typing those include names is super annoying.
//
// NOTE(Jesse): Passing primitives such at:
//
// poof(metaprogram_another_struct(int))
// poof(metaprogram_another_struct(float))
// poof(metaprogram_another_struct(char))
// .. etc
//
// is currently unsupported for somewhat subtle reasons.  I've started down the
// path toward supporting this, but it involves writing a type-checker, so
// it'll continue to be unsupported for a while.
//

