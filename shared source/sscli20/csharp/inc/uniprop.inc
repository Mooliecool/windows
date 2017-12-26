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
// uniprop.inc -- Generate Is... or IsProp... functions
//---------------------------------------------------------------------
// #included by "uniprop.h"
//
// #define IS_FT_CHAR or IS_FT_PROP then #include this file to
// generate the char or prop-based character-type functions.
//
//---------------------------------------------------------------------
#if !defined(IS_FT_CHAR) && !defined(IS_FT_PROP)
#error "ERROR: uniprop.inc control macros not defined"
#endif
#if defined(IS_FT_CHAR) && defined(IS_FT_PROP)
#error ERROR: only one of IS_FT_CHAR and IS_FT_PROP can be defined
#endif

#undef Is_EQU_
#undef Is_LEQ_
#undef Is_GEQ_
#undef Is_RNG_
#undef Is_OF2_

#ifdef IS_FT_CHAR

#define Is_EQU_(name, P) \
    inline BOOL WINAPI Is##name(WCHAR ch) { return ((P) == UProp(ch)); }
#define Is_LEQ_(name, P) \
    inline BOOL WINAPI Is##name(WCHAR ch) { return (UProp(ch) <= (P)); }
#define Is_GEQ_(name, P) \
    inline BOOL WINAPI Is##name(WCHAR ch) { return (UProp(ch) >= (P)); }
#define Is_RNG_(name, P1, P2) \
    inline BOOL WINAPI Is##name(WCHAR ch) { BYTE P = UProp(ch); return __IN_RANGE__(P, (P1), (P2)); }
#define Is_OF2_(name, P1, P2) \
    inline BOOL WINAPI Is##name(WCHAR ch) { BYTE P = UProp(ch); return ((P1) == P || (P2) == P); }

#elif defined(IS_FT_PROP)

#define Is_EQU_(name, P) \
    inline BOOL WINAPI IsProp##name(BYTE prop) { return ((P) == prop); }
#define Is_LEQ_(name, P) \
    inline BOOL WINAPI IsProp##name(BYTE prop) { return (prop <= (P)); }
#define Is_GEQ_(name, P) \
    inline BOOL WINAPI IsProp##name(BYTE prop) { return (prop >= (P)); }
#define Is_RNG_(name, P1, P2) \
    inline BOOL WINAPI IsProp##name(BYTE prop) { return __IN_RANGE__(prop, (P1), (P2)); }
#define Is_OF2_(name, P1, P2) \
    inline BOOL WINAPI IsProp##name(BYTE prop) { return ((P1) == prop || (P2) == prop); }
#else
#error ERROR: uniprop.inc not properly set up.
#endif


    Is_LEQ_(  Alpha                     ,  LetterOther                             )
//  Is_RNG_(  Alpha                     ,  LetterUppercase, LetterOther            )
    Is_EQU_(  Digit                     ,  DigitDecimal                            )
    Is_LEQ_(  AlphaNumeric              ,  DigitDecimal                            )
//  Is_RNG_(  AlphaNumeric              ,  LetterUppercase, DigitDecimal           )
    Is_EQU_(  UppercaseLetter           ,  LetterUppercase                         )
    Is_EQU_(  LowercaseLetter           ,  LetterLowercase                         )
    Is_EQU_(  TitlecaseLetter           ,  LetterTitlecase                         )
    Is_EQU_(  ModifierLetter            ,  LetterModifier                          )
    Is_EQU_(  OtherLetter               ,  LetterOther                             )
    Is_EQU_(  DecimalDigit              ,  DigitDecimal                            )
    Is_EQU_(  LetterDigit               ,  DigitLetter                             )
    Is_EQU_(  OtherDigit                ,  DigitOther                              )
    Is_RNG_(  Punctuation               ,  PunctuationOpen, PunctuationOther       )
    Is_RNG_(  PairPunctuation           ,  PunctuationOpen, PunctuationFinalQuote  )
    Is_EQU_(  OpenPunctuation           ,  PunctuationOpen                         )
    Is_EQU_(  ClosePunctuation          ,  PunctuationClose                        )
    Is_EQU_(  InitialQuotePunctuation   ,  PunctuationInitialQuote                 )
    Is_EQU_(  FinalQuotePunctuation     ,  PunctuationFinalQuote                   )
    Is_EQU_(  DashPunctuation           ,  PunctuationDash                         )
    Is_EQU_(  ConnectorPunctuation      ,  PunctuationConnector                    )
    Is_EQU_(  OtherPunctuation          ,  PunctuationOther                        )
    Is_EQU_(  SpaceSeparator            ,  SeparatorSpace                          )
    Is_EQU_(  LineSeparator             ,  SeparatorLine                           )
    Is_EQU_(  ParagraphSeparator        ,  SeparatorParagraph                      )
    Is_RNG_(  Combining                 ,  MarkNonSpacing, MarkEnclosing           )
    Is_EQU_(  NonSpacingMark            ,  MarkNonSpacing                          )
    Is_EQU_(  CombiningMark             ,  MarkCombining                           )
    Is_EQU_(  EnclosingMark             ,  MarkEnclosing                           )
    Is_RNG_(  Symbol                    ,  SymbolMath, SymbolOther                 )
    Is_EQU_(  MathSymbol                ,  SymbolMath                              )
    Is_EQU_(  CurrencySymbol            ,  SymbolCurrency                          )
    Is_EQU_(  ModifierSymbol            ,  SymbolModifier                          )
    Is_EQU_(  OtherSymbol               ,  SymbolOther                             )
    Is_EQU_(  OtherControl              ,  OtherControl                            )
    Is_EQU_(  OtherFormat               ,  OtherFormat                             )
    Is_EQU_(  OtherSurrogate            ,  OtherSurrogate                          )
    Is_EQU_(  OtherPrivateUse           ,  OtherPrivateUse                         )
    Is_EQU_(  OtherNotAssigned          ,  OtherNotAssigned                        )
    Is_GEQ_(  NotAssigned               ,  OtherNotAssigned                        )

#undef Is_EQU_
#undef Is_LEQ_
#undef Is_GEQ_
#undef Is_RNG_
#undef Is_OF2_
