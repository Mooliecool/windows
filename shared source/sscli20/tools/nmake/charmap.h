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
//
// Access to charmap functions
//
#define OPERATORMSK 0x80        // Is this character the startof an operator
#define LX_OPERATOR 0x80
#define IS_OPERATORCHAR(c) ((Charmap[(UCHAR)(c)] & OPERATORMSK) != 0)

#define MACROMSK 0x01           // Is this character a valid macro character
#define LX_MACRO 0x01
#define IS_MACROCHAR(c) ((Charmap[(UCHAR)(c)] & MACROMSK) != 0)

#define LX_EOS      0x00
#define LX_ILL      0x00
#define LX_WHITE    0x00
#define LX_NL       0x00
#define LX_CR       0x00
#define LX_DQUOTE   0x00
#define LX_POUND    0x00
#define LX_ASCII    0x00
#define LX_SQUOTE   0x00
#define LX_COMMA    0x00
#define LX_MINUS    0x00
#define LX_DOT      0x00
#define LX_NUMBER   0x00
#define LX_COLON    0x00
#define LX_SEMI     0x00
#define LX_EACH     0x00
#define LX_ID       0x00
#define LX_OBRACK   0x00
#define LX_CBRACK   0x00
#define LX_OBRACE   0x00
#define LX_CBRACE   0x00

#define CHARMAP_SIZE 256

extern UCHAR Charmap[CHARMAP_SIZE];
