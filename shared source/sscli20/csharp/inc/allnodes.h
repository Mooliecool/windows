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

#ifndef DECLARE_NODE
#error You must define DECLARE_NODE before including allnodes.h
#endif

#ifndef END_NODE
#define END_NODE()
#endif

#ifndef PARENT_NODE
#define PARENT_NODE(t,n)
#endif
#ifndef CHILD_NODE
#define CHILD_NODE(t,n)
#endif
#ifndef CHILD_OPTIONAL_NODE
#define CHILD_OPTIONAL_NODE(t,n)
#endif
#ifndef CHILD_NAME
#define CHILD_NAME(n)
#endif
#ifndef CHILD_MEMBER
#define CHILD_MEMBER(t,n)
#endif
#ifndef NODE_INDEX
#define NODE_INDEX(n)
#endif
#ifndef FIRST_NODE
#define FIRST_NODE(t,n)
#endif
#ifndef NEXT_NODE
#define NEXT_NODE(t,n)
#endif
#ifndef INTERIOR_NODE
#define INTERIOR_NODE(n)
#endif

////////////////////////////////////////////////////////////////////////////////
// NAMESPACENODE

DECLARE_NODE (NAMESPACE, BASE)
#define CURRENT_TYPE NAMESPACENODE
    CHILD_NODE(BASE, pName)         // Name of namespace (possibly empty, or ""?)
    CHILD_NODE(BASE, pUsing)        // List of using clauses
    CHILD_NODE(BASE, pGlobalAttr)   // List of global attributes
    CHILD_NODE(BASE, pElements)     // Elements declared in this namespace
    NODE_INDEX(iOpen)               // Open curly position
    NODE_INDEX(iClose)              // Close curly position
    CHILD_NAME(pKey)                // Key
    CHILD_NAME(pNameSourceFile)     // Only non-null for the top-level namespace in a file, it has
									// the name of the source file (primarily for error reporting usage).
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// USINGNODE

DECLARE_NODE (USING, BASE)
#define CURRENT_TYPE USINGNODE
    CHILD_NODE(BASE, pName)         // Name used (NULL if an extern alias)
    CHILD_NODE(NAME, pAlias)        // Alias (NULL indicates using-namespace)
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// NAMENODE

DECLARE_NODE (NAME, BASE)
#define CURRENT_TYPE NAMENODE
    CHILD_NAME(pName)               // Name text
    CHILD_OPTIONAL_NODE(GENERICNAME, pPossibleGenericName)
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// GENERICNAMENODE

DECLARE_NODE (GENERICNAME, NAME)
#define CURRENT_TYPE GENERICNAMENODE
    CHILD_NODE(BASE, pParams)       // Parameters to qualified type name, only used for fully qualified names in 
                                    // explicit impls, e.g. System.Collections.IEnumerator<Object>.GetEnumerator()
    NODE_INDEX(iOpen)               // Open "<" token index
    NODE_INDEX(iClose)              // Close ">" token index
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// OPENNAMENODE

DECLARE_NODE (OPENNAME, NAME)
#define CURRENT_TYPE OPENNAMENODE
    NODE_INDEX(iOpen)       // Open "<" token index
    NODE_INDEX(iClose)      // Close ">" token index
    CHILD_MEMBER(int, carg) // Number of blank parameters
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// CLASSNODE -- class/struct

DECLARE_NODE (AGGREGATE, BASE)
#define CURRENT_TYPE AGGREGATENODE
    CHILD_NODE(BASE, pAttr)         // Attributes
    CHILD_NODE(NAME, pName)         // Name of class
    CHILD_NODE(BASE, pBases)        // List of base class/interfaces
    CHILD_NODE(BASE, pTypeParams)   // List of type parameters
    CHILD_NODE(BASE, pConstraints)  // List of Constraints on type parameters
    FIRST_NODE(MEMBER, pMembers)    // List of members
    NODE_INDEX(iStart)              // First non-attribute token
    NODE_INDEX(iOpen)               // Open curly
    NODE_INDEX(iClose)              // Close curly
    CHILD_NAME(pKey)                // Key
#undef CURRENT_TYPE
END_NODE()

DECLARE_NODE (CLASS, AGGREGATE)
#define CURRENT_TYPE CLASSNODE
#undef CURRENT_TYPE
END_NODE();

DECLARE_NODE (ENUM, AGGREGATE)
#define CURRENT_TYPE ENUMNODE
#undef CURRENT_TYPE
END_NODE();

DECLARE_NODE (STRUCT, AGGREGATE)
#define CURRENT_TYPE STRUCTNODE
#undef CURRENT_TYPE
END_NODE();

DECLARE_NODE (INTERFACE, AGGREGATE)
#define CURRENT_TYPE INTERFACENODE
#undef CURRENT_TYPE
END_NODE();

////////////////////////////////////////////////////////////////////////////////
// CONSTRAINTNODE -- constraint on a type parameter

DECLARE_NODE (CONSTRAINT, BASE)
#define CURRENT_TYPE CONSTRAINTNODE
    CHILD_NODE(NAME, pName)         // Name of type variable
    CHILD_NODE(BASE, pBounds)       // List of type bounds
    NODE_INDEX(iEnd)                // Last token index
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// DELEGATENODE

DECLARE_NODE (DELEGATE, BASE)
#define CURRENT_TYPE DELEGATENODE
    CHILD_NODE(BASE, pAttr)         // Attributes
    CHILD_NODE(TYPEBASE, pType)         // Return type
    CHILD_NODE(NAME, pName)         // Name
    NODE_INDEX(iOpenParen)
    CHILD_NODE(BASE, pParms)        // Parameters
    NODE_INDEX(iCloseParen)
    CHILD_NODE(BASE, pTypeParams)   // Type parameters and their bounds
    CHILD_NODE(BASE, pConstraints)  // List of Constraints on type parameters
    NODE_INDEX(iStart)              // First non-attribute token
    NODE_INDEX(iSemi)               // Semicolon position
    CHILD_NAME(pKey)                // Key
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// MEMBERNODE -- base class for all members

DECLARE_NODE (MEMBER, BASE)
#define CURRENT_TYPE MEMBERNODE
    CHILD_NODE(BASE, pAttr)         // Attributes
    NEXT_NODE(MEMBER, pNext)        // Next member
    NODE_INDEX(iStart)              // First non-attribute token
    NODE_INDEX(iClose)              // Semicolon or close-curly terminator
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// ENUMMBRNODE -- member for enums

DECLARE_NODE (ENUMMBR, MEMBER)
#define CURRENT_TYPE ENUMMBRNODE
    CHILD_NODE(NAME, pName)
    CHILD_NODE(BASE, pValue)
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// FIELDNODE

DECLARE_NODE (FIELD, MEMBER)
#define CURRENT_TYPE FIELDNODE
    CHILD_NODE(TYPEBASE, pType)         // Type of field
    CHILD_NODE(BASE, pDecls)        // Declarations
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// METHODBASENODE

DECLARE_NODE (METHODBASE, MEMBER)
#define CURRENT_TYPE METHODBASENODE
    CHILD_NODE(TYPEBASE, pType)         // Return type of method
    NODE_INDEX(iOpenParen)
    CHILD_NODE(BASE, pParms)        // Parameter list
    NODE_INDEX(iCloseParen)
    NODE_INDEX(iOpen)               // Token index of open curly/semicolon
    CHILD_NODE(BLOCK, pBody)         // Method body (if parsed)
    INTERIOR_NODE(pInteriorNode)       // Interior node container object (if parsed)
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// METHODNODE

DECLARE_NODE (METHOD, METHODBASE)
#define CURRENT_TYPE METHODNODE
    CHILD_NODE(BASE, pName)         // (NK_METHOD only -- Name of method)
    CHILD_NODE(BASE, pConstraints)  // List of Constraints on type parameters
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// OPERATORMETHODNODE

DECLARE_NODE (OPERATORMETHOD, METHODBASE)
#define CURRENT_TYPE OPERATORMETHODNODE
    CHILD_MEMBER(OPERATOR, iOp)     // (NK_OPERATOR only -- overloaded operator)
    CHILD_MEMBER(TOKENID, tok)      // Used when iOp is OP_NONE (error cases)
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// CTORMETHODNODE

DECLARE_NODE (CTORMETHOD, METHODBASE)
#define CURRENT_TYPE CTORMETHODNODE
    CHILD_NODE(CALL, pThisBaseCall)  //The call to this( or to base(
#undef CURRENT_TYPE
END_NODE()

///////////////////////////////////////////////////////////////////////////////
// PROPERTYNODE

DECLARE_NODE (PROPERTY, MEMBER)
#define CURRENT_TYPE PROPERTYNODE
    CHILD_NODE(BASE, pName)         // Name of property, or name of interface for indexer
    CHILD_NODE(TYPEBASE, pType)         // Type of property
    NODE_INDEX(iOpenSquare)
    CHILD_NODE(BASE, pParms)        // Index parameters
    NODE_INDEX(iCloseSquare)
    NODE_INDEX(iOpen)               // Token index of opening '{'
    CHILD_NODE(ACCESSOR, pGet)          // Get accessor
    CHILD_NODE(ACCESSOR, pSet)          // Set accessor
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// ACCESSORNODE

DECLARE_NODE (ACCESSOR, BASE)
#define CURRENT_TYPE ACCESSORNODE
    CHILD_NODE(BASE, pAttr)         // Attributes
    NODE_INDEX(iOpen)               // Token index of open curly
    NODE_INDEX(iClose)              // Token index of close curly
    CHILD_NODE(BLOCK, pBody)         // Body of node
    INTERIOR_NODE(pInteriorNode)       // Interior node container object (if parsed)
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// PARAMETERNODE

DECLARE_NODE (PARAMETER, BASE)
#define CURRENT_TYPE PARAMETERNODE
    CHILD_NODE(BASE, pAttr)         // Attributes on parameter
    CHILD_NODE(TYPEBASE, pType)     // Type of parameter
    CHILD_NODE(NAME, pName)         // Name of parameter
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// NESTEDTYPENODE

DECLARE_NODE (NESTEDTYPE, MEMBER)
#define CURRENT_TYPE NESTEDTYPENODE
    CHILD_NODE(BASE, pType)         // Nested type
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// PARTIALMEMBERNODE

DECLARE_NODE (PARTIALMEMBER, MEMBER)
#define CURRENT_TYPE PARTIALMEMBERNODE
    CHILD_NODE(BASE, pNode)         // Whatever
#undef CURRENT_TYPE
END_NODE()


////////////////////////////////////////////////////////////////////////////////
// TYPEBASENODE

DECLARE_NODE (TYPEBASE, BASE)
#define CURRENT_TYPE TYPEBASENODE
#undef CURRENT_TYPE
END_NODE()


////////////////////////////////////////////////////////////////////////////////
// PREDEFINEDTYPENODE

DECLARE_NODE (PREDEFINEDTYPE, TYPEBASE)
#define CURRENT_TYPE PREDEFINEDTYPENODE
    CHILD_MEMBER(BYTE, iType)
#undef CURRENT_TYPE
END_NODE()


////////////////////////////////////////////////////////////////////////////////
// NAMEDTYPENODE

DECLARE_NODE (NAMEDTYPE, TYPEBASE)
#define CURRENT_TYPE NAMEDTYPENODE
    CHILD_NODE(BASE, pName)
#undef CURRENT_TYPE
END_NODE()


////////////////////////////////////////////////////////////////////////////////
// ARRAYTYPENODE

DECLARE_NODE (ARRAYTYPE, TYPEBASE)
#define CURRENT_TYPE ARRAYTYPENODE
    CHILD_NODE(TYPEBASE, pElementType)
    CHILD_MEMBER(int, iDims)             // Number of dimensions (-1 == unknown, i.e. [?])
#undef CURRENT_TYPE
END_NODE()


////////////////////////////////////////////////////////////////////////////////
// POINTERTYPENODE

DECLARE_NODE (POINTERTYPE, TYPEBASE)
#define CURRENT_TYPE POINTERTYPENODE
    CHILD_NODE(TYPEBASE, pElementType)
#undef CURRENT_TYPE
END_NODE()


////////////////////////////////////////////////////////////////////////////////
// NULLABLETYPENODE

DECLARE_NODE (NULLABLETYPE, TYPEBASE)
#define CURRENT_TYPE NULLABLETYPENODE
    CHILD_NODE(TYPEBASE, pElementType)
#undef CURRENT_TYPE
END_NODE()


////////////////////////////////////////////////////////////////////////////////
// STATEMENTNODE

DECLARE_NODE (STATEMENT, BASE)
#define CURRENT_TYPE STATEMENTNODE
    NEXT_NODE(STATEMENT, pNext)         // Next statement
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// BLOCKNODE

DECLARE_NODE (BLOCK, STATEMENT)
#define CURRENT_TYPE BLOCKNODE
    FIRST_NODE(STATEMENT, pStatements)   // List of statements
    NODE_INDEX(iClose)              // Close curly
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// EXPRSTMTNODE -- a multi-purpose statement node (expression statements,
// goto, case, default, return, etc.)

DECLARE_NODE (EXPRSTMT, STATEMENT)
#define CURRENT_TYPE EXPRSTMTNODE
    CHILD_NODE(BASE, pArg)          // Argument
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// LABELSTMTNODE

DECLARE_NODE (LABELSTMT, STATEMENT)
#define CURRENT_TYPE LABELSTMTNODE
    CHILD_NODE(BASE, pLabel)        // Label name
    CHILD_NODE(STATEMENT, pStmt)    // Statement
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// LOOPSTMTNODE -- handles both do and while statements

DECLARE_NODE (LOOPSTMT, STATEMENT)
#define CURRENT_TYPE LOOPSTMTNODE
    CHILD_NODE(BASE, pExpr)         // Expression
    CHILD_NODE(STATEMENT, pStmt)    // Statement
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// FORSTMTNODE

DECLARE_NODE (FORSTMT, STATEMENT)
#define CURRENT_TYPE FORSTMTNODE
    CHILD_NODE(BASE, pInit)         // Init statement/expression(s)
    CHILD_NODE(BASE, pExpr)         // Condition expression
    CHILD_NODE(BASE, pInc)          // Increment expression(s)
    NODE_INDEX(iCloseParen)         // Token index of close paren
    NODE_INDEX(iInKeyword)         // Token index of in keyword in foreach stmt
    CHILD_NODE(STATEMENT, pStmt)    // Iterated statement
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// IFSTMTNODE

DECLARE_NODE (IFSTMT, STATEMENT)
#define CURRENT_TYPE IFSTMTNODE
    CHILD_NODE(BASE, pExpr)         // Condition
    CHILD_NODE(STATEMENT, pStmt)         // TRUE statement
    CHILD_NODE(STATEMENT, pElse)         // FALSE statement
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// DECLSTMTNODE

DECLARE_NODE (DECLSTMT, STATEMENT)
#define CURRENT_TYPE DECLSTMTNODE
    CHILD_NODE(TYPEBASE, pType)         // Type of declaration
    CHILD_NODE(BASE, pVars)         // Declared variable list
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// SWITCHSTMTNODE

DECLARE_NODE (SWITCHSTMT, STATEMENT)
#define CURRENT_TYPE SWITCHSTMTNODE
    CHILD_NODE(BASE, pExpr)         // Switch expression
    CHILD_NODE(BASE, pCases)        // List of cases
    NODE_INDEX(iOpen)               // Open curly
    NODE_INDEX(iClose)              // Close curly
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// CASENODE

DECLARE_NODE (CASE, BASE)
#define CURRENT_TYPE CASENODE
    CHILD_NODE(BASE, pLabels)       // List of case labels
    FIRST_NODE(STATEMENT, pStmts)   // Statement list
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// TRYSTMTNODE

DECLARE_NODE (TRYSTMT, STATEMENT)
#define CURRENT_TYPE TRYSTMTNODE
    CHILD_NODE(BLOCK, pBlock)        // Statement block (guarded)
    CHILD_NODE(BASE, pCatch)        // Catch blocks or finally block
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// CATCHNODE

DECLARE_NODE (CATCH, BASE)
#define CURRENT_TYPE CATCHNODE
    CHILD_NODE(BASE, pType)         // Type caught
    CHILD_NODE(NAME, pName)         // Name (optional)
    CHILD_NODE(BLOCK, pBlock)        // Block
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// UNOPNODE -- operator in flags

DECLARE_NODE (UNOP, BASE)
#define CURRENT_TYPE UNOPNODE
    CHILD_NODE(BASE, p1)            // Operand 1
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// BINOPNODE -- operator in flags

DECLARE_NODE (BINOP, BASE)
#define CURRENT_TYPE BINOPNODE
    CHILD_NODE(BASE, p1)            // Operand 1
    CHILD_NODE(BASE, p2)            // Operand 2
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// CALLNODE

DECLARE_NODE (CALL, BINOP)
#define CURRENT_TYPE CALLNODE
    NODE_INDEX(iClose)              // Token index of close paren
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// CONSTVALNODE -- type in flags

DECLARE_NODE (CONSTVAL, BASE)
#define CURRENT_TYPE CONSTVALNODE
    CHILD_MEMBER(CONSTVAL, val)     // Value of constant
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// VARDECLNODE

DECLARE_NODE (VARDECL, BASE)
#define CURRENT_TYPE VARDECLNODE
    CHILD_NODE(NAME, pName)         // Name of variable
    CHILD_NODE(BASE, pArg)          // Init expression or array dim expression
    PARENT_NODE(BASE, pDecl)        // Pointer to parent decl node
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// NEWNODE

DECLARE_NODE (NEW, BASE)
#define CURRENT_TYPE NEWNODE
    CHILD_NODE(TYPEBASE, pType)         // Type
    CHILD_NODE(BASE, pArgs)         // Constructor args or dim expression list
    CHILD_NODE(BASE, pInit)         // Array initializer
    NODE_INDEX(iOpen)               // Token index of '('
    NODE_INDEX(iClose)              // Token index of ')' or ']' for array
#undef CURRENT_TYPE
END_NODE ()

////////////////////////////////////////////////////////////////////////////////
// ATTRNODE

DECLARE_NODE (ATTR, BASE)
#define CURRENT_TYPE ATTRNODE
    CHILD_NODE(BASE, pName)         // Attribute name (possibly dotted, or possibly NULL if NK_ATTRARG)
    CHILD_NODE(BASE, pArgs)         // Arguments (NK_ATTR) or expression (NK_ATTRARG)
    NODE_INDEX(iOpen)               // Token index of (
    NODE_INDEX(iClose)              // Token index of )
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// ATTRDECLNODE

DECLARE_NODE (ATTRDECL, BASE)
#define CURRENT_TYPE ATTRDECLNODE
    CHILD_NODE(NAME, pNameNode)     // User specified attr location. null if default.
    CHILD_MEMBER(ATTRLOC, location) // Attribute location allways valid.
    NODE_INDEX(iClose)              // Token index of ']'
    CHILD_NODE(BASE, pAttr)         // Attribute list
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// ANONBLOCKNODE

DECLARE_NODE (ANONBLOCK, BASE)
#define CURRENT_TYPE ANONBLOCKNODE
    CHILD_NODE(BASE, pArgs)         // List of parameters
    CHILD_NODE(BLOCK, pBody)         // Body of the anonymous method block
    NODE_INDEX(iClose)              // Token index of the close paren (if it exists)
#undef CURRENT_TYPE
END_NODE()

////////////////////////////////////////////////////////////////////////////////
// TYPEWITHATTRNODE

DECLARE_NODE (TYPEWITHATTR, TYPEBASE)
#define CURRENT_TYPE TYPEWITHATTRNODE
    CHILD_NODE(BASE, pAttr)        // Attributes
    CHILD_NODE(TYPEBASE, pType)    // Real type
#undef CURRENT_TYPE
END_NODE()

#undef DECLARE_NODE
#undef END_NODE

#undef PARENT_NODE
#undef CHILD_OPTIONAL_NODE
#undef CHILD_NODE
#undef CHILD_NAME
#undef CHILD_MEMBER
#undef NODE_INDEX
#undef FIRST_NODE
#undef NEXT_NODE
#undef INTERIOR_NODE
