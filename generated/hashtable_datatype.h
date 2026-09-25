// callsite
// ./poof/poof.h:722:0

// def (hashtable)
// ./include/bonsai_stdlib/src/poof_functions.h:801:0
struct datatype_linked_list_node
{
  b32 Tombstoned;
  datatype Element;
  datatype_linked_list_node *Next;
};

struct datatype_hashtable
poof(
  @collection
  
  
)
{
  u32 Size;
  u32 Pad;
  datatype_linked_list_node **Elements;
  /* OWNED_BY_THREAD_MEMBER() */
};
link_internal b32 AreEqual(datatype_linked_list_node *Node1, datatype_linked_list_node *Node2 );
link_internal b32 AreEqual(datatype *Element1, datatype *Element2 );

link_internal datatype_linked_list_node *
Allocate_datatype_linked_list_node(memory_arena *Memory)
{
  datatype_linked_list_node *Result = Allocate( datatype_linked_list_node, Memory, 1);
  return Result;
}

link_internal datatype_hashtable
Allocate_datatype_hashtable(u32 ElementCount, memory_arena *Memory)
{
  datatype_hashtable Result = {
    .Elements = Allocate( datatype_linked_list_node*, Memory, ElementCount),
    .Size = ElementCount,
    .Pad = 0,
  };
  return Result;
}

link_internal datatype_linked_list_node *
GetHashBucket(u32 HashValue, datatype_hashtable *Table)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  Assert(Table->Size);
  datatype_linked_list_node *Result = Table->Elements[HashValue % Table->Size];
  return Result;
}

link_internal datatype *
GetFirstAtBucket(u32 HashValue, datatype_hashtable *Table)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  datatype_linked_list_node *Bucket = GetHashBucket(HashValue, Table);
  datatype *Result = &Bucket->Element;
  return Result;
}

link_internal datatype_linked_list_node**
GetMatchingBucket(datatype Element, datatype_hashtable *Table, memory_arena *Memory)
{
  u32 HashValue = Hash(&Element) % Table->Size;
  datatype_linked_list_node **Bucket = Table->Elements + HashValue;
  while (*Bucket)
  {
    if (AreEqual(&Bucket[0]->Element, &Element)) { break; }
    Bucket = &(*Bucket)->Next;
  }
  return Bucket;
}

link_internal datatype *
InsertBlank(u32 HashValue, datatype_hashtable *Table, memory_arena *Memory)
{
  Assert(Table->Size);
  datatype_linked_list_node **Bucket = Table->Elements + (HashValue % Table->Size);
  datatype_linked_list_node  *Node   = Allocate_datatype_linked_list_node(Memory);
  Node->Next = *Bucket;
  *Bucket = Node;
  return &Bucket[0]->Element;
}

link_internal datatype*
Insert(datatype Element, datatype_hashtable *Table, memory_arena *Memory)
{
  u32 HashValue = Hash(&Element) % Table->Size;
  auto Result = InsertBlank(HashValue, Table, Memory);
  *Result = Element;
  return Result;
}

link_internal datatype*
Upsert(datatype Element, datatype_hashtable *Table, memory_arena *Memory)
{
  u32 HashValue = Hash(&Element) % Table->Size;
  datatype_linked_list_node **Bucket = Table->Elements + HashValue;
  while (*Bucket)
  {
    if (AreEqual(&Bucket[0]->Element, &Element)) { break; }
    Bucket = &(*Bucket)->Next;
  }

  datatype *Result = {};
  if (*Bucket && Bucket[0]->Tombstoned == False)
  {
    Bucket[0]->Element = Element;
    Result = &Bucket[0]->Element;
  }
  else
  {
    Result = Insert(Element, Table, Memory);
  }

  return Result;
}

//
// Get
//

/* Type.member(@hashtable_key, (key_member) { @var key key_member }); */

/* link_internal (Type.name) * */
/* GetByKey( (Type.name)_hashtable *Table, key.type KeyQuery ) */
/* { */
/*   (Type.name) *Result = {}; */
/*   (Type.name)_linked_list_node *Bucket = GetBucketBy(key.name)(Table, KeyQuery); */
/*   while (Bucket) */
/*   { */
/*     Type.is_primitive? */
/*     { if (Bucket->Tombstoned == False && AreEqual(*E, KeyQuery)) } */
/*     { if (Bucket->Tombstoned == False && AreEqual(E->key.name, KeyQuery)) } */
/*     { */
/*       Result = &Bucket->Element; */
/*       break; */
/*     } */
/*     else */
/*     { */
/*       Bucket = Bucket->Next; */
/*     } */
/*   } */

/*   return Result; */
/* } */

//
// Iterator impl.
//

struct datatype_hashtable_iterator
{
  u32 HashIndex;
  datatype_hashtable *Table;
  datatype_linked_list_node *Node;
};



link_internal datatype_hashtable_iterator
operator++( datatype_hashtable_iterator &Iterator )
{
  if (Iterator.Node)
  {
    Iterator.Node = Iterator.Node->Next;
  }
  else
  {
    Assert (Iterator.HashIndex < Iterator.Table->Size );
    Iterator.Node = Iterator.Table->Elements[++Iterator.HashIndex];
  }

  return Iterator;
}

link_internal b32
operator<( datatype_hashtable_iterator I0, datatype_hashtable_iterator I1)
{
  b32 Result = I0.HashIndex < I1.HashIndex;
  return Result;
}

link_inline datatype_hashtable_iterator
ZerothIndex(datatype_hashtable *Hashtable)
{
  datatype_hashtable_iterator Iterator = {};
  Iterator.Table = Hashtable;
  Iterator.Node = Hashtable->Elements[0];
  return Iterator;
}

link_inline datatype_hashtable_iterator
AtElements(datatype_hashtable *Hashtable)
{
  datatype_hashtable_iterator Result = { Hashtable->Size, 0, 0 };
  return Result;
}

link_inline datatype *
GetPtr(datatype_hashtable *Hashtable, datatype_hashtable_iterator Iterator)
{
  datatype *Result = {};
  if (Iterator.Node)
  {
    Result = &Iterator.Node->Element;
  }
  return Result;
}

link_inline datatype *
TryGetPtr(datatype_hashtable *Hashtable, datatype_hashtable_iterator Iterator)
{
  return GetPtr(Hashtable, Iterator);
}

 
