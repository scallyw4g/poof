#define meta(...)


// Here's a pretty useless datatype for you.
//
struct my_struct
{
  int whatever;
};




// Now, let's define our first meta function.  We're going to make a new struct
// type using `my_struct` we defined above.
meta
(
  // Functions in `poof` are defined using the func keyword.  Here we define
  // derive_useless_struct_from, which takes a type argument.
  func derive_useless_struct_from(Type)
  {
    // And here we define a new type derived from whatever type we passed in
    //
    // `poof` looks for identifiers it knows about surrounded by parens ()
    //
    // The (Type.name) statement below is substituted by poof for the name of
    // the type we pass into this function.
    //
    struct (Type.name)_derived
    {
      int ignored;
    };
  }
)



// Here we call the meta func declared above, which poof executes and outputs
// the result of to the include file immediately afterwards.
meta(derive_useless_struct_from(my_struct))
#include <generated/babys_first_meta_func_my_struct_derived.h>



int main()
{
  struct my_struct foo = {}; // <-- This is the original struct we defined
  struct my_struct_derived bar = {}; // <-- And this is the one we metaprogrammed!

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

