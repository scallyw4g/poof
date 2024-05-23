// ./poof/poof.h:768:0

struct macro_def_linked_list_node
{
  macro_def Element;
  macro_def_linked_list_node *Next;
};

struct macro_def_hashtable
{
  umm Size;
  macro_def_linked_list_node **Elements;
  /* OWNED_BY_THREAD_MEMBER() */
};
link_internal b32 AreEqual(macro_def_linked_list_node *Node1, macro_def_linked_list_node *Node2 );

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
    /* OWNED_BY_THREAD_MEMBER_INIT() */
  };
  return Result;
}

link_internal macro_def_linked_list_node *
GetHashBucket(umm HashValue, macro_def_hashtable *Table)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  Assert(Table->Size);
  macro_def_linked_list_node *Result = Table->Elements[HashValue % Table->Size];
  return Result;
}

link_internal macro_def *
GetFirstAtBucket(umm HashValue, macro_def_hashtable *Table)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  macro_def_linked_list_node *Bucket = GetHashBucket(HashValue, Table);
  macro_def *Result = &Bucket->Element;
  return Result;
}

link_internal macro_def *
Insert(macro_def_linked_list_node *Node, macro_def_hashtable *Table)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  Assert(Table->Size);
  umm HashValue = Hash(&Node->Element) % Table->Size;
  macro_def_linked_list_node **Bucket = Table->Elements + HashValue;
  while (*Bucket)
  {
    /* Assert(!AreEqual(*Bucket, Node)); */
    Bucket = &(*Bucket)->Next;
  }
  *Bucket = Node;
  return &Bucket[0]->Element;
}

link_internal macro_def*
Insert(macro_def Element, macro_def_hashtable *Table, memory_arena *Memory)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  macro_def_linked_list_node *Bucket = Allocate_macro_def_linked_list_node(Memory);
  Bucket->Element = Element;
  Insert(Bucket, Table);
  return &Bucket->Element;
}

link_internal macro_def*
Upsert(macro_def Element, macro_def_hashtable *Table, memory_arena *Memory)
{
  umm HashValue = Hash(&Element) % Table->Size;
  macro_def_linked_list_node **Bucket = Table->Elements + HashValue;
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

struct macro_def_hashtable_iterator
{
  umm HashIndex;
  macro_def_hashtable *Table;
  macro_def_linked_list_node *Node;
};

link_internal macro_def_hashtable_iterator
operator++( macro_def_hashtable_iterator &Iterator )
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
operator<( macro_def_hashtable_iterator I0, macro_def_hashtable_iterator I1)
{
  b32 Result = I0.HashIndex < I1.HashIndex;
  return Result;
}

link_inline macro_def_hashtable_iterator
ZerothIndex(macro_def_hashtable *Hashtable)
{
  macro_def_hashtable_iterator Iterator = {};
  Iterator.Table = Hashtable;
  Iterator.Node = Hashtable->Elements[0];
  return Iterator;
}

link_inline macro_def_hashtable_iterator
AtElements(macro_def_hashtable *Hashtable)
{
  macro_def_hashtable_iterator Result = { Hashtable->Size, 0, 0 };
  return Result;
}

link_inline macro_def *
GetPtr(macro_def_hashtable *Hashtable, macro_def_hashtable_iterator Iterator)
{
  macro_def *Result = {};
  if (Iterator.Node)
  {
    Result = &Iterator.Node->Element;
  }
  return Result;
}
 
