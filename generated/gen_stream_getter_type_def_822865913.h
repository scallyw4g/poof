// poof/poof.cpp:1887:0

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

