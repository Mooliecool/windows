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
// File: nodes.h
//
// Definitions of all parse tree nodes
// ===========================================================================

#ifndef _NODES_H_
#define _NODES_H_

#include "constval.h"
#include "enum.h"
#include "strbuild.h"

////////////////////////////////////////////////////////////////////////////////
// COMPILER_VERSION
//
// This value is used to ensure that a consumer of the compiler DLL is compatible
// with it, meaning they are using the same, or at least binary compatible,
// versions of the header files defining structures, constants, interfaces, etc.
// This value must be incremented when a change is made that breaks compatibility
// to avoid mismatched versions of the compiler and language service from being
// used together.  Change COMPILER_BASE_VERSION since we set the high order bit
// for debug builds so we can distinguish debug from retail, which are
// incompatible, and should cause an error
// NOTE: This includes the Predefined Types and Names tables!


#define COMPILER_VERSION_BEFORE_NT 0x00000057


#define COMPILER_BASE_VERSION (COMPILER_VERSION_BEFORE_NT | ((PN_COUNT << 24) & 0x7F000000))
#ifdef _DEBUG
#define COMPILER_VERSION  (0x80000000 | COMPILER_BASE_VERSION)
#else
#define COMPILER_VERSION  (COMPILER_BASE_VERSION)
#endif

////////////////////////////////////////////////////////////////////////////////
// Forward declarations of node structs

#define NODEKIND(n,s,g,p) struct s##NODE;
#include "nodekind.h"
struct TYPEBASENODE;
struct AGGREGATENODE;

////////////////////////////////////////////////////////////////////////////////
// NODEFLAGS
//
// NOTE:  NF_MOD_* values must be kept in sync with CLSDREC::accessTokens
//
// We have 16 bits of space for flags in BASENODE.  We have more than 16 flags.
// Thus, we compact them into groups such that no two flags that could be set on
// the same node have the same value.  This is easier thought of as groups of
// flags; no two flag GROUPS that could have representation on the same node
// can have overlap.

enum NODEFLAGS
{
    // MODIFIER flags (0x0000-0x0FFF)
    NF_MOD_ABSTRACT         = 0x0001,
    NF_MOD_NEW              = 0x0002,
    NF_MOD_OVERRIDE         = 0x0004,
    NF_MOD_PRIVATE          = 0x0008,
    NF_MOD_PROTECTED        = 0x0010,
    NF_MOD_INTERNAL         = 0x0020,
    NF_MOD_PUBLIC           = 0x0040,
    NF_MOD_ACCESSMODIFIERS  = NF_MOD_PUBLIC | NF_MOD_INTERNAL | NF_MOD_PROTECTED | NF_MOD_PRIVATE,
    NF_MOD_SEALED           = 0x0080,
    NF_MOD_STATIC           = 0x0100,
    NF_MOD_VIRTUAL          = 0x0200,
    NF_MOD_EXTERN           = 0x0400,
    NF_MOD_READONLY         = 0x0800,
    NF_MOD_VOLATILE         = 0x1000,   
    NF_MOD_UNSAFE           = 0x2000,   // ok to overlap with NF_PARMMOD_REF -- can't ever appear on an expression
    NF_MOD_LAST_KWD         = NF_MOD_UNSAFE,
    NF_MOD_PARTIAL          = 0x4000,   // (same)
    NF_MOD_LAST             = NF_MOD_PARTIAL,
    NF_MOD_MASK             = 0x7FFF,

    // EXPRESSION flags -- KEEP THESE UNIQUE, they are tested for without regard to node kind
    NF_PARMMOD_REF          = 0x2000,
    NF_PARMMOD_OUT          = 0x4000,
    NF_CALL_HADERROR        = 0x8000,

    // STATEMENT flags
    NF_UNCHECKED            = 0x0001,
    NF_GOTO_CASE            = 0x0002,
    NF_TRY_CATCH            = 0x0004,
    NF_TRY_FINALLY          = 0x0008,
    NF_CONST_DECL           = 0x0010,
    NF_FOR_FOREACH          = 0x0020,
    NF_FIXED_DECL           = 0x0040,
    NF_USING_DECL           = 0x0080,
    NF_STMT_HADERROR        = 0x0100,

    // VARDECL flags
    NF_VARDECL_EXPR         = 0x0001,
    NF_VARDECL_ARRAY        = 0x0002,

    // Numeric constant flags
    NF_CHECK_FOR_UNARY_MINUS= 0x0001,

    // Member ref flags
    NF_MEMBERREF_EMPTYARGS  = 0x0001,

    // Name, GenericName or Alias flags
    NF_NAME_MISSING         = 0x0001,       // NK_NAME or NK_ALIASNAME where no identifier existed
    NF_NAME_LITERAL         = 0x0002,       // NK_NAME or NK_ALIASNAME with a '@' prefix
    NF_GLOBAL_QUALIFIER     = 0x0004,       // NK_ALIASNAME whose identifier is 'global'

    // 'new' flags
    NF_NEW_STACKALLOC       = 0x0001,       // NEWNODE contains a 'stackalloc' expression
    
    // constraint flags
    NF_CONSTRAINT_NEWABLE   = 0x0001,       // NK_CONSTRAINT has default constructor - 'new()'
    NF_CONSTRAINT_REFTYPE   = 0x0002,       // NK_CONSTRAINT has reference type constraint
    NF_CONSTRAINT_VALTYPE   = 0x0004,       // NK_CONSTRAINT has value type constraint

    // MEMBER flags (EX -- stored in BASENODE::other)
    NFEX_CTOR_BASE          = 0x01,
    NFEX_CTOR_THIS          = 0x02,
    NFEX_METHOD_NOBODY      = 0x04,
    NFEX_INTERIOR_PARSED    = 0x08,         // This interior node has already been parsed for errors...
    NFEX_METHOD_VARARGS     = 0x10,
    NFEX_METHOD_PARAMS      = 0x20,
    NFEX_EVENT              = 0x40,         // field or property is actually an event.

};

////////////////////////////////////////////////////////////////////////////////
// NODEGROUP
//
// These are used to categorize nodes into groups that can easily be tested for
// by using BASENODE::InGroup(group), or given a node kind, checking
// BASENODE::m_rgNodeGroups[kind] & group.

enum NODEGROUP
{
    NG_TYPE                 = 0x00000001,       // Any non-namespace member of a namespace
    NG_AGGREGATE            = 0x00000002,       // Class, struct, enum, interface
    NG_METHOD               = 0x00000004,       // Method, ctor, dtor, operator
    NG_PROPERTY             = 0x00000008,       // Property, indexer
    NG_FIELD                = 0x00000010,       // Field, const
    NG_STATEMENT            = 0x00000020,       // Any statement node
    NG_BINOP                = 0x00000040,       // Any BINOP node
    NG_KEYED                = 0x00000080,       // Any node which can be 'keyed'
    NG_INTERIOR             = 0x00000100,       // Any node that has an interior, parsed on 2nd pass
    NG_MEMBER               = 0x00000200,       // Any member of a type (MEMBERNODE)
    NG_GLOBALCOMPLETION     = 0x00000400,       // Allow "blank-line" completion on this node
    NG_BREAKABLE            = 0x00000800,       // Statement that 'break' works for
    NG_CONTINUABLE          = 0x00001000,       // Statement that 'continue' works for
    NG_EMBEDDEDSTMTOWNER    = 0x00002000,       // Statement that 'owns' embedded stmts
};

////////////////////////////////////////////////////////////////////////////////
// PROTOFLAGS
//
// Flags to control output of BASENODE::BuildPrototype

enum PROTOFLAGS
{
    // Name control
    PTF_BASENAME            = 0x00000000,       // Base name only (default)
    PTF_FULLNAME            = 0x00000001,       // Fully-qualified name (namespace included)
    PTF_TYPENAME            = 0x00000002,       // Name of entity plus type (if member)

    PTF_PARAMETER_TYPE       = 0x00000004,
    PTF_PARAMETER_NAME       = 0x00000008,
    // Parameter control
    PTF_PARAMETERS          = PTF_PARAMETER_TYPE | PTF_PARAMETER_NAME,
    // Typevars control
    PTF_TYPEVARS            = 0x00000010,       // Include type vars
    PTF_FAILONMISSINGNAME   = 0x00000020,
};

////////////////////////////////////////////////////////////////////////////////
// PARAMTYPES
//
// Flags to control output of BASENODE::AppendParametersTo*

enum PARAMTYPES
{
    PRT_METHOD = 0,         // a method, use "(" and ")" (default)
    PRT_INDEXER,            // an indexer, use "[" and "]"
    PRT_GENERIC,            // a generic type speicifer, use "<" and ">"
};


#ifndef RETAILVERIFY
#endif // !RETAILVERIFY


class NRHEAP;
class CInteriorNode;

////////////////////////////////////////////////////////////////////////////////
// BASENODE -- base class for all parse tree nodes

struct BASENODE
{
#ifdef DEBUG
    PCSTR       pszNodeKind;        // Debug only...
    virtual void zDummy() {} // add a vtable for debugging
    bool fHasNid;
#endif

    // struct {
    NODEKIND    kind:8;         // Node kind
    unsigned    flags:16;       // Flags (NF_* bits)
    unsigned    other:8;        // Extra (usage depends on kind)
    // };

    long            tokidx;             // Token index (for access to position info)
    BASENODE        *pParent;           // All nodes have parent pointers

    // Accessor functions.
    OPERATOR    Op() { return (OPERATOR)other; }
    PREDEFTYPE  PredefType() { return (PREDEFTYPE)other; }
    BASENODE    *GetParent();

    __int64 GetNid() {
        ASSERT(fHasNid);
        return ((__int64 *)this)[-1];
    }

    static  DWORD   m_rgNodeGroups[];   // Table of node group bits for each kind

    // Define all the concrete casting methods.
    #define NODEKIND(k1,k2,g,p) struct k2 ## NODE * as ## k1 ();
    #include "nodekind.h"

    // Little noop to enable looping:
    BASENODE        *asBASE() { return this; };


    // Some more complex casting helpers
    AGGREGATENODE   *asAGGREGATE ();
    METHODBASENODE  *asANYMETHOD ();
    PROPERTYNODE    *asANYPROPERTY ();
    FIELDNODE       *asANYFIELD ();
    MEMBERNODE      *asANYMEMBER ();
    NAMENODE        *asANYNAME ();
    NAMENODE        *asSingleName ();
    BINOPNODE       *asANYBINOP ();
    BINOPNODE       *asANYBINARYOPERATOR ();
    UNOPNODE        *asANYUNOP ();
    CALLNODE        *asANYCALL ();
    STATEMENTNODE   *asANYSTATEMENT ();
    TYPEBASENODE    *asANYTYPE ();
    EXPRSTMTNODE    *asANYEXPRSTMT ();
    LABELSTMTNODE   *asANYLABELSTMT ();
    LOOPSTMTNODE    *asANYLOOPSTMT ();
    ATTRNODE        *asANYATTR ();
    TYPEBASENODE    *asTYPEBASE () { return asANYTYPE(); }
    
    // Handy functions
    inline BOOL    InGroup (DWORD dwGroups) {
        dwGroups &= m_rgNodeGroups[kind];
        return dwGroups && ((dwGroups & ~(NG_BREAKABLE | NG_CONTINUABLE)) || !(flags & (NF_USING_DECL | NF_FIXED_DECL)) || kind != NK_FOR);
    }
    NAMENODE *LastNameOfDottedName();
    long    GetGlyph ();
    long    GetOperatorToken (long iOp);
    NAME *  GetContainingFileName();
    NAMESPACENODE *GetRootNamespace();

    //if you change this, make sure to update CNameBinder in the Language Service
    BOOL IsAnyName()
    {
        return kind == NK_NAME || kind == NK_GENERICNAME || kind == NK_ALIASNAME || kind == NK_OPENNAME;
    }

    BOOL    IsSingleName() { return kind == NK_NAME || kind == NK_ALIASNAME; }
    BOOL    IsMissingName() { return IsSingleName() && ((flags & NF_NAME_MISSING) != 0); }
    BOOL    IsAnyBinOp() { return InGroup (NG_BINOP); }
    BOOL    IsAnyBinaryOperator() { return kind == NK_BINOP || kind == NK_CALL || kind == NK_DEREF; }
    BOOL    IsAnyUnOp() { return kind == NK_ARRAYINIT || kind == NK_CASELABEL || kind == NK_UNOP; }
    BOOL    IsAnyCall() { return kind == NK_CALL || kind == NK_DEREF; }

    //if you change this, make sure to update CTypeBinder in the Language Service
    BOOL    IsAnyType()
    {
        switch (kind) {
        case NK_PREDEFINEDTYPE:
        case NK_NAMEDTYPE:
        case NK_ARRAYTYPE:
        case NK_POINTERTYPE:
        case NK_OPENTYPE:
        case NK_TYPEWITHATTR:
        case NK_NULLABLETYPE:
            return true;
        default:
            return false;
        }
    }
    BOOL    IsAnyExprStmt() { return kind == NK_BREAK || kind == NK_CONTINUE || kind == NK_EXPRSTMT || kind == NK_GOTO || 
                                kind == NK_RETURN || kind == NK_YIELD || kind == NK_THROW; }
    BOOL    IsAnyLabelStmt() { return kind == NK_CHECKED || kind == NK_LABEL || kind == NK_UNSAFE; }
    BOOL    IsAnyLoopStmt() { return kind == NK_LOCK || kind == NK_WHILE || kind == NK_DO; }
    BOOL    IsAnyAttr() { return kind == NK_ATTR || kind == NK_ATTRARG; }
    BOOL    IsAnyMember() { return InGroup(NG_MEMBER); }
    BOOL    IsAnyAggregate() { return InGroup(NG_AGGREGATE); }
    BOOL    IsDblColon();
    
    // Another joy of anonymous methods: 
    //  A STATEMENT node (i.e. a NK_BLOCK) in this case is not always a statement
    //  anymore. We need to check the tree context.
    BOOL    IsStatement() 
    { 
        return 
            InGroup (NG_STATEMENT) && 
            (pParent == NULL || 
                (pParent->kind != NK_ANONBLOCK && 
                 pParent->kind != NK_TRY && 
                 pParent->kind != NK_UNSAFE && 
                 pParent->kind != NK_CHECKED && 
                 pParent->kind != NK_CATCH && 
                 !pParent->InGroup(NG_METHOD)));
    }

    //////////////////////////////////////////////
    // CParseTreeMatch::IsStatementOrBlock
    // 
    // BASENODE::IsStatment() doesn't return true on a block in certain cases
    // (like the block of a method)

    bool IsStatementOrBlock()
    {
        return this->kind == NK_BLOCK || this->IsStatement();
    }

    long GetFirstNonAttributeToken();
    ATTRLOC GetDefaultAttrLocation();

    // Name and key construction methods (NEW, BETTER, FASTER)
    HRESULT AppendNameText (CStringBuilder &sb, ICSNameTable *pNameTable);              // Names that are keywords are @-escaped unless pNameTable is NULL
    HRESULT AppendNameTextToPrototype (CStringBuilder &sb, ICSNameTable *pNameTable);              // Names that are keywords are @-escaped unless pNameTable is NULL
    HRESULT AppendNameTextToKey (CStringBuilder &sb, ICSNameTable *pNameTable);              // Names that are keywords are @-escaped unless pNameTable is NULL
    HRESULT AppendTypeText (CStringBuilder &sb, ICSNameTable *pNameTable);              // Ditto.
    HRESULT AppendPrototype (ICSNameTable *pNameTable, PROTOFLAGS ptFlags, CStringBuilder &sb, bool fEscapedName = false);
    void    AppendTypeParametersToKey(BASENODE* pParms, CStringBuilder& sb);
    void    AppendParametersToKey (PARAMTYPES pt, BASENODE *pParms, CStringBuilder &sb);
    void    AppendTypeToKey (TYPEBASENODE *pType, CStringBuilder &sb);
    void    AppendParametersToPrototype (PARAMTYPES pt, BASENODE *pParms, CStringBuilder &sb, bool fParamName);
    void    AppendTypeToPrototype (TYPEBASENODE *pType, CStringBuilder &sb);
    HRESULT BuildKey (ICSNameTable *pNameTable, BOOL fIncludeParent, bool fIncludeGenerics, NAME **ppKey);

    //**** ONLY ACCESSIBLE FROM THE LANGUAGE SERVICE ****
    HRESULT AppendFullName (CComBSTR &sbstr);
    HRESULT BuildFullName (NAME **ppName);
};


typedef BASENODE * PBASENODE;

////////////////////////////////////////////////////////////////////////////////
// Use these macros to define nodes.  Node kind names are derived from the name
// provided, as are the node structures themselves, i.e.,
//
//  DECLARE_NODE(FOO, EXPR)
//  ...
//  END_NODE()
//
// creates a FOONODE derived from EXPRNODE.

#define DECLARE_NODE(nodename, nodebase) struct nodename##NODE : public nodebase##NODE {
#define END_NODE() };

#define CHILD_NODE(type, name) type ## NODE * name;
#define CHILD_OPTIONAL_NODE(type, name) type ## NODE * name;
#define FIRST_NODE(type, name) type ## NODE * name;
#define PARENT_NODE(type, name) type ## NODE * name;
#define NEXT_NODE(type, name) type ## NODE * name;
#define CHILD_NAME(name) NAME * name;
#define INTERIOR_NODE(name) CInteriorNode * name;
#define NODE_INDEX(name) long name;
#define CHILD_MEMBER(type, name) type name;

#include "allnodes.h"


// Define all the concrete cast kinds here.
#define NODEKIND(k1,k2,g,p) \
    __forceinline k2 ## NODE * BASENODE::as ## k1 () {   \
        RETAILVERIFY(this == NULL || this->kind == NK_ ## k1);  \
        return static_cast<k2 ## NODE *>(this);     \
    }
#include "nodekind.h"

__forceinline AGGREGATENODE * BASENODE::asAGGREGATE ()
{
    RETAILVERIFY(this == NULL || InGroup(NG_AGGREGATE));
    return static_cast<AGGREGATENODE *> (this);
}

__forceinline METHODBASENODE * BASENODE::asANYMETHOD ()
{
    RETAILVERIFY(this == NULL || InGroup(NG_METHOD));
    return static_cast<METHODBASENODE *> (this);
}

__forceinline PROPERTYNODE * BASENODE::asANYPROPERTY ()
{
    RETAILVERIFY(this == NULL || InGroup(NG_PROPERTY));
    return static_cast<PROPERTYNODE *> (this);
}

__forceinline FIELDNODE * BASENODE::asANYFIELD ()
{
    RETAILVERIFY(this == NULL || InGroup(NG_FIELD));
    return static_cast<FIELDNODE *> (this);
}

__forceinline MEMBERNODE * BASENODE::asANYMEMBER ()
{
    RETAILVERIFY(this == NULL || InGroup(NG_MEMBER));
    return static_cast<MEMBERNODE *> (this);
}

__forceinline NAMENODE * BASENODE::asANYNAME ()
{
    RETAILVERIFY(this == NULL || this->IsAnyName());
    return static_cast<NAMENODE *> (this);
}

__forceinline NAMENODE * BASENODE::asSingleName ()
{
    RETAILVERIFY(this == NULL || this->IsSingleName());
    return static_cast<NAMENODE *> (this);
}

__forceinline BINOPNODE * BASENODE::asANYBINOP ()
{
    RETAILVERIFY(this == NULL || IsAnyBinOp());
    return static_cast<BINOPNODE *> (this);
}

__forceinline BINOPNODE * BASENODE::asANYBINARYOPERATOR ()
{
    RETAILVERIFY(this == NULL || IsAnyBinaryOperator());
    return static_cast<BINOPNODE *> (this);
}

__forceinline UNOPNODE * BASENODE::asANYUNOP ()
{
    RETAILVERIFY(this == NULL || IsAnyUnOp());
    return static_cast<UNOPNODE *> (this);
}

__forceinline CALLNODE * BASENODE::asANYCALL ()
{
    RETAILVERIFY(this == NULL || this->kind == NK_CALL || this->kind == NK_DEREF);
    return static_cast<CALLNODE *> (this);
}

__forceinline STATEMENTNODE * BASENODE::asANYSTATEMENT ()
{
    RETAILVERIFY(this == NULL || InGroup(NG_STATEMENT));
    return static_cast<STATEMENTNODE *> (this);
}

__forceinline EXPRSTMTNODE * BASENODE::asANYEXPRSTMT ()
{
    RETAILVERIFY(this == NULL || IsAnyExprStmt());
    return static_cast<EXPRSTMTNODE *> (this);
}

__forceinline LABELSTMTNODE * BASENODE::asANYLABELSTMT ()
{
    RETAILVERIFY(this == NULL || IsAnyLabelStmt());
    return static_cast<LABELSTMTNODE *> (this);
}

__forceinline LOOPSTMTNODE * BASENODE::asANYLOOPSTMT ()
{
    RETAILVERIFY(this == NULL || IsAnyLoopStmt());
    return static_cast<LOOPSTMTNODE *> (this);
}

__forceinline ATTRNODE * BASENODE::asANYATTR ()
{
    RETAILVERIFY(this == NULL || IsAnyAttr());
    return static_cast<ATTRNODE *> (this);
}

__forceinline TYPEBASENODE * BASENODE::asANYTYPE ()
{
    RETAILVERIFY(this == NULL || this->IsAnyType());
    return static_cast<TYPEBASENODE *> (this);
}

__forceinline NAMENODE *BASENODE::LastNameOfDottedName()
{
    if (this->IsAnyName())
        return this->asANYNAME();
    ASSERT(this->kind == NK_DOT);
    return this->asDOT()->p2->asANYNAME();
}

__forceinline BASENODE *BASENODE::GetParent () 
{
    if (kind == NK_VARDECL) 
        return this->asVARDECL()->pDecl; 
    return this->pParent; 
}


__forceinline  BOOL BASENODE::IsDblColon()
{
    return kind == NK_DOT && asDOT()->p1->kind == NK_ALIASNAME;
}



class CListIterator
{
private:
    BASENODE    *m_pCurNode;

public:
    CListIterator() : m_pCurNode(NULL) {}
    CListIterator(BASENODE* pStartNode) 
    {
        this->Start(pStartNode);
    }

    void        Start (BASENODE *pNode) { m_pCurNode = pNode; }
    BASENODE    *Next ()
    {
        if (m_pCurNode == NULL)
            return NULL;
        if (m_pCurNode->kind == NK_LIST)
        {
            BASENODE    *pRet = m_pCurNode->asLIST()->p1;
            m_pCurNode = m_pCurNode->asLIST()->p2;
            return pRet;
        }
        else
        {
            BASENODE    *pRet = m_pCurNode;
            m_pCurNode = NULL;
            return pRet;
        }
    }

    static BASENODE *LastNode(BASENODE *pNode)
    {
        CListIterator it;
        it.Start(pNode);

        BASENODE *pPrev = pNode;
        for (BASENODE *pCur = it.Next(); pCur != NULL; pCur = it.Next())
        {
            pPrev = pCur;
        }

        return pPrev;
    }

    static long GetCount(BASENODE *pNode)
    {
        CListIterator it;
        it.Start(pNode);

        long iCount = 0;
        for (BASENODE *pCur = it.Next(); pCur != NULL; pCur = it.Next())
        {
            iCount++;
        }

        return iCount;
    }

    //Get nth node in a list
    static HRESULT GetNode(BASENODE* pStartNode, long index, BASENODE** pNode)
    {
        CListIterator it(pStartNode);
        BASENODE *pParm = NULL;

        // Run to the 'index'th node
        // Expecting a zero based index here
        for(int i = 0; i <= index; ++i)
        {
            pParm = it.Next();
        }

        // Search did not succeed: either invalid arguments or element not found
        if(pParm == NULL) {
           *pNode = NULL;
           return E_INVALIDARG;
        }
	  
        // Return the node if succeded	 
        *pNode = pParm;
	    return S_OK;
    }

    static HRESULT GetIndex(BASENODE* pStartNode, BASENODE* pNode, long* iIndex)
    {
        CListIterator it(pStartNode);
        long          iCount = 0;// returning zero based index
        
        // Compare each of the fields to the given node
        BASENODE *p;
        for (p = it.Next(); p != NULL && p != pNode; p = it.Next())
            iCount++;

        // Element not found or pNode was NULL
        if (p == NULL) {
            *iIndex = -1;
            return E_INVALIDARG;
        }

        *iIndex = iCount;

        return S_OK;
    }
};

////////////////////////////////////////////////////////////////////////////////

class CChainIterator
{
private:
    bool m_fStatementChain;
    BASENODE* m_pChainStart;
    BASENODE* m_pCurrent;

    bool m_fStarted;

public:
    CChainIterator(BASENODE* pChainStart) :
      m_pChainStart(pChainStart)
    {
        this->Reset();
    }

    BASENODE* Current() {
        ASSERT(m_pCurrent != NULL);
        ASSERT(m_fStarted);
        return m_pCurrent;
    }

    bool MoveNext() {
        if (!m_fStarted) {
            m_fStarted = true;
            m_pCurrent = m_pChainStart;
        } else if (m_fStatementChain) {
            m_pCurrent = ((STATEMENTNODE *)m_pCurrent)->pNext;
        } else {
            m_pCurrent = ((MEMBERNODE*)m_pCurrent)->pNext;
        }

        return m_pCurrent != NULL;
    }

    void Reset() {
        ASSERT(m_pChainStart == NULL || m_pChainStart->IsStatement() || m_pChainStart->InGroup(NG_MEMBER));

        m_pCurrent = NULL;
        m_fStarted = false;
        m_fStatementChain = (m_pChainStart != NULL) && m_pChainStart->IsStatement();
    }
};


struct NameNodeResult
{
    NAMENODE *  pNameNode;
    bool        fIsAlias;   // If pNameNode is an alias (lhs of "::")

    NameNodeResult()
    { 
        Reset(); 
    }
    void Reset() 
    { 
        pNameNode = NULL; 
        fIsAlias = false; 
    }
    void SetNode(NAMENODE *pNode)
    {
        pNameNode = pNode;
        fIsAlias = pNode != NULL && pNode->kind == NK_ALIASNAME;
    }
};

class CDottedNameIterator
{
private:
    BASENODE    *   m_pRootNode;        // Root of dotted name tree
    NameNodeResult  m_Result;           // Always the next name node to return
    long            m_iCount;

private:
    BASENODE *left(BASENODE *pNode)
    {
        ASSERT(pNode != NULL);
        ASSERT(pNode->kind == NK_DOT);

        switch(pNode->kind)
        {
        case NK_DOT:
            return pNode->asDOT()->p1;
        default:
            VSFAIL("Wrong kind of node");
            return NULL;
        }
    }
    BASENODE *right(BASENODE *pNode)
    {
        ASSERT(pNode != NULL);
        ASSERT(pNode->kind == NK_DOT);

        switch(pNode->kind)
        {
        case NK_DOT:
            return pNode->asDOT()->p2;
        default:
            VSFAIL("Wrong kind of node");
            return NULL;
        }
    }
public:
    CDottedNameIterator() {
        this->Start(NULL);
    }

    CDottedNameIterator(BASENODE* pNode) {
        this->Start(pNode);
    }

    void Start (BASENODE *pNode)
    {
        m_iCount = 0;
        m_pRootNode = pNode;

        if (pNode != NULL)
        {
            m_iCount = 1;
            while (pNode->kind == NK_DOT)
            {
                m_iCount++;
                pNode = left(pNode);
            }

            m_Result.SetNode(pNode->asANYNAME());
        }
    }

    NameNodeResult Next ()
    {
        NameNodeResult res;

        if (m_Result.pNameNode == NULL)
            return res;

        res = m_Result;
        if (res.pNameNode == m_pRootNode)
        {
            // This is the single-name given case
            m_Result.Reset();
            return res;
        }

        BASENODE *pParent = m_Result.pNameNode->pParent;
        if (pParent->kind == NK_DOT)
        {
            if (left(pParent) == m_Result.pNameNode)
                m_Result.SetNode(right(pParent)->asANYNAME());               // Switch from left-side to right-side
            else if (pParent == m_pRootNode)
                m_Result.SetNode(NULL);                                      // Last name in chain
            else
                m_Result.SetNode(right(pParent->pParent)->asANYNAME());      // Next name is right side of grandparent
        }

        return res;
    }

    long GetCount() { return m_iCount; }


};


inline int CountAnyListNode(BASENODE *pNode, NODEKIND listKind)
{
    int rval = 0;
    while (pNode && pNode->kind == listKind)
    {
        pNode = pNode->asANYBINOP()->p2;
        rval += 1;
    }
    
    if (pNode!= NULL)
        rval += 1;
        
    return rval;
}

inline int CountListNode(BASENODE *pNode)
{
    return CountAnyListNode(pNode, NK_LIST);
}

/////////////////////////////////////////////////////////////////////////////////
// Node Iterator Logic

enum NODE_FIELD_KIND
{
    NFK_NONE,
    NFK_NODE,               // a child node
    NFK_OPTIONAL_NODE,      // an optional node
    NFK_FIRST_NODE,         // a first child node
    NFK_PARENT_NODE,        // a node which is actually a parent node
    NFK_NEXT_NODE,          // a node which is a link in a list
    NFK_NAME,               // a NAME*
    NFK_INTERIOR,           // an interiorNode
    NFK_INDEX,              // a node index
    NFK_OTHER,              // anything else
};

struct NODE_FIELD_INFO
{
    int offset;
    NODE_FIELD_KIND kind;
};

#define MAX_NODE_INFO_FIELDS 12

struct NODE_INFO
{
    const NODE_INFO * baseInfo;
    NODE_FIELD_INFO fields[MAX_NODE_INFO_FIELDS];
};

const NODE_INFO * NodeKindToInfo(NODEKIND kind);

// abstract base class for iterating over the members
// of a BASENODE. Derived classes must specify
// FIELD_TYPE - the type of fields tehy want to iterate over
// and override FieldKindMatch() to filter which kinds
// of fields they want to return
template <class FIELD_TYPE>
class NodeIterator
{
public:
    // advances the iterator and returns true
    // if the Current can be called
    bool MoveNext();
    // the current element of the iterator
    // can only be called after MoveNext() has been called
    // and returned true
    FIELD_TYPE &Current();
    
    // Returns wether its ok to call MoveNext() again
    // Note that when initializing the iterator
    // it is ok to call MoveNext() but not ok to call Current()
    bool Done();

protected:
    NodeIterator(BASENODE *parentNode);
    
    // returns true if the field kind matches
    // the type of field this iterator should return
    virtual bool FieldKindMatch(NODE_FIELD_KIND fk) = 0;

private:
    void NextNode();
    void NextField();
    
    int offset;
    const NODE_FIELD_INFO * currentField;
    const NODE_INFO * currentNode;
    BASENODE *parentNode;
};

// Note that it is OK for parentNode to be NULL
// which results in an empty iteration
template <class FIELD_TYPE>
NodeIterator<FIELD_TYPE>::NodeIterator(BASENODE *parentNode) :
    currentField(0),
    currentNode(0),
    parentNode(parentNode)
{
    if (parentNode)
        currentNode = NodeKindToInfo(parentNode->kind);
}

// Advances the iterator, and returns true if there is another
// element available. Once false is returned it is illegal to
// call MoveNext() or Current() again
template <class FIELD_TYPE>
bool NodeIterator<FIELD_TYPE>::MoveNext()
{
    ASSERT(currentNode && (!currentField || currentField >= currentNode->fields));
    
    do
    {
        NextField();
    } while (!Done() && !FieldKindMatch(currentField->kind));
    
    return !Done();
}

// Returns wether its ok to call MoveNext() again
// Note that when initializing the iterator
// it is ok to call MoveNext() but not ok to call Current()
template <class FIELD_TYPE>
bool NodeIterator<FIELD_TYPE>::Done()
{
    return !currentNode;
}

// Advances to the NODE_INFO for the base type
// Sets the state to Done() when no bases are left
template <class FIELD_TYPE>
void NodeIterator<FIELD_TYPE>::NextNode()
{
    ASSERT(!Done() && currentField >= currentNode->fields);
    ASSERT(currentField->kind == NFK_NONE && currentField->offset == 0);

    currentNode = currentNode->baseInfo;
    if (currentNode)
        currentField = currentNode->fields;
    else
        currentField = NULL;
}

// Advances to the next NODE_FIELD_KIND
template <class FIELD_TYPE>
void NodeIterator<FIELD_TYPE>::NextField()
{
    ASSERT(!Done());

    if (!currentField)
    {
        // initialize the iterator
        currentField = currentNode->fields;
    }
    else if (currentField->kind != NFK_NONE)
    {
        ASSERT(currentField->offset != 0);
        currentField ++;
    }
    else
    {
        ASSERT(currentField->offset == 0);
        NextNode();
    }
}

// Returns the current element in the iteration
// Can only call after MoveNext() has been called and returns true
template <class FIELD_TYPE>
FIELD_TYPE &NodeIterator<FIELD_TYPE>::Current()
{
    ASSERT(!Done() && currentField);
    ASSERT(currentField->offset != 0);
    
    return *(FIELD_TYPE*) (((BYTE*) parentNode) + currentField->offset);
}

// Iterates over the child nodes of a given BASENODE. Note that this doesn't
// process FIRST_NODEs or NEXT_NODEs.
class NonOptionalChildNodeIterator : public NodeIterator<BASENODE*>
{
public:
    NonOptionalChildNodeIterator(BASENODE *baseNode) :
        NodeIterator<BASENODE*> (baseNode)
    {}

protected:
    bool FieldKindMatch(NODE_FIELD_KIND fk)
    {
        return fk == NFK_NODE;
    }
};

// Iterates over all optional child nodes of a given BASENODE. Note that this doesn't
// process FIRST_NODEs or NEXT_NODEs.
class AllChildNodeIterator : public NodeIterator<BASENODE*>
{
public:
    AllChildNodeIterator(BASENODE *baseNode) :
        NodeIterator<BASENODE*> (baseNode)
    {}

protected:
    bool FieldKindMatch(NODE_FIELD_KIND fk)
    {
        return fk == NFK_NODE || fk == NFK_OPTIONAL_NODE;
    }
};

// Iterates over the node indexes of a given BASENODE
class IndexNodeIterator : public NodeIterator<long>
{
public:
    IndexNodeIterator(BASENODE *baseNode) :
        NodeIterator<long> (baseNode)
    {}
    
protected:
    bool FieldKindMatch(NODE_FIELD_KIND fk)
    {
        return fk == NFK_INDEX;
    }
};



#endif  // _NODES_H_
