// ./include/bonsai_stdlib/src/texture.h:47:0






struct texture_ptr_block
{
  u32 Index;
  u32 At;
  texture_ptr *Elements;
  texture_ptr_block *Next;
};

struct texture_ptr_block_array_index
{
  texture_ptr_block *Block;
  u32 BlockIndex;
  u32 ElementIndex;
};

struct texture_ptr_block_array
{
  texture_ptr_block *First;
  texture_ptr_block *Current;
  memory_arena *Memory; poof(@no_serialize)
  
};

link_internal b32
AreEqual(texture_ptr_block_array_index *Thing1, texture_ptr_block_array_index *Thing2)
{
  if (Thing1 && Thing2)
  {
        b32 Result = MemoryIsEqual((u8*)Thing1, (u8*)Thing2, sizeof( texture_ptr_block_array_index ) );

    return Result;
  }
  else
  {
    return (Thing1 == Thing2);
  }
}

link_internal b32
AreEqual(texture_ptr_block_array_index Thing1, texture_ptr_block_array_index Thing2)
{
    b32 Result = MemoryIsEqual((u8*)&Thing1, (u8*)&Thing2, sizeof( texture_ptr_block_array_index ) );

  return Result;
}


typedef texture_ptr_block_array texture_ptr_paged_list;

link_internal texture_ptr_block_array_index
operator++( texture_ptr_block_array_index &I0 )
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
operator<( texture_ptr_block_array_index I0, texture_ptr_block_array_index I1 )
{
  b32 Result = I0.BlockIndex < I1.BlockIndex || (I0.BlockIndex == I1.BlockIndex & I0.ElementIndex < I1.ElementIndex);
  return Result;
}

link_inline umm
GetIndex( texture_ptr_block_array_index *Index)
{
  umm Result = Index->ElementIndex + (Index->BlockIndex*8);
  return Result;
}

link_internal texture_ptr_block_array_index
ZerothIndex( texture_ptr_block_array *Arr)
{
  texture_ptr_block_array_index Result = {};
  Result.Block = Arr->First;
  return Result;
}

link_internal umm
TotalElements( texture_ptr_block_array *Arr)
{
  umm Result = 0;
  if (Arr->Current)
  {
    Result = (Arr->Current->Index * 8) + Arr->Current->At;
  }
  return Result;
}

link_internal texture_ptr_block_array_index
LastIndex( texture_ptr_block_array *Arr)
{
  texture_ptr_block_array_index Result = {};
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

link_internal texture_ptr_block_array_index
AtElements( texture_ptr_block_array *Arr)
{
  texture_ptr_block_array_index Result = {};
  if (Arr->Current)
  {
    Result.Block = Arr->Current;
    Result.BlockIndex = Arr->Current->Index;
    Result.ElementIndex = Arr->Current->At;
  }
  return Result;
}

link_internal umm
Count( texture_ptr_block_array *Arr)
{
  auto Index = AtElements(Arr);
  umm Result = GetIndex(&Index);
  return Result;
}

link_internal texture_ptr 
Set( texture_ptr_block_array *Arr,
  texture_ptr Element,
  texture_ptr_block_array_index Index )
{
  texture_ptr Result = {};
  if (Index.Block)
  {
    Result = Index.Block->Elements[Index.ElementIndex];
    Result = Element;
  }

  return Result;
}

link_internal texture_ptr 
GetPtr(texture_ptr_block_array *Arr, texture_ptr_block_array_index Index)
{
  texture_ptr Result = {};
  if (Index.Block) { Result = *(Index.Block->Elements + Index.ElementIndex); }
  return Result;
}

link_internal texture_ptr 
GetPtr(texture_ptr_block *Block, umm Index)
{
  texture_ptr Result = {};
  if (Index < Block->At) { Result = *(Block->Elements + Index); }
  return Result;
}

link_internal texture_ptr 
GetPtr(texture_ptr_block_array *Arr, umm Index)
{
  umm BlockIndex = Index / 8;
  umm ElementIndex = Index % 8;

  umm AtBlock = 0;
  texture_ptr_block *Block = Arr->First;
  while (AtBlock++ < BlockIndex)
  {
    Block = Block->Next;
  }

  texture_ptr Result = *(Block->Elements+ElementIndex);
  return Result;
}

link_internal texture_ptr 
TryGetPtr(texture_ptr_block_array *Arr, umm Index)
{
  umm BlockIndex = Index / 8;
  umm ElementIndex = Index % 8;

  auto AtE = AtElements(Arr);
  umm Total = GetIndex(&AtE);
  texture_ptr Result = {};
  if (Index < Total) { Result = GetPtr(Arr, Index); }
  return Result;
}

link_internal u32
AtElements(texture_ptr_block *Block)
{
  return Block->At;
}

