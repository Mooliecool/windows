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
// ===========================================================================
// File: expeval.h
// Purpose: Header file for Expression Evaluator
// ===========================================================================

#ifndef __EXPEVAL_H__
#define __EXPEVAL_H__

typedef enum _TokenType {
  TK_MOD,           //1
  TK_PLUS,          //2
  TK_MINUS,         //3
  TK_STAR,          //4
  TK_DIVIDE,        //5
  TK_LEFT_PAREN,    //6
  TK_RIGHT_PAREN,   //7
  TK_NUMBER,        //8
  TK_EOS            //9

} TOKENTYPE, *PTOKENTYPE;

typedef struct _Token {
  TOKENTYPE Type;
  char* Name;
  LONG Value;
} TOKEN, *PTOKEN;


LONG
Expr_Eval(
    void
    );

LONG
Expr_Eval_1(
    void
    );

LONG
Expr_Eval_2(void);

#endif	/* __EXPEVAL_H__ */
