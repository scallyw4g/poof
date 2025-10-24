#define poof(...)

#define MacroIdentifier
#define MacroFunc(foo, bar, baz) foo bar baz

typedef __ undefined_type;

// NOTE(Jesse): Copied from bonsai_stdlib/primitives.h

// 8 Bit types
typedef char           s8;
typedef unsigned char  u8;

// 16 Bit types
typedef short          s16;
typedef unsigned short u16;

// 32 Bit types
typedef int          s32;
typedef unsigned int u32;
typedef float        r32;
typedef u32          b32;

typedef r32      radians;
typedef r32      degrees;

// 64 Bit types
typedef long long int           s64;
typedef unsigned long long int  u64;
typedef double                  r64;
typedef u64                     umm;
typedef u64                     b64;

#define upshift_by_31 << 31
enum foo_enum
{
  foo_enum_value_0 = 1,
  foo_enum_value_1 = 'a',
  foo_enum_value_2 = (1 upshift_by_31),
  foo_enum_value_3
};


struct array_struct
{
  int a1[2];
  u64 a2[42];
  void* a3[420];
};

struct doubly_nested_struct
{
};

struct nested_struct
{
  doubly_nested_struct DoublyNested;
};

struct bar_struct;
struct bar_struct
{
  int fda;
};


struct counted_string {
  int count;
  const char *data;
};

int foofunc(int foo, void *bar, thing **baz)
{
}

typedef foofunc typedefd_foofunc;

typedef int (*foofuncptr)(void);
typedef int (foofunc0)(void);
typedef int foofunc1(void);
typedef foo_enum foofunc2(int);


struct foo_struct_0 poof(@tag)
{
};

struct foo_struct_0 poof(@tag_with_value(69))
{
};

struct foo_struct_1
{
  int *first[0] = 42, *second = -1;

  int bar = 0;
  int baz = (1 << 1) -1;

  float fbaz = 42.0f;
  double dbaz = 42.0;
  long double ldbaz = 42.0;
  long long ldbaz = 42.0;
  long int libaz = 42.0;

  char cbaz = 'f';

  enum foo_enum fenum  = foo_enum_value_0;
  foo_enum fenum2;

  nested_struct foo;

  undefined_type _und0;


  char           _char;
  int            _int;
  s32            _s32;
  u32            _u32;
  u64            _u64;
  umm            _umm;
  const char*    _ccstar;
  int **         _intstarstar[42];
  umm*           _ummstar;

  /* enum enum_type */
  /* { */
  /*   named_enum_value0, */
  /*   named_enum_value1, */
  /* } _named_enum; */

  /* enum */
  /* { */
  /*   anon_enum_value0, */
  /*   anon_enum_value1, */
  /* } _anon_enum; */

  foo_enum _foo_enum;

  undefined_type _und0;
  foo_struct_0   _foo0;


  struct
  {
    int _int2;
  } _embedded1, _embedded2;

  union
  {
    int _int3;
  } _embedded3;

  counted_string S1; // Ensures the special-case hacks we put in work.  This is actually compound, but we want it considered primitive.

  foofuncptr FooFuncPtr;

  foofunc0 FooFun;
  foofunc1 FooFun1;
  foofunc2 FooFun2;

  void **(*func_type_name)(void) AnotherFunc;
  void **(*func_type_name)(void) AnotherFunc[2];

  u32 NamedThing;
};



poof(
  func poof_noop(ignored) @code_fragment {})

// NOTE(Jesse): This just tests we can do a poof inside an enum
enum poof_enum
{
  enum_value_0,
  poof(poof_noop(foo_struct_0))
#include <tests/integration/generated/accessors/poof_noop_foo_struct_0.h>
};

// NOTE(Jesse): This just tests we can do a poof inside a struct
struct poof_struct
{
  int foo;
  poof(poof_noop(foo_struct_0))
#include <tests/integration/generated/accessors/poof_noop_foo_struct_0.h>
  float bar;
};
