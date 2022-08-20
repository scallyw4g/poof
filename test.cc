#include <stdio.h>

struct foo
{
  int bar = 2, baz = 1;
};

int main()
{
  foo instance = {};
  printf("%d %d", instance.bar, instance.baz);
}
