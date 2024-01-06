/* #include <tests/integration/src/common_structures.h> */

#if 1
struct tag_struct
{
  int a1; poof( @foo )
  int a2; poof( @bar(baz) )
};

struct nested_tag_struct
{
  tag_struct b1; poof( @bar(b1) )
  tag_struct b2[42]; poof( @bar(b2) )
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
// tests/integration/generated/conditionals/anonymous_tag_struct_oX7F12Cw.h




poof(
  func (nested_tag_struct TStruct) @omit_include
  {
    TStruct.map_members(OuterMember)
    {
      OuterMember.has_tag(bar)?
      {
        (OuterMember.type) (OuterMember.name) has bar tag (value=(OuterMember.tag_value(bar)))
      }

      OuterMember.map_members(Member)
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


