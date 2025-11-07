// callsite
// tests/integration/src/for_datatypes.h:19:0

// def ((builtin.for_datatypes))
// tests/integration/src/for_datatypes.h:19:0
name(foofunc2)
type((function))
value(foo_enum)
array()
hash(34EC700A)
indirection()
tags()
location(tests/integration/src/common_structures.h:90)


function args
{
  
}






//

name(s16)
type(short )
value((value unsupported))
array()
hash(290D30A5)
indirection()
tags()
location((unknown):-1)








//

name(foofunc0)
type((function))
value(foofunc0)
array()
hash(1BC148A5)
indirection()
tags()
location(tests/integration/src/common_structures.h:88)


function args
{
  
}






//

name(u16)
type(unsigned short )
value((value unsupported))
array()
hash(290D30A7)
indirection()
tags()
location((unknown):-1)








//

name(foo_enum)
type(enum)
value((value unsupported))
array()
hash(284708AD)
indirection()
tags()
location(tests/integration/src/common_structures.h:35)






enum values
{
    foo_enum_value_0 foo_enum_value_0 = 1 
  foo_enum_value_1 foo_enum_value_1 = 'a' 
  foo_enum_value_2 foo_enum_value_2 = (1 << 31) 
  foo_enum_value_3 foo_enum_value_3 

}


//

name(print_type_data)
type(meta_func)
value((value unsupported))
array()
hash(163648B1)
indirection()
tags()
location(tests/integration/src/for_datatypes.h:5)








//

name(foo_struct_1)
type(struct)
value((value unsupported))
array()
hash(2A4BE8BA)
indirection()
tags()
location(tests/integration/src/common_structures.h:101)




struct members
{
    int  *first
  int  *second
  int  bar
  int  baz
  float  fbaz
  double  dbaz
  long double  ldbaz
  long long  ldbaz
  long int  libaz
  char  cbaz
  foo_enum fenum
  foo_enum fenum2
  nested_struct foo
  undefined_type _und0
  char  _char
  int  _int
  s32 _s32
  u32 _u32
  u64 _u64
  umm _umm
  const char  *_ccstar
  int  **_intstarstar
  umm *_ummstar
  foo_enum _foo_enum
  undefined_type _und0
  foo_struct_0 _foo0
  struct (anonymous)
  this_is_a_bug _embedded1
  this_is_a_bug _embedded2
  union (anonymous)
  this_is_a_bug _embedded3
  union (anonymous)
  counted_string S1
  foofuncptr FooFuncPtr
  foofunc0 FooFun
  foofunc1 FooFun1
  foofunc2 FooFun2
  func_type_name **AnotherFunc
  func_type_name **AnotherFunc
  u32 NamedThing

}




//

name(MacroFunc)
type(macro)
value((value unsupported))
array()
hash(71ED0EB)
indirection()
tags()
location(tests/integration/src/common_structures.h:4)








//

name(MacroIdentifier)
type(macro)
value((value unsupported))
array()
hash(132EA968)
indirection()
tags()
location(tests/integration/src/common_structures.h:3)








//

name(poof_noop)
type(meta_func)
value((value unsupported))
array()
hash(2B1EFA62)
indirection()
tags()
location(tests/integration/src/common_structures.h:187)








//

name(typedefd_foofunc)
type(foofunc)
value(int )
array()
hash(B47F294)
indirection()
tags()
location(tests/integration/src/common_structures.h:85)


function args
{
    int  foo
  void  *bar
  thing **baz

}






//

name(umm)
type(u64)
value((value unsupported))
array()
hash(7B0D)
indirection()
tags()
location(tests/integration/src/common_structures.h:31)








//

name(bar_struct)
type(struct)
value((value unsupported))
array()
hash(11D37368)
indirection()
tags()
location(tests/integration/src/common_structures.h:70)




struct members
{
    int  fda

}




//

name(undefined_type)
type(__)
value()
array()
hash(4EAFB6B)
indirection()
tags()
location(tests/integration/src/common_structures.h:6)








//

name(doubly_nested_struct)
type(struct)
value((value unsupported))
array()
hash(31690B79)
indirection()
tags()
location(tests/integration/src/common_structures.h:60)




struct members
{
  
}




//

name(array_struct)
type(struct)
value((value unsupported))
array()
hash(1F378381)
indirection()
tags()
location(tests/integration/src/common_structures.h:53)




struct members
{
    int  a1
  u64 a2
  void  *a3

}




//

name(b64)
type(u64)
value((value unsupported))
array()
hash(290D1BAB)
indirection()
tags()
location(tests/integration/src/common_structures.h:32)








//

name(r64)
type(double )
value((value unsupported))
array()
hash(290D1BBB)
indirection()
tags()
location((unknown):-1)








//

name(s64)
type(long long int )
value((value unsupported))
array()
hash(290D1BBC)
indirection()
tags()
location((unknown):-1)








//

name(u64)
type(unsigned long long int )
value((value unsupported))
array()
hash(290D1BBE)
indirection()
tags()
location((unknown):-1)








//

name(degrees)
type(r32)
value((value unsupported))
array()
hash(475343F)
indirection()
tags()
location(tests/integration/src/common_structures.h:25)








//

name(s8)
type(char )
value((value unsupported))
array()
hash(290F14E4)
indirection()
tags()
location((unknown):-1)








//

name(u8)
type(unsigned char )
value((value unsupported))
array()
hash(290F14E6)
indirection()
tags()
location((unknown):-1)








//

name(b32)
type(u32)
value((value unsupported))
array()
hash(290D051A)
indirection()
tags()
location(tests/integration/src/common_structures.h:22)








//

name(r32)
type(float )
value((value unsupported))
array()
hash(290D052A)
indirection()
tags()
location((unknown):-1)








//

name(s32)
type(int )
value((value unsupported))
array()
hash(290D052B)
indirection()
tags()
location((unknown):-1)








//

name(u32)
type(unsigned int )
value((value unsupported))
array()
hash(290D052D)
indirection()
tags()
location((unknown):-1)








//

name(nested_struct)
type(struct)
value((value unsupported))
array()
hash(3AEF4D40)
indirection()
tags()
location(tests/integration/src/common_structures.h:64)




struct members
{
    doubly_nested_struct DoublyNested

}




//

name(counted_string)
type(struct)
value((value unsupported))
array()
hash(94CDD91)
indirection()
tags()
location(tests/integration/src/common_structures.h:76)








//

name(foofuncptr)
type((function))
value(foofuncptr)
array()
hash(29B9FDBA)
indirection()
tags()
location(tests/integration/src/common_structures.h:87)


function args
{
  
}






//

name((anonymous))
type(struct)
value((value unsupported))
array()
hash(2937360B)
indirection()
tags()
location((unknown):-1)




struct members
{
    int  _int2

}




//

name((anonymous))
type(union)
value((value unsupported))
array()
hash(2937360B)
indirection()
tags()
location((unknown):-1)








//

name((anonymous))
type(union)
value((value unsupported))
array()
hash(2937360B)
indirection()
tags()
location((unknown):-1)








//

name(upshift_by_31)
type(macro)
value((value unsupported))
array()
hash(2DA3F63A)
indirection()
tags()
location(tests/integration/src/common_structures.h:34)








//

name(radians)
type(r32)
value((value unsupported))
array()
hash(1D796683)
indirection()
tags()
location(tests/integration/src/common_structures.h:24)








//

name(bar_enum)
type(enum)
value((value unsupported))
array()
hash(284726AE)
indirection()
tags()
location(tests/integration/src/common_structures.h:43)






enum values
{
    bar_enum_value_0 bar_enum_value_0 
  bar_enum_value_1 bar_enum_value_1 
  bar_enum_value_2 bar_enum_value_2 
  bar_enum_value_3 bar_enum_value_3 

}


//

name(foo_struct_0)
type(struct)
value((value unsupported))
array()
hash(2D3996C3)
indirection()
tags( tag_with_value=69 )
location(tests/integration/src/common_structures.h:97)




struct members
{
  
}




//

name(poof_enum)
type(enum)
value((value unsupported))
array()
hash(3B2926CC)
indirection()
tags()
location(tests/integration/src/common_structures.h:190)






enum values
{
    enum_value_0 enum_value_0 

}


//

name(foofunc1)
type((function))
value(int )
array()
hash(A897753)
indirection()
tags()
location(tests/integration/src/common_structures.h:89)


function args
{
  
}






//

name(poof)
type(macro)
value((value unsupported))
array()
hash(9A798)
indirection()
tags()
location(tests/integration/src/common_structures.h:1)








//

name(foofunc)
type((function))
value(int )
array()
hash(13DD1FA3)
indirection()
tags()
location(tests/integration/src/common_structures.h:81)


function args
{
    int  foo
  void  *bar
  thing **baz

}






//

name(poof_struct)
type(struct)
value((value unsupported))
array()
hash(1B207F9)
indirection()
tags()
location(tests/integration/src/common_structures.h:198)




struct members
{
    int  foo
  float  bar

}




//


