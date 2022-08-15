// `poof` programs compile in two steps.
//
// First, `poof` runs over the program and executes metaprogramming routines as
// specified.  The resulting code is output to header files that are included
// inline immediately after where the meta code was written.
//
// Secondly, our program gets compiled with a regular C or C++ compiler.  In
// the case of these examples we'll use clang, but any compiler will do.
//


// One of the core design philosophies of `poof` is that:
//
// "If the tool disappears, I should be left with a working program"
//
// All `poof` code lives inside the `meta` macro.

#define meta(...)

// As you can see, we can put any arbitrary junk inside the meta macro and the
// subsequent compilation step with `clang` will be unaffected.


int main()
{
  meta(Type whatever random crap in here you want clang will ignore it)
  return 0;
}

// This is not a valid `poof` program, we'll get to that in the next example,
// but it's important to understand that `poof` code evaporates during regular
// compilation.

