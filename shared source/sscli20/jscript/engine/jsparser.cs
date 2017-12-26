// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==

namespace Microsoft.JScript{

    using System;
    using System.Collections;
    using System.Reflection;
    using System.Text.RegularExpressions;
    using Microsoft.Vsa;
    using System.Security.Permissions;
    using System.Globalization;
    
    //***************************************************************************************
    // JSParser
    //
    //  The JScript Parser.
    //***************************************************************************************
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
  public class JSParser{


      private bool demandFullTrustOnFunctionCreation;
      private Context sourceContext;
      private JSScanner scanner;
      private Context currentToken;
      private Context errorToken;  // used for errors to flag that the same token has to be returned.
                                   // We could have used just a boolean but having a Context does not
                                   // add any overhead and allow to really save the info, if that will ever be needed
      private int tokensSkipped;
      private const int c_MaxSkippedTokenNumber = 50;
      private NoSkipTokenSet noSkipTokenSet;
      private long goodTokensProcessed;

      private Block program;

      // label related info
      private ArrayList blockType ;
      private SimpleHashtable labelTable;
      enum BlockType {Block, Loop, Switch, Finally}
      private int finallyEscaped;

      private int breakRecursion = 0;
      private static int s_cDummyName;
      #if DEBUG
      private static int s_filenameSuffix = 0;
      #endif

      Globals Globals;
      private int Severity;

      //---------------------------------------------------------------------------------------
      // JSParser
      //
      // create a parser with a context. The context is the code that has to be compiled.
      // Typically used by the runtime
      //---------------------------------------------------------------------------------------
      public JSParser(Context context){
        this.sourceContext = context;
        this.currentToken = context.Clone();
        this.scanner = new JSScanner(this.currentToken);
        this.noSkipTokenSet = new NoSkipTokenSet();

        this.errorToken = null;
        this.program = null;
        this.blockType = new ArrayList(16);
        this.labelTable = new SimpleHashtable(16);
        this.finallyEscaped = 0;
        this.Globals = context.document.engine.Globals;
        this.Severity = 5;
        this.demandFullTrustOnFunctionCreation = false;
      }

      //---------------------------------------------------------------------------------------
      // Parse
      //
      // Parser main entry point. Parse all the source code and return the root of the AST
      //---------------------------------------------------------------------------------------
      public ScriptBlock Parse(){
        Block ast = ParseStatements(false);
        return new ScriptBlock(this.sourceContext.Clone(), ast);
      }

      //---------------------------------------------------------------------------------------
      // ParseEvalBody
      //
      // Parser main entry point for Eval. Parse all the source code and return the root of the AST
      //---------------------------------------------------------------------------------------
      public Block ParseEvalBody(){
        this.demandFullTrustOnFunctionCreation = true;
        return ParseStatements(true);
      }

      //---------------------------------------------------------------------------------------
      // ParseExpressionItem
      //
      // A VSAITEMTYPE2.EXPRESSION call into here to parse an expression
      //---------------------------------------------------------------------------------------
      internal ScriptBlock ParseExpressionItem(){
        int scopeStackSize = Globals.ScopeStack.Size();
        try{
          Block block = new Block(this.sourceContext.Clone());
          GetNextToken();
          block.Append(new Expression(this.sourceContext.Clone(), ParseExpression()));
          return new ScriptBlock(this.sourceContext.Clone(), block);
        }catch (EndOfFile){
        }catch (ScannerException se){
          // a scanner exception implies that the end of file has been reached with an error.
          // Mark the end of file as the error location
          EOFError(se.m_errorId);
        }
        catch (StackOverflowException)
        {
          // On stack overflow, finally clauses are not executed so we explicitly
          // restore the global stack here.
          Globals.ScopeStack.TrimToSize(scopeStackSize);
          ReportError(JSError.OutOfStack, true);
        }
        return null;
      }

      //---------------------------------------------------------------------------------------
      // ParseStatements
      //
      // statements :
      //   <empty> |
      //   statement statements |
      //   ImportStatement
      //
      //---------------------------------------------------------------------------------------
      private Block ParseStatements(bool insideEval){
        int scopeStackSize = Globals.ScopeStack.Size();
        this.program = new Block(this.sourceContext.Clone());
        this.blockType.Add(BlockType.Block);
        this.errorToken = null;
        try{
          GetNextToken();
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_StartStatementNoSkipTokenSet);
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_TopLevelNoSkipTokenSet);

          try{
            while (this.currentToken.token != JSToken.EndOfFile){
              AST ast = null;
              try{
                if (this.currentToken.token == JSToken.Package && !insideEval){
                  ast = ParsePackage(this.currentToken.Clone());
                }else if (this.currentToken.token == JSToken.Import && !insideEval){
                  this.noSkipTokenSet.Add(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet);
                  try{
                    ast = ParseImportStatement();
                  }catch(RecoveryTokenException exc){
                    if (IndexOfToken(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet, exc) != -1){
                      ast = exc._partiallyComputedNode;
                      if (exc._token == JSToken.Semicolon)
                        GetNextToken();
                    }else{
                      throw exc;
                    }
                  }finally{
                    this.noSkipTokenSet.Remove(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet);
                  }
                }else
                  ast = ParseStatement();
              }catch(RecoveryTokenException exc){
                if (TokenInList(NoSkipTokenSet.s_TopLevelNoSkipTokenSet, exc)
                    || TokenInList(NoSkipTokenSet.s_StartStatementNoSkipTokenSet, exc)){
                  ast = exc._partiallyComputedNode;
                }else{
                  this.errorToken = null;
                  do{
                    GetNextToken();
                  }while(this.currentToken.token != JSToken.EndOfFile && !TokenInList(NoSkipTokenSet.s_TopLevelNoSkipTokenSet, this.currentToken.token)
                    && !TokenInList(NoSkipTokenSet.s_StartStatementNoSkipTokenSet, this.currentToken.token));
                }
              }

              if (null != ast)
                this.program.Append(ast);
            }

          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_TopLevelNoSkipTokenSet);
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_StartStatementNoSkipTokenSet);
          }

        }catch (EndOfFile){
        }catch (ScannerException se){
          // a scanner exception implies that the end of file has been reached with an error.
          // Mark the end of file as the error location
          EOFError(se.m_errorId);
        }
        catch (StackOverflowException)
        {
          // On stack overflow, finally clauses are not executed so we explicitly
          // restore the global stack here.
          Globals.ScopeStack.TrimToSize(scopeStackSize);
          ReportError(JSError.OutOfStack, true);
        }
        return this.program;
      }

      //---------------------------------------------------------------------------------------
      // ParseStatement
      //
      //  OptionalStatement:
      //    Statement |
      //    <empty>
      //
      //  Statement :
      //    Block |
      //  VariableStatement |
      //  EmptyStatement |
      //  ExpressionStatement |
      //  IfStatement |
      //  IterationStatement |
      //  ContinueStatement |
      //  BreakStatement |
      //  ReturnStatement |
      //  WithStatement |
      //  LabeledStatement |
      //  SwitchStatement |
      //  ThrowStatement |
      //  TryStatement |
      //  QualifiedDeclaration |
      //  Class |
      //  FunctionDeclaration
      //
      // IterationStatement :
      //    'for' '(' ForLoopControl ')' |                  ===> ForStatement
      //    'do' Statement 'while' '(' Expression ')' |     ===> DoStatement
      //    'while' '(' Expression ')' Statement            ===> WhileStatement
      //
      //---------------------------------------------------------------------------------------

      // ParseStatement deals with the end of statement issue (EOL vs ';') so if any of the
      // ParseXXX routine does it as well, it should return directly from the switch statement
      // without any further execution in the ParseStatement
      private AST ParseStatement(){
        AST statement = null;
        String id = null;

        switch (this.currentToken.token){
          case JSToken.EndOfFile:
            EOFError(JSError.ErrEOF);
            throw new EndOfFile(); // abort parsing, get back to the main parse routine
          case JSToken.Debugger:
            statement = new DebugBreak(this.currentToken.Clone());
            GetNextToken();
            break;
          case JSToken.Semicolon:
            // make an empty statement
            statement = new Block(this.currentToken.Clone());
            GetNextToken();
            return statement;
          case JSToken.RightCurly:
            ReportError(JSError.SyntaxError);
            SkipTokensAndThrow();
            break;
          case JSToken.LeftCurly:
            return ParseBlock();
          case JSToken.Var:
          case JSToken.Const:
            return ParseVariableStatement((FieldAttributes)0, null, this.currentToken.token);
          case JSToken.If:
            return ParseIfStatement();
          case JSToken.For:
            return ParseForStatement();
          case JSToken.Do:
            return ParseDoStatement();
          case JSToken.While:
            return ParseWhileStatement();
          case JSToken.Continue:
            statement = ParseContinueStatement();
            if (null == statement)
              return new Block(CurrentPositionContext());
            else
              return statement;
          case JSToken.Break:
            statement = ParseBreakStatement();
            if (null == statement)
              return new Block(CurrentPositionContext());
            else
              return statement;
          case JSToken.Return:
            statement = ParseReturnStatement();
            if (null == statement)
              return new Block(CurrentPositionContext());
            else
              return statement;
          case JSToken.With:
            return ParseWithStatement();
          case JSToken.Switch:
            return ParseSwitchStatement();
          case JSToken.Super:
          case JSToken.This:
            Context superCtx = this.currentToken.Clone();
            if (JSToken.LeftParen == this.scanner.PeekToken())
              statement = ParseConstructorCall(superCtx);
            else
              goto default;
            break;
          case JSToken.Throw:
            statement = ParseThrowStatement();
            if (statement == null)
              return new Block(CurrentPositionContext());
            else
              break;
          case JSToken.Try:
            return ParseTryStatement();
          case JSToken.Internal:
          case JSToken.Public:
          case JSToken.Static:
          case JSToken.Private:
          case JSToken.Protected:
          case JSToken.Abstract:
          case JSToken.Final:
            bool parsedOK;
            statement = ParseAttributes(null, false, false, out parsedOK);
            if (!parsedOK){
              statement = ParseExpression(statement, false, true, JSToken.None);
              statement = new Expression(statement.context.Clone(), statement);
              break;
            }else
              return statement;
          case JSToken.Package:
            Context packageContext = this.currentToken.Clone();
            statement = ParsePackage(packageContext);
            if (statement is Package){
              // handle common error of using import in function
              ReportError(JSError.PackageInWrongContext, packageContext, true);
              // make an empty statement
              statement = new Block(packageContext);
            }
            break;                      
          case JSToken.Interface:
          case JSToken.Class:
            return ParseClass((FieldAttributes)0, false, this.currentToken.Clone(), false, false, null);
          case JSToken.Enum:
            return ParseEnum((FieldAttributes)0, this.currentToken.Clone(), null);
          case JSToken.Function:
            return ParseFunction((FieldAttributes)0, false, this.currentToken.Clone(), false, false, false, false, null); //Parse a function as a statement
          case JSToken.Else:
            ReportError(JSError.InvalidElse);
            SkipTokensAndThrow();
            break;
          case JSToken.Import:
            // handle common error of using import in function
            ReportError(JSError.InvalidImport, true);
            // make an empty statement
            statement = new Block(this.currentToken.Clone());
            try{
              ParseImportStatement();
            }catch(RecoveryTokenException){
            }
            break;                      
          default:
            this.noSkipTokenSet.Add(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet);
            bool exprError = false;
            try{
              bool bAssign, canBeAttribute = true;
              statement = ParseUnaryExpression(out bAssign, ref canBeAttribute, false);
              if (canBeAttribute){
                // look for labels
                if (statement is Lookup){
                  if (JSToken.Colon == this.currentToken.token){
                    // can be a label
                    id = statement.ToString();
                    if (null != this.labelTable[id]){
                      // there is already a label with that name. Ignore the current label
                      ReportError(JSError.BadLabel, statement.context.Clone(), true);
                      id = null;
                      GetNextToken(); // skip over ':'
                      return new Block(CurrentPositionContext());
                    }else{
                      GetNextToken();
                      this.labelTable[id] = this.blockType.Count;
                      if (JSToken.EndOfFile != this.currentToken.token)
                        statement = ParseStatement();
                      else
                        statement = new Block(CurrentPositionContext());
                      this.labelTable.Remove(id);
                      return statement;
                    }
                  }
                }
                // look for custom attributes
                if (JSToken.Semicolon != this.currentToken.token && !this.scanner.GotEndOfLine()){
                  bool parsed;
                  statement = ParseAttributes(statement, false, false, out parsed);
                  if (parsed)
                    return statement;
                }
              }
              statement = ParseExpression(statement, false, bAssign, JSToken.None);
              statement = new Expression(statement.context.Clone(), statement);
            }catch(RecoveryTokenException exc){
              if (exc._partiallyComputedNode != null)
                statement = exc._partiallyComputedNode;

              if (statement == null){
                this.noSkipTokenSet.Remove(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet);
                exprError = true;
                SkipTokensAndThrow();
              }

              if (IndexOfToken(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet, exc) == -1){
                exc._partiallyComputedNode = statement;
                throw exc;
              }
            }finally{
              if (!exprError)
                this.noSkipTokenSet.Remove(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet);
            }
            break;
        }

        if (JSToken.Semicolon == this.currentToken.token){
          statement.context.UpdateWith(this.currentToken);
          GetNextToken();
        }else if (!this.scanner.GotEndOfLine() && JSToken.RightCurly != this.currentToken.token && JSToken.EndOfFile != this.currentToken.token)
          ReportError(JSError.NoSemicolon, true);
        return statement;
      }

      //---------------------------------------------------------------------------------------
      // ParseAttributes
      //
      //  Package :
      //    Attributes 'package' QualifiedIdentifier '{' ClassList '}'
      //    AttributesStart 'package' QualifiedIdentifier '{' ClassList '}'
      //
      //  Class :
      //    Attributes 'class' Identifier ['extends' QualifiedIdentifier] '{' ClassBody '}'
      //    AttributesStart 'class' Identifier ['extends' QualifiedIdentifier] '{' ClassBody '}'
      //
      //  FunctionDeclaration :
      //    Attributes 'function' Identifier '(' [FormalParameterList] ')' [':' TypeExpression] '{' FunctionBody '}'
      //    AttributesStart 'function' Identifier '(' [FormalParameterList] ')' [':' TypeExpression] '{' FunctionBody '}'
      //
      //  VariableStatement :
      //    Attributes 'var' VariableDeclarationList
      //    AttributesStart 'var' VariableDeclarationList
      //
      //  Attributes :
      //    AttributesStart AttributesNext |
      //    Modifier [AttributeNext]
      //
      //  AttributesStart :
      //    QualifiedIdentifier /no LineTerminator here/ |
      //    QualifiedIdentifier Arguments /no LineTerminator here/
      //
      //  AttributesNext :
      //    QualifiedIdentifier [AttributesNext] |
      //    QualifiedIdentifier Arguments [AttributesNext] |
      //    Modifier [AttributesNext]
      //
      //  Modifier :
      //    'private' |
      //    'public' |
      //    'protected' |
      //    'internal' |
      //    'package' | //This is going away
      //    'static' |
      //    'abstract'
      //
      //
      // Parse the list of attributes and the statement those attributes refer to.
      // It comes back with one of the following: package, class, function, variable declaration.
      // When in error it may return other AST constructs depending on the context.
      // The input is a possible attribute in the form of 'QualifiedIdentifier' or a sequence
      // 'QualifiedIdentifier Arguments' or null when the first attribute was a Modifier.
      // The whole sequence may be an error in the first place, so we want to keep
      // things such that we can always parse the supposed list of attributes as an ast list
      //---------------------------------------------------------------------------------------

      AST ParseAttributes(AST statement, bool unambiguousContext, bool isInsideClass, out bool parsedOK){

        // used later to verify whether anything at all has been processed
        AST inputAST = statement;

        // keep two list of errors. One holds the 'missing ;' errors that would result in the token
        // stream being parsed as a list of expressions, the other is the list of attributes errors
        // that would result from the token stream being parsed as a list of attributes. We need both because
        // we don't know until the end which is which
        ArrayList semicolonErrors = new ArrayList();

        // keep track of the modifiers context in case they'll have to be parsed as identifiers, we give the warning out
        ArrayList modifiersAsIdentifier = new ArrayList();

        // this list has two elements for each error, the first (even position - 0 based) is the error type
        // the second is the context
        ArrayList attributesErrors = new ArrayList();

        // keep the last ast node parsed. Used if we have to "revert" to the expression list production.
        // We just return the last ast processed, after all is an error condition
        AST lastAST = null;

        // keep the list of attributes
        ArrayList attributes = new ArrayList();

        // hold the context for the 'abstract' keyword and it is used as a flag for its presence
        Context abstractContext = null;
        // hold the context for the 'static' keyword and it is used as a flag for its presence
        Context staticContext = null;
        // hold the context for the 'final' keyword and it is used as a flag for its presence
        Context finalContext = null;

        // keep the number of consecutive token no separated by end of line. Once that number
        // reaches 2 the token stream is parsed *only* as a sequence of attributes
        int TokenCountNoEOL = 0;
        if (unambiguousContext)
          TokenCountNoEOL = 2;

        FieldAttributes visibilitySpec = (FieldAttributes)0, currVis = (FieldAttributes)0;

        Context statementContext = null;
        // null when coming from a visibility modifier
        if (statement != null){
          lastAST = statement;
          attributes.Add(statement);
          semicolonErrors.Add(CurrentPositionContext());
          statementContext = statement.context.Clone();
          TokenCountNoEOL = 1;
        }else{
          statementContext = this.currentToken.Clone();
        }

        parsedOK = true;
        while (true){
          JSToken attributeToken = JSToken.None;
          switch (this.currentToken.token){
            case JSToken.Public:
            case JSToken.Static:
            case JSToken.Private:
            case JSToken.Protected:
            case JSToken.Abstract:
            case JSToken.Final:
            case JSToken.Internal:
              attributeToken = this.currentToken.token;
              goto case JSToken.Identifier;
            case JSToken.Var:
            case JSToken.Const:
              for (int i = 0, n = attributesErrors.Count; i < n; i += 2)
                ReportError((JSError)attributesErrors[i], (Context)attributesErrors[i + 1], true);
              if (abstractContext != null)
                ReportError(JSError.IllegalVisibility, abstractContext, true);
              if (finalContext != null)
                ReportError(JSError.IllegalVisibility, finalContext, true);
              statementContext.UpdateWith(this.currentToken);
              return ParseVariableStatement(visibilitySpec, FromASTListToCustomAttributeList(attributes), this.currentToken.token);
            case JSToken.Function:
              for (int i = 0, n = attributesErrors.Count; i < n; i += 2)
                ReportError((JSError)attributesErrors[i], (Context)attributesErrors[i + 1], true);
              statementContext.UpdateWith(this.currentToken);
              if (staticContext != null)
                if (abstractContext != null){
                  staticContext.HandleError(JSError.AbstractCannotBeStatic);
                  staticContext = null;
                }else if (finalContext != null){
                  finalContext.HandleError(JSError.StaticIsAlreadyFinal);
                  finalContext = null;
                }
              if (abstractContext != null){
                if (finalContext != null){
                  finalContext.HandleError(JSError.FinalPrecludesAbstract);
                  finalContext = null;
                }
                if (currVis == FieldAttributes.Private){
                  abstractContext.HandleError(JSError.AbstractCannotBePrivate);
                  currVis = FieldAttributes.Family;
                }
              }
              return ParseFunction(visibilitySpec, false, statementContext, isInsideClass,
                                   abstractContext != null, finalContext != null, false, FromASTListToCustomAttributeList(attributes));
            case JSToken.Interface:
              if (abstractContext != null){
                ReportError(JSError.IllegalVisibility, abstractContext, true);
                abstractContext = null;
              }
              if (finalContext != null){
                ReportError(JSError.IllegalVisibility, finalContext, true);
                finalContext = null;
              }
              if (staticContext != null){
                ReportError(JSError.IllegalVisibility, staticContext, true);
                staticContext = null;
              }
              goto case JSToken.Class;
            case JSToken.Class:
              for (int i = 0, n = attributesErrors.Count; i < n; i += 2)
                ReportError((JSError)attributesErrors[i], (Context)attributesErrors[i + 1], true);
              statementContext.UpdateWith(this.currentToken);
              if (finalContext != null && abstractContext != null)
                finalContext.HandleError(JSError.FinalPrecludesAbstract);
              return ParseClass(visibilitySpec, staticContext != null, statementContext,
                                abstractContext != null, finalContext != null, FromASTListToCustomAttributeList(attributes));
            case JSToken.Enum:
              for (int i = 0, n = attributesErrors.Count; i < n; i += 2)
                ReportError((JSError)attributesErrors[i], (Context)attributesErrors[i + 1], true);
              statementContext.UpdateWith(this.currentToken);
              if (abstractContext != null)
                ReportError(JSError.IllegalVisibility, abstractContext, true);
              if (finalContext != null)
                ReportError(JSError.IllegalVisibility, finalContext, true);
              if (staticContext != null)
                ReportError(JSError.IllegalVisibility, staticContext, true);
              return ParseEnum(visibilitySpec, statementContext, FromASTListToCustomAttributeList(attributes));
            case JSToken.Void:
            case JSToken.Boolean:
            case JSToken.Byte:
            case JSToken.Char:
            case JSToken.Double:
            case JSToken.Float:
            case JSToken.Int:
            case JSToken.Long:
            case JSToken.Short: {
              
              // The user has some sequence like "public int foo()", which
              // is probably a C-style function declaration.  Save that away,
              // keep parsing and see if this ends in what looks like a function
              // or variable declaration. 

              parsedOK = false;
              lastAST = new Lookup(this.currentToken);
              attributeToken = JSToken.None;
              attributes.Add(lastAST);
              GetNextToken();
              goto continueLoop;
            }
            case JSToken.Identifier: {
              bool bAssign, canBeAttribute = true;
              statement = ParseUnaryExpression(out bAssign, ref canBeAttribute, false, attributeToken == JSToken.None);
              lastAST = statement;
              if (attributeToken != JSToken.None){
                if (statement is Lookup)
                  break;
                else{
                  if (TokenCountNoEOL != 2)
                    modifiersAsIdentifier.Add(this.currentToken.Clone());
                }
              }
              attributeToken = JSToken.None;
              if (canBeAttribute){
                attributes.Add(statement);
                goto continueLoop;
              }else
                goto default;
            }
            // the following cases are somewhat interesting because they can be determined by a user missing
            // the keyword 'var' or 'function', as, for instance, when coming from C/C++/C# code
            default:
              parsedOK = false;
              if (TokenCountNoEOL != 2){
                if (inputAST != statement || statement == null){
                  Debug.Assert(lastAST != null);
                  // only return the last statement
                  statement = lastAST;
                  // output the semicolon errors
                  for (int i = 0, n = modifiersAsIdentifier.Count; i < n; i++)
                    ForceReportInfo((Context)modifiersAsIdentifier[i], JSError.KeywordUsedAsIdentifier);
                  for (int i = 0, n = semicolonErrors.Count; i < n; i++)
                    if (!this.currentToken.Equals((Context)semicolonErrors[i]))
                      ReportError(JSError.NoSemicolon, (Context)semicolonErrors[i], true);
                }
                return statement;
              }else{
                if (attributes.Count > 0){
                  // check for possible common mistakes
                  AST ast = (AST)attributes[attributes.Count - 1];
                  if (ast is Lookup){
                    if (JSToken.Semicolon == this.currentToken.token
                        || JSToken.Colon == this.currentToken.token){
                      ReportError(JSError.BadVariableDeclaration, ast.context.Clone());
                      SkipTokensAndThrow();
                    }
                  }else if (ast is Call && ((Call)ast).CanBeFunctionDeclaration()){
                    if (JSToken.Colon == this.currentToken.token
                        || JSToken.LeftCurly == this.currentToken.token){
                      ReportError(JSError.BadFunctionDeclaration, ast.context.Clone(), true);
                      if (JSToken.Colon == this.currentToken.token){
                        this.noSkipTokenSet.Add(NoSkipTokenSet.s_StartBlockNoSkipTokenSet);
                        try{
                          SkipTokensAndThrow();
                        }catch(RecoveryTokenException){
                        }finally{
                          this.noSkipTokenSet.Remove(NoSkipTokenSet.s_StartBlockNoSkipTokenSet);
                        }
                      }
                      this.errorToken = null;
                      if (JSToken.LeftCurly == this.currentToken.token){
                        FunctionScope fscope = new FunctionScope(Globals.ScopeStack.Peek(), isInsideClass);
                        Globals.ScopeStack.Push(fscope);
                        try{
                          ParseBlock();
                        }finally{
                          Globals.ScopeStack.Pop();
                        }
                        SkipTokensAndThrow();
                      }
                    }
                    else
                      ReportError(JSError.SyntaxError, ast.context.Clone());
                    SkipTokensAndThrow();
                  }
                }

                if (JSToken.LeftCurly == this.currentToken.token){
                  if (isInsideClass){
                    // parse it like a static initializer

                    // flush all the possible attribute errors
                    for (int i = 0, n = attributesErrors.Count; i < n; i += 2)
                      ReportError((JSError)attributesErrors[i], (Context)attributesErrors[i + 1]);
                    if (staticContext == null)
                      ReportError(JSError.StaticMissingInStaticInit, CurrentPositionContext());
                    String className = ((ClassScope)Globals.ScopeStack.Peek()).name;
                    bool reportNoIdentifier = true;
                    foreach (Object attr in attributes){
                      reportNoIdentifier = false;
                      if (staticContext != null && attr is Lookup && attr.ToString() == className &&
                          ((Lookup)attr).context.StartColumn > staticContext.StartColumn)
                        continue;
                      ReportError(JSError.SyntaxError, ((AST)attr).context);
                    }
                    if (reportNoIdentifier)
                      ReportError(JSError.NoIdentifier, CurrentPositionContext());

                    this.errorToken = null; // we want to go to next token regardless of whether or not we had an error
                    // return a static initializer
                    parsedOK = true;
                    return ParseStaticInitializer(statementContext);
                  }
                }

                //mark them all as errors
                ReportError(JSError.MissingConstructForAttributes, statementContext.CombineWith(this.currentToken));
              }

              SkipTokensAndThrow();
              break;
          }

          switch (attributeToken){
            case JSToken.Public:
              currVis = FieldAttributes.Public;
              break;
            case JSToken.Static:
              if (isInsideClass){
                currVis = FieldAttributes.Static;
                if (staticContext != null){
                  attributesErrors.Add(JSError.SyntaxError);
                  attributesErrors.Add(statement.context.Clone());
                }else
                  staticContext = statement.context.Clone();
              }else{
                attributesErrors.Add(JSError.NotInsideClass);
                attributesErrors.Add(statement.context.Clone());
              }
              break;
            case JSToken.Private:
              if (isInsideClass)
                currVis = FieldAttributes.Private;
              else{
                attributesErrors.Add(JSError.NotInsideClass);
                attributesErrors.Add(statement.context.Clone());
              }
              break;
            case JSToken.Protected:
              if (isInsideClass){
                currVis = FieldAttributes.Family;
              }else{
                attributesErrors.Add(JSError.NotInsideClass);
                attributesErrors.Add(statement.context.Clone());
              }
              break;
            case JSToken.Abstract:
              if (abstractContext != null){
                attributesErrors.Add(JSError.SyntaxError);
                attributesErrors.Add(statement.context.Clone());
              }else
                abstractContext = statement.context.Clone();
              goto continueLoop;
            case JSToken.Final:
              if (finalContext != null){
                attributesErrors.Add(JSError.SyntaxError);
                attributesErrors.Add(statement.context.Clone());
              }else
                finalContext = statement.context.Clone();
              goto continueLoop;
            case JSToken.Internal:
              currVis = FieldAttributes.Assembly;
              break;
            default:
              break;
          }
          // come here only after a visibility Modifer
          if ((visibilitySpec & FieldAttributes.FieldAccessMask) == currVis && currVis != (FieldAttributes)0){
            attributesErrors.Add(JSError.DupVisibility);
            attributesErrors.Add(statement.context.Clone());
          }else if ((visibilitySpec & FieldAttributes.FieldAccessMask) > (FieldAttributes)0 && (currVis & FieldAttributes.FieldAccessMask) > (FieldAttributes)0){
            if ((currVis == FieldAttributes.Family && (visibilitySpec & FieldAttributes.FieldAccessMask) ==  FieldAttributes.Assembly) ||
                (currVis == FieldAttributes.Assembly && (visibilitySpec & FieldAttributes.FieldAccessMask) ==  FieldAttributes.Family)){
              visibilitySpec &= ~FieldAttributes.FieldAccessMask;
              visibilitySpec |= FieldAttributes.FamORAssem;
            }else{
              attributesErrors.Add(JSError.IncompatibleVisibility);
              attributesErrors.Add(statement.context.Clone());
            }
          }else{
            visibilitySpec |= currVis;
            statementContext.UpdateWith(statement.context);
          }
        continueLoop:
          if (TokenCountNoEOL != 2){
            if (this.scanner.GotEndOfLine()){
              TokenCountNoEOL = 0;
            }else{
              TokenCountNoEOL++;
              semicolonErrors.Add(this.currentToken.Clone());
            }
          }
        }
      }

      //---------------------------------------------------------------------------------------
      // ParseBlock
      //
      //  Block :
      //    '{' OptionalStatements '}'
      //---------------------------------------------------------------------------------------
      Block ParseBlock(){
        Context ctx;
        return ParseBlock(out ctx);
      }

      Block ParseBlock(out Context closingBraceContext){
        closingBraceContext = null;
        this.blockType.Add(BlockType.Block);
        Block codeBlock = new Block(this.currentToken.Clone());
        GetNextToken();

        this.noSkipTokenSet.Add(NoSkipTokenSet.s_StartStatementNoSkipTokenSet);
        this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockNoSkipTokenSet);
        try{
          try{
            while (JSToken.RightCurly != this.currentToken.token){
              try{
                codeBlock.Append(ParseStatement());
              }catch(RecoveryTokenException exc){
                if (exc._partiallyComputedNode != null)
                  codeBlock.Append(exc._partiallyComputedNode);
                if (IndexOfToken(NoSkipTokenSet.s_StartStatementNoSkipTokenSet, exc) == -1)
                  throw exc;
              }
            }
          }catch(RecoveryTokenException exc){
            if (IndexOfToken(NoSkipTokenSet.s_BlockNoSkipTokenSet, exc) == -1){
              exc._partiallyComputedNode = codeBlock;
              throw exc;
            }
          }
        }finally{
          this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockNoSkipTokenSet);
          this.noSkipTokenSet.Remove(NoSkipTokenSet.s_StartStatementNoSkipTokenSet);
          this.blockType.RemoveAt(this.blockType.Count - 1);
        }

        closingBraceContext = this.currentToken.Clone();
        // update the block context
        codeBlock.context.UpdateWith(this.currentToken);
        GetNextToken();
        return codeBlock;
      }

      //---------------------------------------------------------------------------------------
      // ParseVariableStatement
      //
      //  VariableStatement :
      //    ('var' | 'const') VariableDeclarationList
      //
      //  VariableDeclarationList :
      //    VariableDeclaration |
      //    VariableDeclaration ',' VariableDeclarationList
      //
      //  VariableDeclaration :
      //    Identifier Type Initializer
      //
      //  Type :
      //    <empty> |
      //    ':' TypeExpression
      //
      //  Initializer :
      //    <empty> |
      //    '=' AssignmentExpression
      //---------------------------------------------------------------------------------------
      private AST ParseVariableStatement(FieldAttributes visibility, CustomAttributeList customAttributes, JSToken kind){
        Block varList = new Block(this.currentToken.Clone());
        bool single = true;
        AST vdecl = null;

        for (;;){
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_EndOfLineToken);
          try{
            vdecl = ParseIdentifierInitializer(JSToken.None, visibility, customAttributes, kind);
          }catch(RecoveryTokenException exc){
            // an exception is passing by, possibly bringing some info, save the info if any
            if (exc._partiallyComputedNode != null){
              if (!single){
                varList.Append(exc._partiallyComputedNode);
                varList.context.UpdateWith(exc._partiallyComputedNode.context);
                exc._partiallyComputedNode = varList;
              }
            }
            if (IndexOfToken(NoSkipTokenSet.s_EndOfLineToken, exc) == -1)
              throw exc;
            else{
              if (single)
                vdecl = exc._partiallyComputedNode;
            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_EndOfLineToken);
          }

          if (JSToken.Semicolon == this.currentToken.token || JSToken.RightCurly == this.currentToken.token){
            if (JSToken.Semicolon == this.currentToken.token){
              vdecl.context.UpdateWith(this.currentToken);
              GetNextToken();
            }
            break;
          }else if (JSToken.Comma == this.currentToken.token){
            single = false;
            varList.Append(vdecl);
            continue;
          }else if (this.scanner.GotEndOfLine()){
            break;
          }else{
            // assume the variable statement was terminated and move on
            ReportError(JSError.NoSemicolon, true);
            break;
          }
        }

        if (single)
          return vdecl;
        else{
          varList.Append(vdecl);
          varList.context.UpdateWith(vdecl.context);
          return varList;
        }
      }

      //---------------------------------------------------------------------------------------
      // ParseIdentifierInitializer
      //
      //  Does the real work of parsing a single variable declaration.
      //  inToken is JSToken.In whenever the potential expression that initialize a variable
      //  cannot contain an 'in', as in the for statement. inToken is JSToken.None otherwise
      //---------------------------------------------------------------------------------------
      private AST ParseIdentifierInitializer(JSToken inToken,
                                                              FieldAttributes visibility,
                                                              CustomAttributeList customAttributes,
                                                              JSToken kind){
        Lookup id = null;
        TypeExpression typeExpr = null;
        AST assignmentExpr = null;
        RecoveryTokenException except = null;

        GetNextToken();
        if (JSToken.Identifier != this.currentToken.token){
          String identifier = JSKeyword.CanBeIdentifier(this.currentToken.token);
          if (null != identifier){
            ForceReportInfo(JSError.KeywordUsedAsIdentifier);
            id = new Lookup(identifier, this.currentToken.Clone());
          }else{
            // make up an identifier and keep going; life goes on...
            ReportError(JSError.NoIdentifier);
            id = new Lookup("#_Missing Identifier_#" + s_cDummyName++, CurrentPositionContext());
          }
        }else
          id = new Lookup(this.scanner.GetIdentifier(), this.currentToken.Clone());
        GetNextToken();
        Context context = id.context.Clone();

        this.noSkipTokenSet.Add(NoSkipTokenSet.s_VariableDeclNoSkipTokenSet);
        try{
          if (JSToken.Colon == this.currentToken.token){
            try{
              typeExpr = ParseTypeExpression();
            }catch(RecoveryTokenException exc){
              typeExpr = (TypeExpression)exc._partiallyComputedNode;
              throw exc;
            }finally{
              if (null != typeExpr)
                context.UpdateWith(typeExpr.context);
            }
          }
              
          if (JSToken.Assign == this.currentToken.token || JSToken.Equal == this.currentToken.token){
            if (JSToken.Equal == this.currentToken.token)
              ReportError(JSError.NoEqual, true);
            GetNextToken();
            try{
              assignmentExpr = ParseExpression(true, inToken);
            }catch(RecoveryTokenException exc){
              assignmentExpr = exc._partiallyComputedNode;
              throw exc;
            }finally{
              if (null != assignmentExpr)
                context.UpdateWith(assignmentExpr.context);
            }
          }
        }catch(RecoveryTokenException exc){
          // If the exception is in the vardecl no-skip set then we successfully
          // recovered to the end of the declaration and can just return
          // normally.  Otherwise we re-throw after constructing the partial result.  
          if (IndexOfToken(NoSkipTokenSet.s_VariableDeclNoSkipTokenSet, exc) == -1)
            except = exc;
        }finally{
          this.noSkipTokenSet.Remove(NoSkipTokenSet.s_VariableDeclNoSkipTokenSet);
        }

        AST result = null;
        if (JSToken.Var == kind)
          result = new VariableDeclaration(context, id, typeExpr, assignmentExpr, visibility, customAttributes);
        else{
          if (assignmentExpr == null)
            ForceReportInfo(JSError.NoEqual);
          result = new Constant(context, id, typeExpr, assignmentExpr, visibility, customAttributes);
        }
        if (customAttributes != null)
          customAttributes.SetTarget(result);
        if (null != except){
          except._partiallyComputedNode = result;
          throw except;
        }
        return result;
      }

      //---------------------------------------------------------------------------------------
      // ParseQualifiedIdentifier
      //
      //  QualifiedIdentifier :
      //    'double' | 'float' | 'int' | 'long' | Scope Identifier
      //
      //  Scope
      //    <empty> |
      //    Identifier '.' Scope
      //
      // The argument error is passed by the caller and it is used in error situation
      // to provide better error information. The caller knows in which context this
      // qualified identifier is parsed (i.e. var x : QualifiedIdentifier vs.
      // import QualifiedIdentifier)
      // On error condition this method may return null. Regardless of its return value, though,
      // this method will always be pointing to the next token and no error token will be set.
      //---------------------------------------------------------------------------------------
      private AST ParseQualifiedIdentifier(JSError error){
        GetNextToken();
        AST qualid = null;
        string identifier = null;
        Context idContext = this.currentToken.Clone();
        if (JSToken.Identifier != this.currentToken.token){
          identifier = JSKeyword.CanBeIdentifier(this.currentToken.token);
          if (null != identifier){
            switch (this.currentToken.token){
              case JSToken.Boolean :
              case JSToken.Byte :
              case JSToken.Char :
              case JSToken.Double :
              case JSToken.Float :
              case JSToken.Int :
              case JSToken.Long :
              case JSToken.Short :
              case JSToken.Void :
                break;
              default:
                ForceReportInfo(JSError.KeywordUsedAsIdentifier);
                break;
            }
            qualid = new Lookup(identifier, idContext);
          }else{
            ReportError(error, true);
            SkipTokensAndThrow(); // this will always throw
          }
        }else{
          qualid = new Lookup(this.scanner.GetIdentifier(), idContext);
        }
        GetNextToken();
        if (JSToken.AccessField == this.currentToken.token)
          qualid = ParseScopeSequence(qualid, error);

        return qualid;
      }

      //---------------------------------------------------------------------------------------
      // ParseScopeSequence
      //
      //  Scope
      //    <empty> |
      //    Identifier '.' Scope
      //
      // The argument error is passed by the caller and it is used in error situation
      // to provide better error information. The caller knows in which context this
      // qualified identifier is parsed (i.e. var x : QualifiedIdentifier vs.
      // import QualifiedIdentifier)
      // This function is called after a 'JSToken.Identifier JSToken.AccessField' sequence
      // has been processed and the whole scope production has to be returned
      //---------------------------------------------------------------------------------------
      private AST ParseScopeSequence(AST qualid, JSError error){
        ConstantWrapper id = null;
        string identifier = null;
        do{
          GetNextToken(); // move after '.'
          if (JSToken.Identifier != this.currentToken.token){
            identifier = JSKeyword.CanBeIdentifier(this.currentToken.token);
            if (null != identifier){
              ForceReportInfo(JSError.KeywordUsedAsIdentifier);
              id = new ConstantWrapper(identifier, this.currentToken.Clone());
            }else{
              ReportError(error, true);
              SkipTokensAndThrow(qualid); // this will always throw
            }
          }else
            id = new ConstantWrapper(this.scanner.GetIdentifier(), this.currentToken.Clone());
          qualid = new Member(qualid.context.CombineWith(this.currentToken), qualid, id);
          GetNextToken();
        } while (JSToken.AccessField == this.currentToken.token);

        return qualid;
      }

      //---------------------------------------------------------------------------------------
      // ParseTypeExpression
      //
      //  TypeExpression :
      //    QualifiedIdentifier |
      //    TypeExpression <no line break> '[' ']'
      //
      //--------------------------------------------------------------------------------------
      private TypeExpression ParseTypeExpression(){
        AST typeId = null;
        try{
          typeId = ParseQualifiedIdentifier(JSError.NeedType);
        }catch(RecoveryTokenException exc){
          if (exc._partiallyComputedNode != null){
            exc._partiallyComputedNode = new TypeExpression(exc._partiallyComputedNode);
          }
          throw exc;
        }
        TypeExpression type = new TypeExpression(typeId);
        if (type != null){
          while (!this.scanner.GotEndOfLine() && JSToken.LeftBracket == this.currentToken.token){
            GetNextToken();
            int rank = 1;
            while (JSToken.Comma == this.currentToken.token){
              GetNextToken(); rank++;
            }
            if (JSToken.RightBracket != this.currentToken.token)
              ReportError(JSError.NoRightBracket);
            GetNextToken();
            if (type.isArray)
              type = new TypeExpression(type);
            type.isArray = true;
            type.rank = rank;
          }
        }
        return type;
      }

      //---------------------------------------------------------------------------------------
      // ParseIfStatement
      //
      //  IfStatement :
      //    'if' '(' Expression ')' Statement ElseStatement
      //
      //  ElseStatement :
      //    <empty> |
      //    'else' Statement
      //---------------------------------------------------------------------------------------
      private If ParseIfStatement(){
        Context ifCtx = this.currentToken.Clone();
        AST condition = null;
        AST trueBranch = null;
        AST falseBranch = null;

        this.blockType.Add(BlockType.Block);
        try{
          // parse condition
          GetNextToken();
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
          try{
            if (JSToken.LeftParen != this.currentToken.token)
              ReportError(JSError.NoLeftParen);
            GetNextToken();
            condition = ParseExpression();

            // parse statements
            if (JSToken.RightParen != this.currentToken.token){
              ifCtx.UpdateWith(condition.context);
              ReportError(JSError.NoRightParen);
            }else
              ifCtx.UpdateWith(this.currentToken);

            GetNextToken();
          }catch(RecoveryTokenException exc){
            // make up an if condition
            if (exc._partiallyComputedNode != null)
              condition = exc._partiallyComputedNode;
            else
              condition = new ConstantWrapper(true, CurrentPositionContext());

            if (IndexOfToken(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet, exc) == -1){
              exc._partiallyComputedNode = null; // really not much to pass up
              // the if condition was so bogus we do not have a chance to make an If node, give up
              throw exc;
            }else{
              if (exc._token == JSToken.RightParen)
                GetNextToken();
            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
          }

          if (condition is Assign)
            condition.context.HandleError(JSError.SuspectAssignment);

          if (JSToken.Semicolon == this.currentToken.token)
            ForceReportInfo(JSError.SuspectSemicolon);
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_IfBodyNoSkipTokenSet);
          try{
            trueBranch = ParseStatement();
          }catch(RecoveryTokenException exc){
            // make up a block for the if part
            if (exc._partiallyComputedNode != null)
              trueBranch = exc._partiallyComputedNode;
            else
              trueBranch = new Block(CurrentPositionContext());
            if (IndexOfToken(NoSkipTokenSet.s_IfBodyNoSkipTokenSet, exc) == -1){
              // we have to pass the exception to someone else, make as much as you can from the if
              exc._partiallyComputedNode = new If(ifCtx, condition, trueBranch, falseBranch);
              throw exc;
            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_IfBodyNoSkipTokenSet);
          }

          // parse else, if any
          if (JSToken.Else == this.currentToken.token){
            GetNextToken();
            if (JSToken.Semicolon == this.currentToken.token)
              ForceReportInfo(JSError.SuspectSemicolon);
            try{
              falseBranch = ParseStatement();
            }catch(RecoveryTokenException exc){
              // make up a block for the else part
              if (exc._partiallyComputedNode != null)
                falseBranch = exc._partiallyComputedNode;
              else
                falseBranch = new Block(CurrentPositionContext());
              exc._partiallyComputedNode = new If(ifCtx, condition, trueBranch, falseBranch);
              throw exc;
            }
          }
        }finally{
          this.blockType.RemoveAt(this.blockType.Count - 1);
        }

        return new If(ifCtx, condition, trueBranch, falseBranch);
      }

      //---------------------------------------------------------------------------------------
      // ParseForStatement
      //
      //  ForStatement :
      //    'for' '(' OptionalExpressionNoIn ';' OptionalExpression ';' OptionalExpression ')'
      //    'for' '(' 'var' VariableDeclarationListNoIn ';' OptionalExpression ';' OptionalExpression ')'
      //    'for' '(' LeftHandSideExpression 'in' Expression')'
      //    'for' '(' 'var' Identifier OptionalInitializerNoIn 'in' Expression')'
      //
      //  OptionalExpressionNoIn :
      //    <empty> |
      //    ExpressionNoIn // same as Expression but does not process 'in' as an operator
      //
      //  OptionalInitializerNoIn :
      //    <empty> |
      //    InitializerNoIn // same as initializer but does not process 'in' as an operator
      //---------------------------------------------------------------------------------------
      private AST ParseForStatement(){
        this.blockType.Add(BlockType.Loop);
        AST forNode = null;
        try{
          Context forCtx = this.currentToken.Clone();
          GetNextToken();
          if (JSToken.LeftParen != this.currentToken.token)
            ReportError(JSError.NoLeftParen);
          GetNextToken();
          bool isForIn = false, recoveryInForIn = false;
          AST lhs = null, initializer = null, condOrColl = null, increment = null;

          try{
            if (JSToken.Var == this.currentToken.token){
              isForIn = true;
              initializer = ParseIdentifierInitializer(JSToken.In, (FieldAttributes)0, null, JSToken.Var);

              // a list of variable initializers is allowed only in a for(;;)
              AST var = null;
              while (JSToken.Comma == this.currentToken.token){
                isForIn = false;
                var = ParseIdentifierInitializer(JSToken.In, (FieldAttributes)0, null, JSToken.Var);
                initializer = new Comma(initializer.context.CombineWith(var.context), initializer, var);
              }

              // if it could still be a for..in, now it's time to get the 'in'
              if (isForIn){
                if (JSToken.In == this.currentToken.token){
                  GetNextToken();
                  condOrColl = ParseExpression();
                }else
                  isForIn = false;
              }
            }else{
              if (JSToken.Semicolon != this.currentToken.token){
                bool isLHS;
                initializer = ParseUnaryExpression(out isLHS, false);
                if (isLHS && JSToken.In == this.currentToken.token){
                  isForIn = true;
                  lhs = initializer;
                  initializer = null;
                  GetNextToken();
                  this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
                  try{
                    condOrColl = ParseExpression();
                  }catch(RecoveryTokenException exc){
                    if (IndexOfToken(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet, exc) == -1){
                      exc._partiallyComputedNode = null;
                      throw exc;
                    }else{
                      if (exc._partiallyComputedNode == null)
                        condOrColl = new ConstantWrapper(true, CurrentPositionContext()); // what could we put here?
                      else
                        condOrColl = exc._partiallyComputedNode;
                    }
                    if (exc._token == JSToken.RightParen){
                      GetNextToken();
                      recoveryInForIn = true;
                    }
                  }finally{
                    this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
                  }
                }else
                  initializer = ParseExpression(initializer, false, isLHS, JSToken.In);
              }else
                initializer = new EmptyLiteral(CurrentPositionContext());
          }
          }catch(RecoveryTokenException exc){
            // error is too early abort for
            exc._partiallyComputedNode = null;
            throw exc;
          }

          // at this point we know whether or not is a for..in
          if (isForIn){
            if (!recoveryInForIn){
              if (JSToken.RightParen != this.currentToken.token)
                ReportError(JSError.NoRightParen);
              forCtx.UpdateWith(this.currentToken);
              GetNextToken();
            }
            AST body = null;
            try{
              body = ParseStatement();
            }catch(RecoveryTokenException exc){
              if (exc._partiallyComputedNode == null)
                body = new Block(CurrentPositionContext());
              else
                body = exc._partiallyComputedNode;
              exc._partiallyComputedNode = new ForIn(forCtx, lhs, initializer, condOrColl, body);
              throw exc;
            }
            forNode = new ForIn(forCtx, lhs, initializer, condOrColl, body);
          }else{
            this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
            try{
              if (JSToken.Semicolon != this.currentToken.token){
                ReportError(JSError.NoSemicolon);
                if (JSToken.Colon == this.currentToken.token){
                  this.noSkipTokenSet.Add(NoSkipTokenSet.s_VariableDeclNoSkipTokenSet);
                  try{
                    SkipTokensAndThrow();
                  }catch(RecoveryTokenException exc){
                    if (JSToken.Semicolon == this.currentToken.token)
                      this.errorToken = null;
                    else
                      throw exc;
                  }finally{
                    this.noSkipTokenSet.Remove(NoSkipTokenSet.s_VariableDeclNoSkipTokenSet);
                  }
                }
              }
              GetNextToken();
              if (JSToken.Semicolon != this.currentToken.token){
                condOrColl = ParseExpression();
                if (JSToken.Semicolon != this.currentToken.token)
                  ReportError(JSError.NoSemicolon);
              }else
                condOrColl = new ConstantWrapper(true, CurrentPositionContext());
              GetNextToken();
              if (JSToken.RightParen != this.currentToken.token)
                increment = ParseExpression();
              else
                increment = new EmptyLiteral(CurrentPositionContext());
              if (JSToken.RightParen != this.currentToken.token)
                ReportError(JSError.NoRightParen);
              forCtx.UpdateWith(this.currentToken);
              GetNextToken();
            }catch(RecoveryTokenException exc){
              if (IndexOfToken(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet, exc) == -1){
                exc._partiallyComputedNode = null;
                throw exc;
              }else{
                // discard any partial info, just genrate empty condition and increment and keep going
                exc._partiallyComputedNode = null;
                if (condOrColl == null)
                  condOrColl = new ConstantWrapper(true, CurrentPositionContext());
                if (increment == null)
                  increment = new EmptyLiteral(CurrentPositionContext());
              }
              if (exc._token == JSToken.RightParen){
                GetNextToken();
              }
            }finally{
              this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
            }
            AST body = null;
            try{
              body = ParseStatement();
            }catch(RecoveryTokenException exc){
              if (exc._partiallyComputedNode == null)
                body = new Block(CurrentPositionContext());
              else
                body = exc._partiallyComputedNode;
              exc._partiallyComputedNode = new For(forCtx, initializer, condOrColl, increment, body);
              throw exc;
            }
            forNode = new For(forCtx, initializer, condOrColl, increment, body);
          }
        }finally{
          this.blockType.RemoveAt(this.blockType.Count - 1);
        }

        return forNode;
      }

      //---------------------------------------------------------------------------------------
      // ParseDoStatement
      //
      //  DoStatement:
      //    'do' Statement 'while' '(' Expression ')'
      //---------------------------------------------------------------------------------------
      private DoWhile ParseDoStatement(){
        Context doCtx = null; // this.currentToken.Clone();
        AST body = null;
        AST condition = null;
        this.blockType.Add(BlockType.Loop);
        try{
          GetNextToken();
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_DoWhileBodyNoSkipTokenSet);
          try{
            body = ParseStatement();
          }catch(RecoveryTokenException exc){
            // make up a block for the do while
            if (exc._partiallyComputedNode != null)
              body = exc._partiallyComputedNode;
            else
              body = new Block(CurrentPositionContext());
            if (IndexOfToken(NoSkipTokenSet.s_DoWhileBodyNoSkipTokenSet, exc) == -1){
              // we have to pass the exception to someone else, make as much as you can from the 'do while'
              exc._partiallyComputedNode = new DoWhile(CurrentPositionContext(),
                                                        body,
                                                        new ConstantWrapper(false, CurrentPositionContext()));
              throw exc;
            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_DoWhileBodyNoSkipTokenSet);
          }
          if (JSToken.While != this.currentToken.token){
            ReportError(JSError.NoWhile);
          }
          doCtx = this.currentToken.Clone();
          GetNextToken();
          if (JSToken.LeftParen != this.currentToken.token){
            ReportError(JSError.NoLeftParen);
          }
          GetNextToken();
          // catch here so the body of the do_while is not thrown away
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
          try{
            condition = ParseExpression();
            if (JSToken.RightParen != this.currentToken.token){
              ReportError(JSError.NoRightParen);
              doCtx.UpdateWith(condition.context);
            }else
              doCtx.UpdateWith(this.currentToken);
            GetNextToken();
          }catch(RecoveryTokenException exc){
            // make up a condition
            if (exc._partiallyComputedNode != null)
              condition = exc._partiallyComputedNode;
            else
              condition = new ConstantWrapper(false, CurrentPositionContext());

            if (IndexOfToken(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet, exc) == -1){
              exc._partiallyComputedNode = new DoWhile(doCtx, body, condition);
              throw exc;
            }else{
              if (JSToken.RightParen == this.currentToken.token)
                GetNextToken();
            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
          }
          if (JSToken.Semicolon == this.currentToken.token){
            // JScript 5 allowed statements like
            //   do{print(++x)}while(x<10) print(0)
            // even though that does not strictly follow the automatic semicolon insertion
            // rules for the required semi after the while().  For backwards compatibility
            // we should continue to support this.
            doCtx.UpdateWith(this.currentToken);
            GetNextToken();
          }
          
        }finally{
          this.blockType.RemoveAt(this.blockType.Count - 1);
        }

        return new DoWhile(doCtx, body, condition);
      }

      //---------------------------------------------------------------------------------------
      // ParseWhileStatement
      //
      //  WhileStatement :
      //    'while' '(' Expression ')' Statement
      //---------------------------------------------------------------------------------------
      private While ParseWhileStatement(){
        Context whileCtx = this.currentToken.Clone();
        AST condition = null;
        AST body = null;
        this.blockType.Add(BlockType.Loop);
        try{
        GetNextToken();
        if (JSToken.LeftParen != this.currentToken.token){
          ReportError(JSError.NoLeftParen);
        }
        GetNextToken();
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
          try{
          condition = ParseExpression();
          if (JSToken.RightParen != this.currentToken.token){
            ReportError(JSError.NoRightParen);
            whileCtx.UpdateWith(condition.context);
          }else
            whileCtx.UpdateWith(this.currentToken);

          GetNextToken();
          }catch(RecoveryTokenException exc){
            if (IndexOfToken(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet, exc) == -1){
              // abort the while there is really no much to do here
              exc._partiallyComputedNode = null;
              throw exc;
            }else{
              // make up a condition
              if (exc._partiallyComputedNode != null)
                condition = exc._partiallyComputedNode;
              else
                condition = new ConstantWrapper(false, CurrentPositionContext());

              if (JSToken.RightParen == this.currentToken.token)
                GetNextToken();
            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
          }

          try{
            body = ParseStatement();
          }catch(RecoveryTokenException exc){
            if (exc._partiallyComputedNode != null)
              body = exc._partiallyComputedNode;
            else
              body = new Block(CurrentPositionContext());

            exc._partiallyComputedNode = new While(whileCtx, condition, body);
            throw exc;
          }

        }finally{
          this.blockType.RemoveAt(this.blockType.Count - 1);
        }

      return new While(whileCtx, condition, body);
      }

      //---------------------------------------------------------------------------------------
      // ParseContinueStatement
      //
      //  ContinueStatement :
      //    'continue' OptionalLabel
      //
      //  OptionalLabel :
      //    <empty> |
      //    Identifier
      //
      // This function may return a null AST under error condition. The caller should handle
      // that case.
      // Regardless of error conditions, on exit the parser points to the first token after
      // the continue statement
      //---------------------------------------------------------------------------------------
      private Continue ParseContinueStatement(){
        Context context = this.currentToken.Clone();
        int blocks = 0;
        GetNextToken();
        string label = null;
        if (!this.scanner.GotEndOfLine() && (JSToken.Identifier == this.currentToken.token || (label = JSKeyword.CanBeIdentifier(this.currentToken.token)) != null)){
          context.UpdateWith(this.currentToken);
          // get the label block
          if (null != label)
            ForceReportInfo(JSError.KeywordUsedAsIdentifier);
          else
            label = this.scanner.GetIdentifier();
          Object value = this.labelTable[label];
          if (null == value){
            // the label does not exist. Continue as if it there was no continue at all
            ReportError(JSError.NoLabel, true);
            GetNextToken();
            return null; // ignore it
          }else{
            blocks = (int)value;
            if ((BlockType)this.blockType[blocks] != BlockType.Loop){
              ReportError(JSError.BadContinue, context.Clone(), true);
            }
            GetNextToken();
          }
        }else{
          blocks = this.blockType.Count - 1;
          while (blocks >= 0 && (BlockType)this.blockType[blocks] != BlockType.Loop) blocks--;
          if (blocks < 0){
            // the continue is malformed. Continue as if there was no continue at all
            ReportError(JSError.BadContinue, context, true);
            return null;
          }
        }

        if (JSToken.Semicolon == this.currentToken.token){
          context.UpdateWith(this.currentToken);
          GetNextToken();
        }else if (JSToken.RightCurly != this.currentToken.token && !this.scanner.GotEndOfLine()){
          ReportError(JSError.NoSemicolon, true);
        }

        // must ignore the Finally block
        int finallyNum = 0;
        for (int i = blocks, n = this.blockType.Count; i < n; i++)
          if ((BlockType)this.blockType[i] == BlockType.Finally){
            blocks++;
            finallyNum++;
          }
        if (finallyNum > this.finallyEscaped)
          this.finallyEscaped = finallyNum;

        return new Continue(context, this.blockType.Count - blocks, finallyNum > 0);
      }

      //---------------------------------------------------------------------------------------
      // ParseBreakStatement
      //
      //  BreakStatement :
      //    'break' OptionalLabel
      //
      // This function may return a null AST under error condition. The caller should handle
      // that case.
      // Regardless of error conditions, on exit the parser points to the first token after
      // the break statement.
      //---------------------------------------------------------------------------------------
      private Break ParseBreakStatement(){
        Context context = this.currentToken.Clone();
        int blocks = 0;
        GetNextToken();
        string label = null;
        if (!this.scanner.GotEndOfLine() && (JSToken.Identifier == this.currentToken.token || (label = JSKeyword.CanBeIdentifier(this.currentToken.token)) != null)){
          context.UpdateWith(this.currentToken);
          // get the label block
          if (null != label)
            ForceReportInfo(JSError.KeywordUsedAsIdentifier);
          else
            label = this.scanner.GetIdentifier();
          Object value = this.labelTable[label];
          if (null == value){
            // as if it was a non label case
            ReportError(JSError.NoLabel, true);
            GetNextToken();
            return null; // ignore it
          }else{
            blocks = (int)value - 1; // the outer block
            Debug.Assert((BlockType)this.blockType[blocks] != BlockType.Finally);
            GetNextToken();
          }
        }else{
          blocks = this.blockType.Count - 1;
          // search for an enclosing loop, if there is no loop it is an error
          while (((BlockType)this.blockType[blocks] == BlockType.Block || (BlockType)this.blockType[blocks] == BlockType.Finally) && --blocks >= 0);
          --blocks;
          if (blocks < 0){
            ReportError(JSError.BadBreak, context, true);
            return null;
          }
        }

        if (JSToken.Semicolon == this.currentToken.token){
          context.UpdateWith(this.currentToken);
          GetNextToken();
        }else if (JSToken.RightCurly != this.currentToken.token && !this.scanner.GotEndOfLine()){
          ReportError(JSError.NoSemicolon, true);
        }

        // must ignore the Finally block
        int finallyNum = 0;
        for (int i = blocks, n = this.blockType.Count; i < n; i++)
          if ((BlockType)this.blockType[i] == BlockType.Finally){
            blocks++;
            finallyNum++;
          }
        if (finallyNum > this.finallyEscaped)
          this.finallyEscaped = finallyNum;

        return new Break(context, this.blockType.Count - blocks - 1, finallyNum > 0);
      }

      private bool CheckForReturnFromFinally(){
        int finallyNum = 0;
        for (int i = this.blockType.Count - 1; i >= 0; i--)
          if ((BlockType)this.blockType[i] == BlockType.Finally)
            finallyNum++;

        if (finallyNum > this.finallyEscaped)
          this.finallyEscaped = finallyNum;

        return finallyNum > 0;
      }

      //---------------------------------------------------------------------------------------
      // ParseReturnStatement
      //
      //  ReturnStatement :
      //    'return' Expression
      //
      // This function may return a null AST under error condition. The caller should handle
      // that case.
      // Regardless of error conditions, on exit the parser points to the first token after
      // the return statement.
      //---------------------------------------------------------------------------------------
      private Return ParseReturnStatement(){
        Context retCtx = this.currentToken.Clone();
        if (Globals.ScopeStack.Peek() is FunctionScope){
          AST expr = null;
          GetNextToken();
          if (!this.scanner.GotEndOfLine()){
            if (JSToken.Semicolon != this.currentToken.token && JSToken.RightCurly != this.currentToken.token){
              this.noSkipTokenSet.Add(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet);
              try{
                expr = ParseExpression();
              }catch(RecoveryTokenException exc){
                expr = exc._partiallyComputedNode;
                if (IndexOfToken(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet, exc) == -1){
                  if (expr != null)
                    retCtx.UpdateWith(expr.context);
                  exc._partiallyComputedNode = new Return(retCtx, expr, CheckForReturnFromFinally());
                  throw exc;
                }
              }finally{
                this.noSkipTokenSet.Remove(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet);
              }
              if (JSToken.Semicolon != this.currentToken.token
                  && JSToken.RightCurly != this.currentToken.token
                  && !this.scanner.GotEndOfLine()){
                ReportError(JSError.NoSemicolon, true);
              }
            }
            if (JSToken.Semicolon == this.currentToken.token){
              retCtx.UpdateWith(this.currentToken);
              GetNextToken();
            }else if (expr != null)
              retCtx.UpdateWith(expr.context);
          }
          return new Return(retCtx, expr, CheckForReturnFromFinally());
        }else{
          // the return is not inside a function, report an error and skip to the next token
          ReportError(JSError.BadReturn, retCtx, true);
          GetNextToken();
          return null;
        }
      }

      //---------------------------------------------------------------------------------------
      // ParseImportStatement
      //
      //  ImportStatement :
      //    'import' QualifiedIdentifier
      //---------------------------------------------------------------------------------------
      private Import ParseImportStatement(){
        Context context = this.currentToken.Clone();
        AST name = null;
        try{
          name = ParseQualifiedIdentifier(JSError.PackageExpected);
        }catch(RecoveryTokenException exc){
          // an exception is passing by, mae an Import if possible and rethrow
          if (exc._partiallyComputedNode != null){
            exc._partiallyComputedNode = new Import(context, exc._partiallyComputedNode);
          }
        }
        if (this.currentToken.token != JSToken.Semicolon && !this.scanner.GotEndOfLine())
          ReportError(JSError.NoSemicolon, this.currentToken.Clone());
        return new Import(context, name);
      }

      //---------------------------------------------------------------------------------------
      // ParseWithStatement
      //
      //  WithStatement :
      //    'with' '(' Expression ')' Statement
      //---------------------------------------------------------------------------------------
      private With ParseWithStatement(){
        Context withCtx = this.currentToken.Clone();
        AST obj = null;
        AST block = null;
        this.blockType.Add(BlockType.Block);
        try{
          GetNextToken();
          if (JSToken.LeftParen != this.currentToken.token)
            ReportError(JSError.NoLeftParen);
          GetNextToken();
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
          try{
            obj = ParseExpression();
            if (JSToken.RightParen != this.currentToken.token){
              withCtx.UpdateWith(obj.context);
              ReportError(JSError.NoRightParen);
            }else
              withCtx.UpdateWith(this.currentToken);
            GetNextToken();
          }catch(RecoveryTokenException exc){
            if (IndexOfToken(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet, exc) == -1){
              // give up
              exc._partiallyComputedNode = null;
              throw exc;
            }else{
              if (exc._partiallyComputedNode == null)
                obj = new ConstantWrapper(true, CurrentPositionContext());
              else
                obj = exc._partiallyComputedNode;
              withCtx.UpdateWith(obj.context);

              if (exc._token == JSToken.RightParen)
                GetNextToken();
            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
          }

          try{
            block = ParseStatement();
          }catch(RecoveryTokenException exc){
            if (exc._partiallyComputedNode == null)
              block = new Block(CurrentPositionContext());
            else
              block = exc._partiallyComputedNode;
            exc._partiallyComputedNode = new With(withCtx, obj, block);
          }
        }finally{
          this.blockType.RemoveAt(this.blockType.Count - 1);
        }

        return new With(withCtx, obj, block);
      }

      //---------------------------------------------------------------------------------------
      // ParseSwitchStatement
      //
      //  SwitchStatement :
      //    'switch' '(' Expression ')' '{' CaseBlock '}'
      //
      //  CaseBlock :
      //    CaseList DefaultCaseClause CaseList
      //
      //  CaseList :
      //    <empty> |
      //    CaseClause CaseList
      //
      //  CaseClause :
      //    'case' Expression ':' OptionalStatements
      //
      //  DefaultCaseClause :
      //    <empty> |
      //    'default' ':' OptionalStatements
      //---------------------------------------------------------------------------------------
      private AST ParseSwitchStatement(){
        Context switchCtx = this.currentToken.Clone();
        AST expr = null;
        ASTList cases = null;
        this.blockType.Add(BlockType.Switch);
        try{
          // read switch(expr)
        GetNextToken();
        if (JSToken.LeftParen != this.currentToken.token)
            ReportError(JSError.NoLeftParen);
        GetNextToken();
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_SwitchNoSkipTokenSet);
          try{
            expr = ParseExpression();

          if (JSToken.RightParen != this.currentToken.token){
              ReportError(JSError.NoRightParen);
            }

          GetNextToken();
          if (JSToken.LeftCurly != this.currentToken.token){
              ReportError(JSError.NoLeftCurly);
            }
            GetNextToken();

          }catch(RecoveryTokenException exc){
            if (IndexOfToken(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet, exc) == -1
                  && IndexOfToken(NoSkipTokenSet.s_SwitchNoSkipTokenSet, exc) == -1 ){
              // give up
              exc._partiallyComputedNode = null;
              throw exc;
            }else{
              if (exc._partiallyComputedNode == null)
                expr = new ConstantWrapper(true, CurrentPositionContext());
              else
                expr = exc._partiallyComputedNode;

              if (IndexOfToken(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet, exc) != -1){
                if (exc._token == JSToken.RightParen)
                  GetNextToken();

                if (JSToken.LeftCurly != this.currentToken.token){
                  ReportError(JSError.NoLeftCurly);
                }
                GetNextToken();
              }

            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_SwitchNoSkipTokenSet);
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
          }

          // parse the switch body
          cases = new ASTList(this.currentToken.Clone());
        bool defaultStatement = false;
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockNoSkipTokenSet);
          try{
            while (JSToken.RightCurly != this.currentToken.token){
              SwitchCase caseClause = null;
              AST caseValue = null;
              Context caseCtx = this.currentToken.Clone();
              this.noSkipTokenSet.Add(NoSkipTokenSet.s_CaseNoSkipTokenSet);
              try{
                if (JSToken.Case == this.currentToken.token){
                  // get the case
                  GetNextToken();
                  caseValue = ParseExpression();
                }else if (JSToken.Default == this.currentToken.token){
                  // get the default
                  if (defaultStatement)
                    // we report an error but we still accept the default
                    ReportError(JSError.DupDefault, true);
                  else
                    defaultStatement = true;
                  GetNextToken();
                }else{
                  // This is an error, there is no case or default. Assume a default was missing and keep going
                  defaultStatement = true;
                  ReportError(JSError.BadSwitch);
                }
                if (JSToken.Colon != this.currentToken.token)
                  ReportError(JSError.NoColon);

                // read the statements inside the case or default
                GetNextToken();
              }catch(RecoveryTokenException exc){
                // right now we can only get here for the 'case' statement
                if (IndexOfToken(NoSkipTokenSet.s_CaseNoSkipTokenSet, exc) == -1){
                  // ignore the current case or default
                  exc._partiallyComputedNode = null;
                  throw exc;
                }else{
                  caseValue = exc._partiallyComputedNode;

                  if (exc._token == JSToken.Colon)
                    GetNextToken();
                }
              }finally{
                this.noSkipTokenSet.Remove(NoSkipTokenSet.s_CaseNoSkipTokenSet);
              }

              this.blockType.Add(BlockType.Block);
              try{
                Block statements = new Block(this.currentToken.Clone());
                this.noSkipTokenSet.Add(NoSkipTokenSet.s_SwitchNoSkipTokenSet);
                this.noSkipTokenSet.Add(NoSkipTokenSet.s_StartStatementNoSkipTokenSet);
                try{
                  while (JSToken.RightCurly != this.currentToken.token && JSToken.Case != this.currentToken.token && JSToken.Default != this.currentToken.token){
                    try{
                      statements.Append(ParseStatement());
                    }catch(RecoveryTokenException exc){
                      if (exc._partiallyComputedNode != null){
                        statements.Append(exc._partiallyComputedNode);
                        exc._partiallyComputedNode = null;
                      }
                      if (IndexOfToken(NoSkipTokenSet.s_StartStatementNoSkipTokenSet, exc) == -1)
                        throw exc;
                    }
                  }
                }catch(RecoveryTokenException exc){
                  if (IndexOfToken(NoSkipTokenSet.s_SwitchNoSkipTokenSet, exc) == -1){
                    if (null == caseValue)
                      caseClause = new SwitchCase(caseCtx, statements);
                    else
                      caseClause = new SwitchCase(caseCtx, caseValue, statements);
                    cases.Append(caseClause);

                    throw exc;
                  }
                }finally{
                  this.noSkipTokenSet.Remove(NoSkipTokenSet.s_StartStatementNoSkipTokenSet);
                  this.noSkipTokenSet.Remove(NoSkipTokenSet.s_SwitchNoSkipTokenSet);
                }
                if (JSToken.RightCurly == this.currentToken.token)
                  statements.context.UpdateWith(this.currentToken);
                if (null == caseValue){
                  caseCtx.UpdateWith(statements.context);
                  caseClause = new SwitchCase(caseCtx, statements);
                }else{
                  caseCtx.UpdateWith(statements.context);
                  caseClause = new SwitchCase(caseCtx, caseValue, statements);
                }
                cases.Append(caseClause);
              }finally{
                this.blockType.RemoveAt(this.blockType.Count - 1);
              }
            }
          }catch(RecoveryTokenException exc){
            if (IndexOfToken(NoSkipTokenSet.s_BlockNoSkipTokenSet, exc) == -1){
              //save what you can a rethrow
              switchCtx.UpdateWith(CurrentPositionContext());
              exc._partiallyComputedNode = new Switch(switchCtx, expr, cases);
              throw exc;
            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockNoSkipTokenSet);
          }
          switchCtx.UpdateWith(this.currentToken);
          GetNextToken();
        }finally{
          this.blockType.RemoveAt(this.blockType.Count - 1);
        }

        return new Switch(switchCtx, expr, cases);
      }

      //---------------------------------------------------------------------------------------
      // ParseThrowStatement
      //
      //  ThrowStatement :
      //    throw |
      //    throw Expression
      //---------------------------------------------------------------------------------------
      private AST ParseThrowStatement(){
        Context throwCtx = this.currentToken.Clone();
        GetNextToken();
        AST operand = null;
        if (!this.scanner.GotEndOfLine()){
          if (JSToken.Semicolon != this.currentToken.token){
            this.noSkipTokenSet.Add(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet);
            try{
              operand = ParseExpression();
            }catch(RecoveryTokenException exc){
              operand = exc._partiallyComputedNode;
              if (IndexOfToken(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet, exc) == -1){
                if (operand != null)
                  exc._partiallyComputedNode = new Throw(throwCtx, exc._partiallyComputedNode);
                throw exc;
              }
            }finally{
              this.noSkipTokenSet.Remove(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet);
            }
          }
        }

        if (operand != null)
          throwCtx.UpdateWith(operand.context);
        return new Throw(throwCtx, operand);
      }

      //---------------------------------------------------------------------------------------
      // ParseTryStatement
      //
      //  TryStatement :
      //    'try' Block CatchList Finally
      //
      //  CatchList :
      //    <empty> |
      //    CatchList Catch
      //
      //  Catch :
      //    'catch' '(' Identifier Type ')' Block
      //
      //  Finally :
      //    <empty> |
      //    'finally' Block
      //---------------------------------------------------------------------------------------
      private AST ParseTryStatement(){
        Context tryCtx = this.currentToken.Clone();
        Context tryEndContext = null;
        AST body = null;
        AST id = null;
        AST handler = null;
        AST finally_block = null;
        RecoveryTokenException excInFinally = null;
        TypeExpression type = null;
        this.blockType.Add(BlockType.Block);
        try{
          bool catchOrFinally = false;
          bool foundCatchAll = false;
          GetNextToken();
          if (JSToken.LeftCurly != this.currentToken.token)
            ReportError(JSError.NoLeftCurly);
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_NoTrySkipTokenSet);
          try{
            body = ParseBlock(out tryEndContext);
          }catch(RecoveryTokenException exc){
            if (IndexOfToken(NoSkipTokenSet.s_NoTrySkipTokenSet, exc) == -1)
              // do nothing and just return the containing block, if any
              throw exc;
            else
              body = exc._partiallyComputedNode;
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_NoTrySkipTokenSet);
          }
          while (JSToken.Catch == this.currentToken.token){
            this.noSkipTokenSet.Add(NoSkipTokenSet.s_NoTrySkipTokenSet);
            try{
              if (handler != null){
                body = new Try(tryCtx, body, id, type, handler, null, false, tryEndContext);
                id = null;
                type = null;
                handler = null;
              }
              catchOrFinally = true;
              GetNextToken();
              if (JSToken.LeftParen != this.currentToken.token)
                ReportError(JSError.NoLeftParen);
              GetNextToken();
              if (JSToken.Identifier != this.currentToken.token){
                string identifier = JSKeyword.CanBeIdentifier(this.currentToken.token);
                if (null != identifier){
                  ForceReportInfo(JSError.KeywordUsedAsIdentifier);
                  id = new Lookup(identifier, this.currentToken.Clone());
                }else{
                  ReportError(JSError.NoIdentifier);
                  id = new Lookup("##Exc##" + s_cDummyName++, CurrentPositionContext());
                }
              }else
                id = new Lookup(this.scanner.GetIdentifier(), this.currentToken.Clone());
              GetNextToken();
              this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
              try{
                if (JSToken.Colon == this.currentToken.token)
                  type = ParseTypeExpression();
                else{
                  if (foundCatchAll) //no point in having another
                    ForceReportInfo(id.context, JSError.UnreachableCatch);
                  foundCatchAll = true;
                }
                if (JSToken.RightParen != this.currentToken.token)
                  ReportError(JSError.NoRightParen);
                GetNextToken();
              }catch(RecoveryTokenException exc){
                if (IndexOfToken(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet, exc) == -1){
                  exc._partiallyComputedNode = null;
                  // rethrow
                  throw exc;
                }else{
                  type = (TypeExpression)exc._partiallyComputedNode;
                  if (this.currentToken.token == JSToken.RightParen)
                    GetNextToken();
                }
              }finally{
                this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockConditionNoSkipTokenSet);
              }
              if (JSToken.LeftCurly != this.currentToken.token)
                ReportError(JSError.NoLeftCurly);
              handler = ParseBlock();
              tryCtx.UpdateWith(handler.context);
            }catch(RecoveryTokenException exc){
              if (exc._partiallyComputedNode == null)
                handler = new Block(CurrentPositionContext());
              else
                handler = exc._partiallyComputedNode;
              if (IndexOfToken(NoSkipTokenSet.s_NoTrySkipTokenSet, exc) == -1){
                Debug.Assert((type == null) ? exc._partiallyComputedNode == null : true);
                if (type != null)
                  exc._partiallyComputedNode = new Try(tryCtx, body, id, type, handler, null, false, tryEndContext);
                throw exc;
              }
            }finally{
              this.noSkipTokenSet.Remove(NoSkipTokenSet.s_NoTrySkipTokenSet);
            }
          }

          try{
            if (JSToken.Finally == this.currentToken.token){
              GetNextToken();
              this.blockType.Add(BlockType.Finally);
              try{
                finally_block = ParseBlock();
                catchOrFinally = true;
              }finally{
                this.blockType.RemoveAt(this.blockType.Count - 1);
              }
              tryCtx.UpdateWith(finally_block.context);
            }
          }catch(RecoveryTokenException exc){
            excInFinally = exc; // thrown later so we can execute code below
          }

          if (!catchOrFinally){
            ReportError(JSError.NoCatch, true);
            finally_block = new Block(CurrentPositionContext()); // make a dummy empty block
          }
        }finally{
          this.blockType.RemoveAt(this.blockType.Count - 1);
        }

        bool isFinallyEscaped = false;
        if (this.finallyEscaped > 0){
          this.finallyEscaped--;
          isFinallyEscaped = true;
        }
        if (excInFinally != null){
          excInFinally._partiallyComputedNode = new Try(tryCtx, body, id, type, handler, finally_block, isFinallyEscaped, tryEndContext);
          throw excInFinally;
        }else
          return new Try(tryCtx, body, id, type, handler, finally_block, isFinallyEscaped, tryEndContext);
      }

      //---------------------------------------------------------------------------------------
      // ParseClass
      //
      //  Class :
      //    'class' identifier OptionalExtends ClassBody
      //
      //  Extends :
      //    'extends' QualifiedIdentifier
      //
      //---------------------------------------------------------------------------------------
      private AST ParseClass(FieldAttributes visibilitySpec, bool isStatic, Context classCtx, bool isAbstract, bool isFinal, CustomAttributeList customAttributes){
        AST name = null;
        AST baseId = null;
        TypeExpression baseType = null;
        Block body = null;
        ArrayList interfaces = new ArrayList();

        bool isInterface = JSToken.Interface == this.currentToken.token;

        GetNextToken();
        if (JSToken.Identifier == this.currentToken.token){
          name = new IdentifierLiteral(this.scanner.GetIdentifier(), this.currentToken.Clone());
        }else{
          ReportError(JSError.NoIdentifier);
          if (JSToken.Extends != this.currentToken.token
              &&  JSToken.Implements != this.currentToken.token
              &&  JSToken.LeftCurly != this.currentToken.token)
            SkipTokensAndThrow();
          name = new IdentifierLiteral("##Missing Class Name##" + s_cDummyName++, CurrentPositionContext());
        }

        GetNextToken();
        if (JSToken.Extends == this.currentToken.token || JSToken.Implements == this.currentToken.token){
          if (isInterface && JSToken.Extends == this.currentToken.token)
            this.currentToken.token = JSToken.Implements;
          if (JSToken.Extends == this.currentToken.token){
            this.noSkipTokenSet.Add(NoSkipTokenSet.s_ClassExtendsNoSkipTokenSet);
            try{
              baseId = ParseQualifiedIdentifier(JSError.NeedType);
            }catch(RecoveryTokenException exc){
              if (IndexOfToken(NoSkipTokenSet.s_ClassExtendsNoSkipTokenSet, exc) == -1){
                exc._partiallyComputedNode = null;
                throw exc;
              }else{
                baseId = exc._partiallyComputedNode;
              }
            }finally{
              this.noSkipTokenSet.Remove(NoSkipTokenSet.s_ClassExtendsNoSkipTokenSet);
            }
          }
          if (JSToken.Implements == this.currentToken.token){
            do{
              AST typeId = null;
              this.noSkipTokenSet.Add(NoSkipTokenSet.s_ClassImplementsNoSkipTokenSet);
              try{
                typeId = ParseQualifiedIdentifier(JSError.NeedType);
                interfaces.Add(new TypeExpression(typeId));
              }catch(RecoveryTokenException exc){
                if (IndexOfToken(NoSkipTokenSet.s_ClassImplementsNoSkipTokenSet, exc) == -1){
                  exc._partiallyComputedNode = null;
                  throw exc;
                }else{
                  if (exc._partiallyComputedNode != null)
                    interfaces.Add(new TypeExpression(exc._partiallyComputedNode));
                }
              }finally{
                this.noSkipTokenSet.Remove(NoSkipTokenSet.s_ClassImplementsNoSkipTokenSet);
              }
            }while (JSToken.Comma == this.currentToken.token);
          }
        }
        if (baseId != null)
          baseType = new TypeExpression(baseId);
        if (JSToken.LeftCurly != this.currentToken.token){
          ReportError(JSError.NoLeftCurly);
        }

        // make a new state and save the old one
        ArrayList blockType = this.blockType;
        this.blockType = new ArrayList(16);
        SimpleHashtable labelTable = this.labelTable;
        this.labelTable = new SimpleHashtable(16);

        Globals.ScopeStack.Push(new ClassScope(name, ((IActivationObject)Globals.ScopeStack.Peek()).GetGlobalScope())); //Give declarations a place to go while building AST

        TypeExpression[] ifaces;
        try{
          body = ParseClassBody(false, isInterface);
          classCtx.UpdateWith(body.context);
          ifaces = new TypeExpression[interfaces.Count]; interfaces.CopyTo(ifaces);
          Class result = new Class(classCtx, name, baseType, ifaces, body, visibilitySpec, isAbstract, isFinal, isStatic, isInterface, customAttributes);
          if (customAttributes != null) customAttributes.SetTarget(result);
          return result;
        }catch(RecoveryTokenException exc){
          classCtx.UpdateWith(exc._partiallyComputedNode.context);
          ifaces = new TypeExpression[interfaces.Count]; interfaces.CopyTo(ifaces);
          exc._partiallyComputedNode =
            new Class(classCtx, name, baseType, ifaces, (Block)exc._partiallyComputedNode, visibilitySpec, isAbstract, isFinal, isStatic, isInterface, customAttributes);
          if (customAttributes != null) customAttributes.SetTarget(exc._partiallyComputedNode);
          throw exc;
        }finally{
          Globals.ScopeStack.Pop();
          this.blockType = blockType;
          this.labelTable = labelTable;
        }

      }

      //---------------------------------------------------------------------------------------
      // ParseClassBody
      //
      //  ClassBody :
      //    '{' OptionalClassMembers '}'
      //---------------------------------------------------------------------------------------
      Block ParseClassBody(bool isEnum, bool isInterface){
        this.blockType.Add(BlockType.Block);
        Block codeBlock = new Block(this.currentToken.Clone());
        try{
          GetNextToken();
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockNoSkipTokenSet);
          JSToken[] noSkip = null;
          if (isEnum)
            noSkip = NoSkipTokenSet.s_EnumBodyNoSkipTokenSet;
          else if (isInterface)
            noSkip = NoSkipTokenSet.s_InterfaceBodyNoSkipTokenSet;
          else
            noSkip = NoSkipTokenSet.s_ClassBodyNoSkipTokenSet;
          try{
            while (JSToken.RightCurly != this.currentToken.token){
              if (JSToken.EndOfFile == this.currentToken.token){
                ReportError(JSError.NoRightCurly, true);
                SkipTokensAndThrow();
              }
              this.noSkipTokenSet.Add(noSkip);
              try{
                AST classMember = isEnum ? ParseEnumMember() : ParseClassMember(isInterface);
                if (classMember != null)
                  codeBlock.Append(classMember);
              }catch(RecoveryTokenException exc){
                if (exc._partiallyComputedNode != null)
                  codeBlock.Append(exc._partiallyComputedNode);
                if (IndexOfToken(noSkip, exc) == -1){
                  exc._partiallyComputedNode = null;
                  throw exc;
                }
              }finally{
                this.noSkipTokenSet.Remove(noSkip);
              }
            }

          }catch(RecoveryTokenException exc){
            exc._partiallyComputedNode = codeBlock;
            throw exc;
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockNoSkipTokenSet);
          }
          codeBlock.context.UpdateWith(this.currentToken);
          GetNextToken();
        }finally{
          this.blockType.RemoveAt(this.blockType.Count - 1);
        }

        return codeBlock;
      }

      //---------------------------------------------------------------------------------------
      // ParseClassMember
      //
      //  OptionalClassMembers:
      //    ClassMember OptionalClassMembers |
      //    <empty>
      //
      //  ClassMember :
      //    VariableStatement |
      //    Class |
      //    Enum |
      //    FunctionDeclaration
      //
      //---------------------------------------------------------------------------------------
      private AST ParseClassMember(bool isInterface){
        bool parsed = false;
        // Interface members can be declared public ( as a no-op).
        if (isInterface && this.currentToken.token == JSToken.Public)
          GetNextToken();
        switch (this.currentToken.token){
          case JSToken.RightCurly:
            return null;
          case JSToken.Semicolon:
            GetNextToken();
            return ParseClassMember(isInterface);
          case JSToken.Const:
          case JSToken.Var:
            if (isInterface){
              ReportError(JSError.VarIllegalInInterface, true);
              GetNextToken();
              SkipTokensAndThrow();
            }
            return ParseVariableStatement((FieldAttributes)0, null, this.currentToken.token);
          case JSToken.Internal:
          case JSToken.Public:
          case JSToken.Static:
          case JSToken.Private:
          case JSToken.Protected:
          case JSToken.Abstract:
          case JSToken.Final:
            if (isInterface){
              ReportError(JSError.BadModifierInInterface, true);
              GetNextToken();
              SkipTokensAndThrow();
            }
            return ParseAttributes(null, true, true, out parsed);
          case JSToken.Interface:
            if (isInterface){
              ReportError(JSError.InterfaceIllegalInInterface, true);
              GetNextToken();
              SkipTokensAndThrow();
            }
            return ParseClass((FieldAttributes)0, false, this.currentToken.Clone(), false, false, null);
          case JSToken.Class:
            if (isInterface){
              ReportError(JSError.SyntaxError, true);
              GetNextToken();
              SkipTokensAndThrow();
            }
            return ParseClass((FieldAttributes)0, false, this.currentToken.Clone(), false, false, null);
          case JSToken.Enum:
            return ParseEnum((FieldAttributes)0, this.currentToken.Clone(), null);
          case JSToken.Function:
            return ParseFunction((FieldAttributes)0, false, this.currentToken.Clone(), true, isInterface, false, isInterface, null);
          case JSToken.Identifier:
            if (isInterface){
              ReportError(JSError.SyntaxError, true);
              GetNextToken();
              SkipTokensAndThrow();
            }
            bool bAssign, canBeAttribute = true;
            AST ast = ParseUnaryExpression(out bAssign, ref canBeAttribute, false);
            if (canBeAttribute){
              ast = ParseAttributes(ast, true, true, out parsed);
              if (parsed)
                return ast;
            }
            ReportError(JSError.SyntaxError, ast.context.Clone(), true);
            SkipTokensAndThrow();
            return null; // we'll never be executed, make the compiler happy
          case JSToken.Import:
            // handle common error of using import in class
            ReportError(JSError.InvalidImport, true);
            try{
              ParseImportStatement();
            }catch(RecoveryTokenException){
            }
            return null;                      
          case JSToken.Package:
            // handle common error of using package in class
            Context packageContext = this.currentToken.Clone();
            AST statement = ParsePackage(packageContext);
            if (statement is Package)
              ReportError(JSError.PackageInWrongContext, packageContext, true);
            return null;
          default:
            ReportError(JSError.SyntaxError, true);
            GetNextToken();
            SkipTokensAndThrow();
            return null; // we'll never be executed, make the compiler happy
        }
      }

      //---------------------------------------------------------------------------------------
      // ParseEnum
      //
      //  Enum :
      //    'enum' identifier [':' baseType] EnumBody (in the guise of ClassBody with a param)
      //
      //---------------------------------------------------------------------------------------
      private AST ParseEnum(FieldAttributes visibilitySpec, Context enumCtx, CustomAttributeList customAttributes){
        IdentifierLiteral name = null;
        AST baseId = null;
        TypeExpression baseType = null;
        Block body = null;

        GetNextToken();
        if (JSToken.Identifier == this.currentToken.token){
          name = new IdentifierLiteral(this.scanner.GetIdentifier(), this.currentToken.Clone());
        }else{
          ReportError(JSError.NoIdentifier);
          if (JSToken.Colon != this.currentToken.token && JSToken.LeftCurly != this.currentToken.token)
            SkipTokensAndThrow(); // what the heck is this?
          name = new IdentifierLiteral("##Missing Enum Name##" + s_cDummyName++, CurrentPositionContext());
        }

        GetNextToken();
        if (JSToken.Colon == this.currentToken.token){
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_EnumBaseTypeNoSkipTokenSet);
          try{
            baseId = ParseQualifiedIdentifier(JSError.NeedType);
          }catch(RecoveryTokenException exc){
            if (IndexOfToken(NoSkipTokenSet.s_ClassExtendsNoSkipTokenSet, exc) == -1){
              exc._partiallyComputedNode = null;
              throw exc;
            }else{
              baseId = exc._partiallyComputedNode;
            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_EnumBaseTypeNoSkipTokenSet);
          }
        }
        if (baseId != null)
          baseType = new TypeExpression(baseId);
        if (JSToken.LeftCurly != this.currentToken.token)
          ReportError(JSError.NoLeftCurly);

        // make a new state and save the old one
        ArrayList blockType = this.blockType;
        this.blockType = new ArrayList(16);
        SimpleHashtable labelTable = this.labelTable;
        this.labelTable = new SimpleHashtable(16);

        Globals.ScopeStack.Push(new ClassScope(name, ((IActivationObject)Globals.ScopeStack.Peek()).GetGlobalScope())); //Give declarations a place to go while building AST

        try{
          body = ParseClassBody(true, false);
          enumCtx.UpdateWith(body.context);
          EnumDeclaration result = new EnumDeclaration(enumCtx, name, baseType, body, visibilitySpec, customAttributes);
          if (customAttributes != null) customAttributes.SetTarget(result);
          return result;
        }catch(RecoveryTokenException exc){
          enumCtx.UpdateWith(exc._partiallyComputedNode.context);
          exc._partiallyComputedNode =  new EnumDeclaration(enumCtx, name, baseType, (Block)exc._partiallyComputedNode, visibilitySpec, customAttributes);
          if (customAttributes != null) customAttributes.SetTarget(exc._partiallyComputedNode);
          throw exc;
        }finally{
          Globals.ScopeStack.Pop();
          this.blockType = blockType;
          this.labelTable = labelTable;
        }
      }

      //---------------------------------------------------------------------------------------
      // ParseEnumMember
      //
      //  OptionalEnumMembers:
      //    EnumMember ',' OptionalEnumMembers |
      //    <empty>
      //
      //  EnumMember :
      //    Identifier |
      //    Identifer '=' IntegerLiteral
      //
      //---------------------------------------------------------------------------------------
      private AST ParseEnumMember(){
        AST ast = null;
        Lookup memberName = null;
        AST memberValue = null;
        switch (this.currentToken.token){
          case JSToken.Semicolon:
            GetNextToken();
            return ParseEnumMember();
          case JSToken.Identifier:
            memberName = new Lookup(this.currentToken.Clone());
            Context context = this.currentToken.Clone();
            GetNextToken();
            if (JSToken.Assign == this.currentToken.token){
              GetNextToken();
              memberValue = ParseExpression(true);
            }
            if (JSToken.Comma == this.currentToken.token)
              GetNextToken();
            else if (JSToken.RightCurly != this.currentToken.token)
              ReportError(JSError.NoComma, true);
            return new Constant(context, memberName, null, memberValue, FieldAttributes.Public, null);
          case JSToken.Var:
            // handle common error
            ReportError(JSError.NoVarInEnum, true);
            GetNextToken();
            return ParseEnumMember();
          default:
            ReportError(JSError.SyntaxError, true);
            SkipTokensAndThrow();
            return ast; // will never be executed, but make the C# compiler happy
        }
      }

      private bool GuessIfAbstract(){
        //
        // We have a class method declaraction without the "abstract"
        // attribute and need to know whether to treat it as abstract
        // for error reporting purposes.
        //
        // function bar();  -- clearly abstract, "missing abstract" error
        // function bar(){  -- clearly concrete, correct
        // function bar()   -- clearly an error, but should it be
        //                     "missing abstract" or "missing left curly"?
        //
        // In the last case we look what comes next.
        //
        // * If it looks like something that would start a function body -- a "var" statement or
        //   an expression for instance -- then we assume that the curly is missing.
        //
        // * If it looks like a global/package/class level declaration then we assume that it was supposed
        //   to be abstract.
        // 
        // * If it is a right-curly then we assume that the class scope is being
        //   closed and this is therefore an abstract function.
        //

        switch(this.currentToken.token){
          case JSToken.Const:
          case JSToken.Package:
          case JSToken.Internal:
          case JSToken.Public:
          case JSToken.Static:
          case JSToken.Private:
          case JSToken.Protected:
          case JSToken.Abstract:
          case JSToken.Final:
          case JSToken.Interface:
          case JSToken.Class:
          case JSToken.Enum:
          case JSToken.Function:
          case JSToken.RightCurly:
            return true;
          case JSToken.Semicolon:
            GetNextToken();
            return true;
          case JSToken.Var:
          case JSToken.LeftCurly:
          default:
            return false;
        }
      }

      //---------------------------------------------------------------------------------------
      // ParseFunction
      //
      //  FunctionDeclaration :
      //    VisibilityModifier 'function' GetSet Identifier '('
      //                          FormalParameterList ')' '{' FunctionBody '}'
      //
      //  GetSet:
      //    <empty> |
      //    'get' |
      //    'set'
      //
      //  FormalParameterList :
      //    <empty> |
      //    IdentifierList Identifier
      //
      //  IdentifierList :
      //    <empty> |
      //    Identifier, IdentifierList
      //---------------------------------------------------------------------------------------
      private AST ParseFunction(FieldAttributes visibilitySpec,
                                bool inExpression,
                                Context fncCtx,
                                bool isMethod,
                                bool isAbstract,
                                bool isFinal,
                                bool isInterface,
                                CustomAttributeList customAttributes){
        return ParseFunction(visibilitySpec, inExpression, fncCtx, isMethod, isAbstract, isFinal, isInterface, customAttributes, null);
      }

      private AST ParseFunction(FieldAttributes visibilitySpec,
                                bool inExpression,
                                Context fncCtx,
                                bool isMethod,
                                bool isAbstract,
                                bool isFinal,
                                bool isInterface,
                                CustomAttributeList customAttributes,
                                Call function){
        if (this.demandFullTrustOnFunctionCreation)
          (new SecurityPermission(SecurityPermissionFlag.UnmanagedCode)).Demand();
        IdentifierLiteral name = null;
        AST interfaceName = null;
        ArrayList formalParameters = null;
        TypeExpression returnType = null;
        Block body = null;
        bool isGetter = false;
        bool isSetter = false;

        if (function == null){
          GetNextToken();
          if (isMethod)
            if (JSToken.Get == this.currentToken.token){
              isGetter = true;
              GetNextToken();
            }else if (JSToken.Set == this.currentToken.token){
              isSetter = true;
              GetNextToken();
            }

          // get the function name or make an anonymous function if in expression "position"
          if (JSToken.Identifier == this.currentToken.token){
            name = new IdentifierLiteral(this.scanner.GetIdentifier(), this.currentToken.Clone());
            GetNextToken();
            if (JSToken.AccessField == this.currentToken.token){
              if (isInterface) // "function IBar.foo()" is illegal in an interface declaration
                ReportError(JSError.SyntaxError, true);
              GetNextToken();
              if (JSToken.Identifier == this.currentToken.token){
                interfaceName = new Lookup(name.context);
                name = new IdentifierLiteral(this.scanner.GetIdentifier(), this.currentToken.Clone());
                GetNextToken();
                while (JSToken.AccessField == this.currentToken.token){
                  GetNextToken();
                  if (JSToken.Identifier == this.currentToken.token){
                    interfaceName = new Member(interfaceName.context.CombineWith(this.currentToken), interfaceName,
                      new ConstantWrapper(name.ToString(), name.context));
                    name = new IdentifierLiteral(this.scanner.GetIdentifier(), this.currentToken.Clone());
                    GetNextToken();
                  }else
                    ReportError(JSError.NoIdentifier, true);
                }
              }else
                ReportError(JSError.NoIdentifier, true);
            }
          }else{
            string identifier = JSKeyword.CanBeIdentifier(this.currentToken.token);
            if (null != identifier){
              ForceReportInfo(JSError.KeywordUsedAsIdentifier, isMethod);
              name = new IdentifierLiteral(identifier, this.currentToken.Clone());
              GetNextToken();
            }else{
              if (!inExpression){
                identifier = this.currentToken.GetCode();
                ReportError(JSError.NoIdentifier, true);
                GetNextToken();
              }else
                identifier = "";
              name = new IdentifierLiteral(identifier, CurrentPositionContext());
            }
          }
        }else{ // function was passed in, this is an error condition
          name = function.GetName();
        }

        // make a new state and save the old one
        ArrayList blockType = this.blockType;
        this.blockType = new ArrayList(16);
        SimpleHashtable labelTable = this.labelTable;
        this.labelTable = new SimpleHashtable(16);
        FunctionScope fscope = new FunctionScope(Globals.ScopeStack.Peek(), isMethod);
        Globals.ScopeStack.Push(fscope); //Give declarations a place to go while building AST

        try{
          formalParameters = new ArrayList();
          Context paramArrayContext = null;
          if (function == null){
            // get the formal parameters
            if (JSToken.LeftParen != this.currentToken.token)
              ReportError(JSError.NoLeftParen);
            GetNextToken();
            // create the list of arguments and update the context
            while (JSToken.RightParen != this.currentToken.token){
              if (paramArrayContext != null){
                ReportError(JSError.ParamListNotLast, paramArrayContext, true);
                paramArrayContext = null;
              }
              String id = null;
              TypeExpression typeExpr = null;
              this.noSkipTokenSet.Add(NoSkipTokenSet.s_FunctionDeclNoSkipTokenSet);
              try{
                if (JSToken.ParamArray == this.currentToken.token){
                  paramArrayContext = this.currentToken.Clone();
                  GetNextToken();
                }
                if (JSToken.Identifier != this.currentToken.token && (id = JSKeyword.CanBeIdentifier(this.currentToken.token)) == null){
                  if (JSToken.LeftCurly == this.currentToken.token){
                    ReportError(JSError.NoRightParen);
                    break;
                  }else if (JSToken.Comma == this.currentToken.token){
                    // We're missing an argument (or previous argument was malformed and
                    // we skipped to the comma.)  Keep trying to parse the argument list --
                    // we will skip the comma below.
                    ReportError(JSError.SyntaxError, true);
                  }else{
                    ReportError(JSError.SyntaxError, true);
                    SkipTokensAndThrow();
                  }
                }else{
                  if (null == id)
                    id = this.scanner.GetIdentifier();
                  else
                    ForceReportInfo(JSError.KeywordUsedAsIdentifier);
                  Context paramCtx = this.currentToken.Clone();
                  GetNextToken();
                  if (JSToken.Colon == this.currentToken.token){
                    typeExpr = ParseTypeExpression();
                    if (null != typeExpr)
                      paramCtx.UpdateWith(typeExpr.context);
                  }
                  
                  
                  CustomAttributeList custAttrs = null;
                  if (paramArrayContext != null){
                    custAttrs = new CustomAttributeList(paramArrayContext);
                    custAttrs.Append(new CustomAttribute(paramArrayContext, new Lookup("...", paramArrayContext), new ASTList(null)));
                  }
                  formalParameters.Add(new ParameterDeclaration(paramCtx, id, typeExpr, custAttrs));
                }

                // got an arg, it should be either a ',' or ')'
                if (JSToken.RightParen == this.currentToken.token)
                  break;
                else if (JSToken.Comma != this.currentToken.token){
                  // deal with error in some "intelligent" way
                  if (JSToken.LeftCurly == this.currentToken.token){
                    ReportError(JSError.NoRightParen);
                    break;
                  }else{
                    if (JSToken.Identifier == this.currentToken.token && typeExpr == null){
                      // it's possible that the guy was writing the type in C/C++ style (i.e. int x)
                      ReportError(JSError.NoCommaOrTypeDefinitionError);
                    }else
                      ReportError(JSError.NoComma);
                  }
                }
                GetNextToken();
              }catch(RecoveryTokenException exc){
                if (IndexOfToken(NoSkipTokenSet.s_FunctionDeclNoSkipTokenSet, exc) == -1)
                  throw exc;
              }finally{
                this.noSkipTokenSet.Remove(NoSkipTokenSet.s_FunctionDeclNoSkipTokenSet);
              }
            }
            fncCtx.UpdateWith(this.currentToken);
            // if it is a getter/setter must have 0/1 arg only
            if (isGetter && formalParameters.Count != 0){
              ReportError(JSError.BadPropertyDeclaration, true);
              isGetter = false;
            }else if (isSetter && formalParameters.Count != 1){
              ReportError(JSError.BadPropertyDeclaration, true);
              isSetter = false;
            }
            GetNextToken();

            // check the return type
            if (JSToken.Colon == this.currentToken.token){
              if (isSetter)
                ReportError(JSError.SyntaxError);
              this.noSkipTokenSet.Add(NoSkipTokenSet.s_StartBlockNoSkipTokenSet);
              try{
                returnType = ParseTypeExpression();
              }catch(RecoveryTokenException exc){
                if (IndexOfToken(NoSkipTokenSet.s_StartBlockNoSkipTokenSet, exc) == -1){
                  exc._partiallyComputedNode = null;
                  throw exc;
                }else{
                  if (exc._partiallyComputedNode != null)
                    returnType = (TypeExpression)exc._partiallyComputedNode;
                }
              }finally{
                this.noSkipTokenSet.Remove(NoSkipTokenSet.s_StartBlockNoSkipTokenSet);
              }
              if (isSetter)
                returnType = null;
            }
          }else{ // function was passed in, this is an error condition
            function.GetParameters(formalParameters);
          }

          // read the function body of non-abstract functions.

          if (JSToken.LeftCurly != this.currentToken.token && (isAbstract || (isMethod && GuessIfAbstract()))){
            if (!isAbstract){
              isAbstract = true;
              ReportError(JSError.ShouldBeAbstract, fncCtx, true);
            }
            body = new Block(this.currentToken.Clone());
          }else{
            if (JSToken.LeftCurly != this.currentToken.token)
              ReportError(JSError.NoLeftCurly, true);
            else if (isAbstract)
              ReportError(JSError.AbstractWithBody, fncCtx, true);

            this.blockType.Add(BlockType.Block);
            this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockNoSkipTokenSet);
            this.noSkipTokenSet.Add(NoSkipTokenSet.s_StartStatementNoSkipTokenSet);
            try{
              // parse the block locally to get the exact end of function
              body = new Block(this.currentToken.Clone());
              GetNextToken();

              while (JSToken.RightCurly != this.currentToken.token){
                try{
                  body.Append(ParseStatement());
                }catch(RecoveryTokenException exc){
                  if (exc._partiallyComputedNode != null){
                    body.Append(exc._partiallyComputedNode);
                  }
                  if (IndexOfToken(NoSkipTokenSet.s_StartStatementNoSkipTokenSet, exc) == -1)
                    throw exc;
                }
              }

              body.context.UpdateWith(this.currentToken);
              fncCtx.UpdateWith(this.currentToken);
            }catch(RecoveryTokenException exc){
              if (IndexOfToken(NoSkipTokenSet.s_BlockNoSkipTokenSet, exc) == -1){
                Globals.ScopeStack.Pop(); //Pop current scope so that FunctionDeclaration sees proper scope stack
                try{
                  ParameterDeclaration[] foParameters = new ParameterDeclaration[formalParameters.Count]; formalParameters.CopyTo(foParameters);
                  if (inExpression)
                    exc._partiallyComputedNode =
                      new FunctionExpression(fncCtx, name, foParameters, returnType, body, fscope, visibilitySpec);
                  else
                    exc._partiallyComputedNode =
                      new FunctionDeclaration(fncCtx, interfaceName, name, foParameters, returnType, body, fscope, visibilitySpec,
                      isMethod, isGetter, isSetter, isAbstract, isFinal, customAttributes);
                  if (customAttributes != null) customAttributes.SetTarget(exc._partiallyComputedNode);
                }finally{
                  Globals.ScopeStack.Push(fscope); //Push it back so that the next finally can pop it
                }
                throw exc;
              }
            }finally{
              this.blockType.RemoveAt(this.blockType.Count - 1);
              this.noSkipTokenSet.Remove(NoSkipTokenSet.s_StartStatementNoSkipTokenSet);
              this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockNoSkipTokenSet);
            }

            GetNextToken();
          }
        }finally{
          // restore state
          this.blockType = blockType;
          this.labelTable = labelTable;
          Globals.ScopeStack.Pop();
        }

        ParameterDeclaration[] fParameters = new ParameterDeclaration[formalParameters.Count]; formalParameters.CopyTo(fParameters);
        AST func;
        if (inExpression)
          func = new FunctionExpression(fncCtx, name, fParameters, returnType, body, fscope, visibilitySpec);
        else
          func = new FunctionDeclaration(fncCtx, interfaceName, name, fParameters, returnType, body, fscope, visibilitySpec,
                                         isMethod, isGetter, isSetter, isAbstract, isFinal, customAttributes);

        if (customAttributes != null) customAttributes.SetTarget(func);
        return func;

      }

      //used by FunctionConstructor
      internal AST ParseFunctionExpression(){
        this.demandFullTrustOnFunctionCreation = true;
        GetNextToken(); //skip over function keyword
        return ParseFunction((FieldAttributes)0, true, this.currentToken.Clone(), false, false, false, false, null);
      }

      //---------------------------------------------------------------------------------------
      // ParseNamedBreakpoint
      //
      //  Used by the debugger to parsed a named breakpoint, that is a name of a function with
      //  possible arguments and optionally followed by an il offset
      //---------------------------------------------------------------------------------------
      internal String[] ParseNamedBreakpoint(out int argNumber){
        argNumber = 0;

        // parse the function name
        AST function = ParseQualifiedIdentifier(JSError.SyntaxError);
        if (function != null){
          String[] parsedFunction = new String[4];
          parsedFunction[0] = function.ToString();

          if (JSToken.LeftParen == this.currentToken.token){
            String id = null;
            String typeString = null;
            AST qualid = null;

            parsedFunction[1] = "";

            GetNextToken();
            // parse the formal parameters
            while (JSToken.RightParen != this.currentToken.token){
              id = null;
              if (JSToken.Identifier != this.currentToken.token && (id = JSKeyword.CanBeIdentifier(this.currentToken.token)) == null){
                return null;
              }else{
                if (null == id)
                  id = this.scanner.GetIdentifier();
                qualid = new Lookup(id, this.currentToken.Clone());
                GetNextToken();
                if (JSToken.AccessField == this.currentToken.token){
                  qualid = ParseScopeSequence(qualid, JSError.SyntaxError);
                  typeString = qualid.ToString();
                  while (JSToken.LeftBracket == this.currentToken.token){
                    GetNextToken();
                    if (JSToken.RightBracket != this.currentToken.token)
                      return null;
                    typeString += "[]";
                    GetNextToken();
                  }
                }else if (JSToken.Colon == this.currentToken.token){
                  GetNextToken();
                  if (JSToken.RightParen == this.currentToken.token)
                    return null;
                  continue;
                }else{
                  typeString = qualid.ToString();
                }
                parsedFunction[1] += typeString + " ";
              }
              argNumber++;

              if (JSToken.Comma == this.currentToken.token){
                GetNextToken();
                if (JSToken.RightParen == this.currentToken.token)
                  return null;
              }
            }

            GetNextToken();

            // parse a return value if any
            if (JSToken.Colon == this.currentToken.token){
              GetNextToken();
              id = null;
              if (JSToken.Identifier != this.currentToken.token && (id = JSKeyword.CanBeIdentifier(this.currentToken.token)) == null){
                return null;
              }else{
                if (null == id)
                  id = this.scanner.GetIdentifier();
                qualid = new Lookup(id, this.currentToken.Clone());
                GetNextToken();
                if (JSToken.AccessField == this.currentToken.token){
                  qualid = ParseScopeSequence(qualid, JSError.SyntaxError);
                  parsedFunction[2] = qualid.ToString();
                  while (JSToken.LeftBracket == this.currentToken.token){
                    GetNextToken();
                    if (JSToken.RightBracket != this.currentToken.token)
                      return null;
                    parsedFunction[2] += "[]";
                    GetNextToken();
                  }
                }else{
                  parsedFunction[2] = qualid.ToString();
                }
              }
            }

          }

          if (JSToken.Plus == this.currentToken.token){
            GetNextToken();
            if (JSToken.IntegerLiteral != this.currentToken.token)
              return null;
            parsedFunction[3] = this.currentToken.GetCode();
            GetNextToken();
          }

          if (JSToken.EndOfFile != this.currentToken.token)
            return null;
          return parsedFunction;
        }
        return null;
      }

      //---------------------------------------------------------------------------------------
      // ParsePackage
      //
      //  Package :
      //    'package' QualifiedIdentifier '{' ClassList '}'
      //
      //  ClassList :
      //    <empty> |
      //    Class ClassList |
      //    Attributes Class ClassList |
      //    Attributes Enum ClassList
      //---------------------------------------------------------------------------------------
      // Because 'package' is not a reserved word in JS5 we have to deal with an ambiguity
      // in the grammar. A source sequence like the following
      // package
      // x
      // { }
      // can be legally parsed in two ways:
      // Identifier Identifier Block or
      // Package
      // we give Package priority in this situation.
      // Here is how we deal with some possible cases:
      // 1- ** package <no line break> QualifiedIdentifier ** is parsed unambiguously as a package production regardless of what comes after Identifier
      // 2- ** package <no line break> NotOneOf(Operator | '[' | '.' | '(' | Identifier) '{' ** is parsed as a package production with an error
      // 3- ** package <line break> '{' ** is parsed as a package (anonymous) with an error
      // 4- ** package <line break> Not(Identifier) ** is never parsed as a package
      private AST ParsePackage(Context packageContext){
        GetNextToken();
        AST qualid = null;
        bool gotLineBreak = this.scanner.GotEndOfLine();

        // erroneous package production
        if (JSToken.Identifier != this.currentToken.token){
          if (JSScanner.CanParseAsExpression(this.currentToken.token)){
            // it's an expression. Report a warning. package and this.currentToken can be an expression (i.e. 'package +')
            ReportError(JSError.KeywordUsedAsIdentifier, packageContext.Clone(), true);
            qualid = new Lookup("package", packageContext);
            // get the member expression
            qualid = MemberExpression(qualid, null);
            bool isLeftHandSide;
            qualid = ParsePostfixExpression(qualid, out isLeftHandSide);
            qualid = ParseExpression(qualid, false, isLeftHandSide, JSToken.None);
            return new Expression(qualid.context.Clone(), qualid);
          }else if (!gotLineBreak){
            if (JSToken.Increment == this.currentToken.token || JSToken.Decrement == this.currentToken.token){
              // it's a postfix expression. Report a warning
              ReportError(JSError.KeywordUsedAsIdentifier, packageContext.Clone(), true);
              bool dummy;
              qualid = new Lookup("package", packageContext);
              qualid = ParsePostfixExpression(qualid, out dummy);
              qualid = ParseExpression(qualid, false, false, JSToken.None);
              return new Expression(qualid.context.Clone(), qualid);
            }
          }else{
            // it's an expression. Report a warning which, as a side effect, will make the current token be the next token fetched
            ReportError(JSError.KeywordUsedAsIdentifier, packageContext.Clone(), true);
            return new Lookup("package", packageContext);
          }
        }else{
          // it is an identifier, parse it as a qualified identifier
          this.errorToken = this.currentToken; // this will make GetNextToken() in ParseQualifiedIdentifier() return this.currentToken
          qualid = ParseQualifiedIdentifier(JSError.NoIdentifier);
        }

        // if we are here we have:
        // ** package QualifiedIdentifier ** or
        // ** package SomeNonSenseToken **, that is a token that does not make an expression

        Context nonSenseToken = null;
        if (JSToken.LeftCurly != this.currentToken.token && qualid == null){
          // we want to peek and see whether the next token is a LeftCurly
          nonSenseToken = this.currentToken.Clone();
          GetNextToken();
        }

        if (JSToken.LeftCurly == this.currentToken.token){
          // sounds like a package, possibly with an error. If qualid is not null is actually a good package, otherwise we treat it
          // as an anonymous package and keep going.
          if (qualid == null){
            if (nonSenseToken == null)
              nonSenseToken = this.currentToken.Clone();
            ReportError(JSError.NoIdentifier, nonSenseToken, true);
          }
        }else{
          if (qualid == null){
            ReportError(JSError.SyntaxError, packageContext);
            if (JSScanner.CanStartStatement(nonSenseToken.token)){
              // this is tricky we assign nonSenseToken to this.currentToken and call ParseStatement, because we know it is a statement start token.
              // The parser should then call GetNextToken() which will return the this.currentToken that is assigned to this.errorToken
              this.currentToken = nonSenseToken;
              return ParseStatement();
            }else{
              //ReportError(JSError.SyntaxError, nonSenseToken);
              if (JSScanner.CanStartStatement(this.currentToken.token)){
                this.errorToken = null;
                return ParseStatement();
              }else{
                ReportError(JSError.SyntaxError);
                SkipTokensAndThrow();
              }
            }
          }else{
            if (gotLineBreak){
              // we are here with the following: 'package' <line break> QalifiedIdentifier' however we do not have a left curly.
              // if the token in our hand can start an expression we go with two expressions, otherwise we accept it as a package
              //if (JSScanner.CanParseAsExpression(this.currentToken.token)){
                ReportError(JSError.KeywordUsedAsIdentifier, packageContext.Clone(), true);
                Block block = new Block(packageContext.Clone());
                block.Append(new Lookup("package", packageContext));
                qualid = MemberExpression(qualid, null);
                bool isLeftHandSide;
                qualid = ParsePostfixExpression(qualid, out isLeftHandSide);
                qualid = ParseExpression(qualid, false, true, JSToken.None);
                block.Append(new Expression(qualid.context.Clone(), qualid));
                block.context.UpdateWith(qualid.context);
                return block;
              //}
            }
            // the package production rule is entered regardless of the presence of a left curly.
            ReportError(JSError.NoLeftCurly);
          }
        }

        PackageScope pscope = new PackageScope(Globals.ScopeStack.Peek());
        Globals.ScopeStack.Push(pscope); //Give declarations a place to go while building AST
        try{
          string name = (qualid != null) ? qualid.ToString() : "anonymous package";
          pscope.name = name;
          packageContext.UpdateWith(this.currentToken);
          ASTList classList = new ASTList(packageContext);

          GetNextToken();
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockNoSkipTokenSet);
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_PackageBodyNoSkipTokenSet);
          try{
            while (this.currentToken.token != JSToken.RightCurly){
              AST ast = null;
              try{
                switch (this.currentToken.token){

                  case JSToken.Interface:
                  case JSToken.Class:
                    classList.Append(ParseClass((FieldAttributes)0, false, this.currentToken.Clone(), false, false, null));
                    break;

                  case JSToken.Enum:
                    classList.Append(ParseEnum((FieldAttributes)0, this.currentToken.Clone(), null));
                    break;
                  case JSToken.Internal:
                  case JSToken.Public:
                  case JSToken.Static:
                  case JSToken.Private:
                  case JSToken.Protected:
                  case JSToken.Abstract:
                  case JSToken.Final:
                    bool parsedOK;
                    ast = ParseAttributes(null, true, false, out parsedOK);
                    if (parsedOK){
                      if (ast is Class){
                        classList.Append(ast);
                        break;
                      }
                    }
                    ReportError(JSError.OnlyClassesAllowed, ast.context.Clone(), true);
                    SkipTokensAndThrow();
                    break;

                  case JSToken.Identifier:
                    bool bAssign, canBeAttribute = true;
                    ast = ParseUnaryExpression(out bAssign, ref canBeAttribute, false);
                    if (canBeAttribute){
                      bool parsed;
                      ast = ParseAttributes(ast, true, false, out parsed);
                      if (parsed){
                        if (ast is Class){
                          classList.Append(ast);
                          break;
                        }
                      }
                    }
                    ReportError(JSError.OnlyClassesAllowed, ast.context.Clone(), true);
                    SkipTokensAndThrow();
                    break;

                  case JSToken.EndOfFile:
                    EOFError(JSError.ErrEOF);
                    throw new EndOfFile(); // abort parsing, get back to the main parse routine

                  case JSToken.Semicolon: // ignore any spurious semicolon
                    GetNextToken();
                    break;
                  case JSToken.Import:
                    // handle common error of using import in package
                    ReportError(JSError.InvalidImport, true);
                    try{
                      ParseImportStatement();
                    }catch(RecoveryTokenException){
                    }
                    break;
                  case JSToken.Package:
                    // handle common error of using package in package
                    Context nestedPackageContext = this.currentToken.Clone();
                    AST statement = ParsePackage(nestedPackageContext);
                    if (statement is Package)
                      ReportError(JSError.PackageInWrongContext, nestedPackageContext, true);
                    break;
                  default:
                    ReportError(JSError.OnlyClassesAllowed, (ast != null) ? ast.context.Clone() : CurrentPositionContext(), true);
                    SkipTokensAndThrow();
                    break;
                }
              }catch(RecoveryTokenException exc){
                if (exc._partiallyComputedNode != null && exc._partiallyComputedNode is Class){
                  classList.Append((Class)exc._partiallyComputedNode);
                  exc._partiallyComputedNode = null;
                }
                if (IndexOfToken(NoSkipTokenSet.s_PackageBodyNoSkipTokenSet, exc) == -1)
                  throw exc;
              }
            }
          }catch(RecoveryTokenException exc){
            if (IndexOfToken(NoSkipTokenSet.s_BlockNoSkipTokenSet, exc) == -1){
              ReportError(JSError.NoRightCurly, CurrentPositionContext());
              exc._partiallyComputedNode = new Package(name, qualid, classList, packageContext);
              throw exc;
            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_PackageBodyNoSkipTokenSet);
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockNoSkipTokenSet);
          }
          GetNextToken();
          return new Package(name, qualid, classList, packageContext);
        }finally{
          Globals.ScopeStack.Pop();
        }
      }

      //---------------------------------------------------------------------------------------
      // ParseStaticInitializer
      //
      //  StaticInitializer :
      //    '{' FunctionBody '}'
      //---------------------------------------------------------------------------------------
      private AST ParseStaticInitializer(Context initContext){
        if (this.demandFullTrustOnFunctionCreation)
          (new SecurityPermission(SecurityPermissionFlag.UnmanagedCode)).Demand();
        Block body = null;
        FunctionScope scope = new FunctionScope(Globals.ScopeStack.Peek());
        scope.isStatic = true;

        // make a new state and save the old one
        ArrayList blockType = this.blockType;
        this.blockType = new ArrayList(16);
        SimpleHashtable labelTable = this.labelTable;
        this.labelTable = new SimpleHashtable(16);

        this.blockType.Add(BlockType.Block);
        this.noSkipTokenSet.Add(NoSkipTokenSet.s_BlockNoSkipTokenSet);
        this.noSkipTokenSet.Add(NoSkipTokenSet.s_StartStatementNoSkipTokenSet);
        try{
          Globals.ScopeStack.Push(scope); //Give declarations a place to go while building AST
          // parse the block locally to get the exact end of function
          body = new Block(this.currentToken.Clone());
          GetNextToken();

          while (JSToken.RightCurly != this.currentToken.token){
            try{
              body.Append(ParseStatement());
            }catch(RecoveryTokenException exc){
              if (exc._partiallyComputedNode != null)
                body.Append(exc._partiallyComputedNode);
              if (IndexOfToken(NoSkipTokenSet.s_StartStatementNoSkipTokenSet, exc) == -1)
                throw exc;
            }
          }
        }catch(RecoveryTokenException exc){
          if (IndexOfToken(NoSkipTokenSet.s_BlockNoSkipTokenSet, exc) == -1){
            exc._partiallyComputedNode = new StaticInitializer(initContext, body, scope);
            throw exc;
          }
        }finally{
          this.noSkipTokenSet.Remove(NoSkipTokenSet.s_StartStatementNoSkipTokenSet);
          this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BlockNoSkipTokenSet);
          this.blockType = blockType;
          this.labelTable = labelTable;
          Globals.ScopeStack.Pop();
        }
        body.context.UpdateWith(this.currentToken);
        initContext.UpdateWith(this.currentToken);

        GetNextToken();
        return new StaticInitializer(initContext, body, scope);
      }

      //---------------------------------------------------------------------------------------
      // ParseExpression
      //
      //  Expression :
      //    AssignmentExpressionList AssignmentExpression
      //
      //  AssignmentExpressionList :
      //    <empty> |
      //    AssignmentExpression ',' AssignmentExpressionList
      //
      //  AssignmentExpression :
      //    ConditionalExpression |
      //    LeftHandSideExpression AssignmentOperator AssignmentExpression
      //
      //  ConditionalExpression :
      //    LogicalORExpression OptionalConditionalExpression
      //
      //  OptionalConditionalExpression :
      //    <empty> |
      //    '?' AssignmentExpression ':' AssignmentExpression
      //
      //  LogicalORExpression :
      //    LogicalANDExpression OptionalLogicalOrExpression
      //
      //  OptionalLogicalOrExpression :
      //    <empty> |
      //    '||' LogicalANDExpression OptionalLogicalOrExpression
      //
      //  LogicalANDExpression :
      //    BitwiseORExpression OptionalLogicalANDExpression
      //
      //  OptionalLogicalANDExpression :
      //    <empty> |
      //    '&&' BitwiseORExpression OptionalLogicalANDExpression
      //
      //  BitwiseORExpression :
      //    BitwiseXORExpression OptionalBitwiseORExpression
      //
      //  OptionalBitwiseORExpression :
      //    <empty> |
      //    '|' BitwiseXORExpression OptionalBitwiseORExpression
      //
      //  BitwiseXORExpression :
      //    BitwiseANDExpression OptionalBitwiseXORExpression
      //
      //  OptionalBitwiseXORExpression :
      //    <empty> |
      //    '^' BitwiseANDExpression OptionalBitwiseXORExpression
      //
      //  BitwiseANDExpression :
      //    EqualityExpression OptionalBitwiseANDExpression
      //
      //  OptionalBitwiseANDExpression :
      //    <empty> |
      //    '&' EqualityExpression OptionalBitwiseANDExpression
      //
      //  EqualityExpression :
      //    RelationalExpression |
      //    RelationalExpression '==' EqualityExpression |
      //    RelationalExpression '!=' EqualityExpression |
      //    RelationalExpression '===' EqualityExpression |
      //    RelationalExpression '!==' EqualityExpression
      //
      //  RelationalExpression :
      //    ShiftExpression |
      //    ShiftExpression '<' RelationalExpression |
      //    ShiftExpression '>' RelationalExpression |
      //    ShiftExpression '<=' RelationalExpression |
      //    ShiftExpression '>=' RelationalExpression
      //
      //  ShiftExpression :
      //    AdditiveExpression |
      //    AdditiveExpression '<<' ShiftExpression |
      //    AdditiveExpression '>>' ShiftExpression |
      //    AdditiveExpression '>>>' ShiftExpression
      //
      //  AdditiveExpression :
      //    MultiplicativeExpression |
      //    MultiplicativeExpression '+' AdditiveExpression |
      //    MultiplicativeExpression '-' AdditiveExpression
      //
      //  MultiplicativeExpression :
      //    UnaryExpression |
      //    UnaryExpression '*' MultiplicativeExpression |
      //    UnaryExpression '/' MultiplicativeExpression |
      //    UnaryExpression '%' MultiplicativeExpression
      //---------------------------------------------------------------------------------------
      private AST ParseExpression(){
        bool bAssign;
        AST lhs = ParseUnaryExpression(out bAssign, false);
        return ParseExpression(lhs, false, bAssign, JSToken.None);
      }

      private AST ParseExpression(bool single){
        bool bAssign;
        AST lhs = ParseUnaryExpression(out bAssign, false);
        return ParseExpression(lhs, single, bAssign, JSToken.None);
      }

      private AST ParseExpression(bool single, JSToken inToken){
        bool bAssign;
        AST lhs = ParseUnaryExpression(out bAssign, false);
        return ParseExpression(lhs, single, bAssign, inToken);
      }

      private AST ParseExpression(AST leftHandSide, bool single, bool bCanAssign, JSToken inToken){
        OpListItem opsStack = new OpListItem(JSToken.None, OpPrec.precNone, null); // dummy element
        AstListItem termStack = new AstListItem(leftHandSide, null);

        AST expr = null;

        try{
          for (;;){
            if (JSScanner.IsProcessableOperator(this.currentToken.token) && inToken != this.currentToken.token){

              OpPrec prec = JSScanner.GetOperatorPrecedence(this.currentToken.token);
              bool rightAssoc = JSScanner.IsRightAssociativeOperator(this.currentToken.token);
              // the current operator has lower precedence than the operator at the top of the stack
              // or it has the same precedence and it is left associative (that is, no 'assign op' or 'conditional')
              while (prec < opsStack._prec || prec == opsStack._prec && !rightAssoc){
                //Console.Out.WriteLine("lower prec or same and left assoc");
                expr = CreateExpressionNode(opsStack._operator, termStack._prev._term, termStack._term);

                // pop the operator stack
                opsStack = opsStack._prev;
                // pop the term stack twice
                termStack = termStack._prev._prev;
                // push node onto the stack
                termStack = new AstListItem(expr, termStack);
              }

              // the current operator has higher precedence that every scanned operators on the stack, or
              // it has the same precedence as the one at the top of the stack and it is right associative

              // push operator and next term

              // special case conditional '?:'
              if (JSToken.ConditionalIf == this.currentToken.token){
                //Console.Out.WriteLine("Condition expression");

                AST condition = termStack._term;
                // pop term stack
                termStack = termStack._prev;

                GetNextToken();

                // get expr1 in logOrExpr ? expr1 : expr2
                AST operand1 = ParseExpression(true);

                if (JSToken.Colon != this.currentToken.token)
                  ReportError(JSError.NoColon);
                GetNextToken();

                // get expr2 in logOrExpr ? expr1 : expr2
                AST operand2 = ParseExpression(true, inToken);

                expr = new Conditional(condition.context.CombineWith(operand2.context), condition, operand1, operand2);
                termStack = new AstListItem(expr, termStack);
              }else{
                //Console.Out.WriteLine("higher prec or right assoc");

                if (JSScanner.IsAssignmentOperator(this.currentToken.token)){
                  if (!bCanAssign){
                    ReportError(JSError.IllegalAssignment);
                    SkipTokensAndThrow();
                  }
                }else
                  bCanAssign = false;

                // push the operator onto the operators stack
                opsStack = new OpListItem(this.currentToken.token, prec, opsStack);
                // push new term
                GetNextToken();
                if (bCanAssign)
                  termStack = new AstListItem(ParseUnaryExpression(out bCanAssign, false), termStack);
                else{
                  bool dummy;
                  termStack = new AstListItem(ParseUnaryExpression(out dummy, false), termStack);
                  dummy = dummy;
                }
              }
            }else
              break; // done, go and unwind the stack of expressions/operators
          }

          //Console.Out.WriteLine("unwinding stack");
          // there are still operators to be processed
          while (opsStack._operator != JSToken.None){
            // make the ast operator node
            expr = CreateExpressionNode(opsStack._operator, termStack._prev._term, termStack._term);

            // pop the operator stack
            opsStack = opsStack._prev;
            // pop the term stack twice
            termStack = termStack._prev._prev;

            // push node onto the stack
            termStack = new AstListItem(expr, termStack);
          }

          // if we have a ',' and we are not looking for a single expression reenter
          if (!single && JSToken.Comma == this.currentToken.token){
            //Console.Out.WriteLine("Next expr");
            GetNextToken();
            AST expr2 = ParseExpression(false, inToken);
            termStack._term = new Comma(termStack._term.context.CombineWith(expr2.context), termStack._term, expr2);
          }

          Debug.Assert(termStack._prev == null);
          return termStack._term;
        }catch(RecoveryTokenException exc){
          exc._partiallyComputedNode = leftHandSide;
          throw exc;
        }
      }

      //---------------------------------------------------------------------------------------
      // ParseUnaryExpression
      //
      //  UnaryExpression :
      //    PostfixExpression |
      //    'delete' UnaryExpression |
      //    'void' UnaryExpression |
      //    'typeof' UnaryExpression |
      //    '++' UnaryExpression |
      //    '--' UnaryExpression |
      //    '+' UnaryExpression |
      //    '-' UnaryExpression |
      //    '~' UnaryExpression |
      //    '!' UnaryExpression
      //
      //---------------------------------------------------------------------------------------
      private AST ParseUnaryExpression(out bool isLeftHandSideExpr, bool isMinus){
        bool canBeAttribute = false;
        return ParseUnaryExpression(out isLeftHandSideExpr, ref canBeAttribute, isMinus, false);
      }

      private AST ParseUnaryExpression(out bool isLeftHandSideExpr, ref bool canBeAttribute, bool isMinus){
        return ParseUnaryExpression(out isLeftHandSideExpr, ref canBeAttribute, isMinus, true);
      }

      private AST ParseUnaryExpression(out bool isLeftHandSideExpr, ref bool canBeAttribute, bool isMinus, bool warnForKeyword){
        AST ast = null;
        isLeftHandSideExpr = false;
        bool dummy = false;
        Context exprCtx = null;
        AST expr = null;
        switch (this.currentToken.token){
          case JSToken.Void:
            exprCtx = this.currentToken.Clone();
            GetNextToken();
            canBeAttribute = false;
            expr = ParseUnaryExpression(out dummy, ref canBeAttribute, false);
            exprCtx.UpdateWith(expr.context);
            ast = new VoidOp(exprCtx, expr);
            break;
          case JSToken.Typeof:
            exprCtx = this.currentToken.Clone();
            GetNextToken();
            canBeAttribute = false;
            expr = ParseUnaryExpression(out dummy, ref canBeAttribute, false);
            exprCtx.UpdateWith(expr.context);
            ast = new Typeof(exprCtx, expr);
            break;
          case JSToken.Plus:
            exprCtx = this.currentToken.Clone();
            GetNextToken();
            canBeAttribute = false;
            expr = ParseUnaryExpression(out dummy, ref canBeAttribute, false);
            exprCtx.UpdateWith(expr.context);
            ast = new NumericUnary(exprCtx, expr, JSToken.Plus);
            break;
          case JSToken.Minus:
            exprCtx = this.currentToken.Clone();
            GetNextToken();
            canBeAttribute = false;
            expr = ParseUnaryExpression(out dummy, ref canBeAttribute, true);
            // deal with '-floatNumber' being parsed as a single entity (NumericLiteral) and not as a -NumericLiteral
            if (expr.context.token == JSToken.NumericLiteral){
              exprCtx.UpdateWith(expr.context);
              expr.context = exprCtx;
              ast = expr;
            }else{
              exprCtx.UpdateWith(expr.context);
              ast = new NumericUnary(exprCtx, expr, JSToken.Minus);
            }
            break;
          case JSToken.BitwiseNot:
            exprCtx = this.currentToken.Clone();
            GetNextToken();
            canBeAttribute = false;
            expr = ParseUnaryExpression(out dummy, ref canBeAttribute, false);
            exprCtx.UpdateWith(expr.context);
            ast = new NumericUnary(exprCtx, expr, JSToken.BitwiseNot);
            break;
          case JSToken.LogicalNot:
            exprCtx = this.currentToken.Clone();
            GetNextToken();
            canBeAttribute = false;
            expr = ParseUnaryExpression(out dummy, ref canBeAttribute, false);
            exprCtx.UpdateWith(expr.context);
            ast = new NumericUnary(exprCtx, expr, JSToken.LogicalNot);
            break;
          case JSToken.Delete:
            exprCtx = this.currentToken.Clone();
            GetNextToken();
            canBeAttribute = false;
            expr = ParseUnaryExpression(out dummy, ref canBeAttribute, false);
            exprCtx.UpdateWith(expr.context);
            ast = new Delete(exprCtx, expr);
            break;
          case JSToken.Increment:
            exprCtx = this.currentToken.Clone();
            GetNextToken();
            canBeAttribute = false;
            expr = ParseUnaryExpression(out dummy, ref canBeAttribute, false);
            exprCtx.UpdateWith(expr.context);
            ast = new PostOrPrefixOperator(exprCtx, expr, PostOrPrefix.PrefixIncrement);
            break;
          case JSToken.Decrement:
            exprCtx = this.currentToken.Clone();
            GetNextToken();
            canBeAttribute = false;
            expr = ParseUnaryExpression(out dummy, ref canBeAttribute, false);
            exprCtx.UpdateWith(expr.context);
            ast = new PostOrPrefixOperator(exprCtx, expr, PostOrPrefix.PrefixDecrement);
            break;
          default:
            this.noSkipTokenSet.Add(NoSkipTokenSet.s_PostfixExpressionNoSkipTokenSet);
            try{
              ast = ParseLeftHandSideExpression(isMinus, ref canBeAttribute, warnForKeyword);
            }catch(RecoveryTokenException exc){
              if (IndexOfToken(NoSkipTokenSet.s_PostfixExpressionNoSkipTokenSet, exc) == -1){
                throw exc;
              }else{
                if (exc._partiallyComputedNode == null)
                  SkipTokensAndThrow();
                else
                  ast = exc._partiallyComputedNode;
              }
            }finally{
              this.noSkipTokenSet.Remove(NoSkipTokenSet.s_PostfixExpressionNoSkipTokenSet);
            }
            ast = ParsePostfixExpression(ast, out isLeftHandSideExpr, ref canBeAttribute);
            break;
        }
        dummy = dummy;

        return ast;
      }

      //---------------------------------------------------------------------------------------
      // ParsePostfixExpression
      //
      //  PostfixExpression:
      //    LeftHandSideExpression |
      //    LeftHandSideExpression '++' |
      //    LeftHandSideExpression  '--'
      //
      //---------------------------------------------------------------------------------------
      private AST ParsePostfixExpression(AST ast, out bool isLeftHandSideExpr){
        bool canBeAttribute = false;
        return ParsePostfixExpression(ast, out isLeftHandSideExpr, ref canBeAttribute);
      }

      private AST ParsePostfixExpression(AST ast, out bool isLeftHandSideExpr, ref bool canBeAttribute){
        isLeftHandSideExpr = true;
        Context exprCtx = null;
        if (null != ast){
          if (!this.scanner.GotEndOfLine()){
            if (JSToken.Increment == this.currentToken.token){
              isLeftHandSideExpr = false;
              exprCtx = ast.context.Clone();
              exprCtx.UpdateWith(this.currentToken);
              canBeAttribute = false;
              ast = new PostOrPrefixOperator(exprCtx, ast, PostOrPrefix.PostfixIncrement);
              GetNextToken();
            }else if (JSToken.Decrement == this.currentToken.token){
              isLeftHandSideExpr = false;
              exprCtx = ast.context.Clone();
              exprCtx.UpdateWith(this.currentToken);
              canBeAttribute = false;
              ast = new PostOrPrefixOperator(exprCtx, ast, PostOrPrefix.PostfixDecrement);
              GetNextToken();
            }
          }
        }
        return ast;
      }

      //---------------------------------------------------------------------------------------
      // ParseLeftHandSideExpression
      //
      //  LeftHandSideExpression :
      //    PrimaryExpression Accessor  |
      //    'new' LeftHandSideExpression |
      //    FunctionExpression
      //
      //  PrimaryExpression :
      //    'this' |
      //    Identifier |
      //    Literal |
      //    '(' Expression ')'
      //
      //  FunctionExpression :
      //    'function' OptionalFuncName '(' FormalParameterList ')' { FunctionBody }
      //
      //  OptionalFuncName :
      //    <empty> |
      //    Identifier
      //---------------------------------------------------------------------------------------
      private AST ParseLeftHandSideExpression(){
        return ParseLeftHandSideExpression(false);
      }

      private AST ParseLeftHandSideExpression(bool isMinus){
        bool canBeAttribute = false;
        return ParseLeftHandSideExpression(isMinus, ref canBeAttribute, false);
      }

      private AST ParseLeftHandSideExpression(bool isMinus, ref bool canBeAttribute, bool warnForKeyword){
        AST ast = null;
        bool isFunction = false;
        ArrayList newContexts = null;

        // new expression
        while (JSToken.New == this.currentToken.token){
          if (null == newContexts)
            newContexts = new ArrayList(4);
          newContexts.Add(this.currentToken.Clone());
          GetNextToken();
        }

        JSToken token = this.currentToken.token;
        switch (token){
          // primary expression
          case JSToken.Identifier:
            ast = new Lookup(this.scanner.GetIdentifier(), this.currentToken.Clone());
            break;

          case JSToken.This:
            canBeAttribute = false;
            ast = new ThisLiteral(this.currentToken.Clone(), false);
            break;

          case JSToken.Super:
            canBeAttribute = false;
            ast = new ThisLiteral(this.currentToken.Clone(), true);
            break;

          case JSToken.StringLiteral:
            canBeAttribute = false;
            ast = new ConstantWrapper(this.scanner.GetStringLiteral(), this.currentToken.Clone());
            break;

          case JSToken.IntegerLiteral:
            {canBeAttribute = false;
            String number = this.currentToken.GetCode();
            Object n = Convert.LiteralToNumber(number, this.currentToken);
            if (n == null)
              n = 0;
            ast = new ConstantWrapper(n, this.currentToken.Clone());
            ((ConstantWrapper)ast).isNumericLiteral = true;
            break;}

          case JSToken.NumericLiteral:
            {canBeAttribute = false;
            String number = (isMinus) ? "-" + this.currentToken.GetCode() : this.currentToken.GetCode();
            double d = Convert.ToNumber(number, false, false, Missing.Value);
            ast = new ConstantWrapper(d, this.currentToken.Clone());
            ((ConstantWrapper)ast).isNumericLiteral = true;
            break;}

          case JSToken.True:
            canBeAttribute = false;
            ast = new ConstantWrapper(true, this.currentToken.Clone());
            break;

          case JSToken.False:
            canBeAttribute = false;
            ast = new ConstantWrapper(false, this.currentToken.Clone());
            break;

          case JSToken.Null:
            canBeAttribute = false;
            ast = new NullLiteral(this.currentToken.Clone());
            break;

          case JSToken.PreProcessorConstant:
            canBeAttribute = false;
            ast = new ConstantWrapper(this.scanner.GetPreProcessorValue(), this.currentToken.Clone());
            break;

          case JSToken.Divide:
            canBeAttribute = false;
            // could it be a regexp?
            String source = this.scanner.ScanRegExp();
            if (source != null){
              bool badRegExp = false;
              try {
                new Regex(source, RegexOptions.ECMAScript);
              } catch (System.ArgumentException) {
                // Replace the invalid source with the trivial regular expression.
                source = "";
                badRegExp = true;
              }
              String flags = this.scanner.ScanRegExpFlags();
              if (flags == null)
                ast = new RegExpLiteral(source, null, this.currentToken.Clone());
              else
                try{
                  ast = new RegExpLiteral(source, flags, this.currentToken.Clone());
                }catch (JScriptException){
                  // The flags are invalid, so use null instead.
                  ast = new RegExpLiteral(source, null, this.currentToken.Clone());
                  badRegExp = true;
                }
              if (badRegExp){
                ReportError(JSError.RegExpSyntax, true);
              }
              break;
            }
            goto default;

          // expression
          case JSToken.LeftParen:
            canBeAttribute = false;
            GetNextToken();
            this.noSkipTokenSet.Add(NoSkipTokenSet.s_ParenExpressionNoSkipToken);
            try{
              ast = ParseExpression();
              if (JSToken.RightParen != this.currentToken.token)
                ReportError(JSError.NoRightParen);
            }catch(RecoveryTokenException exc){
              if (IndexOfToken(NoSkipTokenSet.s_ParenExpressionNoSkipToken, exc) == -1)
                throw exc;
              else
                ast = exc._partiallyComputedNode;
            }finally{
              this.noSkipTokenSet.Remove(NoSkipTokenSet.s_ParenExpressionNoSkipToken);
            }
            if (ast == null) //this can only happen when catching the exception and nothing was sent up by the caller
              SkipTokensAndThrow();
            break;

          // array initializer
          case JSToken.LeftBracket:
            canBeAttribute = false;
            Context listCtx = this.currentToken.Clone();
            ASTList list = new ASTList(this.currentToken.Clone());
            GetNextToken();
            if (this.currentToken.token == JSToken.Identifier && this.scanner.PeekToken() == JSToken.Colon){
              this.noSkipTokenSet.Add(NoSkipTokenSet.s_BracketToken);
              try{
                if (this.currentToken.GetCode() == "assembly"){
                  GetNextToken(); GetNextToken();
                  return new AssemblyCustomAttributeList(this.ParseCustomAttributeList());
                }else{
                  ReportError(JSError.ExpectedAssembly);
                  SkipTokensAndThrow();
                }
              }catch(RecoveryTokenException exc){
                exc._partiallyComputedNode = new Block(listCtx);                
                return exc._partiallyComputedNode;
              }finally{
                if (this.currentToken.token == JSToken.RightBracket){
                  this.errorToken = null;
                  GetNextToken();
                }
                this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BracketToken);
              }
            }
            while (JSToken.RightBracket != this.currentToken.token){
              if (JSToken.Comma != this.currentToken.token){
                this.noSkipTokenSet.Add(NoSkipTokenSet.s_ArrayInitNoSkipTokenSet);
                try{
                  list.Append(ParseExpression(true));
                  if (JSToken.Comma != this.currentToken.token){
                    if (JSToken.RightBracket != this.currentToken.token)
                      ReportError(JSError.NoRightBracket);
                    break;
                  }
                }catch(RecoveryTokenException exc){
                  if (exc._partiallyComputedNode != null)
                    list.Append(exc._partiallyComputedNode);
                  if (IndexOfToken(NoSkipTokenSet.s_ArrayInitNoSkipTokenSet, exc) == -1){
                    listCtx.UpdateWith(CurrentPositionContext());
                    exc._partiallyComputedNode = new ArrayLiteral(listCtx, list);
                    throw exc;
                  }else{
                    if (JSToken.RightBracket == this.currentToken.token)
                      break;
                  }
                }finally{
                  this.noSkipTokenSet.Remove(NoSkipTokenSet.s_ArrayInitNoSkipTokenSet);
                }
              }else{
                list.Append(new ConstantWrapper(Missing.Value, this.currentToken.Clone()));
              }
              GetNextToken();
            }
            listCtx.UpdateWith(this.currentToken);
            ast = new ArrayLiteral(listCtx, list);
            break;

          // object initializer
          case JSToken.LeftCurly:
            canBeAttribute = false;
            Context objCtx = this.currentToken.Clone();
            GetNextToken();
            ASTList fields = new ASTList(this.currentToken.Clone());
            if (JSToken.RightCurly != this.currentToken.token){
              for (;;){
                AST field = null;
                AST value = null;

                if (JSToken.Identifier == this.currentToken.token)
                  field = new ConstantWrapper(this.scanner.GetIdentifier(), this.currentToken.Clone());
                else if (JSToken.StringLiteral == this.currentToken.token)
                  field = new ConstantWrapper(this.scanner.GetStringLiteral(), this.currentToken.Clone());
                else if (JSToken.IntegerLiteral == this.currentToken.token || JSToken.NumericLiteral == this.currentToken.token ){
                  String numberString = this.currentToken.GetCode();
                  double dValue = Convert.ToNumber(numberString, true, true, Missing.Value);
                  field = new ConstantWrapper(dValue, this.currentToken.Clone());
                  ((ConstantWrapper)field).isNumericLiteral = true;
                }else{
                  ReportError(JSError.NoMemberIdentifier);
                  field = new IdentifierLiteral("_#Missing_Field#_" + s_cDummyName++, CurrentPositionContext());
                }
                ASTList pair = new ASTList(field.context.Clone());
                GetNextToken();

                this.noSkipTokenSet.Add(NoSkipTokenSet.s_ObjectInitNoSkipTokenSet);
                try{
                  // get the value
                  if (JSToken.Colon != this.currentToken.token){
                    ReportError(JSError.NoColon, true);
                    value = ParseExpression(true);
                  }else{
                    GetNextToken();
                    value = ParseExpression(true);
                  }

                  // put the pair into the list of fields
                  pair.Append(field);
                  pair.Append(value);
                  fields.Append(pair);

                  if (JSToken.RightCurly == this.currentToken.token)
                    break;
                  else{
                    if (JSToken.Comma == this.currentToken.token)
                      GetNextToken();
                    else{
                      if (this.scanner.GotEndOfLine()){
                        ReportError(JSError.NoRightCurly);
                      }else
                        ReportError(JSError.NoComma, true);
                      SkipTokensAndThrow();
                    }
                  }
                }catch(RecoveryTokenException exc){
                  if (exc._partiallyComputedNode != null){
                    // the problem was in ParseExpression trying to determine value
                    value = exc._partiallyComputedNode;
                    pair.Append(field);
                    pair.Append(value);
                    fields.Append(pair);
                  }
                  if (IndexOfToken(NoSkipTokenSet.s_ObjectInitNoSkipTokenSet, exc) == -1){
                    exc._partiallyComputedNode = new ObjectLiteral(objCtx, fields);
                    throw exc;
                  }else{
                    if (JSToken.Comma == this.currentToken.token)
                      GetNextToken();
                    if (JSToken.RightCurly == this.currentToken.token)
                      break;
                  }
                }finally{
                  this.noSkipTokenSet.Remove(NoSkipTokenSet.s_ObjectInitNoSkipTokenSet);
                }
              }
            }
            fields.context.UpdateWith(this.currentToken);
            objCtx.UpdateWith(this.currentToken);
            ast = new ObjectLiteral(objCtx, fields);
            break;

          // function expression
          case JSToken.Function:
            canBeAttribute = false;
            ast = ParseFunction((FieldAttributes)0, true, this.currentToken.Clone(), false, false, false, false, null);
            isFunction = true;
            break;

          default:
            string identifier = JSKeyword.CanBeIdentifier(this.currentToken.token);
            if (null != identifier){
              if (warnForKeyword){
                switch (this.currentToken.token){
                  case JSToken.Boolean :
                  case JSToken.Byte :
                  case JSToken.Char :
                  case JSToken.Double :
                  case JSToken.Float :
                  case JSToken.Int :
                  case JSToken.Long :
                  case JSToken.Short :
                  case JSToken.Void :
                    break;
                  default:
                    ForceReportInfo(JSError.KeywordUsedAsIdentifier);
                    break;
                }
              }
              canBeAttribute = false;
              ast = new Lookup(identifier, this.currentToken.Clone());
            }else if (this.currentToken.token == JSToken.BitwiseAnd){ //& expr used outside of a parameter list
              ReportError(JSError.WrongUseOfAddressOf);
              this.errorToken = null;
              GetNextToken();
              return this.ParseLeftHandSideExpression(isMinus, ref canBeAttribute, warnForKeyword);
            }else{
              ReportError(JSError.ExpressionExpected);
              SkipTokensAndThrow();
            }
            break;
        }

        // can be a CallExpression, that is, followed by '.' or '(' or '['
        if (!isFunction)
          GetNextToken();

        return MemberExpression(ast, newContexts, ref canBeAttribute);
      }
      
      //-------------------------------------------------------------------------------------------
      //  ParseConstructorCall
      //
      //  ConstructorCall :
      //    'this' Arguments
      //    'super' Arguments
      //--------------------------------------------------------------------------------------------
      private AST ParseConstructorCall(Context superCtx){
        bool isSuperConstructorCall = JSToken.Super == this.currentToken.token;
        GetNextToken();
        Context listCtx = this.currentToken.Clone();
        ASTList args = new ASTList(listCtx);

        this.noSkipTokenSet.Add(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet);
        this.noSkipTokenSet.Add(NoSkipTokenSet.s_ParenToken);
        try{
          args = ParseExpressionList(JSToken.RightParen);
          GetNextToken(); //Skip the )
        }catch(RecoveryTokenException exc){
          if (exc._partiallyComputedNode != null)
            args = (ASTList)exc._partiallyComputedNode;
          if (IndexOfToken(NoSkipTokenSet.s_ParenToken, exc) == -1
              && IndexOfToken(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet, exc) == -1){
            exc._partiallyComputedNode = new ConstructorCall(superCtx, args, isSuperConstructorCall);
            throw exc;
          }else{
            if (exc._token == JSToken.RightParen)
              GetNextToken();
          }
        }finally{
          this.noSkipTokenSet.Remove(NoSkipTokenSet.s_ParenToken);
          this.noSkipTokenSet.Remove(NoSkipTokenSet.s_EndOfStatementNoSkipTokenSet);
        }

        superCtx.UpdateWith(listCtx);
        return new ConstructorCall(superCtx, args, isSuperConstructorCall);
      }

      private CustomAttributeList ParseCustomAttributeList(){
        CustomAttributeList result = new CustomAttributeList(this.currentToken.Clone());
        do{
          Context attrStart = currentToken.Clone();
          bool bAssign, canBeAttribute = true;
          AST statement = ParseUnaryExpression(out bAssign, ref canBeAttribute, false, false);
          if (canBeAttribute){
            if (statement is Lookup || statement is Member)
              result.Append(new CustomAttribute(statement.context, statement, new ASTList(null)));
            else
              result.Append(((Call)statement).ToCustomAttribute());
          }else if (this.tokensSkipped == 0)
            ReportError(JSError.SyntaxError, attrStart);
          if (this.currentToken.token == JSToken.RightBracket)
            return result;
          else if (this.currentToken.token == JSToken.Comma)
            this.GetNextToken();
          else{
            ReportError(JSError.NoRightBracketOrComma);
            SkipTokensAndThrow();
          }
        }while(true);
      }

      //---------------------------------------------------------------------------------------
      // MemberExpression
      //
      // Accessor :
      //  <empty> |
      //  Arguments Accessor
      //  '[' Expression ']' Accessor |
      //  '.' Identifier Accessor |
      //
      //  Don't have this function throwing an exception without checking all the calling sites.
      //  There is state in instance variable that is saved on the calling stack in some function
      //  (i.e ParseFunction and ParseClass) and you don't want to blow up the stack
      //---------------------------------------------------------------------------------------
      private AST MemberExpression(AST expression, ArrayList newContexts){
        bool canBeAttribute = false;
        return MemberExpression(expression, newContexts, ref canBeAttribute);
      }

      private AST MemberExpression(AST expression, ArrayList newContexts, ref bool canBeAttribute){
        bool canBeQualid;
        return MemberExpression(expression, newContexts, out canBeQualid, ref canBeAttribute);
      }

      private AST MemberExpression(AST expression, ArrayList newContexts, out bool canBeQualid, ref bool canBeAttribute){
        bool noMoreForAttr = false;
        canBeQualid = true;
        for(;;){
          this.noSkipTokenSet.Add(NoSkipTokenSet.s_MemberExprNoSkipTokenSet);
          try{
            switch (this.currentToken.token){
              case JSToken.LeftParen:
                if (noMoreForAttr)
                  canBeAttribute = false;
                else
                  noMoreForAttr = true;
                canBeQualid = false;

                ASTList args = null;
                RecoveryTokenException callError = null;
                this.noSkipTokenSet.Add(NoSkipTokenSet.s_ParenToken);
                try{
                  args = ParseExpressionList(JSToken.RightParen);
                }catch(RecoveryTokenException exc){
                  args = (ASTList)exc._partiallyComputedNode;
                  if (IndexOfToken(NoSkipTokenSet.s_ParenToken, exc) == -1)
                    callError = exc; // thrown later on
                }finally{
                  this.noSkipTokenSet.Remove(NoSkipTokenSet.s_ParenToken);
                }

                //treat eval and print specially
                if (expression is Lookup){
                  String name = expression.ToString();
                  if (name.Equals("eval")){
                    expression.context.UpdateWith(args.context);
                    if (args.count == 1)
                      expression = new Eval(expression.context, args[0], null);
                    else if (args.count > 1)
                      expression = new Eval(expression.context, args[0], args[1]);
                    else
                      expression = new Eval(expression.context, new ConstantWrapper("", CurrentPositionContext()), null);
                    canBeAttribute = false;
                  }else if (this.Globals.engine.doPrint && name.Equals("print")){
                    expression.context.UpdateWith(args.context);
                    expression = new Print(expression.context, args);
                    canBeAttribute = false;
                  }else {
                    expression = new Call(expression.context.CombineWith(args.context), expression, args, false);
                  }
                }else
                  expression = new Call(expression.context.CombineWith(args.context), expression, args, false);

                if (null != newContexts && newContexts.Count > 0){
                  ((Context)newContexts[newContexts.Count - 1]).UpdateWith(expression.context);
                  if (!(expression is Call))
                    expression = new Call((Context)newContexts[newContexts.Count - 1], expression, new ASTList(CurrentPositionContext()), false);
                  else
                    expression.context = (Context)newContexts[newContexts.Count - 1];
                  ((Call)expression).isConstructor = true;
                  newContexts.RemoveAt(newContexts.Count - 1);
                }

                if (callError != null){
                  callError._partiallyComputedNode = expression;
                  throw callError;
                }

                GetNextToken();
              break;

              case JSToken.LeftBracket:
                canBeQualid = false;
                canBeAttribute = false;
                this.noSkipTokenSet.Add(NoSkipTokenSet.s_BracketToken);
                try{
                  args = ParseExpressionList(JSToken.RightBracket);
                }catch(RecoveryTokenException exc){
                  if(IndexOfToken(NoSkipTokenSet.s_BracketToken, exc) == -1){
                    if (exc._partiallyComputedNode != null){
                      exc._partiallyComputedNode =
                         new Call(expression.context.CombineWith(this.currentToken.Clone()), expression, (ASTList)exc._partiallyComputedNode, true);
                    }else{
                      exc._partiallyComputedNode = expression;
                    }
                    throw exc;
                  }else
                    args = (ASTList)exc._partiallyComputedNode;
                }finally{
                  this.noSkipTokenSet.Remove(NoSkipTokenSet.s_BracketToken);
                }
                expression = new Call(expression.context.CombineWith(this.currentToken.Clone()), expression, args, true);

                if (null != newContexts && newContexts.Count > 0){
                  ((Context)newContexts[newContexts.Count - 1]).UpdateWith(expression.context);
                  expression.context = (Context)newContexts[newContexts.Count - 1];
                  ((Call)expression).isConstructor = true;
                  newContexts.RemoveAt(newContexts.Count - 1);
                }
                GetNextToken();
              break;

              case JSToken.AccessField:
                if (noMoreForAttr)
                  canBeAttribute = false;
                ConstantWrapper id = null;
                GetNextToken();
                if (JSToken.Identifier != this.currentToken.token){
                  string identifier = JSKeyword.CanBeIdentifier(this.currentToken.token);
                  if (null != identifier){
                    ForceReportInfo(JSError.KeywordUsedAsIdentifier);
                    id = new ConstantWrapper(identifier, this.currentToken.Clone());
                  }else{
                    ReportError(JSError.NoIdentifier);
                    SkipTokensAndThrow(expression);
                  }
                }else
                  id = new ConstantWrapper(this.scanner.GetIdentifier(), this.currentToken.Clone());
                GetNextToken();
                expression = new Member(expression.context.CombineWith(id.context), expression, id);
              break;
              default:
                if (null != newContexts){
                  while (newContexts.Count > 0){
                    ((Context)newContexts[newContexts.Count - 1]).UpdateWith(expression.context);
                    expression = new Call((Context)newContexts[newContexts.Count - 1],
                                          expression,
                                          new ASTList(CurrentPositionContext()),
                                          false);
                    ((Call)expression).isConstructor = true;
                    newContexts.RemoveAt(newContexts.Count - 1);
                  }
                }
                return expression;
            }
          }catch(RecoveryTokenException exc){
            if (IndexOfToken(NoSkipTokenSet.s_MemberExprNoSkipTokenSet, exc) != -1)
              expression = exc._partiallyComputedNode;
            else{
              Debug.Assert(exc._partiallyComputedNode == expression);
              throw exc;
            }
          }finally{
            this.noSkipTokenSet.Remove(NoSkipTokenSet.s_MemberExprNoSkipTokenSet);
          }
        }
      }

      //---------------------------------------------------------------------------------------
      // ParseExpressionList
      //
      //  Given a starting this.currentToken '(' or '[', parse a list of expression separated by
      //  ',' until matching ')' or ']'
      //---------------------------------------------------------------------------------------
      private ASTList ParseExpressionList(JSToken terminator){
        Context listCtx = this.currentToken.Clone();
        int line = this.scanner.GetCurrentLine();
        GetNextToken();
        ASTList list = new ASTList(listCtx);
        if (terminator != this.currentToken.token){
          for (;;){
            this.noSkipTokenSet.Add(NoSkipTokenSet.s_ExpressionListNoSkipTokenSet);
            try{
              if (JSToken.BitwiseAnd == this.currentToken.token){ // address of operator
                Context addressOfCtx = this.currentToken.Clone();
                GetNextToken();
                AST lhexpr = ParseLeftHandSideExpression();
                if (lhexpr is Member || lhexpr is Lookup){
                  addressOfCtx.UpdateWith(lhexpr.context);
                  list.Append(new AddressOf(addressOfCtx, lhexpr));
                }else{
                  ReportError(JSError.DoesNotHaveAnAddress, addressOfCtx.Clone());
                  list.Append(lhexpr);
                }
              }else if (JSToken.Comma == this.currentToken.token){
                list.Append(new ConstantWrapper(System.Reflection.Missing.Value, this.currentToken.Clone()));
              }else if (terminator == this.currentToken.token){
                break;
              }else
                list.Append(ParseExpression(true));

              if (terminator == this.currentToken.token)
                break;
              else{
                if (JSToken.Comma != this.currentToken.token){
                  if (terminator == JSToken.RightParen){
                    // in ASP+ it's easy to write a semicolon at the end of an expression
                    // not realizing it is going to go inside a function call
                    // (ie. Response.Write()), so make a special check here
                    if (JSToken.Semicolon == this.currentToken.token){
                      if (JSToken.RightParen == this.scanner.PeekToken()){
                        ReportError(JSError.UnexpectedSemicolon, true);
                        GetNextToken();
                        break;
                      }
                    }
                    ReportError(JSError.NoRightParenOrComma);
                  }else
                    ReportError(JSError.NoRightBracketOrComma);
                  SkipTokensAndThrow();
                }
              }
            }catch(RecoveryTokenException exc){
              if (exc._partiallyComputedNode != null)
                list.Append(exc._partiallyComputedNode);
              if (IndexOfToken(NoSkipTokenSet.s_ExpressionListNoSkipTokenSet, exc) == -1){
                exc._partiallyComputedNode = list;
                throw exc;
              }
            }finally{
              this.noSkipTokenSet.Remove(NoSkipTokenSet.s_ExpressionListNoSkipTokenSet);
            }
            GetNextToken();
          }
        }
        listCtx.UpdateWith(this.currentToken);
        return list;
      }

      //---------------------------------------------------------------------------------------
      // CreateExpressionNode
      //
      //  Create the proper AST object according to operator
      //---------------------------------------------------------------------------------------
      private AST CreateExpressionNode(JSToken op, AST operand1, AST operand2){
        Context context = operand1.context.CombineWith(operand2.context);
        switch (op){
          case JSToken.Assign:
            return new Assign(context, operand1, operand2);
          case JSToken.BitwiseAnd:
            return new BitwiseBinary(context, operand1, operand2, JSToken.BitwiseAnd);
          case JSToken.BitwiseAndAssign:
            return new BitwiseBinaryAssign(context, operand1, operand2, JSToken.BitwiseAnd);
          case JSToken.BitwiseOr:
            return new BitwiseBinary(context, operand1, operand2, JSToken.BitwiseOr);
          case JSToken.BitwiseOrAssign:
            return new BitwiseBinaryAssign(context, operand1, operand2, JSToken.BitwiseOr);
          case JSToken.BitwiseXor:
            return new BitwiseBinary(context, operand1, operand2, JSToken.BitwiseXor);
          case JSToken.BitwiseXorAssign:
            return new BitwiseBinaryAssign(context, operand1, operand2, JSToken.BitwiseXor);
          case JSToken.Comma:
            return new Comma(context, operand1, operand2);
          case JSToken.Divide:
            return new NumericBinary(context, operand1, operand2, JSToken.Divide);
          case JSToken.DivideAssign:
            return new NumericBinaryAssign(context, operand1, operand2, JSToken.Divide);
          case JSToken.Equal:
            return new Equality(context, operand1, operand2, JSToken.Equal);
          case JSToken.GreaterThan:
            return new Relational(context, operand1, operand2, JSToken.GreaterThan);
          case JSToken.GreaterThanEqual:
            return new Relational(context, operand1, operand2, JSToken.GreaterThanEqual);
          case JSToken.In:
            return new In(context, operand1, operand2);
          case JSToken.Instanceof:
            return new Instanceof(context, operand1, operand2);
          case JSToken.LeftShift:
            return new BitwiseBinary(context, operand1, operand2, JSToken.LeftShift);
          case JSToken.LeftShiftAssign:
            return new BitwiseBinaryAssign(context, operand1, operand2, JSToken.LeftShift);
          case JSToken.LessThan:
            return new Relational(context, operand1, operand2, JSToken.LessThan);
          case JSToken.LessThanEqual:
            return new Relational(context, operand1, operand2, JSToken.LessThanEqual);
          case JSToken.LogicalAnd:
            return new Logical_and(context, operand1, operand2);
          case JSToken.LogicalOr:
            return new Logical_or(context, operand1, operand2);
          case JSToken.Minus:
            return new NumericBinary(context, operand1, operand2, JSToken.Minus);
          case JSToken.MinusAssign:
            return new NumericBinaryAssign(context, operand1, operand2, JSToken.Minus);
          case JSToken.Modulo:
            return new NumericBinary(context, operand1, operand2, JSToken.Modulo);
          case JSToken.ModuloAssign:
            return new NumericBinaryAssign(context, operand1, operand2, JSToken.Modulo);
          case JSToken.Multiply:
            return new NumericBinary(context, operand1, operand2, JSToken.Multiply);
          case JSToken.MultiplyAssign:
            return new NumericBinaryAssign(context, operand1, operand2, JSToken.Multiply);
          case JSToken.NotEqual:
            return new Equality(context, operand1, operand2, JSToken.NotEqual);
          case JSToken.Plus:
            return new Plus(context, operand1, operand2);
          case JSToken.PlusAssign:
            return new PlusAssign(context, operand1, operand2);
          case JSToken.RightShift:
            return new BitwiseBinary(context, operand1, operand2, JSToken.RightShift);
          case JSToken.RightShiftAssign:
            return new BitwiseBinaryAssign(context, operand1, operand2, JSToken.RightShift);
          case JSToken.StrictEqual:
            return new StrictEquality(context, operand1, operand2, JSToken.StrictEqual);
          case JSToken.StrictNotEqual:
            return new StrictEquality(context, operand1, operand2, JSToken.StrictNotEqual);
          case JSToken.UnsignedRightShift:
            return new BitwiseBinary(context, operand1, operand2, JSToken.UnsignedRightShift);
          case JSToken.UnsignedRightShiftAssign:
            return new BitwiseBinaryAssign(context, operand1, operand2, JSToken.UnsignedRightShift);
          default:
            Debug.Assert(false);
            return null;
        }
      }

      //---------------------------------------------------------------------------------------
      // GetNextToken
      //
      //  Return the next token or peeked token if this.errorToken is not null.
      //  Usually this.errorToken is set by AddError even though any code can look ahead
      //  by assigning this.errorToken.
      //  At this point the context is not saved so if position information is needed
      //  they have to be saved explicitely
      //---------------------------------------------------------------------------------------
      private void GetNextToken(){
        if (null != this.errorToken){
          if (this.breakRecursion > 10){
      #if DEBUG
            JSParser.WriteToFile(this.errorToken);
      #endif
            this.errorToken = null;
            this.scanner.GetNextToken();
            return;
          }
          this.breakRecursion++;
          this.currentToken = this.errorToken;
          this.errorToken = null;
        }else{
          this.goodTokensProcessed++;
          this.breakRecursion = 0;
          // the scanner shares this.currentToken with the parser
          this.scanner.GetNextToken();
        }
      }

      #if DEBUG
      private static void WriteToFile(Context context){
        String filename = "error_" + (s_filenameSuffix++).ToString(CultureInfo.InvariantCulture) + ".js";
        ScriptStream.WriteLine("possible infinite recursion in parser recovery code...");
        ScriptStream.WriteLine("Please send the file " + filename + " to hermanv");

        using (System.IO.TextWriter stream = new System.IO.StreamWriter(System.IO.File.Create(filename))) {
          String code = context.source_string.Substring(0, context.EndPosition);
          stream.Write(code.ToCharArray());
          stream.Flush();
        }
      }
      #endif

      private Context CurrentPositionContext(){
        Context context = this.currentToken.Clone();
        context.endPos = (context.startPos < context.source_string.Length) ? context.startPos + 1 : context.startPos;
        return context;
      }

      //---------------------------------------------------------------------------------------
      // ReportError
      //
      //  Generate a parser error.
      //  When no context is provided the token is missing so the context is the current position
      //---------------------------------------------------------------------------------------
      private void ReportError(JSError errorId){
        ReportError(errorId, false);
      }

      //---------------------------------------------------------------------------------------
      // ReportError
      //
      //  Generate a parser error.
      //  When no context is provided the token is missing so the context is the current position
      //  The function is told whether or not next call to GetToken() should return the same
      //  token or not
      //---------------------------------------------------------------------------------------
      private void ReportError(JSError errorId, bool skipToken){
        // get the current position token
        Context context = this.currentToken.Clone();
        context.endPos = context.startPos + 1;
        ReportError(errorId, context, skipToken);
      }

      //---------------------------------------------------------------------------------------
      // ReportError
      //
      //  Generate a parser error.
      //  This is usually generated when a bad token is found, the context identifies the
      //  bad token
      //---------------------------------------------------------------------------------------
      private void ReportError(JSError errorId, Context context){
        ReportError(errorId, context, false);
      }

      //---------------------------------------------------------------------------------------
      // ReportError
      //
      //  Generate a parser error.
      //  The function is told whether or not next call to GetToken() should return the same
      //  token or not
      //---------------------------------------------------------------------------------------
      private void ReportError(JSError errorId, Context context, bool skipToken){
        Debug.Assert(context != null);
        int previousSeverity = this.Severity;
        this.Severity = (new JScriptException(errorId)).Severity;
        // EOF error is special and it's the last error we can possibly get
        if (JSToken.EndOfFile == context.token)
          EOFError(errorId); // EOF context is special
        else{
          // report the error if not in error condition and the
          // error for this token is not worse than the one for the
          // previous token
          if (this.goodTokensProcessed > 0 || this.Severity < previousSeverity)
            context.HandleError(errorId);

          // reset proper info
          if (skipToken)
            this.goodTokensProcessed = -1;
          else{
            this.errorToken = this.currentToken;
            this.goodTokensProcessed = 0;
          }
        }
      }

      //---------------------------------------------------------------------------------------
      // ForceReportInfo
      //
      //  Generate a parser error (info), does not change the error state in the parse
      //---------------------------------------------------------------------------------------
      private void ForceReportInfo(JSError errorId){
        ForceReportInfo(this.currentToken.Clone(), errorId);
      }

      //---------------------------------------------------------------------------------------
      // ForceReportInfo
      //
      //  Generate a parser error (info), does not change the error state in the parse
      //---------------------------------------------------------------------------------------
      private void ForceReportInfo(Context context, JSError errorId){
        Debug.Assert(context != null);
        context.HandleError(errorId);
      }

      //---------------------------------------------------------------------------------------
      // ForceReportInfo
      //
      //  Generate a parser error (info), does not change the error state in the parse
      //---------------------------------------------------------------------------------------
      private void ForceReportInfo(JSError errorId, bool treatAsError){
        this.currentToken.Clone().HandleError(errorId, treatAsError);
      }

      //---------------------------------------------------------------------------------------
      // EOFError
      //
      //  Create a context for EOF error. The created context points to the end of the source
      //  code. Assume the the scanner actually reached the end of file
      //---------------------------------------------------------------------------------------
      private void EOFError(JSError errorId){
        Context eofCtx = this.sourceContext.Clone();
        eofCtx.lineNumber = this.scanner.GetCurrentLine();
        eofCtx.endLineNumber = eofCtx.lineNumber;
        eofCtx.startLinePos = this.scanner.GetStartLinePosition();
        eofCtx.endLinePos = eofCtx.startLinePos;
        eofCtx.startPos = this.sourceContext.endPos;
        eofCtx.endPos++;
        eofCtx.HandleError(errorId);
      }

      //---------------------------------------------------------------------------------------
      // SkipTokensAndThrow
      //
      //  Skip tokens until one in the no skip set is found.
      //  A call to this function always ends in a throw statement that will be caught by the
      //  proper rule
      //---------------------------------------------------------------------------------------
      private void SkipTokensAndThrow(){
        SkipTokensAndThrow(null);
      }

      private void SkipTokensAndThrow(AST partialAST){
        this.errorToken = null; // make sure we go to the next token
        bool checkForEndOfLine = this.noSkipTokenSet.HasToken(JSToken.EndOfLine);
        while (!this.noSkipTokenSet.HasToken(this.currentToken.token)){
          if (checkForEndOfLine){
            if (this.scanner.GotEndOfLine()){
              this.errorToken = this.currentToken;
              throw new RecoveryTokenException(JSToken.EndOfLine, partialAST);
            }
          }
          GetNextToken();
          if (++this.tokensSkipped > c_MaxSkippedTokenNumber){
            ForceReportInfo(JSError.TooManyTokensSkipped);
            throw new EndOfFile();
          }
          if (JSToken.EndOfFile == this.currentToken.token)
            throw new EndOfFile();
        }
        this.errorToken = this.currentToken;
        // got a token in the no skip set, throw
        throw new RecoveryTokenException(this.currentToken.token, partialAST);
      }

      //---------------------------------------------------------------------------------------
      // IndexOfToken
      //
      //  check whether the recovery token is a good one for the caller
      //---------------------------------------------------------------------------------------
      private int IndexOfToken(JSToken[] tokens, RecoveryTokenException exc){
        return IndexOfToken(tokens, exc._token);
      }
        
      private int IndexOfToken(JSToken[] tokens, JSToken token){
        int i, c;
        for (i = 0, c = tokens.Length; i < c; i++)
          if (tokens[i] == token)
            break;
        if (i >= c)
          i = -1;
        else{
          // assume that the caller will deal with the token so move the state back to normal
          this.errorToken = null;
        }
        return i;
      }

      private bool TokenInList(JSToken[] tokens, JSToken token){
          return (-1 != IndexOfToken(tokens, token));
      }
          
      private bool TokenInList(JSToken[] tokens, RecoveryTokenException exc){
          return (-1 != IndexOfToken(tokens, exc._token));
      }

      //---------------------------------------------------------------------------------------
      // FromASTListToCustomAttributeList
      //
      //  Utility function that takes a list of AST nodes (Call and Lookup nodes) and makes
      //  a CustomAttributeList. It is invoked once a list of calls and lookups is unambiguosly
      //  parsed as a custom attribute list
      //---------------------------------------------------------------------------------------
      private CustomAttributeList FromASTListToCustomAttributeList(ArrayList attributes){
        CustomAttributeList customAttributes = null;
        if (attributes != null && attributes.Count > 0)
          customAttributes = new CustomAttributeList(((AST)attributes[0]).context);
          for (int i = 0, n = attributes.Count; i < n; i++){
            ASTList args = new ASTList(null);
            if (attributes[i] is Lookup || attributes[i] is Member)
              customAttributes.Append(new CustomAttribute(((AST)attributes[i]).context, (AST)attributes[i], args));
            else
              customAttributes.Append(((Call)attributes[i]).ToCustomAttribute());
          }

        return customAttributes;
      }

      internal bool HasAborted{
        get{
          return this.tokensSkipped > c_MaxSkippedTokenNumber;
        }
      }
    }

    // helper classes
    //***************************************************************************************
    //
    //***************************************************************************************
    internal class AstListItem{
      internal AstListItem _prev;
      internal AST _term;

      internal AstListItem(AST term, AstListItem prev){
        _prev = prev;
        _term = term;
      }

    }

    //***************************************************************************************
    //
    //***************************************************************************************
    internal class OpListItem{
      internal OpListItem _prev;
      internal JSToken _operator;
      internal OpPrec _prec;
      //internal OpAssoc _assoc;

      internal OpListItem(JSToken op, OpPrec prec, /*OpAssoc assoc,*/ OpListItem prev){
        _prev = prev;
        _operator = op;
        _prec = prec;
        //_assoc = assoc;
      }
    }

    //***************************************************************************************
    //
    //***************************************************************************************
    [Serializable]
    public class ParserException : Exception{
      internal ParserException() : base(JScriptException.Localize("Parser Exception", CultureInfo.CurrentUICulture)){
      }
    }


    internal class RecoveryTokenException : ParserException{
      internal JSToken _token;
      internal AST _partiallyComputedNode;

      internal RecoveryTokenException(JSToken token, AST partialAST) : base(){
        _token = token;
        _partiallyComputedNode = partialAST;
      }
    }

    [Serializable]
    public class EndOfFile : ParserException{
      internal EndOfFile() : base(){
      }
    }

    //***************************************************************************************
    // NoSkipTokenSet
    //
    //  This class is a possible implementation of the no skip token set. It relies on the
    //  fact that the array passed in are static. Should you change it, this implementation
    //  should change as well.
    //  It keeps a linked list of token arrays that are passed in during parsing, on error
    //  condition the list is traversed looking for a matching token. If a match is found
    //  the token should not be skipped and an exception is thrown to let the proper
    //  rule deal with the token
    //***************************************************************************************
    internal class NoSkipTokenSet{
      TokenSetListItem _tokenSet;

      internal NoSkipTokenSet(){
        _tokenSet = null;
      }

      internal void Add(JSToken[] tokens){
        _tokenSet = new TokenSetListItem(tokens, _tokenSet);
      }

      internal void Remove(JSToken[] tokens){
        TokenSetListItem curr = _tokenSet, prev = null;
        while (curr != null){
          if (curr._tokens == tokens){
            if (prev == null){
              Debug.Assert(_tokenSet == curr);
              _tokenSet = _tokenSet._next;
            }else{
              prev._next = curr._next;
            }
            return;
          }
          prev = curr;
          curr = curr._next;
        }
        Debug.Assert(false, "Token set not in no skip token");
      }

      internal bool HasToken(JSToken token){
        TokenSetListItem curr = _tokenSet;
        while (curr != null){
          for (int i = 0, c = curr._tokens.Length; i < c; i++){
            if (curr._tokens[i] == token)
              return true;
          }
          curr = curr._next;
        }
        return false;
      }

      // list of static no skip token set for specifc rules
      internal static readonly JSToken[] s_ArrayInitNoSkipTokenSet         = new JSToken[]{JSToken.RightBracket,
                                                                                           JSToken.Comma};
      internal static readonly JSToken[] s_BlockConditionNoSkipTokenSet    = new JSToken[]{JSToken.RightParen,
                                                                                           JSToken.LeftCurly,
                                                                                           JSToken.EndOfLine};
      internal static readonly JSToken[] s_BlockNoSkipTokenSet             = new JSToken[]{JSToken.RightCurly};
      internal static readonly JSToken[] s_BracketToken                    = new JSToken[]{JSToken.RightBracket};
      internal static readonly JSToken[] s_CaseNoSkipTokenSet              = new JSToken[]{JSToken.Case,
                                                                                           JSToken.Default,
                                                                                           JSToken.Colon,
                                                                                           JSToken.EndOfLine};
      internal static readonly JSToken[] s_ClassBodyNoSkipTokenSet         = new JSToken[]{JSToken.Class,
                                                                                           JSToken.Interface,
                                                                                           JSToken.Enum,
                                                                                           JSToken.Function,
                                                                                           JSToken.Var,
                                                                                           JSToken.Const,
                                                                                           JSToken.Static,
                                                                                           JSToken.Public,
                                                                                           JSToken.Private,
                                                                                           JSToken.Protected};
      internal static readonly JSToken[] s_InterfaceBodyNoSkipTokenSet      = new JSToken[]{JSToken.Enum,
                                                                                           JSToken.Function,
                                                                                           JSToken.Public,
                                                                                           JSToken.EndOfLine,
                                                                                           JSToken.Semicolon};
      internal static readonly JSToken[] s_ClassExtendsNoSkipTokenSet      = new JSToken[]{JSToken.LeftCurly,
                                                                                           JSToken.Implements};
      internal static readonly JSToken[] s_ClassImplementsNoSkipTokenSet   = new JSToken[]{JSToken.LeftCurly,
                                                                                           JSToken.Comma};
      internal static readonly JSToken[] s_DoWhileBodyNoSkipTokenSet       = new JSToken[]{JSToken.While};
      internal static readonly JSToken[] s_EndOfLineToken                  = new JSToken[]{JSToken.EndOfLine};
      internal static readonly JSToken[] s_EndOfStatementNoSkipTokenSet    = new JSToken[]{JSToken.Semicolon,
                                                                                           JSToken.EndOfLine};
      internal static readonly JSToken[] s_EnumBaseTypeNoSkipTokenSet      = new JSToken[]{JSToken.LeftCurly};
      internal static readonly JSToken[] s_EnumBodyNoSkipTokenSet          = new JSToken[]{JSToken.Identifier};
      internal static readonly JSToken[] s_ExpressionListNoSkipTokenSet    = new JSToken[]{JSToken.Comma};
      internal static readonly JSToken[] s_FunctionDeclNoSkipTokenSet      = new JSToken[]{JSToken.RightParen,
                                                                                           JSToken.LeftCurly,
                                                                                           JSToken.Comma};
      internal static readonly JSToken[] s_IfBodyNoSkipTokenSet            = new JSToken[]{JSToken.Else};
      internal static readonly JSToken[] s_MemberExprNoSkipTokenSet        = new JSToken[]{JSToken.LeftBracket,
                                                                                           JSToken.LeftParen,
                                                                                           JSToken.AccessField};
      internal static readonly JSToken[] s_NoTrySkipTokenSet               = new JSToken[]{JSToken.Catch,
                                                                                           JSToken.Finally};
      internal static readonly JSToken[] s_ObjectInitNoSkipTokenSet        = new JSToken[]{JSToken.RightCurly,
                                                                                           JSToken.Comma};
      internal static readonly JSToken[] s_PackageBodyNoSkipTokenSet       = new JSToken[]{JSToken.Class,
                                                                                           JSToken.Interface,
                                                                                           JSToken.Enum};
      internal static readonly JSToken[] s_ParenExpressionNoSkipToken      = new JSToken[]{JSToken.RightParen};
      internal static readonly JSToken[] s_ParenToken                      = new JSToken[]{JSToken.RightParen};
      internal static readonly JSToken[] s_PostfixExpressionNoSkipTokenSet = new JSToken[]{JSToken.Increment,
                                                                                           JSToken.Decrement};
      internal static readonly JSToken[] s_StartBlockNoSkipTokenSet        = new JSToken[]{JSToken.LeftCurly};
      internal static readonly JSToken[] s_StartStatementNoSkipTokenSet    = new JSToken[]{JSToken.LeftCurly,
                                                                                           JSToken.Var,
                                                                                           JSToken.Const,
                                                                                           JSToken.If,
                                                                                           JSToken.For,
                                                                                           JSToken.Do,
                                                                                           JSToken.While,
                                                                                           JSToken.With,
                                                                                           JSToken.Switch,
                                                                                           JSToken.Try};
      internal static readonly JSToken[] s_SwitchNoSkipTokenSet            = new JSToken[]{JSToken.Case,
                                                                                           JSToken.Default};
      internal static readonly JSToken[] s_TopLevelNoSkipTokenSet          = new JSToken[]{JSToken.Package,
                                                                                           JSToken.Class,
                                                                                           JSToken.Interface,
                                                                                           JSToken.Enum,
                                                                                           JSToken.Function,
                                                                                           JSToken.Import};
      internal static readonly JSToken[] s_VariableDeclNoSkipTokenSet      = new JSToken[]{JSToken.Comma,
                                                                                           JSToken.Semicolon};

      private class TokenSetListItem{
        internal TokenSetListItem _next;
        internal JSToken[] _tokens;

        internal TokenSetListItem(JSToken[] tokens, TokenSetListItem next){
          _next = next;
          _tokens = tokens;
        }

      }
    }

}
