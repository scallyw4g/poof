struct macro_def_linked_list_node
{
  macro_def Element;
  macro_def_linked_list_node *Next;
};

struct macro_def_hashtable
{
  umm Size;
  macro_def_linked_list_node **Elements;
  OWNED_BY_THREAD_MEMBER()
};

link_internal macro_def_linked_list_node *
Allocate_macro_def_linked_list_node(memory_arena *Memory)
{
  macro_def_linked_list_node *Result = Allocate( macro_def_linked_list_node, Memory, 1);
  return Result;
}

link_internal macro_def_hashtable
Allocate_macro_def_hashtable(umm ElementCount, memory_arena *Memory)
{
  macro_def_hashtable Result = {
    .Elements = Allocate( macro_def_linked_list_node*, Memory, ElementCount),
    .Size = ElementCount,
    OWNED_BY_THREAD_MEMBER_INIT()
  };
  return Result;
}

link_internal macro_def_linked_list_node *
GetHashBucket(umm HashValue, macro_def_hashtable *Table)
{
  ENSURE_OWNED_BY_THREAD(Table);

  Assert(Table->Size);
  macro_def_linked_list_node *Result = Table->Elements[HashValue % Table->Size];
  return Result;
}

link_internal macro_def *
GetFirstAtBucket(umm HashValue, macro_def_hashtable *Table)
{
  ENSURE_OWNED_BY_THREAD(Table);

  macro_def_linked_list_node *Bucket = GetHashBucket(HashValue, Table);
  macro_def *Result = &Bucket->Element;
  return Result;
}

link_internal macro_def *
Insert(macro_def_linked_list_node *Node, macro_def_hashtable *Table)
{
  ENSURE_OWNED_BY_THREAD(Table);

  Assert(Table->Size);
  umm HashValue = Hash(&Node->Element) % Table->Size;
  macro_def_linked_list_node **Bucket = Table->Elements + HashValue;
  while (*Bucket) Bucket = &(*Bucket)->Next;
  *Bucket = Node;
  return &Bucket[0]->Element;
}

link_internal macro_def*
Insert(macro_def Element, macro_def_hashtable *Table, memory_arena *Memory)
{
  ENSURE_OWNED_BY_THREAD(Table);

  macro_def_linked_list_node *Bucket = Allocate_macro_def_linked_list_node(Memory);
  Bucket->Element = Element;
  Insert(Bucket, Table);
  return &Bucket->Element;
}
 
