// ./poof/poof.h:336:0





struct poof_tag_block
{
  /* u32 Index; */
  umm At;
  poof_tag Elements[8];
};

struct poof_tag_block_array_index
{
  umm Index; 
};

struct poof_tag_block_array
{
  poof_tag_block **BlockPtrs; poof(@array_length(Element->BlockCount))
  u32   BlockCount;
  u32   ElementCount;
  memory_arena *Memory; poof(@no_serialize)
  
};

link_internal poof_tag_block_array
PoofTagBlockArray(memory_arena *Memory)
{
  poof_tag_block_array Result = {};
  Result.Memory = Memory;
  return Result;
}

link_internal b32
AreEqual(poof_tag_block_array_index *Thing1, poof_tag_block_array_index *Thing2)
{
  if (Thing1 && Thing2)
  {
        b32 Result = MemoryIsEqual((u8*)Thing1, (u8*)Thing2, sizeof( poof_tag_block_array_index ) );

    return Result;
  }
  else
  {
    return (Thing1 == Thing2);
  }
}

link_internal b32
AreEqual(poof_tag_block_array_index Thing1, poof_tag_block_array_index Thing2)
{
    b32 Result = MemoryIsEqual((u8*)&Thing1, (u8*)&Thing2, sizeof( poof_tag_block_array_index ) );

  return Result;
}


typedef poof_tag_block_array poof_tag_paged_list;

link_internal poof_tag_block_array_index
operator++( poof_tag_block_array_index &I0 )
{
  I0.Index++;
  return I0;
}

link_internal b32
operator<( poof_tag_block_array_index I0, poof_tag_block_array_index I1 )
{
  b32 Result = I0.Index < I1.Index;
  return Result;
}

link_internal b32
operator==( poof_tag_block_array_index I0, poof_tag_block_array_index I1 )
{
  b32 Result = I0.Index == I1.Index;
  return Result;
}

link_inline umm
GetIndex( poof_tag_block_array_index *Index)
{
  umm Result = Index->Index;
  return Result;
}


link_internal poof_tag_block_array_index
ZerothIndex( poof_tag_block_array *Arr )
{
  return {};
}

link_internal poof_tag_block_array_index
Capacity( poof_tag_block_array *Arr )
{
  poof_tag_block_array_index Result = {Arr->BlockCount * 8};
  return Result;
}

link_internal poof_tag_block_array_index
AtElements( poof_tag_block_array *Arr )
{
  poof_tag_block_array_index Result = {Arr->ElementCount};
  return Result;
}


link_internal umm
TotalElements( poof_tag_block_array *Arr )
{
  umm Result = AtElements(Arr).Index;
  return Result;
}


link_internal poof_tag_block_array_index
LastIndex( poof_tag_block_array *Arr )
{
  poof_tag_block_array_index Result = {};
  umm Count = AtElements(Arr).Index;
  if (Count) Result.Index = Count-1;
  return Result;
}

link_internal umm
Count( poof_tag_block_array *Arr )
{
  auto Result = AtElements(Arr).Index;
  return Result;
}

link_internal poof_tag_block *
GetBlock( poof_tag_block_array *Arr, poof_tag_block_array_index Index )
{
  umm BlockIndex   = Index.Index / 8;
  Assert(BlockIndex < Arr->BlockCount);
  poof_tag_block *Block = Arr->BlockPtrs[BlockIndex];
  return Block;
}

link_internal poof_tag *
GetPtr( poof_tag_block_array *Arr, poof_tag_block_array_index Index )
{
  Assert(Arr->BlockPtrs);
  Assert(Index.Index < Capacity(Arr).Index);

  poof_tag_block *Block = GetBlock(Arr, Index);

  umm ElementIndex = Index.Index % 8;
  poof_tag *Result = (Block->Elements + ElementIndex);
  return Result;
}


link_internal poof_tag *
GetPtr( poof_tag_block_array *Arr, umm Index )
{
  poof_tag_block_array_index I = {Index};
  return GetPtr(Arr, I);
}


link_internal poof_tag *
TryGetPtr( poof_tag_block_array *Arr, poof_tag_block_array_index Index)
{
  poof_tag * Result = {};
  if (Arr->BlockPtrs && Index < AtElements(Arr))
  {
    Result = GetPtr(Arr, Index);
  }
  return Result;
}

link_internal poof_tag *
TryGetPtr( poof_tag_block_array *Arr, umm Index)
{
  auto Result = TryGetPtr(Arr, poof_tag_block_array_index{Index});
  return Result;
}





link_internal cs
CS( poof_tag_block_array_index Index )
{
  return FSz("(%u)", Index.Index);
}

link_internal poof_tag *
Set( poof_tag_block_array *Arr,
  poof_tag *Element,
  poof_tag_block_array_index Index )
{
  Assert(Arr->BlockPtrs);
  Assert(Index.Index < Capacity(Arr).Index);
  poof_tag_block *Block = GetBlock(Arr, Index);
  umm ElementIndex = Index.Index % 8;
  auto Slot = Block->Elements+ElementIndex;
  *Slot = *Element;
  return Slot;
}

link_internal void
NewBlock( poof_tag_block_array *Arr )
{
  poof_tag_block  *NewBlock     = Allocate( poof_tag_block , Arr->Memory,                 1);
  poof_tag_block **NewBlockPtrs = Allocate( poof_tag_block*, Arr->Memory, Arr->BlockCount+1);

  RangeIterator_t(u32, BlockI, Arr->BlockCount)
  {
    NewBlockPtrs[BlockI] = Arr->BlockPtrs[BlockI];
  }

  NewBlockPtrs[Arr->BlockCount] = NewBlock;

  
  
  Arr->BlockPtrs = NewBlockPtrs;
  Arr->BlockCount += 1;
}

link_internal void
RemoveUnordered( poof_tag_block_array *Array, poof_tag_block_array_index Index)
{
  auto LastI = LastIndex(Array);
  Assert(Index.Index <= LastI.Index);

  auto LastElement = GetPtr(Array, LastI);
  Set(Array, LastElement, Index);
  Array->ElementCount -= 1;
}

link_internal void
RemoveOrdered( poof_tag_block_array *Array, poof_tag_block_array_index IndexToRemove)
{
  Assert(IndexToRemove.Index < Array->ElementCount);

  poof_tag *Prev = {};

  poof_tag_block_array_index Max = AtElements(Array);
  RangeIteratorRange_t(umm, Index, Max.Index, IndexToRemove.Index)
  {
    poof_tag *E = GetPtr(Array, Index);

    if (Prev)
    {
      *Prev = *E;
    }

    Prev = E;
  }

  Array->ElementCount -= 1;
}

link_internal void
RemoveOrdered( poof_tag_block_array *Array, poof_tag *Element )
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

link_internal poof_tag_block_array_index
Find( poof_tag_block_array *Array, poof_tag *Query)
{
  poof_tag_block_array_index Result = {INVALID_BLOCK_ARRAY_INDEX};
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
IsValid(poof_tag_block_array_index *Index)
{
  poof_tag_block_array_index Test = {INVALID_BLOCK_ARRAY_INDEX};
  b32 Result = (AreEqual(Index, &Test) == False);
  return Result;
}

link_internal poof_tag *
Push( poof_tag_block_array *Array, poof_tag *Element)
{
  Assert(Array->Memory);

  if (AtElements(Array) == Capacity(Array))
  {
    NewBlock(Array);
  }

  poof_tag *Result = Set(Array, Element, AtElements(Array));

  Array->ElementCount += 1;

  return Result;
}

link_internal poof_tag *
Push( poof_tag_block_array *Array )
{
  poof_tag Element = {};
  auto Result = Push(Array, &Element);
  return Result;
}

link_internal void
Shift( poof_tag_block_array *Array, poof_tag *Element )
{
  Assert(Array->Memory);
  poof_tag *Prev = {};

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




