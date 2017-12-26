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
// File: expeval.c
// Purpose: Expression Evaluator
// ===========================================================================
 
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expeval.h"
#define BUFFER 1024
#define MAX_TOKENS 100
 
int LineNumber = 0;
TOKEN Tokens[MAX_TOKENS];
 
void
ConsumeToken(void)
/*++
 
  Routine Description:
  Removes the TOKEN at position 0 in the array from the TOKENs array.
 
  Arguments:
  void
  
  Return Value:
  void
 
--*/
{
  int i;
  for (i=0; Tokens[i].Type!=TK_EOS; i++)
  {
    Tokens[i] = Tokens[i+1];
  }
} //end ConsumeToken
 
int
ishex(char p)
{
  switch(p)
  {
  case 'a':
  case 'A':
  case 'b':
  case 'B':
  case 'c':
  case 'C':
  case 'd':
  case 'D':
  case 'e':
  case 'E':
  case 'f':
  case 'F':
    return 1;
  default:

    return 0;
  }
}
 
int
LexOneLine(char *p)
/*++
  Routine Description:
  Analyzes an expression string.  
  Tokenizes the expression converting
  defined/undefined/unknown macros into TK_TRUE/TK_FALSE/TK_UNKNOWN and
  ||, &&, (, ) operators into their respective TOKENs. These TOKENs are
  placed into an array for later expression evaluation.
 
  Arguments:
  *p --  Pointer to line of input to Analyze
  *strExpr - pointer to store expression for CodeRemoved tag.
  
  Return Value:
  1 if lexer parsed the line, otherwise 0 (i.e. line did not start with a #)
  
--*/  
{
  int TokensCount = 0;
  int ParenCheck = 0;
  int i;
  char tmp[1024];
  PTOKEN Token;
 
  while (TokensCount <= MAX_TOKENS)
  {
    i=0;
    switch(*p) {
      
    case ' ':
    case '\t':
    case '\r':
    case '\n':
    case '\v':
    case '\f':
    case '\b':
    case '\a':
    case '\\':  // line-continuation characters are ignored
      p++;
      continue;

    case '%':
      Token = &Tokens[TokensCount];
      Token->Type = TK_MOD;
      TokensCount++;
      break;
 
    case '+':
      Token = &Tokens[TokensCount];
      Token->Type = TK_PLUS;
      TokensCount++;
      break;
 
    case '-':
      Token = &Tokens[TokensCount];
      Token->Type = TK_MINUS;
      TokensCount++;
      break;
 
    case '*':
      Token = &Tokens[TokensCount];
      Token->Type = TK_STAR;
      TokensCount++;
      break;
 
    case '/':
      Token = &Tokens[TokensCount];
      Token->Type = TK_DIVIDE;
      TokensCount++;
      break;

    case '(':
      Token = &Tokens[TokensCount];
      Token->Type = TK_LEFT_PAREN;
      TokensCount++;
      ParenCheck++;
      break;

    case ')':
      Token = &Tokens[TokensCount];
      Token->Type = TK_RIGHT_PAREN;
      TokensCount++;
      ParenCheck--;
      break;

    case '\0':
      //End of Line. Return
      if (ParenCheck != 0)
          return 0;

      Token = &Tokens[TokensCount];
      Token->Type = TK_EOS;
      TokensCount++;
 
      return 1;
 
    case '0':
      if (p[1] == 'x' || p[1] == 'X') {
            //
            // Found '0x' prefix - the token is a hex constant
            //
            Token = &Tokens[TokensCount];
            TokensCount++;
            Token->Type = TK_NUMBER;
 
            tmp[i] = *p;
            p++;
            i++;
            tmp[i] = *p;
            p++;
            i++;
            
            while (isdigit(*p) || ishex(*p)) {
              tmp[i] = *p;
              p++;
              i++;
            }
            tmp[i] = '\0';
            Token->Value = strtoul(tmp, NULL, 0);
 
            //replace the char
            continue;
            
      } else if (isdigit(p[1])) {
            //
            // Found '0' followed by a valid number - the token is
            // an octal constant.
            //
            Token = &Tokens[TokensCount];
            TokensCount++;
            Token->Type = TK_NUMBER;
            
            tmp[i] = *p;
            p++;
            i++;
            tmp[i] = *p;
            p++;
            i++;
            
            while (isdigit(*p)) {
              tmp[i] = *p;
              p++;
              i++;
            }
            tmp[i] = '\0';
            Token->Value = strtoul(tmp, NULL, 0);
            continue;
      }
      
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
 
      Token = &Tokens[TokensCount];
      TokensCount++;
      Token->Type = TK_NUMBER;
 
      tmp[i] = *p;
      p++;
      i++;
 
      while (isdigit(*p)) {
            tmp[i] = *p;
            p++;
            i++;
      }
      tmp[i] = '\0';
      Token->Value = strtoul(tmp, NULL, 0);
      continue;
    default:
      return 0;
    }
    p++;
  } //end while (TokensCount <= MAX_TOKENS
 
  return 0;
} //end LexOneLine
 
LONG
Expr_Eval(
    void
    )
{
  LONG val = Expr_Eval_1();
  
  do
  {
    switch (Tokens[0].Type) {
    case TK_PLUS:
      ConsumeToken();
      val += Expr_Eval_1();
      break;
      
    case TK_MINUS:
      ConsumeToken();
      val -= Expr_Eval_1();
      break;
      
    default:
      return val;
    }
  } while (1);
}
 
LONG
Expr_Eval_1(
    void
    )
/*++
 
Routine Description:
 
    Part of expression evaluator - handles the highest-precedence operators
    'multiply' and 'divide'.
 
Arguments:
 
    None.
 
Return Value:
 
    Value of the expression.
 
--*/
{
    LONG val = Expr_Eval_2();
 
    do
    {
        switch (Tokens[0].Type) {
        case TK_STAR:
            ConsumeToken();
            val *= Expr_Eval_2();
            break;
 
        case TK_DIVIDE:
            ConsumeToken();
            val /= Expr_Eval_2();
            break;
 
        case TK_MOD:
            ConsumeToken();
            val %= Expr_Eval_2();
            break;
 
        default:
            // done
            return val;
        }
    } while (1);
 
}
 
LONG
Expr_Eval_2(void)
{
    LONG val;
 
    switch (Tokens[0].Type) {
    case TK_NUMBER:
      val = Tokens[0].Value;
      ConsumeToken();
      break;

    case TK_LEFT_PAREN:
      ConsumeToken();
      val = Expr_Eval();
      ConsumeToken();
      break;
      
    default:
      val = -1;
      break;
    }
    
    return val;
}
