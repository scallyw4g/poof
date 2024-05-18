// ./poof/poof.h:456:0

struct datatype_linked_list_node
{
  datatype Element;
  datatype_linked_list_node *Next;
};

struct datatype_hashtable
{
  umm Size;
  datatype_linked_list_node **Elements;
  /* OWNED_BY_THREAD_MEMBER() */
};
link_internal datatype_linked_list_node *
Allocate_datatype_linked_list_node(memory_arena *Memory)
{
  datatype_linked_list_node *Result = Allocate( datatype_linked_list_node, Memory, 1);
  return Result;
}

link_internal datatype_hashtable
Allocate_datatype_hashtable(umm ElementCount, memory_arena *Memory)
{
  datatype_hashtable Result = {
    .Elements = Allocate( datatype_linked_list_node*, Memory, ElementCount),
    .Size = ElementCount,
    /* OWNED_BY_THREAD_MEMBER_INIT() */
  };
  return Result;
}

link_internal datatype_linked_list_node *
GetHashBucket(umm HashValue, datatype_hashtable *Table)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  Assert(Table->Size);
  datatype_linked_list_node *Result = Table->Elements[HashValue % Table->Size];
  return Result;
}

link_internal datatype *
GetFirstAtBucket(umm HashValue, datatype_hashtable *Table)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  datatype_linked_list_node *Bucket = GetHashBucket(HashValue, Table);
  datatype *Result = &Bucket->Element;
  return Result;
}

link_internal datatype *
Insert(datatype_linked_list_node *Node, datatype_hashtable *Table)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  Assert(Table->Size);
  umm HashValue = Hash(&Node->Element) % Table->Size;
  datatype_linked_list_node **Bucket = Table->Elements + HashValue;
  while (*Bucket) Bucket = &(*Bucket)->Next;
  *Bucket = Node;
  return &Bucket[0]->Element;
}

link_internal datatype*
Insert(datatype Element, datatype_hashtable *Table, memory_arena *Memory)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  datatype_linked_list_node *Bucket = Allocate_datatype_linked_list_node(Memory);
  Bucket->Element = Element;
  Insert(Bucket, Table);
  return &Bucket->Element;
}

link_internal datatype*
Upsert(datatype Element, datatype_hashtable *Table, memory_arena *Memory)
{
  umm HashValue = Hash(&Element) % Table->Size;
  datatype_linked_list_node **Bucket = Table->Elements + HashValue;
  while (*Bucket)
  {
    if (AreEqual(&Bucket[0]->Element, &Element)) { break; }
    Bucket = &(*Bucket)->Next;
  }

  if (*Bucket)
  {
    Bucket[0]->Element = Element;
  }
  else
  {
    Insert(Element, Table, Memory);
  }

  return &Bucket[0]->Element;
}

//
// Iterator impl.
//

struct datatype_hashtable_iterator
{
  umm HashIndex;
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
 
