//------------------------------------------------------------------------------
// <copyright file="parse.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace MyC
{
using System;
using System.Text;

class Parse
{
private Io io;
private Tok tok;
private Emit emit;
VarList paramvar;
VarList localvar;
VarList staticvar;
int label_count;
private bool mainseen = false;

void dataClass(Var e)
  {
#if DEBUG
  Console.WriteLine("dataClass1 token=["+tok+"]\n");
#endif
  int id = tok.getId();
  if (id == Tok.T_EXTERN || id == Tok.T_STATIC || id == Tok.T_AUTO)
    {
    e.setClassId(tok.getId());
    tok.scan();
#if DEBUG
    Console.WriteLine("dataClass2 token=["+tok+"]\n");
#endif
    }
  else
    {
    e.setClassId(Tok.T_DEFCLASS); /* default to current context */
    }
  }

void dataType(Var e)
  {
  int id = tok.getId();
#if DEBUG
  Console.WriteLine("dataType1 token=["+tok+"]\n");
#endif
  e.setSign(Tok.T_SIGNED);
  if (id == Tok.T_SIGNED || id == Tok.T_UNSIGNED)
    {
    e.setSign(id);
    tok.scan();
    id = tok.getId();
#if DEBUG
    Console.WriteLine("dataType2 token=["+tok+"]\n");
#endif
    }
  if (id == Tok.T_VOID || id == Tok.T_INT)
    {
    e.setTypeId(id);
    tok.scan();
#if DEBUG
    Console.WriteLine("dataType3 token=["+tok+"]\n");
#endif
    }
  else if (id == Tok.T_LONG)
    Io.ICE("Unhandled type LONG");
  else if (id == Tok.T_CHAR)
    Io.ICE("Unhandled type CHAR");
  else if (id == Tok.T_FLOAT)
    Io.ICE("Unhandled type FLOAT");
  else if (id == Tok.T_DOUBLE)
    Io.ICE("Unhandled type DOUBLE");
  else
    e.setTypeId(Tok.T_DEFTYPE);
  }

VarList paramList()
  {
  VarList v;
  tok.scan();
  if (tok.getFirstChar() != '(')
    io.Abort("Expected '('");

  v = new VarList();		/* init the param list */
  tok.scan();			/* get the next token */
  while (tok.getFirstChar() != ')')
    {
    Var e = new Var();
    e.setClassId(Tok.T_PARAM);	/* mark this as a parameter */
    dataType(e);		/* get the specified datatype */
    e.setName(tok.getValue()); /* copy the variable name */
    v.add(e);			/* add parameter to param list */
    tok.scan();
    if (tok.getFirstChar() == ',')	/* if more params */
      tok.scan();		/* get the next token */
    }
  tok.scan();		/* move to the next token */
  return v;
  }

void outerDecl()
  {
  Var e = new Var();
#if DEBUG
  Console.WriteLine("outerDecl token=["+tok+"]\n");
#endif

  CommentHolder();	/* mark the position in insn stream */

  dataClass(e);
  dataType(e);

  if (io.getNextChar() == '(')
    declFunc(e);
  else
    declOuter(e);
  }

void prolog()
  {
  emit = new Emit(io);
  emit.BeginModule();		// need assembly module
  emit.BeginClass();
  }

void epilog()
  {
  emit.EndClass();
  emit.EndModule();
  }

void addInner(Var e)
  {
  /*
   * validate this declaration as a potential variable
   */
  int id = e.getClassId();
#if DEBUG
  Console.WriteLine("addInner e=["+e+"]\n");
#endif
  if (id == Tok.T_EXTERN || id == Tok.T_STATIC)
    io.Abort("Cannot allocate static within a method\n");
  if (paramvar.FindByName(e.getName()) != null) /* cannot redefine param name */
    io.Abort("Cannot redefine parameter name '" + e.getName() + "'\n");
  if (localvar.FindByName(e.getName()) != null) /* check not already set */
    io.Abort("Local variable '" + e.getName() + "' already defined\n");
  localvar.add(e);
  }

/*
 * parse an inner declaration
 */
void declInner()
  {
  while (tok.IsDeclKeyword())
    {
    Var e = new Var();
#if DEBUG
    Console.WriteLine("declInner1 token=["+tok+"]\n");
#endif

    CommentHolder();	/* mark comment position in insn stream */

    dataClass(e);		// parse the data class (static, auto, etc)
    dataType(e);		// parse the type (int, unsigned, etc)
    e.setName(tok.getValue()); // save the name in var
#if DEBUG
    Console.WriteLine("declInner2.0 token=["+tok+"]\n");
    Console.WriteLine("declInner2.0 e=["+e+"]\n");
#endif
    addInner(e);			/* add this variable */
    /*
     * loop while there are additional variable names
     */
    while (io.getNextChar() == ',')
      {
      tok.scan();
      if (tok.getFirstChar() != ',')
	io.Abort("Expected ','");
      tok.scan();
      if (tok.getId() != Tok.T_IDENT)
	io.Abort("Expected identifier");
      e.setName(tok.getValue()); /* use value as the variable name */
#if DEBUG
      Console.WriteLine("declInner2.1 token=["+tok+"]\n");
      Console.WriteLine("declInner2.1 e=["+e+"]\n");
#endif
      addInner(e);		/* reuse the class & type */
      }
    /*
     * move beyond end of statement indicator
     */
    tok.scan();
    if (tok.getFirstChar() != ';')
	io.Abort("Expected ';'");
    CommentFill();
    tok.scan();		/* get token ready nor next parser step */
#if DEBUG
    Console.WriteLine("declInner3 token=["+tok+"]\n");
#endif
    }
  }

void addOuter(Var e)
  {
#if DEBUG
  Console.WriteLine("addOuter e=["+e+"]\n");
#endif
  /*
   * validate this declaration as a potential variable
   */
  if (e.getClassId() == Tok.T_AUTO)
    io.Abort("?Cannot allocate automatic variable outside a function\n");
  if (staticvar.FindByName(e.getName()) != null)
    io.Abort("?Cannot redefine static name '" + e.getName() + "'\n");
  staticvar.add(e);
  }

/*
 * declOuter presumes that class & type have already been parsed
 * this is done to distinguish between a function declaration and a
 * variable declaration
 */
void declOuter(Var e)
  {
#if DEBUG
  Console.WriteLine("declOuter1 token=["+tok+"]\n");
#endif
  e.setName(tok.getValue());	/* use value as the variable name */
  addOuter(e);		/* add this variable */
  emit.FieldDef(e);		/* issue the declaration */
  if (e.getClassId() == Tok.T_DEFCLASS)
    e.setClassId(Tok.T_STATIC);	/* make sure it knows its storage class */

  /*
   * loop while there are additional variable names
   */
  while (io.getNextChar() == ',')
    {
    tok.scan();
    if (tok.getFirstChar() != ',')
	io.Abort("Expected ','");
    tok.scan();
    if (tok.getId() != Tok.T_IDENT)
	io.Abort("Expected identifier");
    e.setName(tok.getValue()); /* use value as the variable name */
    addOuter(e);		/* add this variable */
    emit.FieldDef(e);		/* issue the declaration */
    if (e.getClassId() == Tok.T_DEFCLASS)
      e.setClassId(Tok.T_STATIC);	/* make sure it knows its storage class */
    }
  /*
   * move beyond end of statement indicator
   */
  tok.scan();
  if (tok.getFirstChar() != ';')
    io.Abort("Expected ';'");
  CommentFill();
  tok.scan();
#if DEBUG
  Console.WriteLine("declOuter2 token=["+tok+"]\n");
#endif
  }

void declFunc(Var e)
  {
#if DEBUG
  Console.WriteLine("declFunc token=["+tok+"]\n");
#endif
  CommentHolder();	// start new comment
  e.setName(tok.getValue()); /* value is the function name */
  if (e.getName().Equals("main"))
    {
    if (Io.gendll)
      io.Abort("Using main entrypoint when generating a DLL");
    mainseen = true;
    }
  staticvar.add(e);		/* add function name to static VarList */
  paramvar = paramList();	// track current param list
  e.setParams(paramvar);	// and set it in func var
  localvar = new VarList();	// track new local parameters
  CommentFillPreTok();

  emit.FuncBegin(e);
  if (tok.getFirstChar() != '{')
    io.Abort("Expected '{'");
  blockOuter(null, null);
  emit.FuncEnd();
  emit.IL();
  if (Io.genlist)
    emit.LIST();
  emit.Finish();
  }

/*
 * parse the outerBlock seperately from inner block.
 * specifically parse the declarations at the beginning of an outerblock
 * the variable r tracks if the last statement was a return or not
 */
void blockOuter(String olabel, String ilabel)
  {
  bool r = false;
#if DEBUG
  Console.WriteLine("blockOuter1 token=["+tok+"]\n");
#endif
  CommentToken();	/* mark the position in insn stream */
  tok.scan();
#if DEBUG
  Console.WriteLine("blockOuter2 token=["+tok+"]\n");
#endif
  declInner();
  emit.LocalVars(localvar);

  while (tok.getFirstChar() != '}')
    {
#if DEBUG
    Console.WriteLine("blockOuter3 token=["+tok+"]\n");
#endif
    if (io.EOF())
      io.Abort("Expected statement, end of file encountered");
    r = false;
    switch (tok.getId())
      {
      case Tok.T_IF: fcIf(olabel, ilabel); continue; /* fcIf updates tok */
      case Tok.T_WHILE: fcWhile(); break;
      case Tok.T_FOR: fcFor(); break;
      case Tok.T_BREAK: fcBreak(olabel); break;
      case Tok.T_CONTINUE: fcContinue(ilabel); break;
      case Tok.T_RETURN: fcReturn(); r = true; break;
      default: statement(); break;
      }
#if DEBUG
    Console.WriteLine("blockOuter4 token=["+tok+"]\n");
#endif
    }
  if (!r)		/* if no outerscope return specified, try to default */
    {
    Var e = emit.GetFunc();	/* get function def */
    if (e.getTypeId() != Tok.T_VOID)	/* make sure we can default to void */
      {
      if (!e.getName().Equals("main")) /* is it not main? */
	io.Abort("No return value specified for non void function");
      emit.LoadConst("0");	/* special case a dummy ret value */
      }
    emit.Ret();
    }
  CommentToken();		/* mark the position in insn stream */
  tok.scan(); /* read beyond end of block */
#if DEBUG
  Console.WriteLine("blockOuter5 token=["+tok+"]\n");
#endif
  }

/* recognize and translate a statement block */
void blockInner(String olabel, String ilabel)
  {
#if DEBUG
  Console.WriteLine("blockInner1 token=["+tok+"]\n");
#endif
  if (tok.getFirstChar() == '{')	/* if begin, then get next token */
    {
    CommentToken();	/* mark the position in insn stream */
    tok.scan();
#if DEBUG
    Console.WriteLine("blockInner2 token=["+tok+"]\n");
#endif
    if (tok.IsDeclKeyword())
      io.Abort("Illegal inner declaration of variable");
    while (tok.getFirstChar() != '}')
      {
      switch (tok.getId())
	{
	case Tok.T_IF: fcIf(olabel, ilabel); continue; /* fcIf updates tvp */
	case Tok.T_WHILE: fcWhile(); break;
	case Tok.T_FOR: fcFor(); break;
	case Tok.T_BREAK: fcBreak(olabel); break;
	case Tok.T_CONTINUE: fcContinue(ilabel); break;
	case Tok.T_RETURN: fcReturn(); break;
	default: statement(); break;
	}
#if DEBUG
      Console.WriteLine("blockInner3 token=["+tok+"]\n");
#endif
      }
    CommentToken(); /* mark the begin of source comment */
    tok.scan(); /* read beyond end of blockInner */
    }
  else
    {
    switch (tok.getId())
      {
      case Tok.T_IF: fcIf(olabel, ilabel); break;
      case Tok.T_WHILE: fcWhile(); break;
      case Tok.T_FOR: fcFor(); break;
      case Tok.T_BREAK: fcBreak(olabel); break;
      case Tok.T_CONTINUE: fcContinue(ilabel); break;
      case Tok.T_RETURN: fcReturn(); break;
      default: statement(); break;
      }
    }
  }

/*
 * the flow of control routines
 */

/* gen a unique label */
String newLabel()
  {
  StringBuilder sb = new StringBuilder("L");
  sb.Append(label_count++);
  return (sb.ToString());
  }

/*
 * parse an if construct
 * form is:
   *	if (e) stmt-block else stmt-block
   */
void fcIf(String olabel, String ilabel)
  {
  String label1;
  String label2;

  CommentHolder();	/* mark the position in insn stream */
  tok.scan();
  if (tok.getFirstChar() != '(')
    io.Abort("Expected '('");

  boolExpr();
  CommentFillPreTok();

  label1 = newLabel();
  label2 = String.Copy(label1);

  emit.Branch("brfalse", label1);

  blockInner(olabel, ilabel); /* parse block or single statement */

#if DEBUG
  Console.WriteLine("fcIf token=["+tok+"]\n");
#endif
  if (tok.getId() == Tok.T_ELSE)	/* sniff for else clause */
    {
    label2 = newLabel();
    emit.Branch("br", label2);

    emit.Label(label1);
    tok.scan();		/* get next token after else */
    blockInner(olabel, ilabel); /* outer label, top of loop */
    }
  emit.Label(label2);
  }

  /* parse and translate a while statement */
void fcWhile()
  {
  String label1 = newLabel();
  String label2 = newLabel();

  CommentHolder();	/* mark the position in insn stream */
  emit.Label(label1);
  tok.scan();
  if (tok.getFirstChar() != '(')
    io.Abort("Expected '('");

  boolExpr();
  CommentFillPreTok();
  emit.Branch("brfalse", label2);

  blockInner(label2, label1);	/* outer label, top of loop */
  emit.Branch("br", label1);

  emit.Label(label2);
  }

void fcFor()
  {
  String label1 = newLabel();
  String label2 = newLabel();
  String label3 = newLabel();
  String label4 = newLabel();

  CommentHolder();	/* mark the position in insn stream */
  tok.scan();
  if (tok.getFirstChar() != '(')
    io.Abort("Expected '('");

  /*
   * the assignment statement
   */
  tok.scan();			/* i=0 */
  if (tok.getFirstChar() == ';')	/* allow null assignment */
    {
    CommentFill();
    tok.scan();		/* move beyond null statement */
    }
  else
    {
    statement();		/* parse the statement */
			      /* statement also closes the comment */
    }

  /*
   * the logical test
   */
  emit.Label(label1);
  CommentHolder();			/* mark the position in insn stream */

  if (tok.getFirstChar() == ';')	/* allow null test */
    {
    CommentFill("<no test>");
    tok.scan();		/* move past the semi */
    }
  else
    {
    boolExpr();		/* i<max */
    if (tok.getFirstChar() != ';')
      io.Abort("Expected ';'");
    CommentFill();
    tok.scan();
    emit.Branch("brfalse", label4); /* test for result */
    }
  emit.Branch("br", label3);	/* emit. branch to statement block */

  /*
   * the increment statement
   */
  CommentHolder();			/* mark comment position in insn stream */

  emit.Label(label2);
  if (tok.getFirstChar() == ')')	/* allow null increment statement */
    {
    CommentFill();		/* end comment here */
    tok.scan();			/* move to next token */
    }
  else
    {
    /*
     * this is similar to assign, but ends with ')'
     */
    String varname = tok.getValue();
    tok.scan();
    if (tok.getFirstChar() != '=')
      io.Abort("Expected '='");
    tok.scan();
    boolExpr();
    emit.Store(lookup_name(varname));
    if (tok.getFirstChar() != ')')
      io.Abort("Expected ')'");
    CommentFill();		/* end comment here */
    tok.scan();			/* move to next token */
    }
  emit.Branch("br", label1);

  /*
   * the statement block
   */
  emit.Label(label3);
  blockInner(label4, label2);	/* outer label, top of loop */
  emit.Branch("br", label2);	/* go to 3rd term (usually incr) */

  emit.Label(label4);
  }

/* recognize and translate a break */
void fcBreak(String olabel)
  {
  CommentHolder();	/* mark the position in insn stream */
  if (olabel != null)
    emit.Branch("br", olabel);
  else
    io.Abort("No loop to break from");
  tok.scan();
  if (tok.getFirstChar() != ';')
    io.Abort("Expected ';'");
  CommentFill();
  tok.scan();
  }

/* parse a continue */
void fcContinue(String ilabel)
  {
  CommentHolder();	/* mark the position in insn stream */
  if (ilabel != null)
    emit.Branch("br", ilabel);
  else
    io.Abort("No loop to break from");
  tok.scan();
  if (tok.getFirstChar() != ';')
    io.Abort("Expected ';'");
  CommentFill();
  tok.scan();
  }

/* parse a return */
void fcReturn()
  {
  Var e = emit.GetFunc();
  CommentHolder();	/* mark the position in insn stream */
  tok.scan();		/* get the return value */
  if (tok.getFirstChar() == ';')	/* if end of statment */
    {
    if (e.getTypeId() != Tok.T_VOID)
      io.Abort("Expected value for return type");
    }
  else
    {
    if (e.getTypeId() == Tok.T_VOID)
      io.Abort("Unexpected value for void return type");
    boolExpr();		/* parse as expression */
    }
  emit.Ret();			/* issue the return (value is on stack) */
  CommentFill();
  tok.scan();			/* move past the semi */
  }

/*
 * expression logic
 */

void ident()
  {
#if DEBUG
  Console.WriteLine("ident token=["+tok+"]\n");
#endif
  if (io.getNextChar() == '(')	/* this must be a function call */
    {
    String vname = tok.getValue();
    Var e;
    tok.scan();		/* eat the left paren */
    tok.scan();		/* get the first parameter (or none) */
    if (tok.getFirstChar() != ')')	/* if there are params */
      {
      boolExpr();		/* parse first param */
      while (tok.getFirstChar() == ',') /* while there are more params */
	{
	tok.scan();		/* get the next token */
	boolExpr();		/* parse now as an expression */
	}
      if (tok.getFirstChar() != ')')	/* must find closing paren */
	io.Abort("Expected ')'");
      tok.scan();		/* move to next token */
      }
    e = staticvar.FindByName(vname); /* find the symbol */
    if (e.getTypeId() == Tok.T_VOID)	/* if function value is void */
      io.Abort("Using void function where expecting a value");
    emit.Call(e);		/* construct the call */
    }
  else
    {
    emit.Load(lookup_name(tok.getValue())); /* load this symbol */
    tok.scan();		/* get the next token */
    }
  }

void factor()
  {
#if DEBUG
  Console.WriteLine("factor token=["+tok+"]\n");
#endif
  if (tok.getFirstChar() == '(')
    {
    tok.scan();			/* get next token */
    boolExpr();
    if (tok.getFirstChar() != ')')
      io.Abort("Expected ')'");
    tok.scan();			/* get next token */
    }
  else if (tok.getId() == Tok.T_IDENT)
    {
    ident();
    }
  else
    {
    emit.LoadConst(tok.getValue());
    tok.scan();			/* get next token */
    }
  }

void unaryFactor()
  {
  if (tok.getId() == Tok.T_ADD_OP) /* unary plus */
    {
    tok.scan();		/* get the next token */
    factor();			/* process the factor */
    return;
    }
  if (tok.getId() == Tok.T_SUB_OP) /* unary minus */
    {
    tok.scan();
    if (tok.getId() == Tok.T_DIGITS)
      {
      StringBuilder sb = new StringBuilder("-"); /* recreate the String */
      sb.Append(tok.getValue());
      emit.LoadConst(sb.ToString());	/* emit. load const for String */
      tok.scan();
      return;
      }
    factor();			/* process the factor */
    emit.Insn("neg");
    return;
    }
  factor();			/* no unary, so pass this token down */
  }

void termMult()
  {
  tok.scan();
  factor();
  emit.Insn("mul");
  }

void termDiv()
  {
  tok.scan();
  factor();
  emit.Insn("div");
  }

/* completion of term processing (called by term and firstTerm) */
void term1()
  {
  while (tok.getId() == Tok.T_MUL_OP || tok.getId() == Tok.T_DIV_OP)
    {
    switch (tok.getId())
      {
      case Tok.T_MUL_OP: termMult(); break;
      case Tok.T_DIV_OP: termDiv(); break;
      }
    }
  }

/* parse and translate math term */
void term()
  {
  factor();
  term1();
  }

/* check term with possible leading sign */
void firstTerm()
  {
  unaryFactor();
  term1();
  }

/* translate an add */
void exprAdd()
  {
  tok.scan();
  term();
  emit.Insn("add");
  }

/* translate a subtract */
void exprSub()
  {
  tok.scan();
  term();
  emit.Insn("sub");
  }

/* parse an expression */
void mathExpr()
  {
#if DEBUG
  Console.WriteLine("mathExpr1 token=["+tok+"]\n");
#endif
  firstTerm();
#if DEBUG
  Console.WriteLine("mathExpr2 token=["+tok+"]\n");
#endif
  while (tok.getId() == Tok.T_ADD_OP || tok.getId() == Tok.T_SUB_OP)
    {
    switch (tok.getId())
      {
      case Tok.T_ADD_OP: exprAdd(); break;
      case Tok.T_SUB_OP: exprSub(); break;
      }
    }
  }

/* recognize and translate a relational "equal" */
void relEQ()
  {
  tok.scan();
  mathExpr();
  emit.Insn("ceq");
  }

/* recognize and translate a relational "not equal" */
void relNEQ()
  {
  tok.scan();
  mathExpr();
  emit.Insn("ceq");
  emit.LoadConst("1");
  emit.Insn("ceq");
  }

/* recognize and translate a relational "less than" */
void relLTR()
  {
  tok.scan();
  mathExpr();
  emit.Insn("clt");
  }

/* recognize and translate a relational "greater than" */
void relGTR()
  {
  tok.scan();
  mathExpr();
  emit.Insn("cgt");
  }

void relGEQ()
  {
  tok.scan();
  mathExpr();
  emit.Insn("clt");
  emit.LoadConst("0");
  emit.Insn("ceq");
  }

void relLEQ()
  {
  tok.scan();
  mathExpr();
  emit.Insn("cgt");
  emit.LoadConst("0");
  emit.Insn("ceq");
  }

/* parse a relation */
void relExpr()
  {
  mathExpr();
#if DEBUG
  Console.WriteLine("relExpr token=["+tok+"]\n");
#endif
  switch (tok.getId())
    {
    case Tok.T_EQ_OP: relEQ(); break;
    case Tok.T_NEQ_OP: relNEQ(); break;
    case Tok.T_LT_OP: relLTR(); break;
    case Tok.T_GT_OP: relGTR(); break;
    case Tok.T_GE_OP: relGEQ(); break;
    case Tok.T_LE_OP: relLEQ(); break;
    }
  }

/* parse factor with NOT */
void notFactor()
  {
#if DEBUG
  Console.WriteLine("notFactor token=["+tok+"]\n");
#endif
  if (tok.getId() == Tok.T_NOT_OP)
    {
    relExpr();
    emit.Insn("not");
    }
  else
    relExpr();
  }

/* recognize and translate a boolean OR */
void boolOr()
  {
  tok.scan();
  termBool();
  emit.Insn("or");
  }

/* recognize and transate an exclusize or (XOR) */
void boolXor()
  {
  tok.scan();
  termBool();
  emit.Insn("xor");
  }

/* recognize and transate a bitwise AND */
void boolAnd()
  {
  tok.scan();
  termBool();
  emit.Insn("and");
  }

/* parse an translate a boolean term */
void termBool()
  {
  notFactor();
  while (tok.getId() == Tok.T_OR_OP
	 || tok.getId() == Tok.T_XOR_OP
	 || tok.getId() == Tok.T_AND_OP)
    {
    switch (tok.getId())
      {
      case Tok.T_OR_OP: boolOr(); break;
      case Tok.T_XOR_OP: boolXor(); break;
      case Tok.T_AND_OP: boolAnd(); break;
      }
#if DEBUG
    Console.WriteLine("termBool token=["+tok+"]\n");
#endif
    }
  }

/* parse and translate a boolean expression */
void boolExpr()
  {
  termBool();
  int id = tok.getId();
  if (id == Tok.T_LOG_OR_OP || id == Tok.T_LOG_AND_OP)
    {
    String label1 = newLabel();
    String label2 = newLabel();
    String label3 = newLabel();
    while (true)
      {
      id = tok.getId();
      if (id == Tok.T_LOG_AND_OP)
	emit.Branch("brfalse", label1);	// optimize the logical and
      else if (id == Tok.T_LOG_OR_OP)
	emit.Branch("brtrue", label2); // optimize the logical or
      else
	break;
      tok.scan();
      termBool();
      }
    emit.Branch("brtrue", label2);
    emit.Label(label1);	// false path
    emit.LoadConst("0");
    emit.Branch("br",label3);
    emit.Label(label2);	// true path
    emit.LoadConst("1");
    emit.Label(label3);	// common path
    }
  }

Var lookup_name(String s)
  {
  Var e;
  if ((e = localvar.FindByName(s)) == null)
    if ((e = paramvar.FindByName(s)) == null)
      if ((e = staticvar.FindByName(s)) == null)
	{
	io.Abort("Undefined variable '"+ s + "'\n");
	}
  return e;
  }

/*
 * parse a generic statement
 *
 * possible forms are:
 * lvalue = <expr> ;
 * function() ;
 * and <expr> can contain: function or arithmetic expressions
 * 
 * so for a recursive descent parser we need to test for a possible
 * '=' or '('
 */
void statement()
  {
  Var e;
  String vname = tok.getValue();

  CommentHolder();	/* mark the position in insn stream */
  switch (io.getNextChar())
    {
    case '(':			/* this is a function call */
      tok.scan();
      if (tok.getFirstChar() != '(')
	io.Abort("Expected '('");
      tok.scan();		/* get the first parameter (or none) */
      if (tok.getFirstChar() != ')')	/* if there are params */
	{
	boolExpr();		/* parse first param */
	while (tok.getFirstChar() == ',') /* while there are more params */
	  {
	  tok.scan();		/* get the next token */
	  boolExpr();		/* parse now as an expression */
	  }
	if (tok.getFirstChar() != ')') /* must find closing paren */
	  io.Abort("Expected ')'");
	}
      CommentFill();
      tok.scan();		/* move to next token */
      e = staticvar.FindByName(vname); /* find the symbol (e cannot be null) */
      emit.Call(e);
      if (e.getTypeId() != Tok.T_VOID) /* if function value not void */
	emit.Insn("pop");	/* then pop the unused return value */
      break;
    case '=':			/* if we have equal, then parse assign */
      tok.scan();
      if (tok.getFirstChar() != '=')
	io.Abort("Expected '='");
      tok.scan();
      boolExpr();
      CommentFill();
      emit.Store(lookup_name(vname));
      break;
    default:
      {
      StringBuilder sb = new StringBuilder();
      sb.Append("Unexpected character '");
      sb.Append(io.getNextChar());
      sb.Append("'\n");
      io.Abort(sb.ToString());
      return;
      }
    }
  if (tok.getFirstChar() != ';')
    io.Abort("Expected ';'");
  tok.scan();
  }

public Parse(Io i, Tok t)
  {
  io = i;
  tok = t;
  staticvar = new VarList();	// init the static variables list
  }

/* parse and translate a program */
public void program()
  {
  prolog();
  while (tok.NotEOF())
    {
    outerDecl();
    }
  if (Io.genexe && !mainseen)
    io.Abort("Generating executable with no main entrypoint");
  epilog();
  }

void CommentToken()
  {
  io.commentBegin(tok.getValue());	/* mark the begin of source comment */
  emit.CommentHolder();
  emit.CommentFill(io.commentEndTok(tok.getValue())); /* copy and emit comment */
  }

void CommentHolder()
  {
  io.commentBegin(tok.getValue());	/* mark the begin of source comment */
  emit.CommentHolder();
  }

void CommentFill()
  {
  emit.CommentFill(io.commentEndTok(tok.getValue())); /* copy and emit comment */
  }

void CommentFill(String s)
  {
  emit.CommentFill(io.commentEndTok(tok.getValue()) + s);		/* end the comment (no test) */
  }

void CommentFillPreTok()
  {
  emit.CommentFill(io.commentEndPreTok(tok.getValue())); /* copy and emit source */
  }
}

}
