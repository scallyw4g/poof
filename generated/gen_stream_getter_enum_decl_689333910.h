// poof/poof.cpp:1892:0


link_internal enum_decl*
GetEnumDeclByNameT( enum_decl_stream* Stream,  c_token  NameT )
{
  enum_decl *Result = 0;
  ITERATE_OVER(Stream)
  {
    enum_decl* T = GET_ELEMENT(Iter);
    if (AreEqual(T->NameT, NameT))
    {
      Result = T;
      break;
    }
  }

  return Result;
}

