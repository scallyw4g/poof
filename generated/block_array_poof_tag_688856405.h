// ./poof/poof.h:345:0

struct poof_tag_block
{
  u32 Index;
  u32 At;
  poof_tag *Elements;
  poof_tag_block *Next;
};

struct poof_tag_block_array_index
{
  poof_tag_block *Block;
  u32 BlockIndex;
  u32 ElementIndex;
};

struct poof_tag_block_array
{
  poof_tag_block *First;
  poof_tag_block *Current;
  memory_arena *Memory; poof(@no_serialize)
  
};

link_internal poof_tag_block_array_index
operator++(poof_tag_block_array_index &I0)
{
  if (I0.Block)
  {
    if (I0.ElementIndex == 8-1)
    {
      I0.ElementIndex = 0;
      I0.BlockIndex++;
      I0.Block = I0.Block->Next;
    }
    else
    {
      I0.ElementIndex++;
    }
  }
  else
  {
    I0.ElementIndex++;
  }
  return I0;
}

link_internal b32
operator<(poof_tag_block_array_index I0, poof_tag_block_array_index I1)
{
  b32 Result = I0.BlockIndex < I1.BlockIndex || (I0.BlockIndex == I1.BlockIndex & I0.ElementIndex < I1.ElementIndex);
  return Result;
}

link_inline umm
GetIndex(poof_tag_block_array_index *Index)
{
  umm Result = Index->ElementIndex + (Index->BlockIndex*8);
  return Result;
}

link_internal poof_tag_block_array_index
ZerothIndex(poof_tag_block_array *Arr)
{
  poof_tag_block_array_index Result = {};
  Result.Block = Arr->First;
  /* Assert(Result.Block->Index == 0); */
  return Result;
}

link_internal umm
TotalElements(poof_tag_block_array *Arr)
{
  umm Result = 0;
  if (Arr->Current)
  {
    Result = (Arr->Current->Index * 8) + Arr->Current->At;
  }
  return Result;
}

link_internal poof_tag_block_array_index
LastIndex(poof_tag_block_array *Arr)
{
  poof_tag_block_array_index Result = {};
  if (Arr->Current)
  {
    Result.Block = Arr->Current;
    Result.BlockIndex = Arr->Current->Index;
    Result.ElementIndex = Arr->Current->At;
    Assert(Result.ElementIndex);
    Result.ElementIndex--;
  }
  return Result;
}

link_internal poof_tag_block_array_index
AtElements(poof_tag_block_array *Arr)
{
  poof_tag_block_array_index Result = {};
  if (Arr->Current)
  {
    Result.Block = Arr->Current;
    Result.BlockIndex = Arr->Current->Index;
    Result.ElementIndex = Arr->Current->At;
  }
  return Result;
}

link_internal poof_tag *
GetPtr(poof_tag_block_array *Arr, poof_tag_block_array_index Index)
{
  poof_tag *Result = {};
  if (Index.Block) { Result = Index.Block->Elements + Index.ElementIndex; }
  return Result;
}

link_internal poof_tag *
GetPtr(poof_tag_block *Block, umm Index)
{
  poof_tag *Result = 0;
  if (Index < Block->At) { Result = Block->Elements + Index; }
  return Result;
}

link_internal poof_tag *
GetPtr(poof_tag_block_array *Arr, umm Index)
{
  umm BlockIndex = Index / 8;
  umm ElementIndex = Index % 8;

  umm AtBlock = 0;
  poof_tag_block *Block = Arr->First;
  while (AtBlock++ < BlockIndex)
  {
    Block = Block->Next;
  }

  poof_tag *Result = Block->Elements+ElementIndex;
  return Result;
}

link_internal u32
AtElements(poof_tag_block *Block)
{
  return Block->At;
}


link_internal poof_tag_block*
Allocate_poof_tag_block(memory_arena *Memory)
{
  poof_tag_block *Result = Allocate(poof_tag_block, Memory, 1);
  Result->Elements = Allocate(poof_tag, Memory, 8);
  return Result;
}

link_internal cs
CS(poof_tag_block_array_index Index)
{
  return FSz("(%u)(%u)", Index.BlockIndex, Index.ElementIndex);
}

link_internal void
RemoveUnordered(poof_tag_block_array *Array, poof_tag_block_array_index Index)
{
  poof_tag_block_array_index LastI = LastIndex(Array);

  poof_tag *Element = GetPtr(Array, Index);
  poof_tag *LastElement = GetPtr(Array, LastI);

  *Element = *LastElement;

  Assert(Array->Current->At);
  Array->Current->At -= 1;

  if (Array->Current->At == 0)
  {
    // Walk the chain till we get to the second-last one
    poof_tag_block *Current = Array->First;
    poof_tag_block *LastB = LastI.Block;

    while (Current->Next && Current->Next != LastB)
    {
      Current = Current->Next;
    }

    Assert(Current->Next == LastB || Current->Next == 0);
    Array->Current = Current;
  }
}

link_internal poof_tag *
Push(poof_tag_block_array *Array, poof_tag *Element)
{
  if (Array->Memory == 0) { Array->Memory = AllocateArena(); }

  if (Array->First == 0) { Array->First = Allocate_poof_tag_block(Array->Memory); Array->Current = Array->First; }

  if (Array->Current->At == 8)
  {
    if (Array->Current->Next)
    {
      Array->Current = Array->Current->Next;
      Assert(Array->Current->At == 0);
    }
    else
    {
      poof_tag_block *Next = Allocate_poof_tag_block(Array->Memory);
      Next->Index = Array->Current->Index + 1;

      Array->Current->Next = Next;
      Array->Current = Next;
    }
  }

  poof_tag *Result = Array->Current->Elements + Array->Current->At;

  Array->Current->Elements[Array->Current->At++] = *Element;

  return Result;
}


