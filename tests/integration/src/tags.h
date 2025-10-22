/* #include <tests/integration/src/common_structures.h> */

#if 1
struct tag_struct
{
  int a1; poof( @foo )
  int a2; poof( @bar(a2) )
};


poof(
  func (tag_struct TStruct) @omit_include
  {
    TStruct.map_members(Member)
    {
      Member.has_tag(foo)?
      {
        (Member.type) (Member.name) has foo tag
      }

      Member.has_tag(bar)?
      {
        (Member.type) (Member.name) has bar tag (value=(Member.tag_value(bar)))
      }
    }
  }
)
// tests/integration/generated/tags/anonymous_tag_struct_oX7F12Cw.h



struct nested_tag_struct
{
  tag_struct b1; poof( @bar(b1) )
  tag_struct b2[42]; poof( @bar(b2) )
};

poof(
  func (nested_tag_struct TStruct) @omit_include
  {
    TStruct.map_members(OuterMember)
    {
      // has_tag bar (outer members)
      OuterMember.has_tag(bar)?
      {
        ((OuterMember)) has bar tag (value=(OuterMember.tag_value(bar)))
      }

      // has_tag foo (inner members)
      OuterMember.map_members(Member)
      {
        Member.has_tag(foo)?
        {
          ((OuterMember))  >  ((Member)) has foo tag
        }
      }

      // has_tag bar (inner members)
      OuterMember.map_members(Member)
      {
        Member.has_tag(bar)?
        {
          ((OuterMember))  >  ((Member)) has bar tag (value=(Member.tag_value(bar)))
        }
      }
    }
  }
)
// tests/integration/generated/tags/anonymous_nested_tag_struct_oX7F12Cw.h

#endif



struct tagged_struct poof( @foo @bar(baz) )
{
  int a1;
};

poof(
  func (tagged_struct TStruct) @omit_include
  {
    TStruct.has_tag(foo)?
    {
      (TStruct.name) has tag foo
    }

    TStruct.has_tag(bar)?
    {
      (TStruct.name) has tag bar (value=(TStruct.tag_value(bar)))
    }
  }
)
// tests/integration/generated/tags/anonymous_tagged_struct_yZhxgWx1.h



enum foo_enum poof(@foo)
{
  enum_value_0 poof(@foo_tag(0)),
  enum_value_1,
  enum_value_2,
};


poof(
  func (foo_enum e) @omit_include
  {
    e.has_tag(foo)?
    {
      e.name has tag foo! poof(@this_should_have_an_at_sign)
    }

    e.map_values(ev)
    {
      ev.has_tag(foo_tag)? { ev.name ev.tag_value(foo_tag) }
    }
  }
)
// tests/integration/generated/tags/anonymous_foo_enum_5rPlBIF8.h



struct has_numeric_tag poof(@numeric_tag(3))
{
  int foo;
};

poof(
  func (has_numeric_tag type) @omit_include
  {
    type.tag_value(numeric_tag).map(index)
    {
      index
    }
  }
)
// tests/integration/generated/tags/anonymous_has_numeric_tag_W4DIb6aG.h



poof(
  for_datatypes(struct enum) @omit_include

  func (struct_t)
  {
    struct ((struct_t)) struct_t.tags
  }

  func (enum_t)
  {
    enum ((enum_t)) enum_t.tags
  }
)
// tests/integration/generated/tags/for_datatypes_gGnNeVTa.h

