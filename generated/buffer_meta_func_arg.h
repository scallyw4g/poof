struct meta_func_arg_buffer
{
  meta_func_arg *Start;
  umm Count;
};

link_internal meta_func_arg_buffer
MetaFuncArgBuffer( umm ElementCount, memory_arena* Memory)
{
  meta_func_arg_buffer Result = {};

  if (ElementCount)
  {
    Result.Start = Allocate( meta_func_arg, Memory, ElementCount );
    Result.Count = ElementCount;
  }
  else
  {
    Warn("Attempted to allocate meta_func_arg_buffer of 0 length.");
  }

  return Result;
}

link_inline umm
ZerothIndex(meta_func_arg_buffer *Buf)
{
  umm Result = 0;
  return Result;
}

link_inline umm
AtElements(meta_func_arg_buffer *Buf)
{
  umm Result = Buf->Count;
  return Result;
}

link_inline umm
TotalElements(meta_func_arg_buffer *Buf)
{
  umm Result = Buf->Count;
  return Result;
}

link_inline umm
CurrentCount(meta_func_arg_buffer *Buf)
{
  umm Result = Buf->Count;
  return Result;
}

link_inline meta_func_arg *
GetPtr(meta_func_arg_buffer *Buf, umm Index)
{
  meta_func_arg *Result = 0;
  if (Index < Buf->Count) { Result = Buf->Start + Index; }
  return Result;
}

link_inline meta_func_arg *
Get(meta_func_arg_buffer *Buf, umm Index)
{
  meta_func_arg *Result = GetPtr(Buf, Index);
  return Result;
}


