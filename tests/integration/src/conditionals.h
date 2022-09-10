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


struct foo_struct_0
{
  int            _int;
  s32            _s32;
  undefined_type _und0;
};

struct counted_string {
  int count;
  const char* data;
};

#if 1
poof(
  func (foo_struct_0 TFooStruct)
  {
    TFooStruct.map_members(Member)
    {
      Member.is_defined?
      {
        (Member.type) (Member.name)
      }
      {
        type not defined for ((Member.type) (Member.name))
      }
    }
  }
)
#include <tests/integration/generated/conditionals/anonymous_foo_struct_0_ROYeHpk2.h>
#endif


int foofunc(void)
{
}

enum foo_enum
{
  foo_enum_val0,
  foo_enum_val1,
  foo_enum_val2,
};


typedef int (*foofuncptr)(void);
typedef int (foofunc0)(void);
typedef int foofunc1(void);
typedef foo_enum foofunc2(int);

struct foo_struct_1
{
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

  void **(*func_type_name)(void) AnotherFunc[2];
};


#if 1
poof(
  func (foo_struct_1 TFooStruct)
  {
    TFooStruct.map_members(Member)
    {
      Member.is_enum?
      {
        (Member.type) (Member.name) is enum
      }
      {
        (Member.type) (Member.name) not enum
      }
    }
  }
)
#include <tests/integration/generated/conditionals/anonymous_foo_struct_1_ZpWJur7o.h>
#endif

#if 1
poof(
  func (foo_struct_1 TFooStruct)
  {
    TFooStruct.map_members(Member)
    {
      Member.is_struct?
      {
        (Member.type) (Member.name) is struct
      }
      {
        (Member.type) (Member.name) not struct
      }
    }
  }
)
#include <tests/integration/generated/conditionals/anonymous_foo_struct_1_m33iB55j.h>
#endif

#if 1
poof(
  func (foo_struct_1 TFooStruct)
  {
    TFooStruct.map_members(Member)
    {
      Member.is_defined?
      {
        (Member.type) (Member.name) defined
      }
      {
        (Member.type) (Member.name) not defined
      }
    }
  }
)
#include <tests/integration/generated/conditionals/anonymous_foo_struct_1_IjaKQKyX.h>
#endif


#if 1
poof(
  func (foo_struct_1 TFooStruct)
  {
    TFooStruct.map_members(Member)
    {
      Member.is_compound?
      {
        (Member.type) (Member.name) is compound
      }
      {
        (Member.type) (Member.name) not compound
      }
    }
  }
)
#include <tests/integration/generated/conditionals/anonymous_foo_struct_1_w3dGd6mh.h>
#endif


#if 1
poof(
  func (foo_struct_1 TFooStruct)
  {
    TFooStruct.map_members(Member)
    {
      Member.is_primitive?
      {
        (Member.type) (Member.name) is primitive
      }
      {
        (Member.type) (Member.name) not primitive
      }
    }
  }
)
#include <tests/integration/generated/conditionals/anonymous_foo_struct_1_TObfsxb6.h>
#endif


#if 1
poof(
  func (foo_struct_1 TFooStruct)
  {
    TFooStruct.map_members(Member)
    {
      Member.is_function?
      {
        (Member.type) (Member.name) is a function
      }
      {
        (Member.type) (Member.name) not a function
      }
    }
  }
)
#include <tests/integration/generated/conditionals/anonymous_foo_struct_1_YvJnQLUi.h>
#endif
