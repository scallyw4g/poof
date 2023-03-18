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

