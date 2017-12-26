//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  These are the current bound trees.
//
//  Update src\vb\Language\Model\Semantics\NodeKind.vb when changing this file
//
//-------------------------------------------------------------------------------------------------

#ifndef DISABLE_STATEMENT_NODES
//-------------------------------------------------------------------------------------------------
//
// BLOCK NODES (SB)
//
SX_NL(SB_PROC        , SXK_EXECUTABLE_BLOCK , ProcedureBlock    )
SX_NL(SB_IF_BLOCK    , SXK_EXECUTABLE_BLOCK ,IfGroup )
SX_NL(SB_IF          , SXK_EXECUTABLE_BLOCK , IfBlock)
SX_NL(SB_ELSE_IF     , SXK_EXECUTABLE_BLOCK , IfBlock)
SX_NL(SB_ELSE        , SXK_EXECUTABLE_BLOCK , IfBlock)
SX_NL(SB_SELECT      , SXK_EXECUTABLE_BLOCK , SelectBlock)
SX_NL(SB_CASE        , SXK_EXECUTABLE_BLOCK , CaseBlock)
SX_NL(SB_TRY         , SXK_EXECUTABLE_BLOCK , TryBlock)
SX_NL(SB_CATCH       , SXK_EXECUTABLE_BLOCK , CatchBlock)
SX_NL(SB_FINALLY     , SXK_EXECUTABLE_BLOCK , FinallyBlock )
SX_NL(SB_FOR         , SXK_EXECUTABLE_BLOCK , ForBlock)
SX_NL(SB_FOR_EACH    , SXK_EXECUTABLE_BLOCK , ForEachBlock)
SX_NL(SB_DO          , SXK_EXECUTABLE_BLOCK , LoopBlock)
SX_NL(SB_SYNCLOCK    , SXK_EXECUTABLE_BLOCK , ExecutableBlock)
SX_NL(SB_USING       , SXK_EXECUTABLE_BLOCK , ExecutableBlock)
SX_NL(SB_LOOP        , SXK_EXECUTABLE_BLOCK , LoopBlock)  // Flags distinguish the loop kind (Do..Loop, While..End While, etc.)
SX_NL(SB_WITH        , SXK_EXECUTABLE_BLOCK , WithBlock)
SX_NL(SB_HIDDEN_CODE_BLOCK , SXK_EXECUTABLE_BLOCK , ExecutableBlock)
SX_NL(SB_STATEMENT_LAMBDA , SXK_EXECUTABLE_BLOCK , StatementLambdaBlock)

//-------------------------------------------------------------------------------------------------
//
// STATEMENT NODES (SL)
//
SX_NL(SL_LABEL       , SXK_STMT             , LabelStatement)
SX_NL(SL_STOP        , SXK_STMT             , Statement)
SX_NL(SL_RESUME      , SXK_STMT             , OnErrorStatement)
SX_NL(SL_GOTO        , SXK_STMT             , GotoStatement)
SX_NL(SL_ASYNCSWITCH , SXK_STMT             , AsyncSwitchStatement)
SX_NL(SL_END         , SXK_STMT             , Statement)
SX_NL(SL_EXIT        , SXK_STMT             , ExitStatement)
SX_NL(SL_RETURN      , SXK_STMT             , ReturnStatement)
SX_NL(SL_YIELD       , SXK_STMT             , YieldStatement)
SX_NL(SL_CONTINUE    , SXK_STMT             , ContinueStatement)
SX_NL(SL_ON_ERR      , SXK_STMT             , OnErrorStatement)
SX_NL(SL_STMT        , SXK_STMT             , StatementWithExpression)  // Base statement node.
SX_NL(SL_REDIM       , SXK_STMT             , RedimStatement)
SX_NL(SL_ERROR       , SXK_STMT             , StatementWithExpression)
SX_NL(SL_VAR_DECL    , SXK_STMT             , StatementWithExpression)  // Statement to represent local variable declarations (for XML generation only).
SX_NL(SL_DEBUGLOCNOP , SXK_STMT             , Statement) // Inserts a nop instruction and sequence point for debugging

#endif // DISABLE_STATEMENT_NODES

#ifndef DISABLE_EXPRESSION_NODES
//-------------------------------------------------------------------------------------------------
//
// EXPRESSION NODES (SX)
//
SX_NL(SX_NAME        , SXK_LEAF             , ArgumentNameExpression)
SX_NL(SX_APPL_ATTR   , SXK_BINOP            , AttributeApplicationExpression)
SX_NL(SX_NEW_ARRAY   , SXK_UNOP             , BinaryExpression   )
SX_NL(SX_CREATE_ARRAY, SXK_UNOP             , BinaryExpression   )
SX_NL(SX_ARG         , SXK_SPECIAL|SXK_BINOP, ArgumentExpression)
SX_NL(SX_CNS_INT     , SXK_LEAF|SXK_CONST   , IntegralConstantExpression)
SX_NL(SX_CNS_DEC     , SXK_LEAF|SXK_CONST   , DecimalConstantExpression)
SX_NL(SX_CNS_FLT     , SXK_LEAF|SXK_CONST   , FloatConstantExpression  )
SX_NL(SX_CNS_STR     , SXK_LEAF|SXK_CONST   , StringConstantExpression)
SX_NL(SX_BAD         , SXK_UNOP             , BinaryExpression   )
SX_NL(SX_CTYPE       , SXK_UNOP             , BinaryExpression   )
SX_NL(SX_DIRECTCAST  , SXK_UNOP             , BinaryExpression   )
SX_NL(SX_TRYCAST     , SXK_UNOP             , BinaryExpression   )
SX_NL(SX_WIDE_COERCE , SXK_UNOP             , BinaryExpression   )  // Used as a place holder for representing coercions from reference types to more general reference types.
SX_NL(SX_ARRAYLEN    , SXK_UNOP             , BinaryExpression   )
SX_NL(SX_NOT         , SXK_UNOP             , BinaryExpression   )
SX_NL(SX_ADDRESSOF   , SXK_UNOP             , BinaryExpression   )  // Produced and consumed during semantic analysis, and should never reach code generation.
SX_NL(SX_PLUS        , SXK_UNOP             , BinaryExpression   )
SX_NL(SX_AWAIT       , SXK_SPECIAL|SXK_UNOP , AwaitExpression    )  // Produced by InterpretAwait, consumed during MakeResumable, and should never reach code generation

//-------------------------------------------------------------------------------------------------
//
// Operator order is important starting here 
// Tables in semantics and codegen depend on order 

SX_NL(SX_NEG         , SXK_UNOP             , BinaryExpression   )
SX_NL(SX_ADD         , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_SUB         , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_MUL         , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_DIV         , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_MOD         , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_POW         , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_IDIV        , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_SHIFT_LEFT  , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_SHIFT_RIGHT , SXK_BINOP            , BinaryExpression   )
SX_NL_EX(SX_ORELSE   , SXK_BINOP|SXK_LOGOP  , BinaryExpression       , ShortCircuitBooleanOperatorExpression   )
SX_NL_EX(SX_ANDALSO  , SXK_BINOP|SXK_LOGOP  , BinaryExpression       , ShortCircuitBooleanOperatorExpression   )
SX_NL(SX_CONC        , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_LIKE        , SXK_BINOP|SXK_RELOP  , BinaryExpression   )
SX_NL(SX_EQ          , SXK_BINOP|SXK_RELOP  , BinaryExpression   )
SX_NL(SX_NE          , SXK_BINOP|SXK_RELOP  , BinaryExpression   )
SX_NL(SX_LE          , SXK_BINOP|SXK_RELOP  , BinaryExpression   )
SX_NL(SX_GE          , SXK_BINOP|SXK_RELOP  , BinaryExpression   )
SX_NL(SX_LT          , SXK_BINOP|SXK_RELOP  , BinaryExpression   )
SX_NL(SX_GT          , SXK_BINOP|SXK_RELOP  , BinaryExpression   )
SX_NL(SX_OR          , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_XOR         , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_AND         , SXK_BINOP            , BinaryExpression   )

//
// End of table dependent on order
//
//-------------------------------------------------------------------------------------------------

SX_NL(SX_IS          , SXK_BINOP|SXK_RELOP  , BinaryExpression   )
SX_NL(SX_ISNOT       , SXK_BINOP|SXK_RELOP  , BinaryExpression   )
SX_NL(SX_LIST        , SXK_BINOP            , ExpressionWithChildren)
SX_NL(SX_IF          , SXK_BINOP            , ExpressionWithChildren)  // Implements a conditional expression. If the left operand evaluates to true, the right operand is evaluated. The result type must be Void.
SX_NL(SX_ISTYPE      , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_NEW         , SXK_SPECIAL          , NewExpression)
SX_NL(SX_ASG         , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_NOTHING     , SXK_LEAF|SXK_CONST   , Expression    )
SX_NL(SX_BOGUS       , SXK_LEAF|SXK_CONST   , Expression    )
SX_NL(SX_METATYPE    , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_ASG_RESADR  , SXK_BINOP            , BinaryExpression   )  // ExpressionWithChildren with an addr result.
SX_NL(SX_INDEX       , SXK_SPECIAL|SXK_BINOP, IndexExpression   )
SX_NL(SX_VARINDEX    , SXK_SPECIAL|SXK_BINOP, VariantIndexExpression)  // v(1), where v is Variant scalar.
SX_NL(SX_LATE        , SXK_SPECIAL|SXK_BINOP, LateBoundExpression)  // Late-bound member access.
SX_NL(SX_ADR         , SXK_UNOP             , BinaryExpression   )
SX_NL(SX_SYM         , SXK_SPECIAL          , SymbolReferenceExpression)
SX_NL(SX_IIF         , SXK_SPECIAL|SXK_BINOP, IfExpression     )
SX_NL(SX_IIFCoalesce , SXK_BINOP            , CoalesceExpression)
SX_NL(SX_CTYPEOP     , SXK_SPECIAL|SXK_UNOP , LiftedCTypeExpression )
SX_NL(SX_CALL        , SXK_SPECIAL|SXK_BINOP, CallExpression)
SX_NL(SX_SEQ         , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_SEQ_OP1     , SXK_BINOP            , BinaryExpression   )
SX_NL(SX_SEQ_OP2     , SXK_BINOP            , BinaryExpression   )

SX_NL(SX_SYNCLOCK_CHECK    , SXK_BINOP            , ExpressionWithChildren               ) // Synclock validator.
SX_NL(SX_DELEGATE_CTOR_CALL, SXK_SPECIAL          , DelegateConstructorCallExpression) // CallExpression to a delegate constructor.
SX_NL(SX_INIT_STRUCTURE    , SXK_SPECIAL          , InitStructureExpression    )
SX_NL(SX_PROPERTY_REFERENCE, SXK_SPECIAL|SXK_BINOP, PropertyReferenceExpression) // Occurs only within semantic analysis to hold an intermediate state of processing references to properties.
SX_NL(SX_LATE_REFERENCE    , SXK_BINOP            , PropertyReferenceExpression) // Occurs only within semantic analysis to hold an intermediate state of processing late-bound references.

SX_NL(SX_NAME_NOT_FOUND    , SXK_LEAF             , Expression             ) // Occurs only within semantic analysis during processing of qualified name expressions under the debugger.
SX_NL(SX_OVERLOADED_GENERIC, SXK_SPECIAL          , OverloadedGenericExpression ) // Produced and consumed during semantic analysis, and should never reach code generation.

SX_NL(SX_EXTENSION_CALL    , SXK_SPECIAL          , ExtensionCallExpression    ) // Occurs only within semantics analysis to hold an intermediate state of processing extension method calls.
SX_NL(SX_LAMBDA            , SXK_SPECIAL          , LambdaExpression) // Occurs only within semantic analysis to hold an intermediate state of processing lambda expressions.
SX_NL(SX_UNBOUND_LAMBDA    , SXK_SPECIAL          , UnboundLambdaExpression) // Occurs only within semantic analysis to hold an intermediate state of processing lambda expressions.
SX_NL(SX_ANONYMOUSTYPE     , SXK_SPECIAL          , AnonymousTypeExpression) // Occurs only within semantic analysis to hold intermediate state of anonymous types.
SX_NL(SX_DEFERRED_TEMP     , SXK_SPECIAL          , DeferredTempExpression) // Occurs only within semantic analysis as a place holder for a temporary local to be allocated an initialized at the top-most procedure level.  Used by xml literals for temps to cache XNames and XNamespaces.
SX_NL(SX_ISTRUE            , SXK_UNOP             , BinaryExpression )
SX_NL(SX_ISFALSE           , SXK_UNOP             , BinaryExpression )
SX_NL(SX_ARRAYLITERAL      , SXK_SPECIAL          , ArrayLiteralExpression )
SX_NL(SX_NESTEDARRAYLITERAL, SXK_SPECIAL          , NestedArrayLiteralExpression)
SX_NL(SX_COLINIT           , SXK_SPECIAL          , ColInitExpression)
SX_NL(SX_COLINITELEMENT    , SXK_SPECIAL          , ColInitElementExpression)
SX_NL(SX_ASYNCSPILL        , SXK_SPECIAL          , AsyncSpillExpression) // Produced by ResumableMethodLowerer from an SX_AWAIT node; reaches code generation.

#endif // DISABLE_EXPRESSION_NODES

#undef  SX_NL
