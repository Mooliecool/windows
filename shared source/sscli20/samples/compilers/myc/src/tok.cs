//------------------------------------------------------------------------------
// <copyright file="tok.cs" company="Microsoft">
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
using System.Collections;

class Tok
  {
  public const int T_LEFT_ASSIGN	= 10001;
  public const int T_RIGHT_ASSIGN	= 10002;
  public const int T_LEFT_OP		= 10003;
  public const int T_LE_OP		= 10004;
  public const int T_RIGHT_OP		= 10005;
  public const int T_GE_OP		= 10006;
  public const int T_EQ_OP		= 10007;
  public const int T_NEQ_OP		= 10008;
  public const int T_INC_OP		= 10009;
  public const int T_ADD_ASSIGN		= 10010;
  public const int T_DEC_OP		= 10011;
  public const int T_SUB_ASSIGN		= 10012;
  public const int T_PTR_OP		= 10013;
  public const int T_MUL_ASSIGN		= 10014;
  public const int T_DIV_ASSIGN		= 10015;
  public const int T_DIV_OP		= 10016;
  public const int T_LOG_OR_OP		= 10017;
  public const int T_LOG_AND_OP		= 10018;
  public const int T_LT_OP		= 10019;
  public const int T_GT_OP		= 10020;
  public const int T_ASSIGN		= 10021;
  public const int T_ADD_OP		= 10022;
  public const int T_SUB_OP		= 10023;
  public const int T_MUL_OP		= 10024;
  public const int T_OR_OP		= 10025;
  public const int T_AND_OP		= 10026;
  public const int T_MOD_OP		= 10027;
  public const int T_XOR_OP		= 10028;
  public const int T_COMPL_OP		= 10029;
  public const int T_NOT_OP		= 10030;

  public const int T_IF			= 20001;
  public const int T_ELSE		= 20002;
  public const int T_WHILE		= 20003;
  public const int T_FOR		= 20004;
  public const int T_DO			= 20005;
  public const int T_BREAK		= 20006;
  public const int T_CONTINUE		= 20007;
  public const int T_RETURN		= 20008;
  public const int T_FLOW_MIN		= T_IF;
  public const int T_FLOW_MAX		= T_RETURN;

  public const int T_EXTERN		= 30001;
  public const int T_STATIC		= 30002;
  public const int T_AUTO		= 30003;
  public const int T_SIGNED		= 30004;
  public const int T_UNSIGNED		= 30005;
  public const int T_DEFCLASS		= 30006;
  public const int T_STORAGE_MIN	= T_EXTERN;
  public const int T_STORAGE_MAX	= T_DEFCLASS;
  public const int T_PARAM		= 30007	/* special for tagging parameters */;

  public const int T_SHORT		= 40001;
  public const int T_CHAR		= 40002;
  public const int T_INT		= 40003;
  public const int T_LONG		= 40004;
  public const int T_FLOAT		= 40005;
  public const int T_DOUBLE		= 40006;
  public const int T_VOID		= 40007;
  public const int T_DEFTYPE		= 40008;
  public const int T_TYPE_MIN		= T_SHORT;
  public const int T_TYPE_MAX		= T_DEFTYPE;

  public const int T_IDENT 		= 50001;
  public const int T_DIGITS 		= 50002;
  public const int T_UNKNOWN		= 99999;
  public const int T_EOF	 	= -1;

  static Hashtable tokens;
  StringBuilder value;
  int token_id;
  Io io;

  public static void AddTok(int i, String s)
    {
    tokens.Add(s, i);
    tokens.Add(i, s);
    }

  public void InitHash()
    {
    tokens = new Hashtable();
    AddTok(T_LEFT_ASSIGN,	"<<=");
    AddTok(T_RIGHT_ASSIGN,	">>=");
    AddTok(T_LEFT_OP,		"<<");
    AddTok(T_LE_OP,		"<=");
    AddTok(T_RIGHT_OP,		">>");
    AddTok(T_GE_OP,		">=");
    AddTok(T_EQ_OP,		"==");
    AddTok(T_NEQ_OP,		"!=");
    AddTok(T_INC_OP,		"++");
    AddTok(T_ADD_ASSIGN,	"+=");
    AddTok(T_DEC_OP,		"--");
    AddTok(T_SUB_ASSIGN,	"-=");
    AddTok(T_PTR_OP,		"->");
    AddTok(T_MUL_ASSIGN,	"*=");
    AddTok(T_DIV_ASSIGN,	"/=");
    AddTok(T_DIV_OP,		"/");
    AddTok(T_LOG_OR_OP,		"||");
    AddTok(T_LOG_AND_OP,	"&&");
    AddTok(T_LT_OP,		"<");
    AddTok(T_GT_OP,		">");
    AddTok(T_ASSIGN,		"=");
    AddTok(T_ADD_OP,		"+");
    AddTok(T_SUB_OP,		"-");
    AddTok(T_MUL_OP,		"*");
    AddTok(T_OR_OP,		"|");
    AddTok(T_AND_OP,		"&");		/* could be addr of, or bitwise and */
    AddTok(T_MOD_OP,		"%");
    AddTok(T_XOR_OP,		"^");
    AddTok(T_COMPL_OP,		"~");
    AddTok(T_NOT_OP,		"!");

    AddTok(T_IF,		"if");
    AddTok(T_ELSE,		"else");
    AddTok(T_WHILE,		"while");
    AddTok(T_FOR,		"for");
    AddTok(T_DO,		"do");
    AddTok(T_BREAK,		"break");
    AddTok(T_CONTINUE,		"continue");
    AddTok(T_RETURN,		"return");

    AddTok(T_EXTERN,		"extern");
    AddTok(T_STATIC,		"static");
    AddTok(T_AUTO,		"auto");
    AddTok(T_SIGNED,		"signed");
    AddTok(T_UNSIGNED,		"unsigned");

    AddTok(T_INT,		"int");
    AddTok(T_LONG,		"long");
    AddTok(T_CHAR,		"char");
    AddTok(T_FLOAT,		"float");
    AddTok(T_DOUBLE,		"double");
    AddTok(T_VOID,		"void");
    }

  public Tok(Io ihandle)
    {
    io = ihandle;
    InitHash();			// initialize the tokens hashtable
    io.ReadChar();
    scan();
    }

  /*
   * table lookup
   *
   * if the input string matches a table entry, return the entry token_id
   * if not, return -1.
   */
  int lookup_id()
    {
    String s = value.ToString();
    Object k = tokens[s];
    if (k == null)
      return 0;
    return (int) k;
    }

  /* constant declaration */
  const char TAB = '\t';
  const char CR = '\r';
  const char LF = '\n';

  /* recognize any operator */
  bool isOp(char c)
    {
    return (c == '+' || c == '-' || c == '*' || c == '/' ||
	    c == '<' || c == '>' || c == '=' ||
	    c == '&' || c == '|' || c == '^' || c == '!'
	    );
    }

  bool isAddOp(char c)
    {
    return ((c == '+' || c == '-'));
    }

  bool isMulOp(char c)
    {
    return ((c == '*' || c == '/'));
    }

  bool isOrOp(char c)
    {
    return ((c == '|') || (c == '~'));
    }

  bool isRelOp(char c)
    {
    return ((c == '=') || (c == '!') || (c == '<') || (c == '>'));
    }

  void skipWhite()
    {
    while (Char.IsWhiteSpace(io.getNextChar()))
      io.ReadChar();
    }

/* get an identifier */
void LoadName()
  {
  value = new StringBuilder(MyC.MAXSTR);
  skipWhite();
  if (!Char.IsLetter(io.getNextChar()))
    throw new ApplicationException("?Expected Name");
  while (Char.IsLetterOrDigit(io.getNextChar()))
    {
    value.Append(io.getNextChar());
    io.ReadChar();
    }
  token_id = lookup_id();
  if (token_id <= 0)
    token_id = T_IDENT;
  skipWhite();
  }

/* get a number */
void LoadNum()
  {
  value = new StringBuilder(MyC.MAXSTR);
  skipWhite();
  if (!Char.IsDigit(io.getNextChar()))
    throw new ApplicationException("?Expected Integer");
  while (Char.IsDigit(io.getNextChar()))
    {
    value.Append(io.getNextChar());
    io.ReadChar();
    }
  token_id = T_DIGITS;
  skipWhite();
  }

/* get an operator */
void LoadOp()
  {
  value = new StringBuilder(MyC.MAXSTR);
  skipWhite();
  if (!isOp(io.getNextChar()))
    throw new ApplicationException("?Expected operator");
  while (isOp(io.getNextChar()))
    {
    value.Append(io.getNextChar());
    io.ReadChar();
    }
  token_id = lookup_id();
  skipWhite();
  }

/* get an identifier */
  public void scan()
    {
    skipWhite();
    if (Char.IsLetter(io.getNextChar()))
      LoadName();
    else if (Char.IsDigit(io.getNextChar()))
      LoadNum();
    else if (isOp(io.getNextChar()))
      LoadOp();
    else if (io.EOF())
      {
      value = null;
      token_id = T_EOF;
      }
    else
      {
      value = new StringBuilder(MyC.MAXSTR);
      value.Append(io.getNextChar());
      token_id = T_UNKNOWN;
      io.ReadChar();
      }
    skipWhite();
#if DEBUG
    Console.WriteLine("[tok.scan tok=["+this+"]");
#endif
    }

  public char getFirstChar()
    {
    if (value == null)
      return '\0';
    return value[0];
    }

  public String getValue()
    {
    if (value == null)
      return "";
    return value.ToString();
    }

  public int getId()
    {
    return (token_id);
    }

  public bool NotEOF()
    {
    return (token_id != T_EOF);
    }

  public bool IsDeclKeyword()
    {
    if ((token_id >= T_TYPE_MIN && token_id <= T_TYPE_MAX) ||
	(token_id >= T_STORAGE_MIN && token_id <= T_STORAGE_MAX))
      return true;
    return false;
    }

  public override string ToString()
    {
    StringBuilder sb = new StringBuilder(getValue());
    sb.Append("(");
    sb.Append(token_id);
    sb.Append(")");
    return sb.ToString();
    }

  }
}
