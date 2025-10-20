// ./poof/poof.h:657:0

struct meta_func_arg_buffer
{
  umm Count;
  meta_func_arg *Start; poof(@array_length(Element->Count))
};

link_internal meta_func_arg_buffer
MetaFuncArgBuffer( umm ElementCount, memory_arena* Memory);

link_internal meta_func_arg_buffer
MetaFuncArgBuffer( meta_func_arg *Start, umm ElementCount)
{
  meta_func_arg_buffer Result = {ElementCount, Start};
  return Result;
}

link_inline umm
LastIndex(meta_func_arg_buffer *Buf)
{
  umm Result = Buf->Count > 0? Buf->Count-1 : 0;
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
TryGetPtr(meta_func_arg_buffer *Buf, umm Index)
{
  return GetPtr(Buf, Index);
}

link_inline meta_func_arg *
Get(meta_func_arg_buffer *Buf, umm Index)
{
  meta_func_arg *Result = GetPtr(Buf, Index);
  return Result;
}

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


