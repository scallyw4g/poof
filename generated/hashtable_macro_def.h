// callsite
// ./poof/poof.h:494:0

// def (hashtable)
// ./include/bonsai_stdlib/src/poof_functions.h:801:0
struct macro_def_linked_list_node
{
  b32 Tombstoned;
  macro_def Element;
  macro_def_linked_list_node *Next;
};

struct macro_def_hashtable
poof(
  @collection
  
  
)
{
  u32 Size;
  u32 Pad;
  macro_def_linked_list_node **Elements;
  /* OWNED_BY_THREAD_MEMBER() */
};
link_internal b32 AreEqual(macro_def_linked_list_node *Node1, macro_def_linked_list_node *Node2 );
link_internal b32 AreEqual(macro_def *Element1, macro_def *Element2 );

link_internal macro_def_linked_list_node *
Allocate_macro_def_linked_list_node(memory_arena *Memory)
{
  macro_def_linked_list_node *Result = Allocate( macro_def_linked_list_node, Memory, 1);
  return Result;
}

link_internal macro_def_hashtable
Allocate_macro_def_hashtable(u32 ElementCount, memory_arena *Memory)
{
  macro_def_hashtable Result = {
    .Elements = Allocate( macro_def_linked_list_node*, Memory, ElementCount),
    .Size = ElementCount,
    .Pad = 0,
  };
  return Result;
}

link_internal macro_def_linked_list_node *
GetHashBucket(u32 HashValue, macro_def_hashtable *Table)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  Assert(Table->Size);
  macro_def_linked_list_node *Result = Table->Elements[HashValue % Table->Size];
  return Result;
}

link_internal macro_def *
GetFirstAtBucket(u32 HashValue, macro_def_hashtable *Table)
{
  /* ENSURE_OWNED_BY_THREAD(Table); */

  macro_def_linked_list_node *Bucket = GetHashBucket(HashValue, Table);
  macro_def *Result = &Bucket->Element;
  return Result;
}

link_internal macro_def_linked_list_node**
GetMatchingBucket(macro_def Element, macro_def_hashtable *Table, memory_arena *Memory)
{
  u32 HashValue = Hash(&Element) % Table->Size;
  macro_def_linked_list_node **Bucket = Table->Elements + HashValue;
  while (*Bucket)
  {
    if (AreEqual(&Bucket[0]->Element, &Element)) { break; }
    Bucket = &(*Bucket)->Next;
  }
  return Bucket;
}

link_internal macro_def *
InsertBlank(u32 HashValue, macro_def_hashtable *Table, memory_arena *Memory)
{
  Assert(Table->Size);
  macro_def_linked_list_node **Bucket = Table->Elements + (HashValue % Table->Size);
  macro_def_linked_list_node  *Node   = Allocate_macro_def_linked_list_node(Memory);
  Node->Next = *Bucket;
  *Bucket = Node;
  return &Bucket[0]->Element;
}

link_internal macro_def*
Insert(macro_def Element, macro_def_hashtable *Table, memory_arena *Memory)
{
  u32 HashValue = Hash(&Element) % Table->Size;
  auto Result = InsertBlank(HashValue, Table, Memory);
  *Result = Element;
  return Result;
}

link_internal macro_def*
Upsert(macro_def Element, macro_def_hashtable *Table, memory_arena *Memory)
{
  u32 HashValue = Hash(&Element) % Table->Size;
  macro_def_linked_list_node **Bucket = Table->Elements + HashValue;
  while (*Bucket)
  {
    if (AreEqual(&Bucket[0]->Element, &Element)) { break; }
    Bucket = &(*Bucket)->Next;
  }

  macro_def *Result = {};
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

struct macro_def_hashtable_iterator
{
  u32 HashIndex;
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

link_inline macro_def *
TryGetPtr(macro_def_hashtable *Hashtable, macro_def_hashtable_iterator Iterator)
{
  return GetPtr(Hashtable, Iterator);
}

 
