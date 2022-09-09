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

#if 0
poof(
  func (foo_struct_0 TFooStruct)
  {
    (TFooStruct.map_members(Member){
      (Member.is_defined?
      {
        (Member.type) (Member.name)
      }
      {
        type not defined for ((Member.type) (Member.name))
      }
      )
    })
  }
)
#include <tests/integration/generated/anonymous_func_0.h>
#endif


int foofunc(void)
{
}


typedef int (*foofuncptr)(void);
typedef int (foofunc)(void);

struct foo_struct_1
{
  int            _int;
  s32            _s32;
  u32            _u32;
  u64            _u64;
  umm            _umm;
  const char*    _ccstar;
  int **         _intstarstar[42];
  umm*           _ummstar;

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
  foofunc FooFun;
};


#if 0
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
#include <tests/integration/generated/anonymous_func_1.h>
#endif


#if 0
poof(
  func (foo_struct_1 TFooStruct)
  {
    (TFooStruct.map_members(Member){
      (Member.is_primitive?
      {
        (Member.type) (Member.name) is primitive
      }
      {
        (Member.type) (Member.name) not primitive
      }
      )
    })
  }
)
#include <tests/integration/generated/anonymous_func_2.h>
#endif


#if 0
poof(
  func (foo_struct_1 TFooStruct)
  {
    (TFooStruct.map_members(Member){
      (Member.is_function?
      {
        (Member.type) (Member.name) is a function
      }
      {
        (Member.type) (Member.name) not a function
      }
      )
    })
  }
)
#include <tests/integration/generated/anonymous_func_3.h>
#endif
