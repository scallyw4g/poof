v0.1.0

* Added @var

  Users can now define local poof variables from an expression, for example:
  ```
  func my_func(input_var)
  {
      @var local_var (input_var.name)_static_string

      local_var
  }

  my_func(foo) // calling my_func() expands to foo_static_string
  ```


* Added triple-slash poof comment

  Comments with three slashes `///` are now swallowed by poof such that they do
  not end up in the output code.  Useful for commenting poof code itself.

* Fix bug when omitting poof operator on a struct definition

  Previously, the following would emit non-intuitive, and never useful
  ```
  struct foo { ... };

  func my_func(input_var)
  {
    input_var
  }

  my_func(foo) /// now expands to `struct foo`, instead of `foo foo`
  ```
