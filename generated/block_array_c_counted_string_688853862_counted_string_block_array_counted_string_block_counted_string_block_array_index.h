// ./include/bonsai_stdlib/src/counted_string.cpp:19:0





/* link_internal cs */
/* CS( index_t Index ) */
/* { */
/*   return FSz("(%u)", Index.Index); */
/* } */

link_internal counted_string *
Set( counted_string_block_array *Arr,
  counted_string *Element,
  counted_string_block_array_index Index )
{
  Assert(Arr->BlockPtrs);
  Assert(Index.Index < Capacity(Arr).Index);
  counted_string_block *Block = GetBlock(Arr, Index);
  umm ElementIndex = Index.Index % 32;
  auto Slot = Block->Elements+ElementIndex;
  *Slot = *Element;
  return Slot;
}

link_internal void
NewBlock( counted_string_block_array *Arr )
{
  counted_string_block  *NewBlock     = Allocate( counted_string_block , Arr->Memory,                 1);
  counted_string_block **NewBlockPtrs = Allocate( counted_string_block*, Arr->Memory, Arr->BlockCount+1);

  RangeIterator_t(u32, BlockI, Arr->BlockCount)
  {
    NewBlockPtrs[BlockI] = Arr->BlockPtrs[BlockI];
  }

  NewBlockPtrs[Arr->BlockCount] = NewBlock;

  
  
  Arr->BlockPtrs = NewBlockPtrs;
  Arr->BlockCount += 1;
}

link_internal void
RemoveUnordered( counted_string_block_array *Array, counted_string_block_array_index Index)
{
  auto LastI = LastIndex(Array);
  Assert(Index.Index <= LastI.Index);

  auto LastElement = GetPtr(Array, LastI);
  Set(Array, LastElement, Index);
  Array->ElementCount -= 1;
}

link_internal void
RemoveOrdered( counted_string_block_array *Array, counted_string_block_array_index IndexToRemove)
{
  Assert(IndexToRemove.Index < Array->ElementCount);

  counted_string *Prev = {};

  counted_string_block_array_index Max = AtElements(Array);
  RangeIteratorRange_t(umm, Index, Max.Index, IndexToRemove.Index)
  {
    counted_string *E = GetPtr(Array, Index);

    if (Prev)
    {
      *Prev = *E;
    }

    Prev = E;
  }

  Array->ElementCount -= 1;
}

link_internal void
RemoveOrdered( counted_string_block_array *Array, counted_string *Element )
{
  IterateOver(Array, E, I)
  {
    if (E == Element)
    {
      RemoveOrdered(Array, I);
      break;
    }
  }
}

link_internal counted_string_block_array_index
Find( counted_string_block_array *Array, counted_string *Query)
{
  counted_string_block_array_index Result = {INVALID_BLOCK_ARRAY_INDEX};
  IterateOver(Array, E, Index)
  {
    if ( E == Query )
    {
      Result = Index;
      break;
    }
  }
  return Result;
}



link_internal b32
IsValid(counted_string_block_array_index *Index)
{
  counted_string_block_array_index Test = {INVALID_BLOCK_ARRAY_INDEX};
  b32 Result = (AreEqual(Index, &Test) == False);
  return Result;
}

link_internal counted_string *
Push( counted_string_block_array *Array, counted_string *Element)
{
  Assert(Array->Memory);

  if (AtElements(Array) == Capacity(Array))
  {
    NewBlock(Array);
  }

  counted_string *Result = Set(Array, Element, AtElements(Array));

  Array->ElementCount += 1;

  return Result;
}

link_internal counted_string *
Push( counted_string_block_array *Array )
{
  counted_string Element = {};
  auto Result = Push(Array, &Element);
  return Result;
}

link_internal void
Shift( counted_string_block_array *Array, counted_string *Element )
{
  Assert(Array->Memory);
  counted_string *Prev = {};

  // Alocate a new thingy
  Push(Array);

  auto End = AtElements(Array);
  RangeIteratorReverse(Index, s32(End.Index))
  {
    auto E = GetPtr(Array, umm(Index));
    if (Prev) { *Prev = *E; }
    Prev = E;
  }

  *Prev = *Element;
}



