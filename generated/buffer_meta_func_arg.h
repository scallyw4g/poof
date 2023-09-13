struct meta_func_arg_buffer
{
  meta_func_arg *Start;
  umm Count;
};

link_internal meta_func_arg_buffer
MetaFuncArgBuffer(umm ElementCount, memory_arena* Memory)
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
CurrentCount(meta_func_arg_buffer *Buf)
{
  umm Result = Buf->Count;
  return Result;
}

link_inline meta_func_arg *
Get(meta_func_arg_buffer *Buf, u32 Index)
{
  Assert(Index < Buf->Count);
  meta_func_arg *Result = Buf->Start + Index;
  return Result;
}

