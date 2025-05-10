// ./poof/poof.h:651:0

link_internal meta_func_arg_buffer
Compact(meta_func_arg_stream *Stream, memory_arena *PermMemory)
{
  meta_func_arg_buffer Result = {};
  if (Stream->ChunkCount)
  {
    Result = MetaFuncArgBuffer(Stream->ChunkCount, PermMemory);
    /* DebugLine("compact %u", Result.Count); */

    u32 Index = 0;
    ITERATE_OVER(Stream)
    {
      meta_func_arg *Spot = GET_ELEMENT(Iter);
      Result.Start[Index] = *Spot;

      ++Index;
    }

    Deallocate(Stream);
  }

  return Result;
}

