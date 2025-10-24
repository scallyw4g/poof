// poof/poof.cpp:7311:0

if (TypeSpec->Qualifier & TypeQual_None)
{
  Append(&Builder, CSz("none "));
}
if (TypeSpec->Qualifier & TypeQual_Static)
{
  Append(&Builder, CSz("static "));
}
if (TypeSpec->Qualifier & TypeQual_Void)
{
  Append(&Builder, CSz("void "));
}
if (TypeSpec->Qualifier & TypeQual_Auto)
{
  Append(&Builder, CSz("auto "));
}
if (TypeSpec->Qualifier & TypeQual_Class)
{
  Append(&Builder, CSz("class "));
}
if (TypeSpec->Qualifier & TypeQual_Struct)
{
  Append(&Builder, CSz("struct "));
}
if (TypeSpec->Qualifier & TypeQual_Union)
{
  Append(&Builder, CSz("union "));
}
if (TypeSpec->Qualifier & TypeQual_Enum)
{
  Append(&Builder, CSz("enum "));
}
if (TypeSpec->Qualifier & TypeQual_Bool)
{
  Append(&Builder, CSz("bool "));
}
if (TypeSpec->Qualifier & TypeQual_Inline)
{
  Append(&Builder, CSz("inline "));
}
if (TypeSpec->Qualifier & TypeQual_ThreadLocal)
{
  Append(&Builder, CSz("threadlocal "));
}
if (TypeSpec->Qualifier & TypeQual_Extern)
{
  Append(&Builder, CSz("extern "));
}
if (TypeSpec->Qualifier & TypeQual_Const)
{
  Append(&Builder, CSz("const "));
}
if (TypeSpec->Qualifier & TypeQual_Volatile)
{
  Append(&Builder, CSz("volatile "));
}
if (TypeSpec->Qualifier & TypeQual_Signed)
{
  Append(&Builder, CSz("signed "));
}
if (TypeSpec->Qualifier & TypeQual_Unsigned)
{
  Append(&Builder, CSz("unsigned "));
}
if (TypeSpec->Qualifier & TypeQual_Char)
{
  Append(&Builder, CSz("char "));
}
if (TypeSpec->Qualifier & TypeQual_Long)
{
  Append(&Builder, CSz("long "));
}
if (TypeSpec->Qualifier & TypeQual_Long_Long)
{
  Append(&Builder, CSz("long "));
}
if (TypeSpec->Qualifier & TypeQual_Int)
{
  Append(&Builder, CSz("int "));
}
if (TypeSpec->Qualifier & TypeQual_Double)
{
  Append(&Builder, CSz("double "));
}
if (TypeSpec->Qualifier & TypeQual_Short)
{
  Append(&Builder, CSz("short "));
}
if (TypeSpec->Qualifier & TypeQual_Float)
{
  Append(&Builder, CSz("float "));
}
if (TypeSpec->Qualifier & TypeQual_Constexpr)
{
  Append(&Builder, CSz("constexpr "));
}
if (TypeSpec->Qualifier & TypeQual_Explicit)
{
  Append(&Builder, CSz("explicit "));
}
if (TypeSpec->Qualifier & TypeQual_Operator)
{
  Append(&Builder, CSz("operator "));
}
if (TypeSpec->Qualifier & TypeQual_Virtual)
{
  Append(&Builder, CSz("virtual "));
}
if (TypeSpec->Qualifier & TypeQual_Noreturn)
{
  Append(&Builder, CSz("noreturn "));
}

