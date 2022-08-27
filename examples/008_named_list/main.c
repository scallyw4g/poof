#define meta(...)


// The debug_print example mentions named_list, so we should probably introduce
// it here.

// TODO(Jesse): Write some example code that shows what we can do with one!

struct foo
{
  int _fooInt;
};

struct bar
{
  int _barInt;
};


meta( named_list(my_list) { foo bar })

int main()
{
}
