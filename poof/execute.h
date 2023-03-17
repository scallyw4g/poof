
link_internal umm
SafeDecrement(umm *N)
{
  umm Result = *N;
  if (Result > 0)
  {
    Result -= 1;
  }
  return Result;
}

link_internal c_token
NormalizeWhitespaceTokens(c_token *T, c_token* PrevT, c_token *NextT, umm *Depth)
{
  c_token Result = *T;

  if (T->Type == '{')
  {
    *Depth += 1;
  }

  if (T->Type == '}')
  {
    *Depth = SafeDecrement(Depth);
  }

  if ( (T->Type == ' ' && PrevT && PrevT->Type == CTokenType_Newline) ||
       (T->Type == ' ' && PrevT == 0) )
  {
    Result.Value.Count = Min(T->Value.Count, (*Depth)*2);
  }

  if (IsNBSP(T))
  {
    if ( NextT && NextT->Type == '}')
    {
      Result.Value.Count = Min(T->Value.Count, (SafeDecrement(Depth))*2);
    }

    if ( PrevT && PrevT->Type == T->Type )
    {
      InvalidCodePath();
      /* Result.Value.Count = 0; */
    }
  }

  return Result;
}

// TODO(Jesse id: 222, tags: immediate, parsing, metaprogramming) : Re-add [[nodiscard]] here
link_internal counted_string
Execute(parser *Scope, meta_func_arg_stream* ReplacePatterns, parse_context* Ctx, memory_arena* Memory, umm *Depth)
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

      case CTokenType_OpenParen:
      case CTokenType_Identifier:
      {
        b32 ImpetusWasOpenParen = BodyToken->Type == CTokenType_OpenParen;
        b32 ImpetusWasIdentifier = BodyToken->Type == CTokenType_Identifier;

        b32 ExecutedChildFunc = False;
        ITERATE_OVER_AS(Replace, ReplacePatterns)
        {
          meta_func_arg* Replace = GET_ELEMENT(ReplaceIter);

          Assert(Replace->Data.Type);

          if ( (ImpetusWasIdentifier && StringsMatch(Replace->Match, BodyToken->Value) ) ||
               (ImpetusWasOpenParen  && OptionalTokenRaw(Scope, CToken(Replace->Match)))
             )
          {
            ExecutedChildFunc = True;
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

              case is_defined:
              {
                RequireToken(Scope, CTokenType_Question);
                /* auto S1 = GetTypeTypeForDatatype(&Replace->Data, TranArena); */
                /* auto S2 = GetTypeNameForDatatype(Ctx, &Replace->Data, TranArena); */

                b32 DoTrueBranch = False;
                switch(Replace->Data.Type)
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
                    datatype ResolvedT = ResolveToBaseType(Ctx, &Replace->Data);
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

                datatype Dt = ResolveToBaseType(Ctx, &Replace->Data);
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
                compound_decl *CD = DatatypeIsCompoundDecl(Ctx, Scope, &Replace->Data, MetaOperatorToken);

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
                compound_decl *CD = DatatypeIsCompoundDecl(Ctx, Scope, &Replace->Data, MetaOperatorToken);

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
                switch (Replace->Data.Type)
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
                    declaration *Decl = SafeAccess(declaration, &Replace->Data);
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
                          E = GetEnumByType(&Datatypes->Enums, VDecl->Type.DatatypeToken->Value);
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
                b32 DoTrueBranch = DatatypeIsArray(Ctx, Scope, &Replace->Data, MetaOperatorToken);
                DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory, Depth);
              } break;

              case is_function:
              {
                RequireToken(Scope, CTokenType_Question);
                b32 IsFunc = DatatypeIsFunction(Ctx, Scope, &Replace->Data, MetaOperatorToken);
                function_decl *D = DatatypeIsFunctionDecl(Ctx, Scope, &Replace->Data, MetaOperatorToken);


                if (D)
                {
                  datatype Base = ResolveToBaseType(Ctx, &Replace->Data);

                  /* Info("(%S) (%S) (%S)", */
                  /*     GetTypeTypeForDatatype(&Replace->Data, Memory), */
                  /*     GetNameForDatatype(&Replace->Data, TranArena), */
                  /*     GetTypeNameForDatatype(&Replace->Data, Memory)); */

                  /* Info("(%S) (%S) (%S)", */
                  /*     GetTypeTypeForDatatype(&Base, Memory), */
                  /*     GetNameForDatatype(&Base, TranArena), */
                  /*     GetTypeNameForDatatype(&Base, Memory)); */

                }


                b32 DoTrueBranch = (IsFunc || D != 0);
                DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory, Depth);
              } break;

              case is_type:
              {
                RequireToken(Scope, CTokenType_OpenParen);
                cs QueryTypeName = RequireToken(Scope, CTokenType_Identifier).Value;
                RequireToken(Scope, CTokenType_CloseParen);
                RequireToken(Scope, CTokenType_Question);

                cs ThisTypeName = GetTypeNameForDatatype(Ctx, &Replace->Data, Memory);

                b32 TypesMatch = StringsMatch(ThisTypeName, QueryTypeName);
                DoTrueFalse(Ctx, Scope, ReplacePatterns, TypesMatch, &OutputBuilder, Memory, Depth);
              } break;

              case is_named:
              {
                NotImplemented;
              } break;

              case contains_type:
              {
                RequireToken(Scope, CTokenType_OpenParen);
                cs TypeName = RequireToken(Scope, CTokenType_Identifier).Value;
                RequireToken(Scope, CTokenType_CloseParen);
                RequireToken(Scope, CTokenType_Question);

                if (auto Decl = TryCast(declaration, &Replace->Data))
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
#if 0
#endif


              } break;

              case array:
              {
                NotImplemented;
              } break;

              case value:
              {
                counted_string Value = GetValueForDatatype(&Replace->Data, Memory);
                Append(&OutputBuilder, Value);
              } break;

              case type:
              {
                counted_string TypeName = GetTypeNameForDatatype(Ctx, &Replace->Data, Memory);
                meta_transform_op Transformations = ParseTransformations(Scope);
                if (Scope->ErrorCode == ParseErrorCode_None)
                {
                  counted_string TransformedName = ApplyTransformations(Transformations, TypeName, Memory);
                  Append(&OutputBuilder, TransformedName);
                }
              } break;

              case name:
              {
                counted_string Name = GetNameForDatatype(&Replace->Data, Memory);
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
                RequireToken(Scope, CTokenType_OpenParen);
                u64 MemberIndex = RequireToken(Scope, CTokenType_IntLiteral).UnsignedValue;
                RequireToken(Scope, CTokenType_Comma);

                RequireToken(Scope, CTokenType_OpenParen);
                counted_string MatchPattern  = RequireToken(Scope, CTokenType_Identifier).Value;
                RequireToken(Scope, CTokenType_CloseParen);

                auto MemberScope = GetBodyTextForNextScope(Scope, Memory);
                RequireToken(Scope, CTokenType_CloseParen);

                declaration_stream *Members = GetMembersFor(&Replace->Data);
                if (Members)
                {
                  u32 AtIndex = 0;
                  declaration_stream_chunk *IndexedMember = Members->FirstChunk;
                  while ( IndexedMember && AtIndex < MemberIndex )
                  {
                    IndexedMember = IndexedMember->Next;
                    AtIndex ++;
                  }

                  if (IndexedMember && AtIndex == MemberIndex)
                  {
                    meta_func_arg_stream NewArgs = CopyStream(ReplacePatterns, Memory);
                    Push(&NewArgs, ReplacementPattern(MatchPattern, Datatype(&IndexedMember->Element)));
                    counted_string StructFieldOutput = Execute(&MemberScope, &NewArgs, Ctx, Memory, Depth);
                    if (MemberScope.ErrorCode)
                    {
                      Scope->ErrorCode = MemberScope.ErrorCode;
                    }
                    else
                    {
                      TrimTrailingNBSP(&OutputBuilder.Chunks.LastChunk->Element);
                      Append(&OutputBuilder, StructFieldOutput);
                    }
                  }
                  else
                  {
                    PoofTypeError( Scope,
                                   ParseErrorCode_InvalidArgument,
                                   FormatCountedString( TranArena,
                                                        CSz("Attempted to access member index (%u) on (%S), which has (%d) members!"),
                                                        MemberIndex,
                                                        GetNameForDatatype(&Replace->Data, TranArena),
                                                        Members->ChunkCount
                                                        ),
                                   MetaOperatorToken);
                  }
                }
                else
                {
                  PoofTypeError( Scope,
                                 ParseErrorCode_InvalidArgument,
                                 FormatCountedString( TranArena,
                                                      CSz("Attempted to access member index (%u) on (%S), which didn't have any members!"),
                                                      MemberIndex,
                                                      GetNameForDatatype(&Replace->Data, TranArena)),
                                 MetaOperatorToken);
                }

              } break;

              case map_array:
              {
                RequireToken(Scope, CTokenType_OpenParen);
                /* counted_string MatchPattern  = RequireToken(Scope, CTokenType_Identifier).Value; */
                RequireToken(Scope, CTokenType_CloseParen);

                parser MapMemberScope = GetBodyTextForNextScope(Scope, Memory);

                ast_node *Node = DatatypeStaticBufferSize(Ctx, Scope, &Replace->Data, MetaOperatorToken);
                u64 Size = ResolveConstantExpression(Scope, Node);
                for (u64 Index = 0; Index < Size; ++Index)
                {
                  // TODO(Jesse): We need to make meta_func_args have room for more than just datatypes
                  // We now need to have literals
                  meta_func_arg_stream NewArgs = CopyStream(ReplacePatterns, Memory);
                  /* Push(&NewArgs, ReplacementPattern(MatchPattern, Datatype(Index))); */
                  Rewind(MapMemberScope.Tokens);
                  counted_string StructFieldOutput = Execute(&MapMemberScope, &NewArgs, Ctx, Memory, Depth);
                  if (MapMemberScope.ErrorCode)
                  {
                    Scope->ErrorCode = MapMemberScope.ErrorCode;
                  }
                  else
                  {
                    TrimTrailingNBSP(&OutputBuilder.Chunks.LastChunk->Element);
                    Append(&OutputBuilder, StructFieldOutput);
                  }
                }
              } break;

              case map_members:
              {
                RequireToken(Scope, CTokenType_OpenParen);
                counted_string MatchPattern  = RequireToken(Scope, CTokenType_Identifier).Value;
                RequireToken(Scope, CTokenType_CloseParen);

                parser MapMemberScope = GetBodyTextForNextScope(Scope, Memory);
                declaration_stream *Members = GetMembersFor(&Replace->Data);

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
                        meta_func_arg_stream NewArgs = CopyStream(ReplacePatterns, Memory);
                        Push(&NewArgs, ReplacementPattern(MatchPattern, Datatype(Member)));
                        Rewind(MapMemberScope.Tokens);
                        counted_string StructFieldOutput = Execute(&MapMemberScope, &NewArgs, Ctx, Memory, Depth);
                        if (MapMemberScope.ErrorCode)
                        {
                          Scope->ErrorCode = MapMemberScope.ErrorCode;
                        }
                        else
                        {
                          TrimTrailingNBSP(&OutputBuilder.Chunks.LastChunk->Element);
                          Append(&OutputBuilder, StructFieldOutput);
                        }
                      } break;
                    }
                    continue;
                  }
                }
                else
                {
                  PoofTypeError( Scope,
                                 ParseErrorCode_InvalidArgument,
                                 FormatCountedString( TranArena,
                                                      CSz("Called map_members on a datatype that didn't have members (%S)"),
                                                      GetNameForDatatype(&Replace->Data, TranArena)),
                                 MetaOperatorToken);
                }

              } break;

              case map_values:
              {
                RequireToken(Scope, CTokenType_OpenParen);
                c_token *EnumValueMatch  = RequireTokenPointer(Scope, CTokenType_Identifier);
                RequireToken(Scope, CTokenType_CloseParen);
                parser NextScope = GetBodyTextForNextScope(Scope, Memory);

                if (Replace->Data.Type == type_declaration)
                {
                  declaration *D = SafeAccess(declaration, &Replace->Data);
                  if (D->Type == type_enum_decl)
                  {
                    ITERATE_OVER(&D->enum_decl.Members)
                    {
                      enum_member* EnumMember = GET_ELEMENT(Iter);
                      meta_func_arg_stream NewArgs = CopyStream(ReplacePatterns, Memory);
                      Push(&NewArgs, ReplacementPattern(EnumValueMatch->Value, Datatype(EnumMember)));
                      Rewind(NextScope.Tokens);
                      counted_string EnumFieldOutput = Execute(&NextScope, &NewArgs, Ctx, Memory, Depth);
                      if (NextScope.ErrorCode)
                      {
                        Scope->ErrorCode = NextScope.ErrorCode;
                      }
                      else
                      {
                        TrimTrailingNBSP(&OutputBuilder.Chunks.LastChunk->Element);
                        Append(&OutputBuilder, EnumFieldOutput);
                      }
                    }
                  }
                  else
                  {
                    PoofTypeError( Scope,
                                   ParseErrorCode_InvalidArgument,
                                   FormatCountedString( TranArena,
                                                        CSz("Called map_values on a datatype that wasn't an enum (%S)"),
                                                        GetNameForDatatype(&Replace->Data, Memory)),
                                   MetaOperatorToken);
                  }
                }
                else
                {
                  PoofTypeError( Scope,
                                 ParseErrorCode_InvalidArgument,
                                 FormatCountedString( TranArena,
                                                      CSz("Called map_values on a datatype that wasn't an enum (%S)"),
                                                      GetNameForDatatype(&Replace->Data, Memory)),
                                 MetaOperatorToken);
                }

              } break;
            }

            if (ImpetusWasOpenParen)
            {
              RequireToken(Scope, CTokenType_CloseParen);
            }
          }
        }

        meta_func* NestedFunc = StreamContains( FunctionDefs, PeekToken(Scope).Value );
        if (NestedFunc)
        {
          RequireToken(Scope, CToken(NestedFunc->Name));

          RequireToken(Scope, CTokenType_OpenParen);

          c_token *ArgNameT = RequireTokenPointer(Scope, CTokenType_Identifier);
          meta_func_arg* Arg = StreamContains(ReplacePatterns, ArgNameT->Value);

          if (Arg)
          {
            RequireToken(Scope, CTokenType_CloseParen);
            RequireToken(Scope, CTokenType_CloseParen);

            meta_func_arg_stream NewArgs = {};
            Push(&NewArgs, ReplacementPattern(NestedFunc->ArgName, Arg->Data));
            counted_string NestedCode = Execute(NestedFunc, &NewArgs, Ctx, Memory, Depth);
            if (NestedFunc->Body.ErrorCode)
            {
              Scope->ErrorCode = NestedFunc->Body.ErrorCode;
            }
            else
            {
              TrimTrailingNBSP(&OutputBuilder.Chunks.LastChunk->Element);
              Append(&OutputBuilder, NestedCode);
            }
          }
          else
          {
            PoofTypeError( Scope,
                           ParseErrorCode_InvalidArgument,
                           CSz("Unable to resolve poof function argument."),
                           ArgNameT);
          }

        }
        else if (ExecutedChildFunc)
        {
        }
        else
        {
          Append(&OutputBuilder, BodyToken->Value);
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

  counted_string Result = Finalize(&OutputBuilder, Memory);
  return Result;
}

link_internal void
DoTrueFalse( parse_context *Ctx,
             parser *Scope,
             meta_func_arg_stream* ReplacePatterns,
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
      TrimTrailingNBSP(&OutputBuilder->Chunks.LastChunk->Element);
      Append(OutputBuilder, Code);
    }
  }
}

link_internal counted_string
Execute(meta_func* Func, meta_func_arg_stream *Args, parse_context* Ctx, memory_arena* Memory, umm *Depth)
{
  Assert(Func->Body.Tokens->At == Func->Body.Tokens->Start);

  counted_string Result = Execute(&Func->Body, Args, Ctx, Memory, Depth);

  Assert(Func->Body.Tokens->At == Func->Body.Tokens->End);
  Rewind(Func->Body.Tokens);
  return Result;
}
