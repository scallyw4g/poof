
typedef __ undefined_type;

typedef float f32;

typedef int s32;

struct foo_struct
{
  int            _int;
  s32            _s32;
  f32            _float;
  undefined_type _und0;
};

meta(
  func (foo_struct TFooStruct)
  {
    (TFooStruct.map_members(Member){
      (Member.is_defined? {
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

