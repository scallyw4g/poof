// poof/poof.cpp:1875:0

link_internal enum_decl*
GetEnumDeclByName( enum_decl_stream* Stream, counted_string  Name )
{
  enum_decl *Result = 0;
  ITERATE_OVER(Stream)
  {
    enum_decl* T = GET_ELEMENT(Iter);
    if (AreEqual(T->Name, Name))
    {
      Result = T;
      break;
    }
  }

  return Result;
}

