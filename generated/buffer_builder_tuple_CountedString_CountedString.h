struct tuple_CountedString_CountedString_buffer_builder
{
  tuple_CountedString_CountedString_stream Chunks;
};

link_internal void
Append( tuple_CountedString_CountedString_buffer_builder* Builder, tuple_CountedString_CountedString E)
{
  Push(&Builder->Chunks, E);
}

struct tuple_CountedString_CountedString_buffer
{
  umm Count;
  tuple_CountedString_CountedString *E;
};

link_internal tuple_CountedString_CountedString_buffer
TupleCountedStringCountedStringBuffer(umm TotalElements, memory_arena *PermMemory)
{
  tuple_CountedString_CountedString_buffer Result = {};
  Result.Count = TotalElements;
  Result.E = Allocate( tuple_CountedString_CountedString, PermMemory, TotalElements);
  return Result;
}

link_internal tuple_CountedString_CountedString_buffer
Finalize( tuple_CountedString_CountedString_buffer_builder *Builder, memory_arena *PermMemory)
{
  TIMED_FUNCTION();

  u32 TotalElements = 0;

  // TODO(Jesse): Keep track of # of chunks?
  ITERATE_OVER(&Builder->Chunks)
  {
    ++TotalElements;
  }

  auto Result = TupleCountedStringCountedStringBuffer(TotalElements, PermMemory);

  u32 ElementIndex = 0;
  ITERATE_OVER(&Builder->Chunks)
  {
    auto At = GET_ELEMENT(Iter);
    Result.E[ElementIndex] = *At;
    Assert(ElementIndex < Result.Count);
    ++ElementIndex;
  }
  Assert(ElementIndex == Result.Count);

  return Result;
}

