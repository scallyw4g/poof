struct datatype_linked_list_node
{
  datatype Element;
  datatype_linked_list_node *Next;
};

struct datatype_hashtable
{
  umm Size;
  datatype_linked_list_node **Elements;
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
  datatype_hashtable Result = {};
  Result.Elements = Allocate( datatype_linked_list_node*, Memory, ElementCount);
  Result.Size = ElementCount;
  return Result;
}

link_internal datatype_linked_list_node *
GetHashBucket(umm HashValue, datatype_hashtable *Table)
{
  Assert(Table->Size);
  datatype_linked_list_node *Result = Table->Elements[HashValue % Table->Size];
  return Result;
}

link_internal datatype *
GetFirstAtBucket(umm HashValue, datatype_hashtable *Table)
{
  datatype_linked_list_node *Bucket = GetHashBucket(HashValue, Table);
  datatype *Result = &Bucket->Element;
  return Result;
}

link_internal datatype *
Insert(datatype_linked_list_node *Node, datatype_hashtable *Table)
{
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
  datatype_linked_list_node *Bucket = Allocate_datatype_linked_list_node(Memory);
  Bucket->Element = Element;
  Insert(Bucket, Table);
  return &Bucket->Element;
}
 
