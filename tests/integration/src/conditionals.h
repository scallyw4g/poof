

typedef __ undefined_type;

typedef int s32;

struct foo_struct_0
{
  int            _int;
  s32            _s32;
  undefined_type _und0;
};

#if 1
meta(
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




struct foo_struct_1
{
  int            _int;
  s32            _s32;
  u32            _u32;
  u64            _u64;
  umm            _umm;
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

  int _foo1;

};


meta(
  func (foo_struct_1 TFooStruct)
  {
    (TFooStruct.map_members(Member){
      (Member.is_compound?
      {
        (Member.type) (Member.name) is compound
      }
      {
        (Member.type) (Member.name) not compound
      }
      )
    })
  }
)
#include <tests/integration/generated/anonymous_func_1.h>


meta(
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
