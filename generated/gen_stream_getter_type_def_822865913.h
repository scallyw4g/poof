// callsite
// poof/poof.cpp:1895:0
// (gen_stream_getter) def
// poof/poof.cpp:1874:0
link_internal type_def*
GetTypeDefByAlias( type_def_stream* Stream,  counted_string  Alias )
{
  type_def *Result = 0;
  ITERATE_OVER(Stream)
  {
    type_def* T = GET_ELEMENT(Iter);
    if (AreEqual(T->Alias, Alias))
    {
      Result = T;
      break;
    }
  }

  return Result;
}

