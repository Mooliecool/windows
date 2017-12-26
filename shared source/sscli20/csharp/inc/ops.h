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
// File: ops.h
//
// ===========================================================================
#ifndef OP
#error OP must be defined prior to including this file!
#endif

// Operator         Precedence  Right-Associative   Stmt?    Token                    Predefined Name    Exprkind
// Binary
OP(NONE,            0 ,         FALSE,              FALSE,   TID_UNKNOWN             , PN_COUNT          , EK_COUNT  )
OP(ASSIGN,          1 ,         TRUE ,              TRUE ,   TID_EQUAL               , PN_COUNT          , EK_COUNT  )
OP(ADDEQ,           1 ,         TRUE ,              TRUE ,   TID_PLUSEQUAL           , PN_COUNT          , EK_MULTIOFFSET + EK_ADD    )
OP(SUBEQ,           1 ,         TRUE ,              TRUE ,   TID_MINUSEQUAL          , PN_COUNT          , EK_MULTIOFFSET + EK_SUB    )
OP(MULEQ,           1 ,         TRUE ,              TRUE ,   TID_SPLATEQUAL          , PN_COUNT          , EK_MULTIOFFSET + EK_MUL    )
OP(DIVEQ,           1 ,         TRUE ,              TRUE ,   TID_SLASHEQUAL          , PN_COUNT          , EK_MULTIOFFSET + EK_DIV    )
OP(MODEQ,           1 ,         TRUE ,              TRUE ,   TID_MODEQUAL            , PN_COUNT          , EK_MULTIOFFSET + EK_MOD    )
OP(ANDEQ,           1 ,         TRUE ,              TRUE ,   TID_ANDEQUAL            , PN_COUNT          , EK_MULTIOFFSET + EK_BITAND )
OP(XOREQ,           1 ,         TRUE ,              TRUE ,   TID_HATEQUAL            , PN_COUNT          , EK_MULTIOFFSET + EK_BITXOR )
OP(OREQ,            1 ,         TRUE ,              TRUE ,   TID_BAREQUAL            , PN_COUNT          , EK_MULTIOFFSET + EK_BITOR  )
OP(LSHIFTEQ,        1 ,         TRUE ,              TRUE ,   TID_SHIFTLEFTEQ         , PN_COUNT          , EK_MULTIOFFSET + EK_LSHIFT )
OP(RSHIFTEQ,        1 ,         TRUE ,              TRUE ,   TID_SHIFTRIGHTEQ        , PN_COUNT          , EK_MULTIOFFSET + EK_RSHIFT )
OP(QUESTION,        2 ,         FALSE,              FALSE,   TID_QUESTION            , PN_COUNT          , EK_COUNT  )
OP(VALORDEF,        3 ,         TRUE,               FALSE,   TID_QUESTQUEST          , PN_COUNT          , EK_COUNT  )
OP(LOGOR,           4 ,         FALSE,              FALSE,   TID_LOG_OR              , PN_COUNT          , EK_LOGOR  )
OP(LOGAND,          5 ,         FALSE,              FALSE,   TID_LOG_AND             , PN_COUNT          , EK_LOGAND )
OP(BITOR,           6 ,         FALSE,              FALSE,   TID_BAR                 , PN_OPBITWISEOR    , EK_BITOR  )
OP(BITXOR,          7 ,         FALSE,              FALSE,   TID_HAT                 , PN_OPXOR          , EK_BITXOR )
OP(BITAND,          8 ,         FALSE,              FALSE,   TID_AMPERSAND           , PN_OPBITWISEAND   , EK_BITAND )
OP(EQ,              9 ,         FALSE,              FALSE,   TID_EQUALEQUAL          , PN_OPEQUALITY     , EK_EQ     )
OP(NEQ,             9 ,         FALSE,              FALSE,   TID_NOTEQUAL            , PN_OPINEQUALITY   , EK_NE     )
OP(LT,              10,         FALSE,              FALSE,   TID_LESS                , PN_OPLESSTHAN     , EK_LT     )
OP(LE,              10,         FALSE,              FALSE,   TID_LESSEQUAL           , PN_OPLESSTHANOREQUAL, EK_LE   )
OP(GT,              10,         FALSE,              FALSE,   TID_GREATER             , PN_OPGREATERTHAN  , EK_GT     )
OP(GE,              10,         FALSE,              FALSE,   TID_GREATEREQUAL        , PN_OPGREATERTHANOREQUAL, EK_GE)
OP(IS,              10,         FALSE,              FALSE,   TID_IS                  , PN_COUNT          , EK_COUNT  )
OP(AS,              10,         FALSE,              FALSE,   TID_AS                  , PN_COUNT          , EK_COUNT  )
OP(LSHIFT,          11,         FALSE,              FALSE,   TID_SHIFTLEFT           , PN_OPLEFTSHIFT    , EK_LSHIFT )
OP(RSHIFT,          11,         FALSE,              FALSE,   TID_SHIFTRIGHT          , PN_OPRIGHTSHIFT   , EK_RSHIFT )
OP(ADD,             12,         FALSE,              FALSE,   TID_PLUS                , PN_OPPLUS         , EK_ADD    )
OP(SUB,             12,         FALSE,              FALSE,   TID_MINUS               , PN_OPMINUS        , EK_SUB    )
OP(MUL,             13,         FALSE,              FALSE,   TID_STAR                , PN_OPMULTIPLY     , EK_MUL    )
OP(DIV,             13,         FALSE,              FALSE,   TID_SLASH               , PN_OPDIVISION     , EK_DIV    )
OP(MOD,             13,         FALSE,              FALSE,   TID_PERCENT             , PN_OPMODULUS      , EK_MOD    )

// Unary
OP(NOP,             14,         FALSE,              FALSE,   TID_UNKNOWN             , PN_COUNT          , EK_COUNT  )
OP(UPLUS,           14,         FALSE,              FALSE,   TID_PLUS                , PN_OPUNARYPLUS    , EK_COUNT  )
OP(NEG,             14,         FALSE,              FALSE,   TID_MINUS               , PN_OPUNARYMINUS   , EK_COUNT  )
OP(BITNOT,          14,         FALSE,              FALSE,   TID_TILDE               , PN_OPCOMPLEMENT   , EK_COUNT  )
OP(LOGNOT,          14,         FALSE,              FALSE,   TID_BANG                , PN_OPNEGATION     , EK_COUNT  )
OP(PREINC,          14,         FALSE,              TRUE ,   TID_PLUSPLUS            , PN_OPINCREMENT    , EK_COUNT  )
OP(PREDEC,          14,         FALSE,              TRUE ,   TID_MINUSMINUS          , PN_OPDECREMENT    , EK_COUNT  )
OP(TYPEOF,          14,         FALSE,              FALSE,   TID_TYPEOF              , PN_COUNT          , EK_COUNT  )
OP(SIZEOF,          14,         FALSE,              FALSE,   TID_SIZEOF              , PN_COUNT          , EK_COUNT  )
OP(CHECKED,         14,         FALSE,              FALSE,   TID_CHECKED             , PN_COUNT          , EK_COUNT  )
OP(UNCHECKED,       14,         FALSE,              FALSE,   TID_UNCHECKED           , PN_COUNT          , EK_COUNT  )

OP(MAKEREFANY,      14,         FALSE,              FALSE,   TID_MAKEREFANY          , PN_COUNT          , EK_COUNT  )
OP(REFVALUE,        14,         FALSE,              FALSE,   TID_REFVALUE            , PN_COUNT          , EK_COUNT  )
OP(REFTYPE,         14,         FALSE,              FALSE,   TID_REFTYPE             , PN_COUNT          , EK_COUNT  )
OP(ARGS,            0,          FALSE,              FALSE,   TID_ARGS                , PN_COUNT          , EK_COUNT  )

OP(CAST,            15,         FALSE,              FALSE,   TID_UNKNOWN             , PN_COUNT          , EK_COUNT  )                   // (Actually a binary operator, but weird...)
OP(INDIR,           16,         FALSE,              FALSE,   TID_STAR                , PN_COUNT          , EK_COUNT  )
OP(ADDR,            17,         FALSE,              FALSE,   TID_AMPERSAND           , PN_COUNT          , EK_COUNT  )

OP(COLON,           0,          FALSE,              FALSE,   TID_COLON               , PN_COUNT          , EK_COUNT  )                   // Other
OP(THIS,            0,          FALSE,              FALSE,   TID_THIS                , PN_COUNT          , EK_COUNT  )
OP(BASE,            0,          FALSE,              FALSE,   TID_BASE                , PN_COUNT          , EK_COUNT  )
OP(NULL,            0,          FALSE,              FALSE,   TID_NULL                , PN_COUNT          , EK_COUNT  )
OP(TRUE,            0,          FALSE,              FALSE,   TID_TRUE                , PN_OPTRUE         , EK_COUNT  )
OP(FALSE,           0,          FALSE,              FALSE,   TID_FALSE               , PN_OPFALSE        , EK_COUNT  )
OP(CALL,            0,          FALSE,              TRUE ,   TID_UNKNOWN             , PN_COUNT          , EK_COUNT  )
OP(DEREF,           0,          FALSE,              FALSE,   TID_UNKNOWN             , PN_COUNT          , EK_COUNT  )
OP(PAREN,           0,          FALSE,              FALSE,   TID_UNKNOWN             , PN_COUNT          , EK_COUNT  )
OP(POSTINC,         0,          FALSE,              TRUE ,   TID_PLUSPLUS            , PN_COUNT          , EK_COUNT  )
OP(POSTDEC,         0,          FALSE,              TRUE ,   TID_MINUSMINUS          , PN_COUNT          , EK_COUNT  )
OP(DOT,             0,          FALSE,              FALSE,   TID_DOT                 , PN_COUNT          , EK_COUNT  )
OP(IMPLICIT,        0,          FALSE,              FALSE,   TID_IMPLICIT            , PN_OPIMPLICITMN   , EK_COUNT  )
OP(EXPLICIT,        0,          FALSE,              FALSE,   TID_EXPLICIT            , PN_OPEXPLICITMN   , EK_COUNT  )

OP(EQUALS,          0,          FALSE,              FALSE,   TID_UNKNOWN             , PN_OPEQUALS       , EK_COUNT  )
OP(COMPARE,         0,          FALSE,              FALSE,   TID_UNKNOWN             , PN_OPCOMPARE      , EK_COUNT  )

OP(DEFAULT,         0,          FALSE,              FALSE,   TID_UNKNOWN             , PN_COUNT          , EK_COUNT  )

#undef OP
