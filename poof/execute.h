
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
    if ( NextT && (NextT->Type == '}' || NextT->Type == ')'))
    {
      Result.Value.Count = Min(T->Value.Count, (SafeDecrement(Depth))*2);
    }
    else
    {
      Result.Value.Count = Min(T->Value.Count, (*Depth)*2);
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
        LastStr->Count = 0;
      }

      if (LastNBSPChar(&Output) == '\n')
      {
        TrimTrailingNBSP(&Output);
        if (TrimOutputTrailingNewline)
        {
          TrimTrailingNewline(&Output);
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

// TODO(Jesse)(abstract): How would we articulate this if we wanted to have
// multiple different maybe boxes for counted string?  I guess we'd have to
// mangle the name to include both types.. but that seems like it sucks a lot..
poof( maybe(counted_string, parse_error_code) )
#include <generated/maybe_counted_string_parse_error_code.h>

link_internal maybe_counted_string
MapEnumValues(parse_context *Ctx, enum_decl *Enum, meta_func *EnumFunc, cs *EnumValueMatch, cs *Sep, memory_arena *Memory, umm *Depth)
{
  maybe_counted_string Result = {};
  string_builder OutputBuilder = {};

  meta_func_arg_buffer NewArgs = ExtendBuffer(&EnumFunc->Args, 1, Memory);
  ITERATE_OVER(&Enum->Members)
  {
    Rewind(EnumFunc->Body.Tokens);

    enum_member* EnumMember = GET_ELEMENT(Iter);
    SetLast(&NewArgs, ReplacementPattern(*EnumValueMatch, Datatype(EnumMember)));

    counted_string EnumFieldOutput = Execute(EnumFunc, &NewArgs, Ctx, Memory, Depth);
    if (EnumFunc->Body.ErrorCode)
    {
      Result.Error = EnumFunc->Body.ErrorCode;
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

// TODO(Jesse id: 222, tags: immediate, parsing, metaprogramming) : Re-add [[nodiscard]] here
link_internal counted_string
Execute(parser *Scope, meta_func_arg_buffer *ReplacePatterns, parse_context *Ctx, memory_arena *Memory, umm *Depth)
{
  TIMED_FUNCTION();

  program_datatypes* Datatypes = &Ctx->Datatypes;
  meta_func_stream* FunctionDefs = &Ctx->MetaFunctions;

  Assert(Scope->Tokens->At == Scope->Tokens->Start);

  string_builder OutputBuilder = {};
  while ( c_token *BodyToken = PopTokenRawPointer(Scope) )
  {
    switch (BodyToken->Type)
    {
      case CTokenType_StringLiteral:
      {
        counted_string TempStr = StripQuotes(BodyToken->Value);
        parser *StringParse = ParserForAnsiStream(Ctx, AnsiStream(TempStr), TokenCursorSource_MetaprogrammingExpansion);

        umm IgnoreDepth = 0;
        counted_string Code = Execute(StringParse, ReplacePatterns, Ctx, Memory, &IgnoreDepth);

        if (StringParse->ErrorCode)
        {
          Scope->ErrorCode = StringParse->ErrorCode;
        }
        else
        {
          Append(&OutputBuilder, CSz("\""));
          Append(&OutputBuilder, EscapeDoubleQuotes(Code, OutputBuilder.Memory));
          Append(&OutputBuilder, CSz("\""));
        }
      } break;

      case CTokenType_Identifier:
      case CTokenType_OpenParen:
      {
        b32 ImpetusWasIdentifier = BodyToken->Type == CTokenType_Identifier;
        b32 ImpetusWasOpenParen  = BodyToken->Type == CTokenType_OpenParen;

        b32 DidPoofOp = False;
        for (u32 ArgIndex = 0; ArgIndex < ReplacePatterns->Count; ++ArgIndex)
        {
          meta_func_arg *Replace = ReplacePatterns->Start + ArgIndex;
          if ( (ImpetusWasIdentifier && StringsMatch(Replace->Match, BodyToken->Value)) ||
               (ImpetusWasOpenParen  && OptionalTokenRaw(Scope, CToken(Replace->Match)))
             )
          {
            DidPoofOp = True;
            switch (Replace->Type)
            {
              case type_meta_func_arg_noop:
              {
                InvalidCodePath();
              } break;

              case type_poof_index:
              {
                auto RepIndex = SafeCast(poof_index, Replace);
                counted_string S = FormatCountedString(Memory, CSz("%u"), RepIndex->Index);
                Append(&OutputBuilder, S);
              } break;

              case type_poof_symbol:
              {
                auto Symbol = SafeCast(poof_symbol, Replace);
                Append(&OutputBuilder, Symbol->Value);
              } break;

              case type_datatype:
              {
                datatype *ReplaceData = SafeCast(datatype, Replace);
                Assert(ReplaceData->Type);

                RequireToken(Scope, CTokenType_Dot);

                c_token *MetaOperatorToken = RequireTokenPointer(Scope, CTokenType_Identifier);

                meta_arg_operator Operator = MetaArgOperator( MetaOperatorToken->Value );
                switch (Operator)
                {
                  case meta_arg_operator_noop:
                  {
                    PoofTypeError( Scope,
                                   ParseErrorCode_InvalidOperator,
                                   FormatCountedString(TranArena, CSz("(%S) is not a valid poof operator"), MetaOperatorToken->Value),
                                   MetaOperatorToken);
                  } break;

                  case sep:
                  {
                    // TODO(Jesse): Emit error message here.
                    NotImplemented;
                  } break;

                  case is_defined:
                  {
                    RequireToken(Scope, CTokenType_Question);
                    /* auto S1 = GetTypeTypeForDatatype(ReplaceData, TranArena); */
                    /* auto S2 = GetTypeNameForDatatype(Ctx, ReplaceData, TranArena); */

                    b32 DoTrueBranch = False;
                    switch(ReplaceData->Type)
                    {
                      case type_datatype_noop:
                      {
                        // TODO(Jesse): When would this ever fire?  I thought this
                        // was an undefined case..
                        DoTrueBranch = False;
                      } break;

                      case type_declaration:
                      case type_enum_member:
                      case type_primitive_def:
                      {
                        datatype ResolvedT = ResolveToBaseType(Ctx, ReplaceData);
                        DoTrueBranch = (ResolvedT.Type != type_datatype_noop);
                      } break;

                      case type_type_def:
                      {
                        InvalidCodePath();
                      } break;
                    }

                    DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory, Depth);
                  } break;

                  case is_primitive:
                  {
                    RequireToken(Scope, CTokenType_Question);

                    datatype Dt = ResolveToBaseType(Ctx, ReplaceData);
                    b32 DoTrueBranch = (Dt.Type == type_primitive_def);

                    counted_string DTName = GetNameForDatatype(&Dt, TranArena);

                    // @counted_string_primitive_hack
                    if (StringsMatch(DTName, CSz("counted_string")))
                    {
                      DoTrueBranch = True;
                    }

                    DoTrueFalse(Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory, Depth);
                  } break;

                  case is_compound:
                  {
                    RequireToken(Scope, CTokenType_Question);
                    compound_decl *CD = DatatypeIsCompoundDecl(Ctx, Scope, ReplaceData, MetaOperatorToken);

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

                    DoTrueFalse(Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory, Depth);
                  } break;

                  case is_struct:
                  case is_union:
                  {
                    RequireToken(Scope, CTokenType_Question);
                    compound_decl *CD = DatatypeIsCompoundDecl(Ctx, Scope, ReplaceData, MetaOperatorToken);

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

                    DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory, Depth);
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
                            enum_decl *E = {};
                            if (VDecl->Type.DatatypeToken)
                            {
                              E = GetEnumDeclByName(&Datatypes->Enums, VDecl->Type.DatatypeToken->Value);
                            }
                            DoTrueBranch = (E != 0);
                          } break;
                        }
                      } break;
                    }

                    DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory, Depth);
                  } break;

                  case is_array:
                  {
                    RequireToken(Scope, CTokenType_Question);
                    b32 DoTrueBranch = DatatypeIsArray(Ctx, Scope, ReplaceData, MetaOperatorToken);
                    DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory, Depth);
                  } break;

                  case is_function:
                  {
                    RequireToken(Scope, CTokenType_Question);
                    b32 IsFunc = DatatypeIsFunction(Ctx, Scope, ReplaceData, MetaOperatorToken);
                    function_decl *D = DatatypeIsFunctionDecl(Ctx, Scope, ReplaceData, MetaOperatorToken);


                    if (D)
                    {
                      datatype Base = ResolveToBaseType(Ctx, ReplaceData);

                      /* Info("(%S) (%S) (%S)", */
                      /*     GetTypeTypeForDatatype(ReplaceData, Memory), */
                      /*     GetNameForDatatype(ReplaceData, TranArena), */
                      /*     GetTypeNameForDatatype(ReplaceData, Memory)); */

                      /* Info("(%S) (%S) (%S)", */
                      /*     GetTypeTypeForDatatype(&Base, Memory), */
                      /*     GetNameForDatatype(&Base, TranArena), */
                      /*     GetTypeNameForDatatype(&Base, Memory)); */

                    }


                    b32 DoTrueBranch = (IsFunc || D != 0);
                    DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory, Depth);
                  } break;

                  case is_named:
                  case is_type:
                  {
                    RequireToken(Scope, CTokenType_OpenParen);
                    cs QueryTypeName = RequireToken(Scope, CTokenType_Identifier).Value;
                    RequireToken(Scope, CTokenType_CloseParen);
                    RequireToken(Scope, CTokenType_Question);

                    cs ThisTypeName = GetResolvedTypeNameForDatatype(Ctx, ReplaceData, Memory);

                    b32 TypesMatch = StringsMatch(ThisTypeName, QueryTypeName);
                    DoTrueFalse(Ctx, Scope, ReplacePatterns, TypesMatch, &OutputBuilder, Memory, Depth);
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
                              auto DT = GetDatatypeByName(Ctx, Var->Type.DatatypeToken->Value);
                              Members = GetMembersFor(&DT);

                            }
                          }
                          else
                          {
                            // Compound decls must have members .. right?
                            Members = GetMembersFor(Decl);
                            Assert(Members);
                          }

                          if (Members)
                          {
                            ITERATE_OVER_AS(Member, Members)
                            {
                              declaration* MemberDecl = GET_ELEMENT(MemberIter);
                              counted_string MemberName = GetTypeNameForDecl(Ctx, MemberDecl, Memory);
                              if (StringsMatch(MemberName, TypeName))
                              {
                                Contains = True;
                              }
                            }
                          }

                          DoTrueFalse(Ctx, Scope, ReplacePatterns, Contains, &OutputBuilder, Memory, Depth);
                        } break;
                      }
                    }

                  } break;

                  case array:
                  {
                    NotImplemented;
                  } break;

                  case value:
                  {
                    counted_string Value = GetValueForDatatype(ReplaceData, Memory);
                    Append(&OutputBuilder, Value);
                  } break;

                  case type:
                  {
                    counted_string TypeName = GetResolvedTypeNameForDatatype(Ctx, ReplaceData, Memory);
                    meta_transform_op Transformations = ParseTransformations(Scope);
                    if (Scope->ErrorCode == ParseErrorCode_None)
                    {
                      counted_string TransformedName = ApplyTransformations(Transformations, TypeName, Memory);
                      Append(&OutputBuilder, TransformedName);
                    }
                  } break;

                  case name:
                  {
                    counted_string Name = GetNameForDatatype(ReplaceData, Memory);
                    if (OptionalToken(Scope, CTokenType_Question))
                    {
                      b32 DoTrueBranch = StringsMatch(Name, CSz("(anonymous)")) == False;
                      DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory, Depth);
                    }
                    else
                    {
                      meta_transform_op Transformations = ParseTransformations(Scope);
                      if (Scope->ErrorCode == ParseErrorCode_None)
                      {
                        counted_string TransformedName = ApplyTransformations(Transformations, Name, Memory);
                        Append(&OutputBuilder, TransformedName);
                      }
                    }

                  } break;

                  case member:
                  {
                    declaration_stream *Members = GetMembersFor(ReplaceData);
                    if (Members)
                    {
                      RequireToken(Scope, CTokenType_OpenParen);
                      u64 MemberIndex = u64_MAX;
                      declaration_stream_chunk *TargetMember = 0;

                      if (PeekToken(Scope).Type == CTokenType_IntLiteral)
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
                                         FormatCountedString( TranArena,
                                                              CSz("Attempted to access member index (%u) on (%S), which has (%d) members!"),
                                                              MemberIndex,
                                                              GetNameForDatatype(ReplaceData, TranArena),
                                                              Members->ChunkCount
                                                              ),
                                         MetaOperatorToken);
                        }
                      }
                      else if (PeekToken(Scope).Type == CTokenType_Identifier)
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
                                         FormatCountedString( TranArena,
                                                              CSz("Attempted to access member (%S) on (%S), which does not have that member!"),
                                                              MemberName,
                                                              GetNameForDatatype(ReplaceData, TranArena)),
                                         MetaOperatorToken);
                        }
                      }

                      RequireToken(Scope, CTokenType_Comma);

                      RequireToken(Scope, CTokenType_OpenParen);
                      counted_string MatchPattern  = RequireToken(Scope, CTokenType_Identifier).Value;
                      RequireToken(Scope, CTokenType_CloseParen);

                      auto MemberScope = GetBodyTextForNextScope(Scope, Memory);
                      RequireToken(Scope, CTokenType_CloseParen);

                      if (TargetMember)
                      {
                        /* meta_func_arg_stream NewArgs = CopyStream(ReplacePatterns, Memory); */
                        /* Push(&NewArgs, ReplacementPattern(MatchPattern, Datatype(&TargetMember->Element))); */

                        meta_func_arg_buffer NewArgs = ExtendBuffer(ReplacePatterns, 1, Memory);
                        SetLast(&NewArgs, ReplacementPattern(MatchPattern, Datatype(&TargetMember->Element)));

                        counted_string StructFieldOutput = Execute(&MemberScope, &NewArgs, Ctx, Memory, Depth);
                        if (MemberScope.ErrorCode)
                        {
                          Scope->ErrorCode = MemberScope.ErrorCode;
                        }
                        else
                        {
                          HandleWhitespaceAndAppend(&OutputBuilder, StructFieldOutput);
                        }
                      }
                    }
                    else
                    {
                      PoofTypeError( Scope,
                                     ParseErrorCode_InvalidArgument,
                                     FormatCountedString( TranArena,
                                                          CSz("Attempted to access member on (%S), which didn't have any members!"),
                                                          GetNameForDatatype(ReplaceData, TranArena)),
                                     MetaOperatorToken);
                    }

                  } break;

                  case map:
                  case map_array:
                  case map_values:
                  case map_members:
                  {
                    RequireToken(Scope, CTokenType_OpenParen);
                    c_token *MatchPattern  = RequireTokenPointer(Scope, CTokenType_Identifier);
                    RequireToken(Scope, CTokenType_CloseParen);

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

                    parser MapScope = GetBodyTextForNextScope(Scope, Memory);
                    meta_func_arg_buffer NewArgs = ExtendBuffer(ReplacePatterns, 1, Memory);

                    switch (ReplaceData->Type)
                    {
                      case type_datatype_noop:
                      {
                        InternalCompilerError(Scope, CSz("Infinite sadness"), MetaOperatorToken);
                      } break;

                      case type_declaration:
                      {
                        declaration *Decl = SafeAccess(declaration, ReplaceData);

                        switch(Decl->Type)
                        {
                          case type_declaration_noop:
                          {
                            InternalCompilerError(Scope, CSz("Infinite sadness"), MetaOperatorToken);
                          } break;

                          case type_function_decl:
                          {
                            // TODO(Jesse): Does mapping over a function mean anything?
                            // TODO(Jesse)(error_messages): If it doesn't, emit a sensible error.
                            NotImplemented;
                          } break;

                          case type_enum_decl:
                          {
                            if (Operator == map || Operator == map_values)
                            {
                              auto EnumDecl = SafeAccess(enum_decl, Decl);
                              meta_func EnumFunc = MetaFunc(CSz("map_enum_values"), *ReplacePatterns, MapScope);

                              maybe_counted_string MapResult = MapEnumValues(Ctx, EnumDecl, &EnumFunc, &MatchPattern->Value, &Sep, Memory, Depth);
                              if (MapResult.Error)
                              {
                              }
                              else
                              {
                                HandleWhitespaceAndAppend(&OutputBuilder, MapResult.E);
                              }
                            }
                            else
                            {
                              PoofTypeError( Scope,
                                             ParseErrorCode_InvalidArgument,
                                             FormatCountedString( TranArena,
                                                                  CSz("Incorrectly called (%S) on an enum."),
                                                                  MetaOperatorToken->Value),
                                             MetaOperatorToken);
                            }
                          } break;

                          case type_compound_decl:
                          {
                            if (Operator == map || Operator == map_members)
                            {
                              declaration_stream *Members = GetMembersFor(ReplaceData);
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
                                      Rewind(MapScope.Tokens);

                                      SetLast(&NewArgs, ReplacementPattern(MatchPattern->Value, Datatype(Member)));

                                      counted_string StructFieldOutput = Execute(&MapScope, &NewArgs, Ctx, Memory, Depth);
                                      if (MapScope.ErrorCode)
                                      {
                                        Scope->ErrorCode = MapScope.ErrorCode;
                                      }
                                      else
                                      {
                                        // TODO(Jesse): Pretty sure there should be a better way of handling this now that
                                        // all the map paths have collapsed..
                                        if (MemberIter.At->Next)
                                        {
                                          HandleWhitespaceAndAppend(&OutputBuilder, StructFieldOutput, Sep);
                                        }
                                        else
                                        {
                                          HandleWhitespaceAndAppend(&OutputBuilder, StructFieldOutput, {}, True);
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
                                NotImplemented;
                              }
                            }
                            else
                            {
                              PoofTypeError( Scope,
                                             ParseErrorCode_InvalidArgument,
                                             FormatCountedString( TranArena,
                                                                  CSz("Incorrectly called (%S) on a compound decl."),
                                                                  MetaOperatorToken->Value),
                                             MetaOperatorToken);
                            }
                          } break;

                          case type_variable_decl:
                          {
                            if (Operator == map || Operator == map_array)
                            {
                              variable_decl *VDecl = SafeAccess(variable_decl, Decl);
                              ast_node *ArraySizeNode = VDecl->StaticBufferSize;

                              u64 ArrayLength = ResolveConstantExpression(Scope, ArraySizeNode);
                              for (u64 ArrayIndex = 0; ArrayIndex < ArrayLength; ++ArrayIndex)
                              {
                                SetLast(&NewArgs, ReplacementPattern(MatchPattern->Value, PoofIndex(SafeTruncateToU32(ArrayIndex), SafeTruncateToU32(ArrayLength))));

                                Rewind(MapScope.Tokens);
                                counted_string StructFieldOutput = Execute(&MapScope, &NewArgs, Ctx, Memory, Depth);
                                if (MapScope.ErrorCode)
                                {
                                  Scope->ErrorCode = MapScope.ErrorCode;
                                }
                                else
                                {
                                  // TODO(Jesse): Pretty sure there should be a better way of handling this now that
                                  // all the map paths have collapsed..
                                  if (ArrayIndex+1 < ArrayLength)
                                  {
                                    HandleWhitespaceAndAppend(&OutputBuilder, StructFieldOutput, Sep);
                                  }
                                  else
                                  {
                                    HandleWhitespaceAndAppend(&OutputBuilder, StructFieldOutput, {}, True);
                                  }
                                }
                              }
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
                        NotImplemented; // TODO(Jesse): Resolve to base type and try again?
                      } break;
                    }

                  } break;

                }
              } break;

            }

          }

          if (DidPoofOp) break;
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
          Assert(NestedFuncT);
          DidPoofOp = True;

          meta_func_arg_buffer ArgInstances = {};
          if (ParseAndTypeCheckArgs(Ctx, Scope, NestedFuncT, NestedFunc, &ArgInstances, ReplacePatterns, Memory))
          {
            auto Code = CallFunction(Ctx, NestedFuncT, NestedFunc, &ArgInstances, Memory, Depth);
            if (Code.Start)
            {
              HandleWhitespaceAndAppend(&OutputBuilder, Code);
            }
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
          c_token ToPush = NormalizeWhitespaceTokens(BodyToken, PeekTokenRawPointer(Scope, -2), PeekTokenRawPointer(Scope), Depth);
          Append(&OutputBuilder, ToPush.Value);
        }

      } break;

      default:
      {
        c_token ToPush = NormalizeWhitespaceTokens(BodyToken, PeekTokenRawPointer(Scope, -2), PeekTokenRawPointer(Scope), Depth);
        Append(&OutputBuilder, ToPush.Value);
      } break;

    }

    continue;
  }

  /* TrimLeadingWhitespace(&OutputBuilder.Chunks.FirstChunk->Element); */
  /* TrimTrailingWhitespace(&OutputBuilder.Chunks.LastChunk->Element); */

  counted_string Result = Finalize(&OutputBuilder, Memory);
  return Result;
}

link_internal void
DoTrueFalse( parse_context *Ctx,
             parser *Scope,
             meta_func_arg_buffer *ReplacePatterns,
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
    counted_string Code = Execute(ParserToUse, ReplacePatterns, Ctx, Memory, Depth);
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


link_internal counted_string
Execute(meta_func* Func, meta_func_arg_buffer *Args, parse_context* Ctx, memory_arena* Memory, umm *Depth)
{
  Assert(Func->Body.Tokens->At == Func->Body.Tokens->Start);

  counted_string Result = Execute(&Func->Body, Args, Ctx, Memory, Depth);

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
