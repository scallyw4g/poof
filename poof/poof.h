

enum d_union_flags
{
  d_union_flag_none,
  d_union_flag_enum_only
};

enum poof_global_keyword
{
  poof_global_keyword_noop,

  poof_error,
  are_equal,

  var,
};
poof( string_and_value_tables(poof_global_keyword) )
#include <string_and_value_tables_poof_global_keyword.h>
/* #include <string_and_value_tables_poof_global_keyword.h> */
/* #include <generated/string_and_value_tables_poof_global_keyword.h> */


enum metaprogramming_directive
{
  meta_directive_noop,

  enum_only,
  d_union,
  named_list,
  for_datatypes,

  func,
  polymorphic_func,
};
poof( string_and_value_tables(metaprogramming_directive) )
#include <generated/string_and_value_tables_metaprogramming_directive.h>

enum meta_func_directive
{
  meta_func_directive_noop,

  omit_include          = (1 << 0),
  code_fragment         = (1 << 1),
  origin_comment_format = (1 << 2),
};
poof( string_and_value_tables(meta_func_directive) )
#include <string_and_value_tables_meta_func_directive.h>



enum meta_arg_operator
{
  meta_arg_operator_noop,

  // 'literal' values
  name,
  type,
  value,
  array,
  hash,  // hash of the datatype
  indirection,
  tags,

  file, // file the type occurs in
  line, // line the type occurs on

  // iterative
  map_array,
  map_values,
  map_members,
  map_args,
  map,

  // utility
  sep,

  // accessors
  member,

  // predicates
  is_enum,
  is_struct,
  is_union,
  is_pointer,
  is_defined,
  is_compound,
  is_primitive,
  is_function,
  is_array,
  is_type,
  is_named,
  contains_type,

  has_tag,
  tag_value,
};

poof( generate_value_table(meta_arg_operator) )
#include <generated/generate_value_table_meta_arg_operator.h>

enum meta_transform_op
{
  meta_transform_op_noop,

  to_capital_case     ,
  to_snake_case       ,
  to_lowercase        ,
  to_uppercase        ,
  strip_single_prefix ,
  strip_all_prefix    , // TODO(Jesse): Change name to strip_all_prefixes
};
poof(generate_value_table(meta_transform_op))
#include <generated/generate_value_table_enum.h>






#if 0
struct c_token_cursor;
struct c_token;
struct up_info
{
  c_token_cursor *Tokens;
  c_token *At;
};

up_info
UpInfo(c_token_cursor *Tokens, c_token *At)
{
  up_info Result = {
    .Tokens = Tokens,
    .At = At,
  };
  return Result;
}
#endif



enum va_args_flags
{
  va_args_flags_none = 0,
  va_args_flags_expand_with_commas = (1 << 0),
};

struct declaration_stream_chunk;

/* TODO(Jesse id: 191, tags: metaprogramming): This can be generated, but it
 * requires lazily evaluating functions once their dependant types have been
 * generated.  Not sure if this is a good idea or not, but we could do it.
 *
 * poof( generate_stream_struct(declaration) )
 *
 * Currently the invariant of being able to remove and re-generate all poof
 * output in a single run of the metaprogramming system is broken by this.
 */

struct declaration_stream
{
  memory_arena *Memory = &Global_PermMemory;
  declaration_stream_chunk *FirstChunk;
  declaration_stream_chunk *LastChunk;
  umm ChunkCount;
};

struct compound_decl // structs and unions
{
  c_token *Type;
  declaration_stream Members;
  b32 IsUnion;
};

link_internal b32
AreEqual(compound_decl *Thing0, compound_decl *Thing1)
{
  NotImplemented;
  return False;
}

link_internal b32
AreEqual(compound_decl Thing0, compound_decl Thing1)
{
  return AreEqual(&Thing0, &Thing1);
}

poof(stream_and_cursor(compound_decl))
#include <generated/stream_and_cursor_struct_def.h> // TODO(Jesse): Change this name

enum linkage_type
{
  linkage_noop,
  linkage_extern,
  linkage_extern_c,
  linkage_extern_cpp,
};

struct type_indirection_info
{
  b32 ConstPointer;
  b32 VolatileValue;

  u32 ReferenceLevel;
  u32 IndirectionLevel;

  b32 IsFunction;
  b32 IsFunctionPtr;

  /* c_token *FunctionNameT; */
};

global_variable type_indirection_info NullIndirection = {};

// TODO(Jesse): Adding an index feature to map_values would let us generate
// bitfields like this.  We could also do stuff like statically ensure a
// particular width, or quickly re-order fields without having to manually
// re-jig the shifts.
//
enum type_qualifier
{
  TypeQual_None        = 0,
  TypeQual_Static      = (1 << 0),
  TypeQual_Void        = (1 << 1),
  TypeQual_Auto        = (1 << 2),
  TypeQual_Class       = (1 << 3),
  TypeQual_Struct      = (1 << 4),
  TypeQual_Union       = (1 << 5),
  TypeQual_Enum        = (1 << 6),
  TypeQual_Bool        = (1 << 7),
  TypeQual_Inline      = (1 << 8),
  TypeQual_ThreadLocal = (1 << 9),
  TypeQual_Extern      = (1 << 10),
  TypeQual_Const       = (1 << 11),
  TypeQual_Volatile    = (1 << 12),
  TypeQual_Signed      = (1 << 13),
  TypeQual_Unsigned    = (1 << 14),
  TypeQual_Char        = (1 << 15),
  TypeQual_Long        = (1 << 16),
  TypeQual_Long_Long   = (1 << 17),
  TypeQual_Int         = (1 << 18),
  TypeQual_Double      = (1 << 19),
  TypeQual_Short       = (1 << 20),
  TypeQual_Float       = (1 << 21),
  TypeQual_Constexpr   = (1 << 22),
  TypeQual_Explicit    = (1 << 23),
  TypeQual_Operator    = (1 << 24),
  TypeQual_Virtual     = (1 << 25),
  TypeQual_Noreturn    = (1 << 26),
};
poof(string_and_value_tables(type_qualifier))
#include <generated/string_and_value_tables_type_qualifier.h>

#if 0
enum type_spec_type
{
  type_type_spec_type_noop,

  type_named,     // DatatypeToken is set, Datatype is set , Qualifier maybe
                  // typedef, struct, enum, union

  type_primitive, // DatatypeToken is   0, Datatype is set?, Qualifier is set
                  // int, float, const long double, etc..

  type_undefined, // DatatypeToken is set, Datatype is   0 , Qualifier maybe
                  // something that came from outside our codebase
                  // probably from stdlib .. FILE* .. whatever
};
#endif

struct datatype;
struct type_spec
{
  c_token *QualifierNameT;
  c_token *DatatypeToken;

  // @base_type_hack
  datatype *BaseType;     // ATM only used by variable instances that are defined using an anonymous compound decl

  type_qualifier        Qualifier;
  type_indirection_info Indirection;

  // TODO(Jesse): Can we not just check if TemplateSource is nonzero for this..?
  b32 HasTemplateArguments;
  cs TemplateSource;

  linkage_type Linkage;
};

struct ast_node;
struct variable_decl
{
  type_spec Type; // TODO(Jesse): Change name to TypeSpec
   c_token *NameT;
  ast_node *StaticBufferSize;
  ast_node *Value;

  u32 StrictBitWidth; // For declarations in structs like : `int foo_bool : 1`
};
poof(generate_stream(variable_decl))
#include <generated/generate_stream_variable_decl.h>

enum function_type
{
  function_type_noop,
  function_type_constructor,
  function_type_destructor,
  function_type_operator,
  function_type_normal,
};
poof(string_and_value_tables(function_type))
#include <generated/string_and_value_tables_function_type.h>

struct ast_node_statement;
struct function_decl
{
  function_type Type;

  type_spec  ReturnType;
    c_token *NameT;

  variable_decl_stream Args;

  // TODO(Jesse): Pack these into a u8 flags
  b32 IsVariadic;
  b32 ImplIsDefault;
  b32 ImplIsDeleted;

  ast_node *Ast;
    parser  Body;
};

poof(generate_stream(function_decl))
#include <generated/generate_stream_function_decl.h>

struct poof_tag
{
  cs Name;// poof(user_tag(foo))
  cs Value;
};

poof(block_array(poof_tag, {2}))
#include <generated/block_array_poof_tag_688856405.h>



struct ast_node_expression;
struct enum_member
{
  c_token *NameT;
  /* cs Name; */
  ast_node_expression *Expr;
  poof_tag_block_array Tags;
};
poof(generate_stream(enum_member))
#include <generated/generate_stream_enum_member.h>

struct enum_decl
{
  c_token *NameT;
  enum_member_stream Members; // = {&Global_PermMemory, 0, 0, 0};
};


link_internal b32
AreEqual(enum_decl *Thing0, enum_decl *Thing1)
{
  NotImplemented;
  return False;
}

link_internal b32
AreEqual(enum_decl Thing0, enum_decl Thing1)
{
  return AreEqual(&Thing0, &Thing1);
}

poof(stream_and_cursor(enum_decl))
#include <generated/stream_and_cursor_enum_def.h>

poof(
  d_union declaration
  {
    enum_decl
    function_decl
    compound_decl
    variable_decl
  },
  {
    poof_tag_block_array Tags;
  }
)
#include <generated/d_union_declaration.h>


link_internal b32
AreEqual(declaration *Thing0, declaration *Thing1)
{
  NotImplemented;
  return False;
}

link_internal b32
AreEqual(declaration Thing0, declaration Thing1)
{
  return AreEqual(&Thing0, &Thing1);
}

poof(string_and_value_tables(declaration_type))
#include <generated/string_and_value_tables_declaration_type.h>

poof(generate_cursor(declaration))
#include <generated/generate_cursor_declaration.h>

poof(generate_stream_chunk_struct(declaration))
#include <generated/generate_stream_chunk_declaration.h>

// TODO(Jesse): Should type_spec actually be representative of primitive types?
struct primitive_def
{
  type_spec TypeSpec;
};

enum datatype_type
{
  type_datatype_noop,
  type_declaration,
  type_enum_member,
  type_type_def,
  type_primitive_def,
};
poof(generate_string_table(datatype_type))
#include <generated/generate_string_table_datatype_type.h>

struct enum_member;

struct type_def
{
  counted_string Alias;
  type_spec Type;
};
poof(generate_stream(type_def))
#include <generated/generate_stream_type_def.h>

struct datatype poof(@d_union)
{
  datatype_type Type;

  union poof(@d_union_target)
  {
    declaration    declaration;
    primitive_def  primitive_def;
    enum_member    enum_member;
    type_def       type_def;
  };
};

poof(maybe(datatype))
#include <generated/maybe_datatype.h>

typedef datatype* datatype_ptr;
poof(maybe(datatype_ptr))
#include <generated/maybe_datatype_ptr.h>

/* poof(dunion_debug_print(declaration)) */
/* #include <generated/dunion_debug_print_declaration.h> */

/* poof(debug_print(enum_decl)) */
/* #include <generated/debug_print_enum_decl.h> */
/* poof(debug_print(function_decl)) */
/* #include <generated/debug_print_function_decl.h> */
/* poof(debug_print(compound_decl)) */
/* #include <generated/debug_print_compound_decl.h> */
/* poof(debug_print(variable_decl)) */
/* #include <generated/debug_print_variable_decl.h> */

/* poof(debug_print(primitive_def)) */
/* #include <generated/debug_print_primitive_def.h> */
/* poof(debug_print(enum_member)) */
/* #include <generated/debug_print_enum_member.h> */
/* poof(debug_print(type_def)) */
/* #include <generated/debug_print_type_def.h> */

poof(dunion_debug_print(datatype))
#include <generated/dunion_debug_print_datatype.h>

link_internal umm Hash(datatype *D);

poof(hashtable(datatype))
#include <generated/hashtable_datatype.h>

struct d_union_member
{
  counted_string Type;
  counted_string Name;
  d_union_flags Flags;
};
poof(generate_stream(d_union_member))
#include <generated/generate_stream_d_union_member.h>

link_internal datatype
Datatype()
{
  datatype Result = {};
  return Result;
}

link_internal datatype
Datatype(declaration *M)
{
  datatype Result = {
    .Type = type_declaration,
    .declaration = *M,
  };
  return Result;
}

link_internal datatype
Datatype(enum_member *E)
{
  datatype Result = {
    .Type = type_enum_member,
    .enum_member = *E,
  };
  return Result;
}

link_internal declaration
Declaration(compound_decl *S)
{
  declaration Result = {};
  Result.Type = type_compound_decl;
  Result.compound_decl = *S;
  return Result;
}

link_internal declaration
Declaration(variable_decl *S)
{
  declaration Result = {};
  Result.Type = type_variable_decl;
  Result.variable_decl = *S;
  return Result;
}

link_internal declaration
Declaration(function_decl *F)
{
  declaration Result = {};
  Result.Type = type_function_decl;
  Result.function_decl = *F;
  return Result;
}

link_internal datatype
Datatype(function_decl *F)
{
  datatype Result = {};
  Result.Type = type_declaration;
  Result.declaration = Declaration(F);
  return Result;
}

link_internal datatype
Datatype(compound_decl *S)
{
  datatype Result = {};
  Result.Type = type_declaration;
  Result.declaration = Declaration(S);
  return Result;
}

link_internal datatype
Datatype(type_def* E)
{
  datatype Result = {
    .Type = type_type_def,
    .type_def = *E,
  };
  return Result;
}

link_internal declaration
Declaration(enum_decl* E)
{
  declaration Result = {};
  Result.Type = type_enum_decl;
  Result.enum_decl = *E;
  return Result;
}

link_internal datatype
Datatype(enum_decl* E)
{
  datatype Result = {};
  Result.Type = type_declaration;
  Result.declaration = Declaration(E);
  return Result;
}

link_internal declaration
Declaration(type_spec *T)
{
  declaration Result = {};
  Result.Type = type_function_decl;
  return Result;
}

#if 1
link_internal datatype
Datatype(type_spec S)
{
  datatype Result = {};
  if ( S.Indirection.IsFunction   ||
       S.Indirection.IsFunctionPtr)
  {
    Result.Type = type_declaration;
    Result.declaration = Declaration(&S);
  }
  else
  {
    Result.Type = type_primitive_def;
    Result.primitive_def.TypeSpec = S;
  }

  return Result;
}
#endif


struct poof_index
{
  u32 Index;
  u32 MaxIndex;
};

poof(gen_constructor(poof_index))
#include <generated/gen_constructor_poof_index.h>


struct poof_symbol
{
  cs Value;
};
poof(gen_constructor(poof_symbol))
#include <generated/gen_constructor_poof_symbol.h>



poof(
  d_union meta_func_arg
  {
    datatype // TODO(Jesse): Change to pointer
    poof_index
    poof_symbol
  },
  {
    counted_string Match;
  }
)
#include <generated/d_union_meta_func_arg.h>

poof(string_and_value_tables(meta_func_arg_type))
#include <generated/string_and_value_tables_meta_func_arg_type.h>

poof(d_union_constructors(meta_func_arg))
#include <generated/d_union_constructors_meta_func_arg.h>

poof(generate_stream(meta_func_arg))
#include <generated/generate_stream_meta_func_arg.h>

poof(buffer(meta_func_arg))
#include <generated/buffer_meta_func_arg.h>

poof(generate_stream_compact(meta_func_arg))
#include <generated/generate_stream_compact_meta_func_arg.h>

link_internal meta_func_arg
MetaFuncArg(parse_context *Ctx, poof_tag Tag);

// TODO(Jesse)(metaprogramming): Generate this!  There's a bunch of this kind
// of code that could be generated.  I think some functions I wrote by hand are
// called "GetByName" or something.
//
link_internal meta_func_arg *
GetByMatch(meta_func_arg_buffer *Buf, counted_string Match)
{
  meta_func_arg *Result = {};

  // NOTE(Jesse): This handles null buffers as a convenience for the callers to
  // be able to not check, such that we can do `if (auto Match = GetByMatch(...)) { ... }`
  //
  // If we did not handle nulls the calling code gets a lot uglier
  if (Buf)
  {
    for (u32 ArgIndex = 0; ArgIndex < Buf->Count; ++ArgIndex)
    {
      meta_func_arg *Arg = Buf->Start + ArgIndex;
      if (AreEqual(Arg->Match, Match))
      {
        Result = Arg;
        break;
      }
    }
  }
  return Result;
}



link_internal void
CopyBufferIntoBuffer(meta_func_arg_buffer *Buf1, meta_func_arg_buffer *Buf2, umm Offset = 0)
{
  Assert( (Buf1->Count+Offset) <= Buf2->Count);
  for (umm Index = Offset; Index < Buf1->Count; ++Index)
  {
    Assert(Index < Buf1->Count && Index < Buf2->Count);
    Buf2->Start[Index] = Buf1->Start[Index];
  }
}

link_internal meta_func_arg_buffer
ExtendBuffer(meta_func_arg_buffer *Buf, umm ExtendCount, memory_arena *Memory)
{
  meta_func_arg_buffer Result = MetaFuncArgBuffer(Buf->Count + ExtendCount, Memory);
  CopyBufferIntoBuffer(Buf, &Result);
  return Result;
}

link_internal meta_func_arg_buffer
MergeBuffers(meta_func_arg_buffer *Buf1, meta_func_arg_buffer *Buf2, memory_arena *Memory)
{
  meta_func_arg_buffer Result = MetaFuncArgBuffer(Buf1->Count + Buf2->Count, Memory);
  CopyBufferIntoBuffer(Buf1, &Result);
  CopyBufferIntoBuffer(Buf2, &Result, Buf1->Count);
  return Result;
}

link_internal void
SetReverse(s32 IndexMod, meta_func_arg_buffer *Buf, meta_func_arg Arg)
{
  Assert(Buf->Count);
  s32 Index = (s32(Buf->Count-1) - IndexMod);
  Assert (Index >= 0 && Index < s32(Buf->Count));
  Buf->Start[Index] = Arg;
}

link_internal void
SetLast(meta_func_arg_buffer *Buf, meta_func_arg Arg)
{
  Assert(Buf->Count);
  SetReverse(0, Buf, Arg);
}

struct d_union_decl
{
  counted_string Name;

  d_union_member_stream Members;
  compound_decl CommonMembers;

  counted_string CustomEnumType;
};

enum macro_type
{
  type_macro_noop,

  type_macro_keyword,
  type_macro_function,
};

struct macro_def
{
  macro_type Type;
  c_token *NameT;
  c_token_cursor Body;

  counted_string_buffer NamedArguments;
  b32 Variadic;
  b32 Undefed; // Gets toggled when we hit an undef
  b32 IsExpanding;
};
poof(generate_stream(macro_def))
#include <generated/generate_stream_macro_def.h>

link_internal u64
Hash(macro_def *M)
{
  u64 Result = Hash(&M->NameT->Value);
  // TODO(Jesse): Hash arguments and incorporate Type & Variadic ..?
  return Result;
}

poof(are_equal(macro_def))
#include <generated/are_equal_macro_def.h>

poof(hashtable(macro_def))
#include <generated/hashtable_macro_def.h>


#define DEFAULT_META_FUNC_HEADER_FORMAT_STRING CSz("// %S:%u:0\n")

struct meta_func
{
  cs Name;
  meta_func_arg_buffer Args;
  parser Body;
  meta_func_directive Directives;
  cs HeaderFormatString = DEFAULT_META_FUNC_HEADER_FORMAT_STRING;
};
poof(generate_stream(meta_func))
#include <generated/generate_stream_meta_func.h>

poof(gen_constructor(meta_func))
#include <generated/gen_constructor_meta_func.h>

link_internal meta_func
MetaFunc(cs Name, meta_func_arg_buffer Args)
{
  meta_func Reuslt = {
    .Name = Name,
    .Args = Args,
    .Body = {},
    .Directives = {},
  };
  return Reuslt;
}


struct todo
{
  counted_string Id;
  counted_string Value;
  b32 FoundInCodebase;
};
poof(generate_stream(todo))
#include <generated/generate_stream_todo.h>

struct tag
{
  counted_string Tag;
  todo_stream Todos;
};
poof(generate_stream(tag))
#include <generated/generate_stream_tag.h>

struct person
{
  counted_string Name;
  tag_stream Tags;
};
poof(generate_stream(person))
#include <generated/generate_stream_person.h>

// TODO(Jesse): Delete these
#define SafeAccessObj(T, Obj) ((Obj).T); Assert((Obj).Type == type_##T)
#define SafeAccessObjPtr(T, Obj) (&(Obj).T); Assert((Obj).Type == type_##T)

struct ast_node_expression
{
  ast_node *Value;
  ast_node_expression *Next;
};
poof(generate_stream(ast_node_expression))
#include <generated/generate_stream_ast_node_expression.h>

struct ast_node_statement
{
  ast_node_expression *LHS;
  ast_node_statement  *RHS;
  ast_node_statement  *Next;
};

struct ast_node_function_call
{
  counted_string Name;
  function_decl *Prototype;
  ast_node_expression_stream Args;
};

struct ast_node_type_specifier
{
  datatype Datatype;
  type_spec TypeSpec;
  ast_node_expression *Name;
};

struct ast_node_variable_def
{
  datatype Type;
  variable_decl Decl;
  ast_node *Value;
};
poof(generate_stream(ast_node_variable_def))
#include <generated/generate_stream_ast_node_variable_def.h>

struct ast_node_access
{
  c_token AccessType; // Arrow or Dot
  ast_node_expression *Symbol;
};

struct ast_node_parenthesized
{
  ast_node_expression *Expr;

  // TODO(Jesse id: 262): These should probably be in their own type_ast_node_cast .. ?
  b32 IsCast;
  ast_node_expression *CastValue;
};

struct ast_node_initializer_list
{
  u32 Ignored;
};

struct ast_node_operator
{
  ast_node_expression *Operand;
  c_token Token;
};

struct ast_node_literal
{
  c_token Token;
};

struct ast_node_symbol
{
  c_token Token;
};

struct ast_node_return
{
  ast_node_expression *Value;
};

struct ast_node_predicated
{
  c_token Token;
  ast_node_expression *Predicate;
};

poof(
  d_union ast_node
  {
    ast_node_statement
    ast_node_access
    ast_node_literal
    ast_node_expression
    ast_node_parenthesized
    ast_node_operator
    ast_node_function_call
    ast_node_return
    ast_node_initializer_list
    ast_node_symbol
    ast_node_variable_def
    ast_node_type_specifier
  }
)
#include <generated/d_union_ast_node.h>

poof(generate_stream(ast_node))
#include <generated/generate_stream_ast_node.h>

poof(string_and_value_tables(ast_node_type))
#include <generated/string_and_value_tables_ast_node_type.h>


link_internal ast_node*
AllocateAstNode(ast_node_type T, ast_node **Result, memory_arena* Memory)
{
  Assert(Result && (*Result == 0)); // We got a valid pointer, and it hasn't been allocated yet.

  *Result = AllocateProtection(ast_node, Memory, 1, False);
  (*Result)->Type = T;

  return *Result;
}

#define AllocateAndCastTo(T, NodeDest, Memory) (&AllocateAstNode(type_##T, (NodeDest), (Memory))->T)

struct arguments
{
  counted_string Outpath;
  counted_string_cursor Files;

  counted_string_cursor IncludePaths;

  b32 DoDebugWindow;
  b32 HelpTextPrinted;
  b32 DoNotNormalizeWhitespace;
};

struct program_datatypes
{
  datatype_hashtable       DatatypeHashtable;
  macro_def_hashtable      Macros;
  counted_string_hashtable FilesParsed;
};

struct for_enum_constraints
{
  // Replacement Patterns
  counted_string TypeName;
  counted_string ValueName;
};

/* struct body_text_constraints */
/* { */
/*   counted_string MemberContains; */
/*   // Replacement Patterns */
/*   counted_string TypeTag; */
/*   counted_string TypeName; */
/*   counted_string ValueName; */
/* }; */

struct todo_list_info
{
  person_stream People;
  tagged_counted_string_stream_stream NameLists;
};

enum output_mode
{
  Output_NoOverwrite,
  Output_Unsafe,
};


poof(generate_stream_iterator(declaration))
#include <generated/generate_stream_iterator_datatype.h>

poof(generate_stream_push(declaration))
#include <generated/generate_stream_push_datatype.h>

enum parser_push_type
{
  parser_push_type_noop,

  parser_push_type_root,
  parser_push_type_include,
  parser_push_type_macro,
};

struct parse_context
{
  parser                *CurrentParser;

  parser_hashtable       ParserHashtable;

  program_datatypes      Datatypes;
  counted_string_cursor *IncludePaths;
  meta_func_stream       MetaFunctions;
  memory_arena          *Memory;

  tagged_counted_string_stream_stream NamedLists;

  arguments Args;

  u32 CallStackDepth;
};

enum erase_token_mode
{
  PreserveTokens = 0,
  EraseTokens = 1,
};

// TODO(Jesse): Make this more formal?  Seems like this should basically be a type_spec
struct comma_separated_decl
{
  c_token *NameT;
  type_indirection_info Indirection;
  ast_node *StaticBufferSize;
  ast_node *Value;
};

enum typedef_resolution_behavior
{
  TypedefResoultion_Undefined,

  TypedefResoultion_ResolveTypedefs,
  TypedefResoultion_DoNotResolveTypedefs,
};

link_internal parse_context
AllocateParseContext(memory_arena *Memory)
{
  parse_context Ctx = {};

  Ctx.Memory = Memory;

  // TODO(Jesse): Collapse these
  Ctx.ParserHashtable = Allocate_parser_hashtable(128, Memory);
  Ctx.Datatypes.FilesParsed = Allocate_counted_string_hashtable(512, Memory);

  Ctx.Datatypes.Macros = Allocate_macro_def_hashtable(4096, Memory);
  Ctx.Datatypes.DatatypeHashtable = Allocate_datatype_hashtable(2048, Memory);

  Ctx.NamedLists = TaggedCountedStringStreamStream(Memory);
  Ctx.MetaFunctions = MetaFuncStream(Memory);

  return Ctx;
}


