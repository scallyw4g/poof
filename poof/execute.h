
link_internal c_token
NormalizeWhitespaceTokens(c_token *T, c_token* PrevT, c_token *NextT, umm *Depth)
{
  c_token Result = *T;

  if (T->Type == '{' || T->Type == '(')
  {
    *Depth += 1;
  }

  if (T->Type == '}' || T->Type == ')')
  {
    *Depth = SafeDecrement(Depth);
  }

  if ( (IsNBSP(T) && PrevT && PrevT->Type == CTokenType_Newline) ||
       (IsNBSP(T) && PrevT == 0) )
  {

    s32 NewCount = s32((*Depth)*2);
    /* s32 NewCount = Max(0, s32(T->Value.Count) - s32((*Depth)*2)-4); */
    /* s32 NewCount = Max(0, s32(T->Value.Count) - s32((*Depth)*2)); */
    if ( NextT && (NextT->Type == '}' || NextT->Type == ')'))
    {
      NewCount = Max(0, NewCount-2);
      Result.Value.Count = Min(T->Value.Count, umm(NewCount));
    }
    else
    {
      Result.Value.Count = Min(T->Value.Count, umm(NewCount));
    }
  }

  if (IsNBSP(T))
  {

    if ( PrevT && PrevT->Type == T->Type )
    {
      InvalidCodePath();
      /* Result.Value.Count = 0; */
    }
  }

  return Result;
}

link_internal b32
IsAllWhitespace(cs *Str)
{
  b32 Result = True;
  for (u32 Index = 0; Index < Str->Count; ++Index)
  {
    Result &= IsWhitespace((c_token_type)Str->Start[Index]);
    if (!Result) break;
  }
  return Result;
}

link_internal void
HandleWhitespaceAndAppend( string_builder *OutputBuilder, counted_string Output, counted_string Sep = {}, b32 TrimOutputTrailingNewline = False)
{
  if (IsAllWhitespace(&Output))
  {
    // Skip anything that just amounts to whitespace
  }
  else
  {
    cs *LastStr = &OutputBuilder->Chunks.LastChunk->Element;
    if (LastStr)
    {
      if (IsAllWhitespace(LastStr))
      {
        // TODO(Jesse): WTF?  Don't we skip appending stuff that's all whitespace.. ???
        // InvalidCodePath fires here..
        LastStr->Count = 0;
      }

      while (LastNBSPChar(&Output) == '\n')
      {
        TrimTrailingNBSP(&Output);
        if (TrimOutputTrailingNewline)
        {
          TrimTrailingNewline(&Output);
        }
        else
        {
          break;
        }
      }

      if (LastNBSPChar(LastStr) == '\n')
      {
        TrimTrailingNBSP(LastStr);
      }

      if (Sep.Count)
      {
        if (TrimTrailingNewline(&Output))
        {
          if (LastChar(Sep) != '\n')
          {
            Sep = Concat(Sep, CSz("\n"), OutputBuilder->Memory);
          }
        }
      }
    }

    Append(OutputBuilder, Output);
    if (Sep.Count) { Append(OutputBuilder, Sep); }
  }
}

poof(
  func maybe(ValueType, ErrorType)
  {
    struct maybe_(ValueType.name.to_lowercase)
    {
      ValueType.name E;
      ErrorType.name Error;
    };
  }
)

#if 0
// TODO(Jesse)(abstract): How would we articulate this if we wanted to have
// multiple different maybe boxes for counted string?  I guess we'd have to
// mangle the name to include both types.. but that seems like it sucks a lot..
poof( maybe(counted_string, parse_error_code) )
#include <generated/maybe_counted_string_parse_error_code.h>

#else
// TODO(Jesse): WTF is this nonsense??
struct maybe_counted_string
{
  parse_error_code Error;
  counted_string E;
};
#endif


link_internal maybe_counted_string
MapFunctionDeclArgs(parse_context *Ctx, function_decl *FuncDecl, meta_func *MetaF, cs *EnumValueMatch, cs *Sep, memory_arena *Memory, umm *Depth)
{
  maybe_counted_string Result = {};
  string_builder OutputBuilder = {};

  meta_func_arg_buffer NewArgs = ExtendBuffer(&MetaF->Args, 1, Memory);
  ITERATE_OVER(&FuncDecl->Args)
  {
    Rewind(MetaF->Body.Tokens);

    variable_decl *Arg = GET_ELEMENT(Iter);
    auto D = Declaration(Arg);
    SetLast(&NewArgs, ReplacementPattern(*EnumValueMatch, Datatype(&D)));

    cs Output = Execute(MetaF, &NewArgs, Ctx, Memory, Depth);
    if (MetaF->Body.ErrorCode)
    {
      Result.Error = MetaF->Body.ErrorCode;
      break;
    }
    else
    {
      // TODO(Jesse): Pretty sure there should be a better way of handling this now that
      // all the map paths have collapsed..
      if (Iter.At->Next)
      {
        HandleWhitespaceAndAppend(&OutputBuilder, Output, *Sep);
      }
      else
      {
        HandleWhitespaceAndAppend(&OutputBuilder, Output, {}, True);
      }
    }
  }


  if (Result.Error == 0)
  {
    Result.E = Finalize(&OutputBuilder, Memory);
  }
  else
  {
    Discard(&OutputBuilder);
  }

  return Result;
}
link_internal maybe_counted_string
MapEnumValues(parse_context *Ctx, enum_decl *Enum, meta_func *MetaF, cs *EnumValueMatch, cs *Sep, memory_arena *Memory, umm *Depth)
{
  maybe_counted_string Result = {};
  string_builder OutputBuilder = {};

  meta_func_arg_buffer NewArgs = ExtendBuffer(&MetaF->Args, 1, Memory);
  ITERATE_OVER(&Enum->Members)
  {
    Rewind(MetaF->Body.Tokens);

    enum_member* EnumMember = GET_ELEMENT(Iter);
    SetLast(&NewArgs, ReplacementPattern(*EnumValueMatch, Datatype(EnumMember)));

    counted_string EnumFieldOutput = Execute(MetaF, &NewArgs, Ctx, Memory, Depth);
    if (MetaF->Body.ErrorCode)
    {
      Result.Error = MetaF->Body.ErrorCode;
      break;
    }
    else
    {
      // TODO(Jesse): Pretty sure there should be a better way of handling this now that
      // all the map paths have collapsed..
      if (Iter.At->Next)
      {
        HandleWhitespaceAndAppend(&OutputBuilder, EnumFieldOutput, *Sep);
      }
      else
      {
        HandleWhitespaceAndAppend(&OutputBuilder, EnumFieldOutput, {}, True);
      }
    }
  }


  if (Result.Error == 0)
  {
    Result.E = Finalize(&OutputBuilder, Memory);
  }
  else
  {
    Discard(&OutputBuilder);
  }

  return Result;
}

link_internal cs
MaybeParseSepOperator(parser *Scope)
{
  cs Sep = {};
  if (OptionalTokenRaw(Scope, CTokenType_Dot))
  {
    cs Modifier = RequireToken(Scope, CTokenType_Identifier).Value;
    meta_arg_operator ModOp = MetaArgOperator(Modifier);
    switch (ModOp)
    {
      case sep:
      {
        Sep = EatBetweenExcluding_Str(Scope, CTokenType_OpenParen, CTokenType_CloseParen);
      } break;

      InvalidDefaultCase;
    }
  }

  return Sep;
}

// TODO(Jesse id: 222, tags: immediate, parsing, metaprogramming) : Re-add [[nodiscard]] here
link_internal counted_string
Execute(parser *Scope, meta_func_arg_buffer *Args, parse_context *Ctx, memory_arena *Memory, umm *Depth)
{
   meta_func F = MetaFunc(CSz("(anonymous)"), *Args, *Scope, False, False);
   cs Result = Execute(&F, Ctx, Memory, Depth);
   if (F.Body.ErrorCode)
   {
     Scope->ErrorCode = F.Body.ErrorCode;
     /* Assert(Result.Start == 0); */
     /* Assert(Result.Count == 0); */
   }
   return Result;
}

link_internal void
DoTrueFalse( parse_context *Ctx,
             parser *Scope,
             meta_func_arg_buffer *Args,
             b32 DoTrueBranch,
             string_builder *OutputBuilder,
             memory_arena *Memory,
             umm *Depth)
{
  parser TrueScope = GetBodyTextForNextScope(Scope, Memory);
  parser FalseScope = {};

  if (PeekToken(Scope).Type == CTokenType_OpenBrace)
  {
    FalseScope = GetBodyTextForNextScope(Scope, Memory);
  }

  parser *ParserToUse = DoTrueBranch ? &TrueScope : &FalseScope;

  if (ParserToUse->Tokens)
  {
    counted_string Code = Execute(ParserToUse, Args, Ctx, Memory, Depth);
    if (ParserToUse->ErrorCode)
    {
      Scope->ErrorCode = ParserToUse->ErrorCode;
    }
    else
    {
      HandleWhitespaceAndAppend(OutputBuilder, Code);
    }
  }
}

link_internal void
MapCompoundDeclMembers(parse_context *Ctx, parser *ParentScope, parser *MapScope, datatype *CompoundDatatype, cs *MatchValue, cs *Sep, meta_func_arg_buffer *Args, string_builder *OutputBuilder, memory_arena *Memory, umm *Depth)
{
  declaration_stream *Members = GetMembersFor(Ctx, CompoundDatatype);
  if (Members)
  {
    ITERATE_OVER_AS(Member, Members)
    {
      declaration* Member = GET_ELEMENT(MemberIter);

      switch (Member->Type)
      {
        case type_declaration_noop:
        {
          InvalidCodePath();
        } break;

        case type_enum_decl:
        case type_function_decl:
        {
          // What do we do for functions and enum declaratons ..?  Maybe the same thing as compound_decl?
        } break;

        case type_variable_decl:
        case type_compound_decl:
        {
          Rewind(MapScope->Tokens);

          meta_func_arg_buffer NewArgs = ExtendBuffer(Args, 1, Memory);
          SetLast(&NewArgs, ReplacementPattern(*MatchValue, Datatype(Member)));

          counted_string StructFieldOutput = Execute(MapScope, &NewArgs, Ctx, Memory, Depth);
          if (MapScope->ErrorCode)
          {
            ParentScope->ErrorCode = MapScope->ErrorCode;
          }
          else
          {
            // TODO(Jesse): Pretty sure there should be a better way of handling this now that
            // all the map paths have collapsed..
            if (MemberIter.At->Next)
            {
              HandleWhitespaceAndAppend(OutputBuilder, StructFieldOutput, *Sep);
            }
            else
            {
              HandleWhitespaceAndAppend(OutputBuilder, StructFieldOutput, {}, True);
            }
          }
        } break;
      }
      continue;
    }
  }
  else
  {
    // TODO(Jesse): Pretty sure this is InternalCompilerError
    InternalCompilerError(ParentScope , FSz("Tried to map compound decl that had no members.  Datatype (%S)", GetNameForDatatype(CompoundDatatype, GetTranArena())), PeekTokenPointer(ParentScope));
  }
}


link_internal void
Map( parse_context *Ctx,

     meta_arg_operator Operator,
     c_token *MetaOperatorToken,

     parser *ParentScope,
     parser *MapScope,

     datatype *ReplaceData,
     cs *MatchValue,
     cs *Sep,
     meta_func_arg_buffer *Args,

     string_builder *OutputBuilder,
     memory_arena *Memory,
     umm *Depth )
{
  switch (ReplaceData->Type)
  {
    case type_datatype_noop:
    {
      InternalCompilerError(ParentScope, CSz("Infinite sadness"), MetaOperatorToken);
    } break;

    case type_declaration:
    {
      declaration *Decl = SafeAccess(declaration, ReplaceData);

      switch(Decl->Type)
      {
        case type_declaration_noop:
        {
          InternalCompilerError(ParentScope, CSz("Infinite sadness"), MetaOperatorToken);
        } break;

        case type_function_decl:
        {
          if (Operator == map || Operator == map_args)
          {
            auto FuncDecl = SafeAccess(function_decl, Decl);
            meta_func MetaF = MetaFunc(CSz("map_function_decl_args"), *Args, *MapScope, False, False);

            maybe_counted_string MapResult = MapFunctionDeclArgs(Ctx, FuncDecl, &MetaF, MatchValue, Sep, Memory, Depth);
            if (MapResult.Error)
            {
            }
            else
            {
              HandleWhitespaceAndAppend(OutputBuilder, MapResult.E);
            }
          }
          else
          {
            PoofTypeError( ParentScope,
                           ParseErrorCode_InvalidArgument,
                           FormatCountedString( GetTranArena(),
                                                CSz("Incorrectly called (%S) on a function decl."),
                                                MetaOperatorToken->Value),
                           MetaOperatorToken);
          }
        } break;

        case type_enum_decl:
        {
          if (Operator == map || Operator == map_values)
          {
            auto EnumDecl = SafeAccess(enum_decl, Decl);
            meta_func MetaF = MetaFunc(CSz("map_enum_values"), *Args, *MapScope, False, False);

            maybe_counted_string MapResult = MapEnumValues(Ctx, EnumDecl, &MetaF, MatchValue, Sep, Memory, Depth);
            if (MapResult.Error)
            {
            }
            else
            {
              HandleWhitespaceAndAppend(OutputBuilder, MapResult.E);
            }
          }
          else
          {
            PoofTypeError( ParentScope,
                           ParseErrorCode_InvalidArgument,
                           FormatCountedString( GetTranArena(),
                                                CSz("Incorrectly called (%S) on an enum."),
                                                MetaOperatorToken->Value),
                           MetaOperatorToken);
          }
        } break;

        case type_compound_decl:
        {
          if (Operator == map || Operator == map_members)
          {
            MapCompoundDeclMembers(Ctx, ParentScope, MapScope, ReplaceData, MatchValue, Sep, Args, OutputBuilder, Memory, Depth);
          }
          else
          {
            PoofTypeError( ParentScope,
                           ParseErrorCode_InvalidArgument,
                           FormatCountedString( GetTranArena(),
                                                CSz("Incorrectly called (%S) on a compound decl."),
                                                MetaOperatorToken->Value),
                           MetaOperatorToken);
          }
        } break;

        case type_variable_decl:
        {
          variable_decl *VDecl = SafeAccess(variable_decl, Decl);

          switch (Operator)
          {
            default: {} break;;

            case map:
            {
              PoofTypeError( ParentScope,
                             ParseErrorCode_InvalidArgument,
                             FormatCountedString( GetTranArena(),
                                                  CSz("Calling (%S) on a variable decl is underspecified; use `map_members`, `map_values` or `map_array` specifically."),
                                                  MetaOperatorToken->Value),
                             MetaOperatorToken);
            } break;

            case map_members:
            {
              MapCompoundDeclMembers(Ctx, ParentScope, MapScope, ReplaceData, MatchValue, Sep, Args, OutputBuilder, Memory, Depth);
            } break;

            case map_array:
            {
              ast_node *ArraySizeNode = VDecl->StaticBufferSize;

              u64 ArrayLength = ResolveConstantExpression(ParentScope, ArraySizeNode);
              for (u64 ArrayIndex = 0; ArrayIndex < ArrayLength; ++ArrayIndex)
              {
                meta_func_arg_buffer NewArgs = ExtendBuffer(Args, 1, Memory);
                SetLast(&NewArgs, ReplacementPattern(*MatchValue, PoofIndex(SafeTruncateToU32(ArrayIndex), SafeTruncateToU32(ArrayLength))));

                Rewind(MapScope->Tokens);
                counted_string StructFieldOutput = Execute(MapScope, &NewArgs, Ctx, Memory, Depth);
                if (MapScope->ErrorCode)
                {
                  ParentScope->ErrorCode = MapScope->ErrorCode;
                }
                else
                {
                  // TODO(Jesse): Pretty sure there should be a better way of handling this now that
                  // all the map paths have collapsed..
                  if (ArrayIndex+1 < ArrayLength)
                  {
                    HandleWhitespaceAndAppend(OutputBuilder, StructFieldOutput, *Sep);
                  }
                  else
                  {
                    HandleWhitespaceAndAppend(OutputBuilder, StructFieldOutput, {}, True);
                  }
                }
              }
            } break;

            case map_values:
            {
              datatype *Base = ResolveToBaseType(Ctx, VDecl->Type);

              switch (Base->Type)
              {
                InvalidCase(type_datatype_noop);

                case type_declaration:
                {
                  declaration *BaseDecl = SafeAccess(declaration, Base);
                  if (BaseDecl->Type == type_enum_decl)
                  {
                    auto EnumDecl = SafeAccess(enum_decl, BaseDecl);
                    meta_func MetaF = MetaFunc(CSz("map_enum_values"), *Args, *MapScope, False, False);
                    maybe_counted_string MapResult = MapEnumValues(Ctx, EnumDecl, &MetaF, MatchValue, Sep, Memory, Depth);
                    if (MapResult.Error)
                    {
                    }
                    else
                    {
                      HandleWhitespaceAndAppend(OutputBuilder, MapResult.E);
                    }

                  }
                  else
                  {
                    PoofTypeError( ParentScope,
                                   ParseErrorCode_InvalidArgumentType,
                                   FormatCountedString( GetTranArena(),
                                                        CSz("Incorrectly called (%S) on a variable decl of non-enum type."),
                                                        MetaOperatorToken->Value),
                                   MetaOperatorToken);
                  }
                } break;

                case type_enum_member:
                case type_type_def:
                {
                  InternalCompilerError(ParentScope, CSz("Got type_def or declaration when resolving base type of a variable_decl during map_values"), MetaOperatorToken);
                } break;

                case type_primitive_def:
                {
                  PoofTypeError( ParentScope,
                                 ParseErrorCode_InvalidArgumentType,
                                 FormatCountedString( GetTranArena(),
                                                      CSz("Incorrectly called (%S) on a variable decl of a primitive type."),
                                                      MetaOperatorToken->Value),
                                 MetaOperatorToken);
                } break;
              }

            } break;

          }

        } break;
      }
    } break;

    case type_enum_member:
    {
      NotImplemented; // TODO(Jesse)(error_messages): emit error message
    } break;

    case type_primitive_def:
    {
      NotImplemented; // TODO(Jesse)(error_messages): emit error message
    } break;

    case type_type_def:
    {
      datatype *T = ResolveToBaseType(Ctx, ReplaceData);
      Map(Ctx, Operator, MetaOperatorToken, ParentScope, MapScope, T, MatchValue, Sep, Args, OutputBuilder, Memory, Depth);
    } break;
  }
}

link_internal void
SetMapFuncArgs(parse_context *Ctx, meta_func_arg_buffer *Local, c_token *A0Arg, poof_index A1Arg)
{
}

link_internal void // void?
ResolveMetaOperator(parse_context *Ctx, meta_func_arg_buffer *Args, meta_func_arg *Replace, parser *Scope, meta_arg_operator Operator, c_token *BodyToken, c_token *MetaOperatorToken, memory_arena *Memory, umm *Depth, string_builder *OutputBuilder)
{
  switch (Replace->Type)
  {
    case type_meta_func_arg_noop:
    {
      InternalCompilerError(Scope, CSz("Got an invalid ReplaceType of type_meta_func_arg_noop in ResolveMetaOperator!"), MetaOperatorToken);
    } break;

    case type_poof_index:
    {
      poof_index *RepIndex = SafeCast(poof_index, Replace);

      switch (Operator)
      {
        case meta_arg_operator_noop:
        {
          counted_string S = FormatCountedString(Memory, CSz("%u"), RepIndex->Index);
          Append(OutputBuilder, S);
        } break;

        case map:
        case map_values:
        {
          cs Sep = MaybeParseSepOperator(Scope);

          meta_func MapFunc = ParseMapMetaFunctionInstance(Scope, CSz("(builtin.map.index)"), Memory);
          meta_func_arg_buffer NewArgs = ExtendBuffer(Args, MapFunc.Args.Count, Memory);

          // TODO(Jesse): Throw an error?
          Assert(MapFunc.Args.Count == 1);


          for(u32 Index = 0; Index < RepIndex->Index; ++Index)
          {
            SetLast(&NewArgs, ReplacementPattern(MapFunc.Args.Start->Match, PoofIndex(Index, 0)));

            Rewind(MapFunc.Body.Tokens);
            counted_string Code = Execute(&MapFunc, &NewArgs, Ctx, Memory, Depth);

            if (MapFunc.Body.ErrorCode)
            {
              Scope->ErrorCode = MapFunc.Body.ErrorCode;
              break;
            }
            else
            {
              // TODO(Jesse): Pretty sure there should be a better way of handling this now that
              // all the map paths have collapsed..
              if (Index < RepIndex->Index)
              {
                HandleWhitespaceAndAppend(OutputBuilder, Code, Sep);
              }
              else
              {
                HandleWhitespaceAndAppend(OutputBuilder, Code, {}, True);
              }
            }
          }
        } break;

        case name:
        case type:
        case value:
        case array:
        case hash:
        case indirection:
        case map_array:
        case map_members:
        case map_args:
        case sep:
        case member:
        case is_enum:
        case is_struct:
        case is_union:
        case is_pointer:
        case is_defined:
        case is_compound:
        case is_primitive:
        case is_function:
        case is_array:
        case is_type:
        case is_named:
        case contains_type:
        case has_tag:
        case tag_value:
        case tags:
        {
          PoofTypeError( Scope,
                         ParseErrorCode_InvalidOperator,
                         FSz("Operator (%S) is not valid on an index.", MetaOperatorToken->Value),
                         MetaOperatorToken);
        } break;

      }
    } break;

    case type_poof_symbol:
    {
      auto Symbol = SafeCast(poof_symbol, Replace);
      switch (Operator)
      {
        case meta_arg_operator_noop:
        {
          Append(OutputBuilder, Symbol->Value);
        } break;

        case map:
        case map_values:
        case map_members:
        {
          cs Sep = MaybeParseSepOperator(Scope);

          meta_func MapFunc = ParseMapMetaFunctionInstance(Scope, CSz("(builtin.map.symbol)"), Memory);
          Assert(MapFunc.Args.Count == 1 || MapFunc.Args.Count == 2);

          meta_func_arg_buffer NewArgs = ExtendBuffer(Args, MapFunc.Args.Count, Memory);

          poof_index Index = {};
          parser *SymbolParser = ParserForAnsiStream(Ctx, AnsiStream(Symbol->Value), TokenCursorSource_MetaprogrammingExpansion, Ctx->Memory);
          while (c_token *MapToken = PopTokenPointer(SymbolParser))
          {
            {
              meta_func_arg *A0 = MapFunc.Args.Start;
              datatype *D = GetDatatypeByName(&Ctx->Datatypes, MapToken->Value);
              if (D->Type)
              {
                SetLast(&NewArgs, ReplacementPattern(A0->Match, *D));
              }
              else
              {
                SetLast(&NewArgs, ReplacementPattern(A0->Match, PoofSymbol(MapToken->Value)));
              }
            }

            {
              meta_func_arg *A1 = {};
              if (MapFunc.Args.Count == 2) { A1 = MapFunc.Args.Start + 1; }
              if (A1) { SetReverse(1, &NewArgs, ReplacementPattern(A1->Match, Index)); }
            }

            Rewind(MapFunc.Body.Tokens);
            counted_string Code = Execute(&MapFunc, &NewArgs, Ctx, Memory, Depth);

            if (MapFunc.Body.ErrorCode)
            {
              Scope->ErrorCode = MapFunc.Body.ErrorCode;
              break;
            }
            else
            {
              // TODO(Jesse): Pretty sure there should be a better way of handling this now that
              // all the map paths have collapsed..
              if (PeekTokenPointer(SymbolParser))
              {
                HandleWhitespaceAndAppend(OutputBuilder, Code, Sep);
              }
              else
              {
                HandleWhitespaceAndAppend(OutputBuilder, Code, {}, True);
              }
            }

            ++Index.Index;
          }

        } break;

        default:
        {
          PoofTypeError( Scope,
                         ParseErrorCode_InvalidOperator,
                         FormatCountedString(GetTranArena(), CSz("Operator (%S) is not valid on a symbol."), MetaOperatorToken->Value),
                         MetaOperatorToken);
        } break;
      }
    } break;

    case type_datatype:
    {
      datatype *ReplaceData = SafeCast(datatype, Replace);
      Assert(ReplaceData->Type);

      switch (Operator)
      {
        case meta_arg_operator_noop:
        {
          if (MetaOperatorToken)
          {
            PoofTypeError( Scope,
                           ParseErrorCode_InvalidOperator,
                           FormatCountedString(GetTranArena(), CSz("(%S) is not a valid poof operator"), MetaOperatorToken->Value),
                           MetaOperatorToken);
          }
          else
          {
            cs DatatypeString = ToString(Ctx, Scope, ReplaceData, MetaOperatorToken, Memory);
            HandleWhitespaceAndAppend(OutputBuilder, DatatypeString);
            /* PoofTypeError( Scope, */
            /*                ParseErrorCode_InvalidOperator, */
            /*                CSz("poof datatype variable must be followed by a poof operator"), */
            /*                BodyToken); */
          }
        } break;

        case sep:
        {
          // TODO(Jesse): Emit error message here.
          NotImplemented;
        } break;

        case tags:
        {
          /* RequireToken(Scope, CTokenType_OpenParen); */
          /* cs TagName = RequireToken(Scope, CTokenType_Identifier).Value; */
          /* RequireToken(Scope, CTokenType_CloseParen); */

          poof_tag_block_array *Tags = GetTagsFromDatatype(Ctx, ReplaceData, Scope, MetaOperatorToken);

          IterateOver(Tags, Tag, TagIndex)
          {
            Assert(Tag->Name.Count);
            Append(OutputBuilder, Tag->Name);
            if (Tag->Value.Count)
            {
              Append(OutputBuilder, CSz("("));
              Append(OutputBuilder, Tag->Value);
              Append(OutputBuilder, CSz(")"));
            }
            Append(OutputBuilder, CSz(" "));
          }
        } break;

        case tag_value:
        {
          RequireToken(Scope, CTokenType_OpenParen);
          cs TagName = RequireToken(Scope, CTokenType_Identifier).Value;
          RequireToken(Scope, CTokenType_CloseParen);

          poof_tag Tag = GetTagFromDatatype(Ctx, TagName, ReplaceData, Scope, MetaOperatorToken);

#if 1
          if (OptionalTokenRaw(Scope, CTokenType_Dot))
          {
            c_token *ChainedOpT = RequireTokenPointer(Scope, CTokenType_Identifier);
            Assert(ChainedOpT);

            meta_arg_operator ChainedOp = MetaArgOperator( ChainedOpT->Value );

            meta_func_arg NewArg = MetaFuncArg(Ctx, Tag, CSz(""));
            ResolveMetaOperator(Ctx, Args, &NewArg, Scope, ChainedOp, BodyToken, ChainedOpT, Memory, Depth, OutputBuilder);
          }
          else
          {
            if (Tag.Name.Count)
            {
              HandleWhitespaceAndAppend(OutputBuilder, Tag.Value);
            }
          }
#else
          if (Tag.Name.Count)
          {
            HandleWhitespaceAndAppend(OutputBuilder, Tag.Value);
          }
#endif

        } break;

        case has_tag:
        {
          RequireToken(Scope, CTokenType_OpenParen);
          cs TagName = RequireToken(Scope, CTokenType_Identifier).Value;
          RequireToken(Scope, CTokenType_CloseParen);
          RequireToken(Scope, CTokenType_Question);

          b32 DoTrueBranch = DatatypeHasTag(Ctx, TagName, ReplaceData, Scope, MetaOperatorToken);
          DoTrueFalse( Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
        } break;

        case indirection:
        {
          if (OptionalToken(Scope, CTokenType_Question))
          {
            b32 DoTrueBranch = DatatypeIsPointer(Ctx, ReplaceData, Scope, MetaOperatorToken) == True;
            DoTrueFalse( Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
          }
          else
          {
            cs Indirection = PrintIndirection(ReplaceData, Memory);
            HandleWhitespaceAndAppend(OutputBuilder, Indirection);
          }
        } break;

        case is_pointer:
        {
          RequireToken(Scope, CTokenType_Question);

          b32 DoTrueBranch = DatatypeIsPointer(Ctx, ReplaceData, Scope, MetaOperatorToken) == True;
          DoTrueFalse( Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
        } break;

        case is_defined:
        {
          RequireToken(Scope, CTokenType_Question);
          /* auto S1 = GetTypeTypeForDatatype(ReplaceData, GetTranArena()); */
          /* auto S2 = GetTypeNameFor(Ctx, ReplaceData, GetTranArena()); */

          b32 DoTrueBranch = False;
          switch(ReplaceData->Type)
          {
            case type_datatype_noop:
            {
              // TODO(Jesse): When would this ever fire?  I thought this was an undefined case..
              DoTrueBranch = False;
              InternalCompilerError(Scope, CSz("Got datatype_noop for replace datatype?"), MetaOperatorToken);
            } break;

            case type_type_def:
            case type_declaration:
            case type_enum_member:
            case type_primitive_def:
            {
              datatype *ResolvedT = ResolveToBaseType(Ctx, ReplaceData);
              DoTrueBranch = (ResolvedT->Type != type_datatype_noop);
            } break;
          }

          DoTrueFalse( Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
        } break;

        case is_primitive:
        {
          RequireToken(Scope, CTokenType_Question);

          datatype *Dt = ResolveToBaseType(Ctx, ReplaceData);
          b32 DoTrueBranch = (Dt->Type == type_primitive_def);

          counted_string DTName = GetNameForDatatype(Dt, GetTranArena());

          // @counted_string_primitive_hack
          if (StringsMatch(DTName, CSz("counted_string")))
          {
            DoTrueBranch = True;
          }

          DoTrueFalse(Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
        } break;

        case is_compound:
        {
          RequireToken(Scope, CTokenType_Question);
          compound_decl *CD = DatatypeIsCompoundDecl(Ctx, ReplaceData, Scope, MetaOperatorToken);

          b32 DoTrueBranch = (CD != 0);

          // @counted_string_primitive_hack
          if (CD)
          {
            declaration TempDecl = Declaration(CD);
            counted_string DTName = GetNameForDecl(&TempDecl);
            if (StringsMatch(DTName, CSz("counted_string")))
            {
              DoTrueBranch = False;
            }
          }

          DoTrueFalse(Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
        } break;

        case is_struct:
        case is_union:
        {
          RequireToken(Scope, CTokenType_Question);
          compound_decl *CD = DatatypeIsCompoundDecl(Ctx, ReplaceData, Scope, MetaOperatorToken);

          b32 DoTrueBranch = False;
          if (CD)
          {
            b32 Negate = (Operator == is_struct);
            DoTrueBranch = (CD->IsUnion ^ Negate);

            // @counted_string_primitive_hack
            declaration TempDecl = Declaration(CD);
            counted_string DTName = GetNameForDecl(&TempDecl);
            if (StringsMatch(DTName, CSz("counted_string")))
            {
              DoTrueBranch = False;
            }

          }

          DoTrueFalse( Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
        } break;

        case is_enum:
        {
          RequireToken(Scope, CTokenType_Question);
          b32 DoTrueBranch = False;
          switch (ReplaceData->Type)
          {
            case type_datatype_noop:
            {
              InvalidCodePath();
            } break;

            case type_type_def:
            {
              NotImplemented;
            } break;

            case type_primitive_def:
            {
            } break;

            case type_enum_member:
            {
              DoTrueBranch = True;
            } break;

            case type_declaration:
            {
              declaration *Decl = SafeAccess(declaration, ReplaceData);
              switch (Decl->Type)
              {
                case type_declaration_noop:
                {
                  InvalidCodePath();
                } break;

                case type_compound_decl:
                case type_function_decl:
                {
                } break;

                case type_enum_decl:
                {
                  DoTrueBranch = True;
                } break;

                case type_variable_decl:
                {
                  variable_decl *VDecl = SafeAccess(variable_decl, Decl);
                  enum_decl E = {};
                  if (VDecl->Type.DatatypeToken)
                  {
                    E = GetEnumDeclByName(Ctx, VDecl->Type.DatatypeToken->Value);
                  }
                  DoTrueBranch = (E.Name.Start != 0);
                } break;
              }
            } break;
          }

          DoTrueFalse( Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
        } break;

        case is_array:
        {
          RequireToken(Scope, CTokenType_Question);
          b32 DoTrueBranch = DatatypeIsArray(Ctx, Scope, ReplaceData, MetaOperatorToken);
          DoTrueFalse( Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
        } break;

        case is_function:
        {
          RequireToken(Scope, CTokenType_Question);
          b32 IsFunc = DatatypeIsFunction(Ctx, Scope, ReplaceData, MetaOperatorToken);
          function_decl *D = DatatypeIsFunctionDecl(Ctx, ReplaceData, Scope, MetaOperatorToken);


#if 0
          if (D)
          {
            datatype Base = ResolveToBaseType(Ctx, ReplaceData);
            Info("(%S) (%S) (%S)",
                GetTypeTypeForDatatype(ReplaceData, Memory),
                GetNameForDatatype(ReplaceData, GetTranArena()),
                GetTypeNameFor(ReplaceData, Memory));
            Info("(%S) (%S) (%S)",
                GetTypeTypeForDatatype(&Base, Memory),
                GetNameForDatatype(&Base, GetTranArena()),
                GetTypeNameFor(&Base, Memory));
          }
#endif


          b32 DoTrueBranch = (IsFunc || D != 0);
          DoTrueFalse( Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
        } break;

        case is_named:
        case is_type:
        {
          RequireToken(Scope, CTokenType_OpenParen);
          cs QueryTypeName = RequireToken(Scope, CTokenType_Identifier).Value;
          RequireToken(Scope, CTokenType_CloseParen);
          RequireToken(Scope, CTokenType_Question);

          cs ThisName = {};

          switch (Operator)
          {
            case is_named:
            {
              ThisName = GetNameForDatatype(ReplaceData, Memory);
            } break;

            case is_type:
            {
              ThisName = GetTypeNameFor(Ctx, ReplaceData, TypedefResoultion_ResolveTypedefs, Memory);
            } break;

            InvalidDefaultCase;
          }

          b32 TypesMatch = StringsMatch(ThisName, QueryTypeName);
          DoTrueFalse(Ctx, Scope, Args, TypesMatch, OutputBuilder, Memory, Depth);
        } break;

        case contains_type:
        {
          RequireToken(Scope, CTokenType_OpenParen);
          cs TypeName = RequireToken(Scope, CTokenType_Identifier).Value;
          RequireToken(Scope, CTokenType_CloseParen);
          RequireToken(Scope, CTokenType_Question);

          if (auto Decl = TryCast(declaration, ReplaceData))
          {
            switch (Decl->Type)
            {
              case type_declaration_noop:
              {
                InvalidCodePath();
              } break;

              case type_enum_decl:
              case type_function_decl:
              {
              } break;

              case type_variable_decl:
              case type_compound_decl:
              {
                b32 Contains = False;
                declaration_stream* Members = 0;

                if (auto Var = TryCast(variable_decl, Decl))
                {
                  // We could be asking about a primitive type
                  if (Var->Type.DatatypeToken)
                  {
                    datatype *DT = GetDatatypeByName(&Ctx->Datatypes, Var->Type.DatatypeToken->Value);
                    Members = GetMembersFor(Ctx, DT);

                  }
                }
                else
                {
                  // Compound decls must have members .. right?
                  Members = GetMembersFor(Ctx, Decl);
                  Assert(Members);
                }

                if (Members)
                {
                  ITERATE_OVER_AS(Member, Members)
                  {
                    declaration* MemberDecl = GET_ELEMENT(MemberIter);
                    counted_string MemberName = GetTypeNameFor(Ctx, MemberDecl, Memory);
                    if (StringsMatch(MemberName, TypeName))
                    {
                      Contains = True;
                    }
                  }
                }

                DoTrueFalse(Ctx, Scope, Args, Contains, OutputBuilder, Memory, Depth);
              } break;
            }
          }

        } break;

        case array:
        {
          PrintAstNode(DatatypeStaticBufferSize(Ctx, Scope, ReplaceData, MetaOperatorToken), OutputBuilder);
          /* Append(OutputBuilder, CS(ArraySize)); */
        } break;

        case value:
        {
          cs Value = GetValueForDatatype(ReplaceData, Memory);
          if (OptionalToken(Scope, CTokenType_Question))
          {
            b32 DoTrueBranch = Value.Count > 0;
            DoTrueFalse( Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
          }
          else
          {
            Append(OutputBuilder, Value);
          }
        } break;

        case type:
        {
          counted_string TypeName = GetTypeNameFor(Ctx, ReplaceData, TypedefResoultion_ResolveTypedefs, Memory);
          Append(OutputBuilder, TypeName);
        } break;

        case name:
        {
          counted_string Name = GetNameForDatatype(ReplaceData, Memory);
          if (OptionalToken(Scope, CTokenType_Question))
          {
            b32 DoTrueBranch = StringsMatch(Name, CSz("(anonymous)")) == False;
            DoTrueFalse( Ctx, Scope, Args, DoTrueBranch, OutputBuilder, Memory, Depth);
          }
          else
          {
            Append(OutputBuilder, Name);
          }

        } break;

        case hash:
        {
          u64 HashValue = Hash(ReplaceData);
          Info("(%S) %x", GetNameForDatatype(ReplaceData, GetTranArena()), HashValue);
          Append(OutputBuilder, ToHex(HashValue));
        } break;

        case member:
        {
          declaration_stream *Members = GetMembersFor(Ctx, ReplaceData);
          if (Members)
          {
            RequireToken(Scope, CTokenType_OpenParen);
            u64 MemberIndex = u64_MAX;
            declaration_stream_chunk *TargetMember = 0;

            c_token *NextT = PeekTokenPointer(Scope);
            if (NextT)
            {
              if (NextT->Type == CTokenType_IntLiteral)
              {
                MemberIndex = RequireToken(Scope, CTokenType_IntLiteral).UnsignedValue;

                u32 AtIndex = 0;
                declaration_stream_chunk *IndexedMember = Members->FirstChunk;
                while ( IndexedMember && AtIndex < MemberIndex )
                {
                  IndexedMember = IndexedMember->Next;
                  AtIndex ++;
                }
                TargetMember = IndexedMember;

                if (TargetMember)
                {
                }
                else
                {
                  PoofTypeError( Scope,
                                 ParseErrorCode_InvalidArgument,
                                 FormatCountedString( GetTranArena(),
                                                      CSz("Attempted to access member index (%u) on (%S), which has (%d) members!"),
                                                      MemberIndex,
                                                      GetNameForDatatype(ReplaceData, GetTranArena()),
                                                      Members->ChunkCount
                                                      ),
                                 MetaOperatorToken);
                }
              }
              else if (NextT->Type == CTokenType_Identifier)
              {
                cs MemberName = RequireToken(Scope, CTokenType_Identifier).Value;
                declaration_stream_chunk *IndexedMember = Members->FirstChunk;
                while ( IndexedMember )
                {
                  // TODO(Jesse)(memory_leak): begin/end temporary memory here!
                  if (AreEqual(MemberName, GetNameForDecl(&IndexedMember->Element)))
                  {
                    TargetMember = IndexedMember;
                    break;
                  }

                  IndexedMember = IndexedMember->Next;
                }

                if (!TargetMember)
                {
                  PoofTypeError( Scope,
                                 ParseErrorCode_InvalidArgument,
                                 FormatCountedString( GetTranArena(),
                                                      CSz("Attempted to access member (%S) on (%S), which does not have that member!"),
                                                      MemberName,
                                                      GetNameForDatatype(ReplaceData, GetTranArena())),
                                 MetaOperatorToken);
                }
              }
              else
              {
                PoofSyntaxError( Scope,
                                 FormatCountedString(GetTranArena(), CSz("Wanted CTokenType_Identifier or CTokenType_IntLiteral, got (%S)"), ToString(NextT->Type)),
                                 NextT);
              }
            }
            else
            {
              // NOTE(Jesse): Stream ended unexpectedly
              NotImplemented;
            }

            RequireToken(Scope, CTokenType_Comma);

            RequireToken(Scope, CTokenType_OpenParen);
            counted_string MatchPattern  = RequireToken(Scope, CTokenType_Identifier).Value;
            RequireToken(Scope, CTokenType_CloseParen);

            auto MemberScope = GetBodyTextForNextScope(Scope, Memory);
            RequireToken(Scope, CTokenType_CloseParen);

            if (TargetMember)
            {
              /* meta_func_arg_stream NewArgs = CopyStream(Args, Memory); */
              /* Push(&NewArgs, ReplacementPattern(MatchPattern, Datatype(&TargetMember->Element))); */

              meta_func_arg_buffer NewArgs = ExtendBuffer(Args, 1, Memory);
              SetLast(&NewArgs, ReplacementPattern(MatchPattern, Datatype(&TargetMember->Element)));

              counted_string StructFieldOutput = Execute(&MemberScope, &NewArgs, Ctx, Memory, Depth);
              if (MemberScope.ErrorCode)
              {
                Scope->ErrorCode = MemberScope.ErrorCode;
              }
              else
              {
                HandleWhitespaceAndAppend(OutputBuilder, StructFieldOutput);
              }
            }
          }
          else
          {
            PoofTypeError( Scope,
                           ParseErrorCode_InvalidArgument,
                           FormatCountedString( GetTranArena(),
                                                CSz("Attempted to access member on (%S), which didn't have any members!"),
                                                GetNameForDatatype(ReplaceData, GetTranArena())),
                           MetaOperatorToken);
          }

        } break;

        case map:
        case map_array:
        case map_values:
        case map_members:
        case map_args:
        {
          RequireToken(Scope, CTokenType_OpenParen);
          c_token *MatchPattern  = RequireTokenPointer(Scope, CTokenType_Identifier);
          RequireToken(Scope, CTokenType_CloseParen);

          cs Sep = MaybeParseSepOperator(Scope);

          parser MapScope = GetBodyTextForNextScope(Scope, Memory);
          Map(Ctx, Operator, MetaOperatorToken, Scope, &MapScope, ReplaceData, &MatchPattern->Value, &Sep, Args, OutputBuilder, Memory, Depth);
        } break;

      }
    } break;

  }
}

// TODO(Jesse): Instead of the error living on the parser, it should live in the result from this fn
link_internal counted_string
Execute(meta_func *Func, meta_func_arg_buffer *Args, parse_context *Ctx, memory_arena *Memory, umm *Depth)
{
  TIMED_FUNCTION();
  Assert(Func->Body.Tokens->At == Func->Body.Tokens->Start);

  /* counted_string Result = Execute(&Func->Body, Args, Ctx, Memory, Depth); */
  parser *Scope = &Func->Body;

  program_datatypes* Datatypes = &Ctx->Datatypes;
  meta_func_stream* FunctionDefs = &Ctx->MetaFunctions;

  Assert(Scope->Tokens->At == Scope->Tokens->Start);

  string_builder OutputBuilder = {};
  while ( c_token *BodyToken = PopTokenRawPointer(Scope) )
  {
    b32 ImpetusWasAt = False;
    switch (BodyToken->Type)
    {
      case CTokenType_CharLiteral:
      case CTokenType_StringLiteral:
      {
        cs TempStr = StripQuotes(BodyToken->Value);

        parser *StringParse = ParserForAnsiStream(Ctx, AnsiStream(TempStr), TokenCursorSource_MetaprogrammingExpansion, Ctx->Memory);

        umm IgnoreDepth = 0;

        b32 DoNotNormalizeWhitespace = Ctx->Args.DoNotNormalizeWhitespace;

        Ctx->Args.DoNotNormalizeWhitespace = True;
        counted_string Code = Execute(StringParse, Args, Ctx, Memory, &IgnoreDepth);
        Ctx->Args.DoNotNormalizeWhitespace = DoNotNormalizeWhitespace;

        if (StringParse->ErrorCode)
        {
          Scope->ErrorCode = StringParse->ErrorCode;
        }
        else
        {
          const char *Bookend = BodyToken->Type == CTokenType_CharLiteral ? "'" : "\"";
          Append(&OutputBuilder, CS(Bookend, 1));
          Append(&OutputBuilder, EscapeDoubleQuotes(Code, OutputBuilder.Memory));
          Append(&OutputBuilder, CS(Bookend, 1));
        }
      } break;

      case CTokenType_At:
      {
        ImpetusWasAt = True;
        BodyToken = RequireTokenRawPointer(Scope, CTokenType_Identifier);
      } [[fallthrough]];

      case CTokenType_Identifier:
      case CTokenType_OpenParen:
      {
        b32 ImpetusWasIdentifier = BodyToken->Type == CTokenType_Identifier;
        b32 ImpetusWasOpenParen  = BodyToken->Type == CTokenType_OpenParen;

        b32 DidPoofOp = False;

        poof_global_keyword Keyword = PoofGlobalKeyword(BodyToken->Value);
        switch (Keyword)
        {
          case poof_global_keyword_noop:
          {
          } break;

          case var:
          {
            if (ImpetusWasAt)
            {
              cs NewName = RequireToken(Scope, CTokenType_Identifier).Value;

              parser ExpansionContext = EatUntilExcluding_Parser(Scope, CTokenType_Newline, GetTranArena());

              meta_func ExpansionFunc = MetaFunc(CSz(""), *Args, ExpansionContext, True, True);
              cs Expanded = Execute(&ExpansionFunc, Args, Ctx, Memory, Depth);

              meta_func_arg_buffer NewArgs = ExtendBuffer(Args, 1, Memory);
              datatype *D = ResolveNameToDatatype(Ctx, Scope, ExpansionFunc.Body.Tokens->Start, Args, Expanded);
              if (D->Type)
              {
                SetLast(&NewArgs, ReplacementPattern(NewName, *D));
              }
              else
              {
                SetLast(&NewArgs, ReplacementPattern(NewName, PoofSymbol(Expanded)));
              }

              *Args = NewArgs;

              DidPoofOp = True;
            }
            else
            {
              // TODO(Jesse): Should this just be a warning?  Cause `var` could
              // presumably occur in regular code ..
              PoofTypeError( Scope,
                             ParseErrorCode_SyntaxError,
                             FormatCountedString( GetTranArena(),
                                                  CSz("@var keyword missing preceeding @ symbol"),
                                                  BodyToken->Value),
                             BodyToken);
            }
          } break;

          case poof_error:
          {
            parser Body = GetBodyTextForNextScope(Scope, Memory);
            cs ErrorText = Execute(&Body, Args, Ctx, Memory, Depth);
            if (ErrorText.Count == 0) { ErrorText = ToString(&Body, Memory); }
            PoofUserlandError(Scope, ErrorText, BodyToken);
          } break;

          case are_equal:
          {
            RequireToken(Scope, CTokenType_OpenParen);
              /* datatype *D0 = ResolveExpression(Scope); */
              c_token *T0 = RequireTokenPointer(Scope, CTokenType_Identifier);
                            RequireTokenPointer(Scope, CTokenType_Comma);
              /* datatype *D1 = ResolveExpression(Scope); */
              c_token *T1 = RequireTokenPointer(Scope, CTokenType_Identifier);
            RequireToken(Scope, CTokenType_CloseParen);
            RequireToken(Scope, CTokenType_Question);

            Assert(T0 && T1);

            datatype *D0 = ResolveNameToDatatype(Ctx, Scope, T0, Args, T0->Value);
            datatype *D1 = ResolveNameToDatatype(Ctx, Scope, T1, Args, T1->Value);

            b32 DoTrueBranch = AreEqual(D0, D1);
            DoTrueFalse( Ctx, Scope, Args, DoTrueBranch, &OutputBuilder, Memory, Depth);
            DidPoofOp = True;
          } break;
        }

        for (u32 ArgIndex = 0; ArgIndex < Args->Count; ++ArgIndex)
        {
          if (DidPoofOp) break;

          meta_func_arg *Replace = Args->Start + ArgIndex;

          // NOTE(Jesse): Have to parse top-level operations here and check if
          // we're trying to throw an error.

          c_token *NextMatch = OptionalTokenRaw(Scope, CToken(Replace->Match));

          if ( (ImpetusWasIdentifier && StringsMatch(Replace->Match, BodyToken->Value)) ||
               (ImpetusWasOpenParen  && NextMatch)
             )
          {
            if (NextMatch) { BodyToken = NextMatch; }

            DidPoofOp = True;

            meta_arg_operator Operator = {};
            c_token *NextToken = {};
            if (OptionalTokenRaw(Scope, CTokenType_Dot))
            {
              NextToken = RequireTokenPointer(Scope, CTokenType_Identifier);
              Operator = MetaArgOperator(NextToken->Value);
            }

            string_builder OperatorBuilder = {};
            ResolveMetaOperator(Ctx, Args, Replace, Scope, Operator, BodyToken, NextToken, Memory, Depth, &OperatorBuilder);
            cs OperatorOutput = Finalize(&OperatorBuilder, Memory);

            //
            // Apply textual transformations
            //
            if (NextToken && Operator == meta_arg_operator_noop)
            {
              cs Transformed = CopyString(OperatorOutput, Memory);

              b32 Done = False;
              while (Done == False)
              {
                meta_transform_op NextOp = MetaTransformOp(NextToken->Value);
                if (NextOp)
                {
                  Transformed = ApplyTransformations(NextOp, Transformed, Memory);
                }
                else
                {
                  PoofTypeError( Scope,
                                 ParseErrorCode_InvalidMetaTransformOp,
                                 FormatCountedString( GetTranArena(),
                                                      CSz("Invalid transform op encountered (%S)"),
                                                      NextToken->Value),
                                 NextToken);
                }

                if (OptionalTokenRaw(Scope, CTokenType_Dot))
                {
                  NextToken = RequireTokenRawPointer(Scope, CTokenType_Identifier);
                }
                else
                {
                  Done = True;
                }
              }

              Append(&OutputBuilder, Transformed);
            }
            else
            {
              Append(&OutputBuilder, OperatorOutput);
            }


          }
        }

        meta_func *NestedFunc = {};
        c_token *NestedFuncT = {};

        if (ImpetusWasOpenParen)
        {
          NestedFunc = StreamContains( FunctionDefs, PeekToken(Scope).Value );
          if (NestedFunc) { NestedFuncT = RequireTokenPointer(Scope, CTokenType_Identifier); }
        }
        else
        {
          Assert(ImpetusWasIdentifier);
          NestedFunc = StreamContains( FunctionDefs, BodyToken->Value );
          NestedFuncT = BodyToken;
        }

        if (NestedFunc)
        {
          if (NestedFunc != Func)
          {
            Assert(NestedFuncT);
            DidPoofOp = True;

            meta_func_arg_buffer ArgInstances = {};
            if (ParseAndTypeCheckArgs(Ctx, Scope, NestedFuncT, NestedFunc, &ArgInstances, Args, Memory))
            {
              auto Code = CallFunction(Ctx, NestedFuncT, NestedFunc, &ArgInstances, Memory, Depth);
              if (Code.Start)
              {
                HandleWhitespaceAndAppend(&OutputBuilder, Code);
              }
            }
          }
          else
          {
            PoofTypeError( Scope,
                           ParseErrorCode_InvalidFunction,
                           FormatCountedString( GetTranArena(),
                                                CSz("Could not call poof func (%S); recursive functions are illegal."),
                                                NestedFuncT->Value),
                           NestedFuncT);
          }
        }

        if (DidPoofOp)
        {
          if (ImpetusWasOpenParen)
          {
            RequireToken(Scope, CTokenType_CloseParen);
          }
        }
        else
        {
          // NOTE(Jesse): Have to call this here to track depth properly
          c_token ToPush = {};
          if (Ctx->Args.DoNotNormalizeWhitespace) { ToPush = *BodyToken; }
          else                                    { ToPush = NormalizeWhitespaceTokens(BodyToken, PeekTokenRawPointer(Scope, -2), PeekTokenRawPointer(Scope), Depth); }
          if (ImpetusWasAt)
          {
            Append(&OutputBuilder, CSz("@"));
          }
          Append(&OutputBuilder, ToPush.Value);
        }

      } break;

      case CTokenType_CommentSingleLine:
      {
        // Eat comments that start with three slashes
        if (StartsWith(BodyToken->Value, CSz("///")))
        {
          continue;
        }
      } [[fallthrough]];

      default:
      {
        c_token ToPush = {};
        if (Ctx->Args.DoNotNormalizeWhitespace) { ToPush = *BodyToken; }
        else                                    { ToPush = NormalizeWhitespaceTokens(BodyToken, PeekTokenRawPointer(Scope, -2), PeekTokenRawPointer(Scope), Depth); }
        Append(&OutputBuilder, ToPush.Value);
      } break;

    }

    continue;
  }

  /* TrimLeadingWhitespace(&OutputBuilder.Chunks.FirstChunk->Element); */
  /* TrimTrailingWhitespace(&OutputBuilder.Chunks.LastChunk->Element); */

  counted_string Result = Finalize(&OutputBuilder, Memory);

  Assert(Func->Body.Tokens->At == Func->Body.Tokens->End);
  Rewind(Func->Body.Tokens);

  return Result;
}

link_internal counted_string
Execute(meta_func* Func, parse_context* Ctx, memory_arena* Memory, umm *Depth)
{
  cs Result = Execute(Func, &Func->Args, Ctx, Memory, Depth);
  return Result;
}

link_internal void
ExecuteMetaprogrammingDirective(parse_context *Ctx, metaprogramming_directive Directive, c_token *DirectiveT, tuple_cs_cs_buffer_builder *Builder)
{
  program_datatypes *Datatypes   = &Ctx->Datatypes;
  meta_func_stream *FunctionDefs = &Ctx->MetaFunctions;
  memory_arena *Memory           = Ctx->Memory;
  parser *Parser                 = Ctx->CurrentParser;

  switch (Directive)
  {
    case meta_directive_noop:
    {
      meta_func *Func = StreamContains(FunctionDefs, DirectiveT->Value);
      if (Func)
      {
        meta_func_arg_buffer ArgInstances = {};
        if (ParseAndTypeCheckArgs(Ctx, Parser, DirectiveT, Func, &ArgInstances, {}, Memory))
        {
          umm Depth = 0;
          auto Code = CallFunction(Ctx, DirectiveT, Func, &ArgInstances, Memory, &Depth);
          if (Code.Start)
          {
            counted_string OutfileName = GenerateOutfileNameFor(Ctx, Func, &ArgInstances, Memory);
            counted_string ActualOutputFile = FlushOutputToDisk(Ctx, Code, OutfileName, FSz("%S:%u:0", DirectiveT->Filename, DirectiveT->LineNumber ), {} /*TodoInfo*/, Memory, False, Func->OmitInclude, Func->CodeFragment);

            auto FilenameAndCode = Tuple(ActualOutputFile, Code);
            Append(Builder, FilenameAndCode);
          }
          else
          {
            ParseInfoMessage( Parser,
                              FormatCountedString( GetTranArena(),
                                                   CSz("Function execution (%S) failed."),
                                                   DirectiveT->Value ),
                              DirectiveT );
          }
        }
      }
      else
      {
        PoofTypeError( Parser,
                       ParseErrorCode_InvalidName,
                       FormatCountedString( GetTranArena(),
                                            CSz("(%S) is not a poof keyword or function name."),
                                            DirectiveT->Value ),
                       DirectiveT );
      }
    } break;

    case polymorphic_func:
    {
      TIMED_NAMED_BLOCK(polymorphic_func);
      /* function_decl F = ParseFunctionDef(Parser, Memory); */
      /* DebugPrint(F); */
    } break;

    case func:
    {
      if (OptionalToken(Parser, CTokenType_OpenParen)) // Anonymous function
      {
        c_token *ArgTypeT = RequireTokenPointer(Parser, CTokenType_Identifier);

        counted_string ArgType = ArgTypeT->Value;
        counted_string ArgName = RequireToken(Parser, CTokenType_Identifier).Value;

        RequireToken(Parser, CTokenType_CloseParen);

        meta_func Func = MetaFunc(CSz("anonymous"), {});
        ParseMetaFuncTags(Parser, &Func);

        Func.Body = GetBodyTextForNextScope(Parser, Memory);
        /* meta_func Func = ParseMetaFunctionDef(Parser, CSz("anonymous"), Memory); */

        datatype *ArgDatatype = GetDatatypeByName(&Ctx->Datatypes, ArgType);

        if (ArgDatatype->Type)
        {
          auto Args = MetaFuncArgBuffer(1, Memory);
          Args.Start[0] = ReplacementPattern(ArgName, *ArgDatatype);
          umm Depth = 0;
          counted_string Code = Execute(&Func, &Args, Ctx, Memory, &Depth);
          RequireToken(Parser, CTokenType_CloseParen);

          if (Func.Body.ErrorCode)
          {
            Parser->ErrorCode = Func.Body.ErrorCode;
            ParseInfoMessage( Parser,
                              FormatCountedString(GetTranArena(),
                                                  CSz("Unable to generate code for (func %S)"), Func.Name),
                              DirectiveT);
          }
          else
          {
            counted_string OutfileName = GenerateOutfileNameFor( Func.Name, ArgType, Memory, GetRandomString(8, umm(Hash(&Code)), Memory));
            counted_string ActualOutputFile = FlushOutputToDisk(Ctx, Code, OutfileName, FSz("%S:%u:0", DirectiveT->Filename, DirectiveT->LineNumber ), {} /* todoinfo */, Memory, True, Func.OmitInclude, Func.CodeFragment);
            Append(Builder, Tuple(ActualOutputFile, Code));
          }
        }
        else
        {
          PoofTypeError( Parser,
                         ParseErrorCode_UndefinedDatatype,
                         FormatCountedString(GetTranArena(), CSz("Unable to find definition for datatype (%S)"), ArgTypeT->Value),
                         ArgTypeT);
        }
      }
      else
      {
        counted_string FuncName = RequireToken(Parser, CTokenType_Identifier).Value;
        meta_func Func = ParseMetaFunctionDef(Parser, FuncName, Memory);
        Push(FunctionDefs, Func);
      }

    } break;

    case named_list:
    {
      TIMED_NAMED_BLOCK(named_list);
      RequireToken(Parser, CTokenType_OpenParen);

      tagged_counted_string_stream NameList = {};
      NameList.Tag = RequireToken(Parser, CTokenType_Identifier).Value;

      RequireToken(Parser, CTokenType_CloseParen);

      RequireToken(Parser, CTokenType_OpenBrace);
      while (PeekToken(Parser).Type == CTokenType_Identifier)
      {
        counted_string Name = RequireToken(Parser, CTokenType_Identifier).Value;
        Push(&NameList.Stream, Name);
        OptionalToken(Parser, CTokenType_Comma);
      }

      RequireToken(Parser, CTokenType_CloseBrace);

      Push(&Ctx->NamedLists, NameList);

    } break;

    case for_datatypes:
    {
      TIMED_NAMED_BLOCK(for_datatypes);
      RequireToken(Parser, CTokenType_OpenParen);
      RequireToken(Parser, CToken(CSz("all")));
      RequireToken(Parser, CTokenType_CloseParen);

      counted_string_stream Excludes = {};
      if (OptionalToken(Parser, CTokenType_Dot))
      {
        RequireToken(Parser, CToken(CSz("exclude")));
        RequireToken(Parser, CTokenType_OpenParen);
        Excludes = ParseDatatypeList(Ctx, Parser, Datatypes, &Ctx->NamedLists, Memory);
        RequireToken(Parser, CTokenType_CloseParen);
      }


      // ParseDatatypeList can fail if you pass in an undefined dataype
      // in the excludes constraint
      if (Parser->ErrorCode == ParseErrorCode_None)
      {
        // NOTE(Jesse): This is just here to parse the func tags out.
        meta_func ForAllDummyFunc = {};
        ParseMetaFuncTags(Parser, &ForAllDummyFunc);

        RequireToken(Parser, CToken(ToString(func)));
        meta_func StructFunc = ParseMetaFunctionDef(Parser, CSz("for_datatypes_struct_callback"), Memory);

        RequireToken(Parser, CToken(ToString(func)));
        meta_func EnumFunc = ParseMetaFunctionDef(Parser, CSz("for_datatypes_enum_callback"), Memory);

        RequireToken(Parser, CTokenType_CloseParen);

        string_builder OutputBuilder = {};

        {
          IterateOver(&Datatypes->DatatypeHashtable, DT, DatatypeIndex)
          {
            if (DT)
            {
              if (compound_decl *Struct = TryCastToCompoundDecl(Ctx, DT))
              {
                if (Struct->Type && Struct->IsUnion == False)
                {
                  if (!StreamContains(&Excludes, Struct->Type->Value))
                  {
                    Assert(StructFunc.Args.Count == 1);
                    StructFunc.Args.Start[0] = ReplacementPattern(StructFunc.Args.Start[0].Match, *DT);

                    umm Depth = 0;
                    counted_string Code = Execute(&StructFunc, Ctx, Memory, &Depth);
                    Append(&OutputBuilder, Code);
                  }
                }
              }
            }
          }
        }

        {
          IterateOver(&Datatypes->DatatypeHashtable, DT, DatatypeIndex)
          {
            if (DT)
            {
              if (enum_decl *Enum = TryCastToEnumDecl(Ctx, DT))
              {
                if (!StreamContains(&Excludes, Enum->Name))
                {
                  Assert(EnumFunc.Args.Count == 1);
                  EnumFunc.Args.Start[0] = ReplacementPattern(EnumFunc.Args.Start[0].Match, *DT);
                  umm Depth = 0;
                  counted_string Code = Execute(&EnumFunc, Ctx, Memory, &Depth);
                  Append(&OutputBuilder, Code);
                }
              }
            }
          }
        }

        cs Code = Finalize(&OutputBuilder, Memory);
        cs OutfileName = GenerateOutfileNameFor(ToString(Directive), GetRandomString(8, umm(Hash(&Code)), Memory), Memory);
        cs ActualOutputFile = FlushOutputToDisk(Ctx, Code, OutfileName, FSz("%S:%u:0", DirectiveT->Filename, DirectiveT->LineNumber ), {} /* Todoinfo */, Memory, False, ForAllDummyFunc.OmitInclude, ForAllDummyFunc.CodeFragment );
        Append(Builder, Tuple(ActualOutputFile, Code));
      }
    } break;

    case d_union:
    {
      TIMED_NAMED_BLOCK(d_union);
      c_token *DatatypeT = RequireTokenPointer(Parser, CTokenType_Identifier);
      d_union_decl dUnion = ParseDiscriminatedUnion(Ctx, Parser, Datatypes, DatatypeT, Memory);
      if (Parser->ErrorCode == ParseErrorCode_None)
      {
        string_builder CodeBuilder = {};
        if (!dUnion.CustomEnumType.Count)
        {
          counted_string EnumString = GenerateEnumDef(&dUnion, Memory);
          Append(&CodeBuilder, EnumString);
        }

        {
          counted_string StructString = GenerateStructDef(Ctx, &dUnion, Memory);
          Append(&CodeBuilder, StructString);
        }

        counted_string Code = Finalize(&CodeBuilder, Memory);

        RequireToken(Parser, CTokenType_CloseParen);
        while(OptionalToken(Parser, CTokenType_Semicolon));

        counted_string OutfileName = GenerateOutfileNameFor(ToString(Directive), DatatypeT->Value, Memory);
        counted_string ActualOutputFile = FlushOutputToDisk(Ctx, Code, OutfileName, FSz("%S:%u:0", DirectiveT->Filename, DirectiveT->LineNumber ), {} /* todoinfo */, Memory);
        Append(Builder, Tuple(ActualOutputFile, Code));
      }
      else
      {
        ParseInfoMessage(Parser, CSz("While parsing d_union"), DatatypeT);
      }

    } break;

    case enum_only:
    {
      InvalidCodePath();
    }
  }
}
