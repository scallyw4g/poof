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

//
// Iterator impl.
//
link_inline umm
ZerothIndex(datatype_hashtable *Hashtable)
{
  return 0;
}
 
