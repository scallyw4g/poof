
// TODO(Jesse id: 222, tags: immediate, parsing, metaprogramming) : Re-add [[nodiscard]] here
bonsai_function counted_string
Execute(parser *Scope, meta_func_arg_stream* ReplacePatterns, parse_context* Ctx, memory_arena* Memory)
{
  TIMED_FUNCTION();

  program_datatypes* Datatypes = &Ctx->Datatypes;
  meta_func_stream* FunctionDefs = &Ctx->MetaFunctions;

  Assert(Scope->Tokens->At == Scope->Tokens->Start);

  string_builder OutputBuilder = {};
  while ( c_token *BodyToken = PopTokenRawPointer(Scope) )
  {
    if ( BodyToken->Type == CTokenType_StringLiteral )
    {
      counted_string TempStr = StripQuotes(BodyToken->Value);
      parser *StringParse = ParserForAnsiStream(Ctx, AnsiStream(TempStr), TokenCursorSource_MetaprogrammingExpansion);
      counted_string Code = Execute(StringParse, ReplacePatterns, Ctx, Memory);
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
    }
    else if ( BodyToken->Type == CTokenType_OpenParen ||
              BodyToken->Type == CTokenType_Identifier )
    {
      b32 ImpetusWasOpenParen = BodyToken->Type == CTokenType_OpenParen;
      b32 ImpetusWasIdentifier = BodyToken->Type == CTokenType_Identifier;

      b32 ExecutedChildFunc = False;
      ITERATE_OVER_AS(Replace, ReplacePatterns)
      {
        meta_func_arg* Replace = GET_ELEMENT(ReplaceIter);
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
              auto S1 = GetTypeTypeForDatatype(&Replace->Data, TranArena);
              auto S2 = GetTypeNameForDatatype(Ctx, &Replace->Data, TranArena);

              b32 DoTrueBranch = False;
              switch(Replace->Data.Type)
              {
                case type_datatype_noop:
                {
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

              DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory);
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

              DoTrueFalse(Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory);
            } break;

            case is_compound:
            {
              RequireToken(Scope, CTokenType_Question);
              compound_decl *CD = DatatypeIsCompoundDecl(Ctx, Scope, &Replace->Data, MetaOperatorToken);

              b32 DoTrueBranch = (CD != 0);

              // @counted_string_primitive_hack
              if (CD)
              {
                datatype TmpDt = Datatype(CD);
                counted_string DTName = GetNameForDatatype(&TmpDt, TranArena);
                if (StringsMatch(DTName, CSz("counted_string")))
                {
                  DoTrueBranch = False;
                }
              }

              DoTrueFalse(Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory);
            } break;

            case is_struct:
            case is_union:
            {
              RequireToken(Scope, CTokenType_Question);
              compound_decl *D = DatatypeIsCompoundDecl(Ctx, Scope, &Replace->Data, MetaOperatorToken);

              b32 DoTrueBranch = False;
              if (D)
              {
                b32 Negate = (Operator == is_struct);
                DoTrueBranch = (D->IsUnion ^ Negate);
              }

              DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory);
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

              DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory);
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
              DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory);
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
                DoTrueFalse( Ctx, Scope, ReplacePatterns, DoTrueBranch, &OutputBuilder, Memory);
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

            case map_members:
            {
              RequireToken(Scope, CTokenType_OpenParen);
              counted_string MatchPattern  = RequireToken(Scope, CTokenType_Identifier).Value;
              RequireToken(Scope, CTokenType_CloseParen);

              counted_string ContainingConstraint = {};
              counted_string ChildName = {};
              if ( OptionalToken(Scope, CTokenType_Dot) )
              {
                RequireToken(Scope, CToken(CSz("containing")));
                RequireToken(Scope, CTokenType_OpenParen);
                ContainingConstraint = RequireToken(Scope, CTokenType_Identifier).Value;

                if ( OptionalToken(Scope, CToken(CSz("as"))) )
                {
                  ChildName = RequireToken(Scope, CTokenType_Identifier).Value;
                }

                RequireToken(Scope, CTokenType_CloseParen);
              }

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
                    {
                      b32 ConstraintPassed = False;
                      b32 HaveConstraint = ContainingConstraint.Count > 0;

                      if (HaveConstraint)
                      {
                        auto Var = SafeAccess(variable_decl, Member);
                        Assert(Var->Type.DatatypeToken);
                        auto DT = GetDatatypeByName(Ctx, Var->Type.DatatypeToken->Value);
                        Assert(DT.Type);
                        auto VarMembers = GetMembersFor(&DT);

                        if (VarMembers)
                        {
                          ITERATE_OVER_AS(VarMember, VarMembers)
                          {
                            declaration* Decl = GET_ELEMENT(VarMemberIter);
                            counted_string MemberName = GetTypeNameForDecl(Ctx, Decl, Memory);
                            if (StringsMatch(MemberName, ContainingConstraint))
                            {
                              ConstraintPassed = True;
                            }
                          }
                        }
                      }

                      if (HaveConstraint == False           ||
                         (HaveConstraint && ConstraintPassed) )
                      {
                        meta_func_arg_stream NewArgs = CopyStream(ReplacePatterns, Memory);
                        Push(&NewArgs, ReplacementPattern(MatchPattern, Datatype(Member)), Memory);
                        Rewind(MapMemberScope.Tokens);
                        counted_string StructFieldOutput = Execute(&MapMemberScope, &NewArgs, Ctx, Memory);
                        if (MapMemberScope.ErrorCode)
                        {
                          Scope->ErrorCode = MapMemberScope.ErrorCode;
                        }
                        else
                        {
                          Append(&OutputBuilder, StructFieldOutput);
                        }
                      }

                    } break;

                    case type_compound_decl:
                    {
                      meta_func_arg_stream NewArgs = CopyStream(ReplacePatterns, Memory);
                      Push(&NewArgs, ReplacementPattern(MatchPattern, Datatype(Member)), Memory);
                      Rewind(MapMemberScope.Tokens);
                      counted_string StructFieldOutput = Execute(&MapMemberScope, &NewArgs, Ctx, Memory);
                      if (MapMemberScope.ErrorCode)
                      {
                        Scope->ErrorCode = MapMemberScope.ErrorCode;
                      }
                      else
                      {
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
                    Push(&NewArgs, ReplacementPattern(EnumValueMatch->Value, Datatype(EnumMember)), Memory);
                    Rewind(NextScope.Tokens);
                    counted_string EnumFieldOutput = Execute(&NextScope, &NewArgs, Ctx, Memory);
                    if (NextScope.ErrorCode)
                    {
                      Scope->ErrorCode = NextScope.ErrorCode;
                    }
                    else
                    {
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
          Push(&NewArgs, ReplacementPattern(NestedFunc->ArgName, Arg->Data), Memory);
          counted_string NestedCode = Execute(NestedFunc, &NewArgs, Ctx, Memory);
          if (NestedFunc->Body.ErrorCode)
          {
            Scope->ErrorCode = NestedFunc->Body.ErrorCode;
          }
          else
          {
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
    }
    else
    {
      Append(&OutputBuilder, BodyToken->Value);
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
             memory_arena *Memory)
{
  parser TrueScope = GetBodyTextForNextScope(Scope, Memory);
  parser FalseScope = {};

  if (PeekToken(Scope).Type == CTokenType_OpenBrace)
  {
    FalseScope = GetBodyTextForNextScope(Scope, Memory);
  }

  if (DoTrueBranch)
  {
    counted_string Code = Execute(&TrueScope, ReplacePatterns, Ctx, Memory);
    if (TrueScope.ErrorCode)
    {
      Scope->ErrorCode = TrueScope.ErrorCode;
    }
    else
    {
      Append(OutputBuilder, Code);
    }
  }
  else
  {
    if (FalseScope.Tokens)
    {
      counted_string Code = Execute(&FalseScope, ReplacePatterns, Ctx, Memory);
      if (FalseScope.ErrorCode)
      {
        Scope->ErrorCode = FalseScope.ErrorCode;
      }
      else
      {
        Append(OutputBuilder, Code);
      }
    }
  }
}

bonsai_function counted_string
Execute(meta_func* Func, meta_func_arg_stream *Args, parse_context* Ctx, memory_arena* Memory)
{
  Assert(Func->Body.Tokens->At == Func->Body.Tokens->Start);

  counted_string Result = Execute(&Func->Body, Args, Ctx, Memory);

  Assert(Func->Body.Tokens->At == Func->Body.Tokens->End);
  Rewind(Func->Body.Tokens);
  return Result;
}
