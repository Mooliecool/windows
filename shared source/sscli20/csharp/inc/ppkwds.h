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
// File: ppkwds.h
//
// ===========================================================================

#if !defined(PPKWD)
#error Must define PPWKD macro before including ppkwds.h
#endif

//     Name         Token ID            IsDirective
PPKWD(L"define",    PPT_DEFINE,         TRUE    )           // NOTE:  Must be first!
PPKWD(L"undef",     PPT_UNDEF,          TRUE    )
PPKWD(L"error",     PPT_ERROR,          TRUE    )
PPKWD(L"warning",   PPT_WARNING,        TRUE    )
PPKWD(L"if",        PPT_IF,             TRUE    )
PPKWD(L"elif",      PPT_ELIF,           TRUE    )
PPKWD(L"else",      PPT_ELSE,           TRUE    )
PPKWD(L"endif",     PPT_ENDIF,          TRUE    )
PPKWD(L"region",    PPT_REGION,         TRUE    )
PPKWD(L"endregion", PPT_ENDREGION,      TRUE    )
PPKWD(L"line",      PPT_LINE,           TRUE    )
PPKWD(L"pragma",    PPT_PRAGMA,         TRUE    )
PPKWD(L"true",      PPT_TRUE,           FALSE   )
PPKWD(L"false",     PPT_FALSE,          FALSE   )
PPKWD(L"hidden",    PPT_HIDDEN,         FALSE   )           // NOTE:  Used with #line
PPKWD(L"default",   PPT_DEFAULT,        FALSE   )
PPKWD(L"disable",   PPT_DISABLE,        FALSE   )           // NOTE:  Used with #pragma warning
PPKWD(L"restore",   PPT_RESTORE,        FALSE   )
PPKWD(L"checksum",  PPT_CHECKSUM,       FALSE   )

PPKWD(NULL,         PPT_IDENTIFIER,     FALSE   )           // NOTE:  Must be first non-keyword!
PPKWD(NULL,         PPT_NUMBER,         FALSE   )
PPKWD(NULL,         PPT_STRING,         FALSE   )           // NOTE:  No escaping in these strings!

PPKWD(NULL,         PPT_OPENPAREN,      FALSE   )
PPKWD(NULL,         PPT_CLOSEPAREN,     FALSE   )
PPKWD(NULL,         PPT_COMMA,          FALSE   )

PPKWD(NULL,         PPT_OR,             FALSE   )           // NOTE:  These must be in order of precedence, lowest to highest!
PPKWD(NULL,         PPT_AND,            FALSE   )
PPKWD(NULL,         PPT_EQUAL,          FALSE   )
PPKWD(NULL,         PPT_NOTEQUAL,       FALSE   )
PPKWD(NULL,         PPT_NOT,            FALSE   )

PPKWD(NULL,         PPT_EOL,            FALSE   )
PPKWD(NULL,         PPT_UNKNOWN,        FALSE   )

#undef PPKWD
