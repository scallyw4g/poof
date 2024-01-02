#include <tests/integration/src/common_structures.h>

#if 1
poof(
  func (foo_struct_1 TFooStruct) @omit_include
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
// tests\integration\generated\conditionals/anonymous_foo_struct_1_8guZPTf0.h
#endif

#if 1
poof(
  func (foo_struct_1 TFooStruct) @omit_include
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
// tests\integration\generated\conditionals/anonymous_foo_struct_1_h7hirkAZ.h
#endif

#if 1
poof(
  func (foo_struct_1 TFooStruct) @omit_include
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
// tests\integration\generated\conditionals/anonymous_foo_struct_1_HxUplAeU.h
#endif


#if 1
poof(
  func (foo_struct_1 TFooStruct) @omit_include
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
// tests\integration\generated\conditionals/anonymous_foo_struct_1_LVid4PGg.h
#endif


#if 1
poof(
  func (foo_struct_1 TFooStruct) @omit_include
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
// tests\integration\generated\conditionals/anonymous_foo_struct_1_CPfSoIS7.h
#endif


#if 1
poof(
  func (foo_struct_1 TFooStruct) @omit_include
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
// tests\integration\generated\conditionals/anonymous_foo_struct_1_QAGBtJOS.h
#endif



#if 1
poof(
  func (foo_struct_1 TFooStruct) @omit_include
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
// tests\integration\generated\conditionals/anonymous_foo_struct_1_BaBAMF6m.h
#endif


#if 1
poof(
  func (foo_struct_1 TFooStruct) @omit_include
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
// tests\integration\generated\conditionals/anonymous_foo_struct_1_6M8tx4gp.h
#endif

poof(
  func (foo_struct_1 TFooStruct) @omit_include
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
// tests\integration\generated\conditionals/anonymous_foo_struct_1_xuh38nWl.h

poof(
  func (foo_struct_1 TFooStruct) @omit_include
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
// tests\integration\generated\conditionals/anonymous_foo_struct_1_dlHRwaDJ.h

