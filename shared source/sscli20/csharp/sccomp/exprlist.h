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
// File: exprlist.h
//
// Defines the nodes of the bound expression tree
// ===========================================================================

/*
 * Define the different expr node kinds
 */
#ifdef _MSC_VER
#pragma once
#endif

#ifndef __EXPRLIST_H__
#define __EXPRLIST_H__

BEGIN_NAMESPACE_CSHARP
class IVisualizerData;
END_NAMESPACE_CSHARP

enum EXPRKIND {
    #define EXPRDEF(kind) EK_ ## kind,
    #define EXPRKINDDEF(kind) EK_ ## kind,
    #define EXPRKIND_EXTRA(expr) expr,

    #include "exprkind.h"

    EK_MULTIOFFSET, // This has to be last!!! To deal /w multiops we add this to the op to obtain the ek in the op table

    // Statements are all before expressions and the first expression is EK_BINOP
    EK_ExprMin = EK_BINOP,
    EK_StmtLim = EK_ExprMin,
};


enum EXPRFLAG {
    // These are specific to various node types.

    // Order these by value. If you need a new flag, search for the first value that isn't currently valid on your expr kind.

    // 0x1
    EXF_BINOP           = 0x1,  // On ** Many Non Statement Exprs! ** This gets its own BIT!

    // 0x2
    EXF_CTOR            = 0x2,  // Only on EXPRMEMGRP, indicates a constructor.
    EXF_NEEDSRET        = 0x2,  // Only on EXPRBLOCK
    EXF_ASLEAVE         = 0x2,  // Only on EXPRGOTO, EXPRRETURN, means use leave instead of br instruction
    EXF_ISFAULT         = 0x2,  // Only on EXPRTRY, used for fabricated try/fault (no user code)
    EXF_HASHTABLESWITCH = 0x2,  // Only on EXPRSWITCH
    EXF_BOX             = 0x2,  // Only on EXPRCAST, indicates a boxing operation (value type -> object)
    EXF_ARRAYCONST      = 0x2,  // Only on EXPRARRINIT, indicates that we should init using a memory block
    EXF_MEMBERSET       = 0x2,  // Only on EXPRFIELD, indicates that the reference is for set purposes
    EXF_OPENTYPE        = 0x2,  // Only on EXPRTYPEOF. Indicates that the type is an open type.
    EXF_LABELREFERENCED = 0x2,  // Only on EXPRLABEL. Indicates the label was targeted by a goto.

    // 0x4
    EXF_INDEXER         = 0x4,  // Only on EXPRMEMGRP, indicates an indexer.
    EXF_GOTOCASE        = 0x4,  // Only on EXPRGOTO, means goto case or goto default
    EXF_HASDEFAULT      = 0x4,  // Only on EXPRSWITCH
    EXF_REMOVEFINALLY   = 0x4,  // Only on EXPRTRY, means that the try-finally should be converted to normal code
    EXF_UNBOX           = 0x4,  // Only on EXPRCAST, indicates a unboxing operation (object -> value type)
    EXF_ARRAYALLCONST   = 0x4,  // Only on EXPRARRINIT, indicated that all elems are constant (must also have ARRAYCONST set)
    EXF_CTORPREAMBLE    = 0x4,  // Only on EXPRBLOCK, indicates that the block is the preamble of a constructor - contains field inits and base ctor call

    // 0x8
    EXF_OPERATOR        = 0x8,  // Only on EXPRMEMGRP, indicates an operator.
    EXF_ISPOSTOP        = 0x8,  // Only on EXPRMULTI, indicates <x>++
    EXF_UNREALIZEDCONCAT = 0x8, // Only on EXPRCONCAT, means that we need to realize the list to a constructor call...
    EXF_FINALLYBLOCKED  = 0x8,  // Only on EXPRTRY, EXPRGOTO, EXPRRETURN, means that FINALLY block end is unreachable
    EXF_REFCHECK        = 0x8,  // Only on EXPRCAST, indicates an reference checked cast is required
    EXF_WRAPASTEMP      = 0x8,  // Only on EXPRWRAP, indicates that this wrap represents an actual local

    // 0x10
    EXF_LITERALCONST    = 0x10, // Only on EXPRCONSTANT, means this was not a folded constant
    EXF_BADGOTO         = 0x10, // Only on EXPRGOTO, indicates an unrealizable goto
    EXF_RETURNISYIELD   = 0x10, // Only on EXPRRETURN, means this is really a yield, and flow continues
    EXF_ISFINALLY       = 0x10, // Only on EXPRTRY
    EXF_NEWOBJCALL      = 0x10, // Only on EXPRCALL and EXPRMEMGRP, to indicate new <...>(...)
    EXF_INDEXEXPR       = 0x10, // Only on EXPRCAST, indicates a special cast for array indexing
    EXF_REPLACEWRAP     = 0x10, // Only on EXPRWRAP, it means the wrap should be replaced with its expr (during rewriting)

    // 0x20
    EXF_UNREALIZEDGOTO  = 0x20, // Only on EXPRGOTO, means target unknown
    EXF_CONSTRAINED     = 0x20, // Only on EXPRCALL, EXPRPROP, indicates a call through a method or prop on a type variable or value type
    EXF_FORCE_BOX       = 0x20, // Only on EXPRCAST, GENERICS: indicates a "forcing" boxing operation (if type parameter boxed then nop, i.e. object -> object, else value type -> object)

    // 0x40
    EXF_ASFINALLYLEAVE  = 0x40, // Only on EXPRGOTO, EXPRRETURN, means leave through a finally, ASLEAVE must also be set
    EXF_BASECALL        = 0x40, // Only on EXPRCALL, EXPRFNCPTR, EXPRPROP, EXPREVENT, and EXPRMEMGRP, indicates a "base.XXXX" call
    EXF_FORCE_UNBOX     = 0x40, // Only on EXPRCAST, GENERICS: indicates a "forcing" unboxing operation (if type parameter boxed then castclass, i.e. object -> object, else object -> value type)
    EXF_ADDRNOCONV      = 0x40, // Only on EXPRBINOP, with kind == EK_ADDR, indicates that a conv.u should NOT be emitted.

    // 0x80
    EXF_GOTONOTBLOCKED  = 0x80, // Only on EXPRGOTO, means the goto is known to not pass through a finally which does not terminate
    EXF_DELEGATE        = 0x80, // Only on EXPRMEMGRP, indicates an implicit invocation of a delegate: d() vs d.Invoke().
    EXF_HASREFPARAM     = 0x80, // Only on EXPRCALL, indicates that some of the params are out or ref
    EXF_STATIC_CAST     = 0x80, // Only on EXPRCAST, indicates a static cast is required. We implement with stloc, ldloc to a temp of the correct type.

    // 0x100
    EXF_USERCALLABLE    = 0x100, // Only on EXPRMEMGRP, indicates a user callable member group.

    // 0x200
    EXF_NEWSTRUCTASSG   = 0x200, // Only on EXPRCALL, indicates that this is a constructor call which assigns to object
    EXF_GENERATEDSTMT   = 0x200, // Only on statement exprs. Indicates that the statement is compiler generated
                                 // so we shouldn't report things like "unreachable code" on it.

    // 0x400
    EXF_IMPLICITSTRUCTASSG = 0x400, // Only on EXPRCALL, indicates that this an implicit struct assg call
    EXF_MARKING         = 0x400, // Only on statement exprs. Indicates that we're currently marking
                                 // its children for reachability (it's up the stack).

    // *** The following are usable on multiple node types. ***
    // 0x000800 and above

    EXF_UNREACHABLEBEGIN = 0x000800, // indicates an unreachable statement
    EXF_UNREACHABLEEND   = 0x001000, // indicates the end of the statement is unreachable
    EXF_USEORIGDEBUGINFO = 0x002000, // Only set on EXPRDEBUGNOOP, but tested generally. Indicates foreach node should not be overridden to in token
    EXF_LASTBRACEDEBUGINFO = 0x004000, // indicates override tree to set debuginfo on last brace
    EXF_NODEBUGINFO     = 0x008000, // indicates no debug info for this statement
    EXF_IMPLICITTHIS    = 0x010000, // indicates a compiler provided this pointer (in the EE, when doing autoexp, this can be anything)
    EXF_CANTBENULL      = 0x020000, // indicate this expression can't ever be null (e.g., "this").
    EXF_CHECKOVERFLOW   = 0x040000, // indicates that operation should be checked for overflow
    EXF_PUSH_OP_FIRST   = 0x100000, // On any expr, indicates that the first operand must be placed on the stack before
                                    // anything else - this is needed for multi-ops involving string concat.
    EXF_ASSGOP          = 0x200000, // On any non stmt exprs, indicates assignment node...
    EXF_LVALUE          = 0x400000, // On any exprs. An lvalue - whether it's legal to assign.

    // THIS IS THE HIGHEST FLAG:
    // Indicates that the expression came from a LOCVARSYM, MEMBVARSYM, or PROPSYM whose type has the same name so
    // it's OK to use the type instead of the element if using the element would generate an error.
    EXF_SAMENAMETYPE    = 0x800000,

    EXF_MASK_ANY = EXF_UNREACHABLEBEGIN | EXF_UNREACHABLEEND |
        EXF_USEORIGDEBUGINFO | EXF_LASTBRACEDEBUGINFO | EXF_NODEBUGINFO |
        EXF_IMPLICITTHIS | EXF_CANTBENULL | EXF_CHECKOVERFLOW |
        EXF_PUSH_OP_FIRST | EXF_ASSGOP | EXF_LVALUE | EXF_SAMENAMETYPE
        ,

    // Used to mask the cast flags off an EXPRCAST.
    EXF_CAST_ALL = EXF_BOX | EXF_UNBOX | EXF_REFCHECK | EXF_INDEXEXPR | EXF_FORCE_BOX | EXF_FORCE_UNBOX | EXF_STATIC_CAST
        ,

};

// Typedefs for some pointer types up front.
typedef class EXPR * PEXPR;
typedef class EXPRLIST * PEXPRLIST;


/* 
 * EXPR - the base expression node. 
 */
class EXPR {
public:
    EXPR() : type(NULL) {}
    // struct {
    EXPRKIND kind: 8;     // the exprnode kind
    int flags : 24;
    // };
    
    // We have more bits available here!
    BASENODE * tree;
    TYPESYM  * const type;
    void setType(TYPESYM * value) {
        ((TYPESYM*&)type) = value;
        UpdateType();
    };

private:
    void UpdateType() {};
public:

    // We define the casts by hand so that statement completion will be aware of them...
    class EXPRBLOCK * asBLOCK();
    class EXPRBINOP * asBINOP();
    class EXPRSTMTAS * asSTMTAS();
    class EXPRCALL * asCALL();
    class EXPRCAST * asCAST();
    class EXPRDELIM * asDELIM();
    class EXPREVENT * asEVENT();
    class EXPRFIELD * asFIELD();
    class EXPRLOCAL * asLOCAL();
    class EXPRRETURN * asRETURN();
    class EXPRCONSTANT * asCONSTANT();
    class EXPRCLASS * asCLASS();
    class EXPRTYPE * asTYPE();
    class EXPRNSPACE * asNSPACE();
    class EXPRERROR * asERROR();
    class EXPRDECL * asDECL();
    class EXPRLABEL * asLABEL();
    class EXPRGOTO * asGOTO();
    class EXPRGOTOIF * asGOTOIF();
    class EXPRFUNCPTR * asFUNCPTR();
    class EXPRSWITCH * asSWITCH();
    class EXPRSWITCHLABEL * asSWITCHLABEL();
    class EXPRPROP * asPROP();
    class EXPRHANDLER * asHANDLER();
    class EXPRTRY * asTRY();
    class EXPRTHROW * asTHROW();
    class EXPRMULTI * asMULTI();
    class EXPRMULTIGET * asMULTIGET();
    class EXPRSTTMP * asSTTMP();
    class EXPRLDTMP * asLDTMP();
    class EXPRFREETMP * asFREETMP();
    class EXPRWRAP * asWRAP();
    class EXPRCONCAT * asCONCAT();
    class EXPRARRINIT * asARRINIT();
    class EXPRTYPEOF * asTYPEOF();
    class EXPRSIZEOF * asSIZEOF();
    class EXPRZEROINIT * asZEROINIT();
    class EXPRNOOP * asNOOP();
    class EXPRDEBUGNOOP * asDEBUGNOOP();
    class EXPRUSERLOGOP * asUSERLOGOP();
    class EXPRMEMGRP * asMEMGRP();
    class EXPRANONMETH * asANONMETH();
    class EXPRDBLQMARK * asDBLQMARK();

    EXPR * asEXPR() { return this; };
    class EXPRSTMT * asSTMT() {
        ASSERT(!this || this->kind < EK_StmtLim);
        return (EXPRSTMT *)this;
    }

    EXPRBINOP * asBIN() { ASSERT(!this || (this->flags & EXF_BINOP)); return (EXPRBINOP*)this;};
    EXPRGOTO * asGT() { ASSERT(!this || this->kind == EK_GOTO || this->kind == EK_GOTOIF); return (EXPRGOTO*) this; };
    EXPRLABEL * asANYLABEL() { ASSERT(!this || this->kind == EK_LABEL || this->kind == EK_SWITCHLABEL); return (EXPRLABEL*)this; };

    // Allocate from a no-release allocator.
#ifdef _MSC_VER
#pragma push_macro("new")
#undef new
#endif // _MSC_VER
    void * operator new(size_t sz, NRHEAP * allocator)
#ifdef _MSC_VER
#pragma pop_macro("new")
#endif // _MSC_VER
    {
        return allocator->AllocZero(sz);
    };

    int isTrue(bool sense = true);
    int isFalse(bool sense = true);
    int isTrueResult(bool sense = true); // Checks for EK_SEQUENCE as well
    int isFalseResult(bool sense = true); // Checks for EK_SEQUENCE as well
    int isConstantResult(); // Checks for EK_SEQUENCE as well
    enum CONSTRESKIND {
        ConstTrue,
        ConstFalse,
        ConstNotConst,
    };
    EXPR::CONSTRESKIND GetConstantResult(); // Checks for EK_SEQUENCE as well
    static int ConstantMatchesSense(EXPR::CONSTRESKIND crk, bool sense);
    bool isNull();
    bool isZero(bool fDefValue);

    bool isOK()
    {
        return kind != EK_ERROR;
    };

    bool isFixedAggField();

    int getOffset();
    bool hasSideEffects(COMPILER *compiler);

    EXPR * getArgs();
    EXPR ** getArgsPtr();
    EXPR * getObject();
    SymWithType & GetSymWithType();

    EXPR * GetSeqVal();
    EXPR * GetConst();

#if DEBUG
    virtual void zDummy() {};
#endif
};

typedef enum _DELIMKIND
{
    DELIM_START,
    DELIM_END
} DELIMKIND;

// Declare the EXPR node trees
#define DECLARE_EXPR( name, base)           class EXPR ## name: public base { public:
#define DECLARE_EXPR_NO_EK( name, base)     class EXPR ## name: public base { public:
#define CHILD_EXPR( name, expr_type)            expr_type * name;
#define CHILD_EXPR_NO_RECURSE( name, expr_type) expr_type * name;
#define NEXT_EXPR( name, expr_type)             expr_type * name;
#define CHILD_SYM( name, sym_type)              sym_type *  name;
#define CHILD_TYPESYM( name, sym_type)          sym_type *  name;
#define CHILD_TYPEARRAY( name)                  TypeArray * name;
#define CHILD_OTHER( name, type)                type        name;
#define CHILD_OTHER_SZ( name, type, size)       type        name : size;
#define CHILD_WITHINST( name, type)             type        name;
#define CHILD_WITHTYPE( name, type)             type        name;
#define END_EXPR( )                         };
#define INCLUDE_IMPL

#include "exprnodes.h"

#undef INCLUDE_IMPL

// Casts from the base type:
#define EXPRDEF(k) \
    __forceinline EXPR ## k * EXPR::as ## k () {   \
        RETAILVERIFY(this == NULL || this->kind == EK_ ## k);  \
        return static_cast< EXPR ## k *>(this);     \
    }
#include "exprkind.h"
#undef EXPRDEF

__forceinline int EXPR::isTrue(bool sense) 
{
    ASSERT((int)sense == 0 || (int)sense == 1);
    return this && kind == EK_CONSTANT && !(asCONSTANT()->getVal().iVal ^ (int)sense);
}

__forceinline int EXPR::isFalse(bool sense) 
{
    ASSERT((int)sense == 0 || (int)sense == 1);
    return this && kind == EK_CONSTANT && (asCONSTANT()->getVal().iVal ^ (int)sense);
}

__forceinline int EXPR::isTrueResult(bool sense)
{
    return isFalseResult(!sense);
}

__forceinline int EXPR::isConstantResult()
{
    return EXPR::ConstNotConst != GetConstantResult();
}

__forceinline int EXPR::ConstantMatchesSense(EXPR::CONSTRESKIND crk, bool sense)
{
    if (crk == EXPR::ConstNotConst) return false;
    return ((crk == EXPR::ConstTrue) == sense);
}

__forceinline EXPR::CONSTRESKIND EXPR::GetConstantResult()
{
    if (!this)
        return EXPR::ConstNotConst;

    for (EXPR * expr = this; ; ) {
        switch (expr->kind) {
        case EK_CONSTANT:
            return expr->asCONSTANT()->getVal().iVal ? EXPR::ConstTrue : EXPR::ConstFalse;
        case EK_SEQUENCE:
            expr = expr->asBIN()->p2;
            break;
        default:
            return EXPR::ConstNotConst;
        }
    }
}

__forceinline int EXPR::isFalseResult(bool sense)
{
    ASSERT((int)sense == 0 || (int)sense == 1);

    return ConstantMatchesSense(GetConstantResult(), !sense);
}

__forceinline bool EXPR::isNull()
{
    return kind == EK_CONSTANT && (type->fundType() == FT_REF) && !asCONSTANT()->getSVal().strVal;
}

__forceinline bool EXPR::isZero(bool fDefValue)
{
    if (kind == EK_CONSTANT) {
        switch( type->fundType() ) {
        case FT_I1: case FT_U1:
        case FT_I2: case FT_U2:
        case FT_I4: 
            return asCONSTANT()->getVal().iVal == 0;
        case FT_U4:
            return asCONSTANT()->getVal().uiVal == 0;
        case FT_I8: 
            return *asCONSTANT()->getVal().longVal == 0;
        case FT_U8:
            return *asCONSTANT()->getVal().ulongVal == 0;
        case FT_R4: case FT_R8:
            return *asCONSTANT()->getVal().doubleVal == 0.0;
        case FT_STRUCT: // Decimal
            {
                // if we are looking for the default value, then check the scale to make sure the value is 0M,
                // otherwise return true for any amount of zeros (i.e. 0.000M)
                DECIMAL *pdec = asCONSTANT()->getVal().decVal;
                return (DECIMAL_HI32(*pdec) == 0 && DECIMAL_MID32(*pdec) == 0 && DECIMAL_LO32(*pdec) == 0 && (!fDefValue || DECIMAL_SIGNSCALE(*pdec) == 0));
            }
        case FT_REF:
            return fDefValue && isNull();
        default:
            break;
        }
    }
    return false;
}


__forceinline int EXPR::getOffset()
{
    if (kind == EK_LOCAL) {
        ASSERT(!asLOCAL()->local->isConst);
        return asLOCAL()->local->slot.JbitDefAssg();
    } else if (kind == EK_FIELD) {
        return asFIELD()->offset;
    } else {
        return 0;
    }
};

__forceinline EXPR * EXPR::getArgs()
{
    RETAILVERIFY(kind == EK_CALL || kind == EK_PROP || kind == EK_FIELD || kind == EK_ARRINDEX);
    if (kind == EK_FIELD)
        return NULL;
    ASSERT(offsetof(EXPRCALL, args) == offsetof(EXPRPROP, args));
    ASSERT(offsetof(EXPRCALL, args) == offsetof(EXPRBINOP, p2));
    return (static_cast<EXPRCALL*>(this))->args;
}

__forceinline EXPR ** EXPR::getArgsPtr()
{
    RETAILVERIFY(kind == EK_CALL || kind == EK_PROP);
    ASSERT(offsetof(EXPRCALL, args) == offsetof(EXPRPROP, args));
    return &((static_cast<EXPRCALL*>(this))->args);
}

__forceinline EXPR * EXPR::getObject()
{
    RETAILVERIFY(kind == EK_CALL || kind == EK_PROP || kind == EK_FIELD);
    ASSERT(offsetof(EXPRCALL, object) == offsetof(EXPRPROP, object));
    ASSERT(offsetof(EXPRCALL, object) == offsetof(EXPRFIELD, object));
    return (static_cast<EXPRCALL*>(this))->object;
}


__forceinline bool EXPR::isFixedAggField()
{
    return kind == EK_FIELD && asFIELD()->FixedAgg() != NULL;
}

__forceinline SymWithType & EXPR::GetSymWithType()
{
    switch (kind) {
    default:
        ASSERT(!"Bad expr kind in EXPR::GetSymWithType"); // Fall through.
    case EK_CALL:
        return static_cast<EXPRCALL*>(this)->mwi;
    case EK_PROP:
        return static_cast<EXPRPROP*>(this)->pwtSlot;
    case EK_FIELD:
        return static_cast<EXPRFIELD*>(this)->fwt;
    case EK_EVENT:
        return static_cast<EXPREVENT*>(this)->ewt;
    }
}
#endif // __EXPRLIST_H__


