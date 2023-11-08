#include <tests/integration/src/common_structures.h>

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



#if 1
poof(
  func (foo_struct_1 TFooStruct)
  {
    TFooStruct.map_members(Member)
    {
      Member.contains_type(doubly_nested_struct)?
      {
        (Member.type) (Member.name) contains a doubly_nested_struct
      }
    }
  }
)
#include <tests/integration/generated/conditionals/anonymous_foo_struct_1_KLiRNdrO.h>



#endif


#if 1
poof(
  func (foo_struct_1 TFooStruct)
  {
    TFooStruct.map_members(Member)
    {
      Member.is_type(nested_struct)?
      {
        (Member.type) (Member.name) is a nested_struct
      }
    }
  }
)
#include <tests/integration/generated/conditionals/anonymous_foo_struct_1_gB856lL7.h>
#endif

poof(
  func (foo_struct_1 TFooStruct)
  {
    TFooStruct.map_members(Member)
    {
      Member.is_named(NamedThing)?
      {
        (Member.type) (Member.name) is named NamedThing
      }
    }
  }
)
#include <tests/integration/generated/conditionals/anonymous_foo_struct_1_rHXPMI2N.h>

poof(
  func (foo_struct_1 TFooStruct)
  {
    TFooStruct.map_members(Member)
    {
      Member.is_pointer?
      {
        (Member.type) (Member.name) is a pointer
      }
      {
        (Member.type) (Member.name) not a pointer
      }
    }
  }
)
#include <tests/integration/generated/conditionals/anonymous_foo_struct_1_ACfUY3gL.h>
