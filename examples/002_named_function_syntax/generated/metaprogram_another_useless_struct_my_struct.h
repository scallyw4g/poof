    // And here we define a new type derived from whatever type we passed in
    //
    // `poof` looks for identifiers it knows about surrounded by parens ()
    //
    // The (Type.name) statement below is substituted by poof for the name of
    // the type we pass into this function.
    //
    // In this example, we're going to pass in `my_struct` to this function, so
    // the line below will be expanded to `struct metaprogrammed_my_struct`
    //
    struct metaprogrammed_my_struct
    {
      int trevor;
    };

