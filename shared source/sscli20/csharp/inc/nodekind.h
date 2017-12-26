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
// File: nodekind.h
//
// NOTE:  If you change this list, there are places that MUST ALSO be updated.
// NOTE:  These include:
//          dumpnode.h (all node dumping)
//          srcmod.cpp (GetFirstToken, GetLastToken, FindLeafNode)
//
// There may be others depending on the node you add (such as GetInteriorTree, etc.)
// ===========================================================================


//       NK_* constant               *NODE struct            NG_* flags (or'd if appropriate)                                                               Pretty print name
NODEKIND(ACCESSOR               ,     ACCESSOR              ,     NG_INTERIOR                                                                                   ,    Accessor               )
NODEKIND(ALIASNAME              ,     NAME                  ,     0                                                                                             ,    AliasName              )
NODEKIND(ANONBLOCK              ,     ANONBLOCK             ,     0                                                                                             ,    AnonymousBlock         )
NODEKIND(ARRAYINIT              ,     UNOP                  ,     0                                                                                             ,    ArrayInitializer       )
NODEKIND(ARRAYTYPE              ,     ARRAYTYPE             ,     0                                                                                             ,    ArrayType              )
NODEKIND(ARROW                  ,     BINOP                 ,     NG_BINOP                                                                                      ,    Arrow                  )
NODEKIND(ATTR                   ,     ATTR                  ,     0                                                                                             ,    Attribute              )
NODEKIND(ATTRARG                ,     ATTR                  ,     0                                                                                             ,    AttributeArgument      )
NODEKIND(ATTRDECL               ,     ATTRDECL              ,     0                                                                                             ,    AttributeDeclaration   )
NODEKIND(BINOP                  ,     BINOP                 ,     NG_BINOP                                                                                      ,    BinaryOperator         )
NODEKIND(BLOCK                  ,     BLOCK                 ,     NG_STATEMENT                                                                                  ,    Block                  )
NODEKIND(BREAK                  ,     EXPRSTMT              ,     NG_STATEMENT                                                                                  ,    Break                  )
NODEKIND(CALL                   ,     CALL                  ,     NG_BINOP                                                                                      ,    Call                   )
NODEKIND(CASE                   ,     CASE                  ,     NG_BREAKABLE                                                                                  ,    Case                   )
NODEKIND(CASELABEL              ,     UNOP                  ,     0                                                                                             ,    CaseLabel              )
NODEKIND(CATCH                  ,     CATCH                 ,     0                                                                                             ,    Catch                  )
NODEKIND(CHECKED                ,     LABELSTMT             ,     NG_STATEMENT | NG_EMBEDDEDSTMTOWNER                                                           ,    Checked                )
NODEKIND(CLASS                  ,     CLASS                 ,     NG_TYPE | NG_AGGREGATE | NG_KEYED                                                             ,    Class                  )
NODEKIND(CONST                  ,     FIELD                 ,     NG_FIELD | NG_MEMBER                                                                          ,    Const                  )
NODEKIND(CONSTRAINT             ,     CONSTRAINT            ,     0                                                                                             ,    Constraint             )
NODEKIND(CONSTVAL               ,     CONSTVAL              ,     0                                                                                             ,    ConstValue             )
NODEKIND(CONTINUE               ,     EXPRSTMT              ,     NG_STATEMENT                                                                                  ,    Continue               )
NODEKIND(CTOR                   ,     CTORMETHOD            ,     NG_METHOD | NG_KEYED | NG_INTERIOR | NG_MEMBER                                                ,    Constructor            )
NODEKIND(DECLSTMT               ,     DECLSTMT              ,     NG_STATEMENT                                                                                  ,    DeclarationStatement   )
NODEKIND(DELEGATE               ,     DELEGATE              ,     NG_TYPE | NG_KEYED                                                                            ,    Delegate               )
NODEKIND(DEREF                  ,     CALL                  ,     NG_BINOP                                                                                      ,    Dereference            )
NODEKIND(DO                     ,     LOOPSTMT              ,     NG_STATEMENT | NG_GLOBALCOMPLETION | NG_BREAKABLE | NG_CONTINUABLE | NG_EMBEDDEDSTMTOWNER     ,    Do                     )
NODEKIND(DOT                    ,     BINOP                 ,     NG_BINOP                                                                                      ,    Dot                    )
NODEKIND(DTOR                   ,     METHODBASE            ,     NG_METHOD | NG_KEYED | NG_INTERIOR | NG_MEMBER                                                ,    Destructor             )
NODEKIND(EMPTYSTMT              ,     STATEMENT             ,     NG_STATEMENT | NG_GLOBALCOMPLETION                                                            ,    EmptyStatement         )
NODEKIND(ENUM                   ,     ENUM                  ,     NG_TYPE | NG_AGGREGATE | NG_KEYED                                                             ,    Enum                   )
NODEKIND(ENUMMBR                ,     ENUMMBR               ,     NG_KEYED | NG_MEMBER                                                                          ,    EnumMember             )
NODEKIND(EXPRSTMT               ,     EXPRSTMT              ,     NG_STATEMENT                                                                                  ,    ExpressionStatement    )
NODEKIND(FIELD                  ,     FIELD                 ,     NG_FIELD | NG_MEMBER                                                                          ,    Field                  )
NODEKIND(FOR                    ,     FORSTMT               ,     NG_STATEMENT | NG_GLOBALCOMPLETION | NG_BREAKABLE | NG_CONTINUABLE | NG_EMBEDDEDSTMTOWNER     ,    For                    )
NODEKIND(GENERICNAME            ,     GENERICNAME           ,     0                                                                                             ,    GenericName            )
NODEKIND(GOTO                   ,     EXPRSTMT              ,     NG_STATEMENT                                                                                  ,    Goto                   )
NODEKIND(IF                     ,     IFSTMT                ,     NG_STATEMENT | NG_GLOBALCOMPLETION | NG_EMBEDDEDSTMTOWNER                                     ,    If                     )
NODEKIND(INTERFACE              ,     INTERFACE             ,     NG_TYPE | NG_AGGREGATE | NG_KEYED                                                             ,    Interface              )
NODEKIND(LABEL                  ,     LABELSTMT             ,     NG_STATEMENT | NG_GLOBALCOMPLETION | NG_EMBEDDEDSTMTOWNER                                     ,    Label                  )
NODEKIND(LIST                   ,     BINOP                 ,     NG_BINOP                                                                                      ,    List                   )
NODEKIND(LOCK                   ,     LOOPSTMT              ,     NG_STATEMENT | NG_GLOBALCOMPLETION | NG_EMBEDDEDSTMTOWNER                                     ,    Lock                   )
NODEKIND(MEMBER                 ,     MEMBER                ,     0                                                                                             ,    Member                 )
NODEKIND(METHOD                 ,     METHOD                ,     NG_METHOD | NG_KEYED | NG_INTERIOR | NG_MEMBER                                                ,    Method                 )
NODEKIND(NAME                   ,     NAME                  ,     0                                                                                             ,    Name                   )
NODEKIND(NAMEDTYPE              ,     NAMEDTYPE             ,     0                                                                                             ,    NamedType              )
NODEKIND(NAMESPACE              ,     NAMESPACE             ,     NG_KEYED                                                                                      ,    Namespace              )
NODEKIND(NESTEDTYPE             ,     NESTEDTYPE            ,     NG_MEMBER                                                                                     ,    NestedType             )
NODEKIND(NEW                    ,     NEW                   ,     0                                                                                             ,    New                    )
NODEKIND(NULLABLETYPE           ,     NULLABLETYPE          ,     0                                                                                             ,    NullableType           )
NODEKIND(OP                     ,     BASE                  ,     0                                                                                             ,    Operator               )
NODEKIND(OPENNAME               ,     OPENNAME              ,     0                                                                                             ,    OpenName               )
NODEKIND(OPENTYPE               ,     NAMEDTYPE             ,     0                                                                                             ,    OpenType               )
NODEKIND(OPERATOR               ,     OPERATORMETHOD        ,     NG_METHOD | NG_KEYED | NG_INTERIOR | NG_MEMBER                                                ,    OperatorMethod         )
NODEKIND(PARAMETER              ,     PARAMETER             ,     0                                                                                             ,    Parameter              )
NODEKIND(PARTIALMEMBER          ,     PARTIALMEMBER         ,     NG_MEMBER                                                                                     ,    PartialMember          )
NODEKIND(POINTERTYPE            ,     POINTERTYPE           ,     0                                                                                             ,    PointerType            )
NODEKIND(PREDEFINEDTYPE         ,     PREDEFINEDTYPE        ,     0                                                                                             ,    PredefinedType         )
NODEKIND(PROPERTY               ,     PROPERTY              ,     NG_PROPERTY | NG_KEYED | NG_MEMBER                                                            ,    Property               )
NODEKIND(INDEXER                ,     PROPERTY              ,     NG_PROPERTY | NG_KEYED | NG_MEMBER                                                            ,    Indexer                )
NODEKIND(RETURN                 ,     EXPRSTMT              ,     NG_STATEMENT                                                                                  ,    Return                 )
NODEKIND(THROW                  ,     EXPRSTMT              ,     NG_STATEMENT                                                                                  ,    Throw                  )
NODEKIND(TRY                    ,     TRYSTMT               ,     NG_STATEMENT                                                                                  ,    Try                    )
NODEKIND(TYPEWITHATTR           ,     TYPEWITHATTR          ,     0                                                                                             ,    TypeWithAttr           )
NODEKIND(STRUCT                 ,     STRUCT                ,     NG_TYPE | NG_AGGREGATE | NG_KEYED                                                             ,    Struct                 )
NODEKIND(SWITCH                 ,     SWITCHSTMT            ,     NG_STATEMENT | NG_GLOBALCOMPLETION | NG_BREAKABLE                                             ,    Switch                 )
NODEKIND(UNOP                   ,     UNOP                  ,     0                                                                                             ,    UnaryOperator          )
NODEKIND(UNSAFE                 ,     LABELSTMT             ,     NG_STATEMENT                                                                                  ,    Unsafe                 )
NODEKIND(USING                  ,     USING                 ,     0                                                                                             ,    Using                  )
NODEKIND(VARDECL                ,     VARDECL               ,     NG_KEYED                                                                                      ,    VariableDeclaration    )
NODEKIND(WHILE                  ,     LOOPSTMT              ,     NG_STATEMENT | NG_GLOBALCOMPLETION | NG_BREAKABLE | NG_CONTINUABLE | NG_EMBEDDEDSTMTOWNER     ,    While                  )
NODEKIND(YIELD                  ,     EXPRSTMT              ,     NG_STATEMENT                                                                                  ,    Yield                  )
#undef NODEKIND
