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
// File: exprnodes.h
//
// Definitions of the expr tree nodes
// ===========================================================================

#ifndef DECLARE_EXPR
#error You must define DECLARE_EXPR before including exprnodes.h
#endif // DECLARE_EXPR
#ifndef DECLARE_EXPR_NO_EK
#error You must define DECLARE_EXPR_NO_EK before including exprnodes.h
#endif // DECLARE_EXPR_NO_EK
#ifndef CHILD_EXPR
#error You must define CHILD_EXPR before including exprnodes.h
#endif // CHILD_EXPR
#ifndef CHILD_EXPR_NO_RECURSE
#error You must define CHILD_EXPR_NO_RECURSE before including exprnodes.h
#endif // CHILD_EXPR_NO_RECURSE
#ifndef NEXT_EXPR
#error You must define NEXT_EXPR before including exprnodes.h
#endif // NEXT_EXPR
#ifndef CHILD_SYM
#error You must define CHILD_SYM before including exprnodes.h
#endif // CHILD_SYM
#ifndef CHILD_TYPESYM
#error You must define CHILD_TYPESYM before including exprnodes.h
#endif // CHILD_TYPESYM
#ifndef CHILD_TYPEARRAY
#error You must define CHILD_TYPEARRAY before including exprnodes.h
#endif // CHILD_TYPEARRAY
#ifndef CHILD_OTHER
#error You must define CHILD_OTHER before including exprnodes.h
#endif // CHILD_OTHER
#ifndef CHILD_OTHER_SZ
#error You must define CHILD_OTHER_SZ before including exprnodes.h
#endif // CHILD_OTHER_SZ
#ifndef CHILD_WITHINST
#error You must define CHILD_WITHINST before including exprnodes.h
#endif // CHILD_WITHINST
#ifndef CHILD_WITHTYPE
#error You must define CHILD_WITHTYPE before including exprnodes.h
#endif // CHILD_WITHTYPE
#ifndef END_EXPR
#error You must define END_EXPR before including exprnodes.h
#endif // END_EXPR

#define THIS_TYPE( prefix) prefix ## STMT
DECLARE_EXPR_NO_EK( STMT, EXPR)
    NEXT_EXPR( stmtNext, EXPRSTMT) // next statement
    CHILD_EXPR_NO_RECURSE( stmtPar, EXPRSTMT) // parent statement
#ifdef INCLUDE_IMPL
    bool FReachable() { return !(flags & EXF_UNREACHABLEBEGIN); }
    bool FReachableEnd() { return !(flags & EXF_UNREACHABLEEND); }
    void SetReachable() { flags &= ~EXF_UNREACHABLEBEGIN; }
    void SetReachableEnd() { flags &= ~(EXF_UNREACHABLEBEGIN | EXF_UNREACHABLEEND); }
    void ClearReachable() { flags |= (EXF_UNREACHABLEBEGIN | EXF_UNREACHABLEEND); }
    void ClearReachableEnd() { flags |= EXF_UNREACHABLEEND; }
#endif // INCLUDE_IMPL
END_EXPR()
#undef THIS_TYPE

#define THIS_TYPE( prefix) prefix ## BINOP
DECLARE_EXPR( BINOP, EXPR)
    CHILD_EXPR( p1, EXPR)      // first member
    CHILD_EXPR( p2, EXPR)      // last member
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## USERLOGOP
DECLARE_EXPR( USERLOGOP, EXPR)
    CHILD_EXPR( opX, EXPR)
    CHILD_EXPR( callTF, EXPR)
    CHILD_EXPR( callOp, EXPR)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## DBLQMARK
DECLARE_EXPR( DBLQMARK, EXPR)
    CHILD_EXPR( exprTest, EXPR)
    CHILD_EXPR( exprConv, EXPR)
    CHILD_EXPR( exprElse, EXPR)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## TYPEOF
DECLARE_EXPR( TYPEOF, EXPR)
    CHILD_TYPESYM( sourceType, TYPESYM)
    CHILD_SYM( method, METHSYM)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## SIZEOF
DECLARE_EXPR( SIZEOF, EXPR)
    CHILD_TYPESYM( sourceType, TYPESYM)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## CAST
DECLARE_EXPR( CAST, EXPR)
    CHILD_EXPR( p1, EXPR)      // thing being cast
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## ZEROINIT
DECLARE_EXPR( ZEROINIT, EXPR)
    CHILD_EXPR( p1, EXPR)      // thing being inited, if any...
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## BLOCK
DECLARE_EXPR( BLOCK, EXPRSTMT)
    CHILD_EXPR( statements, EXPRSTMT)
    CHILD_EXPR_NO_RECURSE( owningBlock, EXPRBLOCK)     // the block in which this block appears...
    CHILD_SYM( scopeSymbol, SCOPESYM)       // corresponding scope symbol (could be NULL if none).
#ifdef INCLUDE_IMPL
    BitSet bsetExit;
#endif // INCLUDE_IMPL
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## STMTAS
DECLARE_EXPR( STMTAS, EXPRSTMT)
    CHILD_EXPR( expression, EXPR)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## MEMGRP
DECLARE_EXPR( MEMGRP, EXPR)
    CHILD_OTHER( nodeName, BASENODE *) // The node that indicates just the "name" portion.
    CHILD_OTHER( name, NAME *)
    CHILD_TYPEARRAY( typeArgs)
    CHILD_OTHER( sk, SYMKIND)

    // The type containing the members. This may be a TYVARSYM or an AGGTYPESYM.
    // This may be NULL (if types is not NULL).
    CHILD_TYPESYM( typePar, TYPESYM)

    CHILD_SYM( mps, METHPROPSYM)

    // The object expression. NULL for a static invocation.
    CHILD_EXPR( object, EXPR)

    // The types (within type) that contain the members. These are computed by MemberLookup.Lookup.
    // This may be null if typePar is a class.
    CHILD_TYPEARRAY( types)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## CALL
DECLARE_EXPR( CALL, EXPR)
    CHILD_EXPR( object, EXPR)           // must be 1st
    CHILD_EXPR( args, EXPR)             // must be 2nd to match EXPRBINOP.p2
    CHILD_WITHINST( mwi, MethWithInst)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## PROP
DECLARE_EXPR( PROP, EXPR)
    CHILD_EXPR( object, EXPR)           // must be 1st
    CHILD_EXPR( args, EXPR)             // must be 2nd to match EXPRBINOP.p2
    CHILD_WITHTYPE( pwtSlot, PropWithType)
    CHILD_WITHTYPE( mwtGet, MethWithType)
    CHILD_WITHTYPE( mwtSet, MethWithType)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## FIELD
DECLARE_EXPR( FIELD, EXPR)
    CHILD_EXPR( object, EXPR)           // must be 1st
    CHILD_WITHTYPE( fwt, FieldWithType)
    CHILD_OTHER( offset, unsigned)
#ifdef DEBUG
    CHILD_OTHER( fCheckedMarshalByRef, bool)
#endif
    #ifdef INCLUDE_IMPL
    AGGSYM * FixedAgg() {
        return fwt.Field()->fixedAgg;
    }
    #endif // INCLUDE_IMPL

END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## EVENT
DECLARE_EXPR( EVENT, EXPR)
    CHILD_EXPR( object, EXPR)           // must be 1st
    CHILD_WITHTYPE( ewt, EventWithType)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## DECL
DECLARE_EXPR( DECL, EXPRSTMT)
    CHILD_SYM( sym, LOCVARSYM)
    CHILD_EXPR( init, EXPR)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## LOCAL
DECLARE_EXPR( LOCAL, EXPR)
    CHILD_SYM( local, LOCVARSYM)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## RETURN
DECLARE_EXPR( RETURN, EXPRSTMT)
    CHILD_SYM( currentScope, SCOPESYM)
    CHILD_EXPR( object, EXPR)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## THROW
DECLARE_EXPR( THROW, EXPRSTMT)
    CHILD_EXPR( object, EXPR)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## CONSTANT
DECLARE_EXPR( CONSTANT, EXPR)
#ifdef INCLUDE_IMPL
private:
#endif // INCLUDE_IMPL

    CHILD_OTHER( val, CONSTVAL)

#ifdef INCLUDE_IMPL
public:
#endif // INCLUDE_IMPL

    CHILD_OTHER( pList, EXPR **)
    CHILD_EXPR_NO_RECURSE( list, EXPR)
    CHILD_OTHER( allocator, NRHEAP *)

#ifdef INCLUDE_IMPL
    void realizeStringConstant();

    bool isNull() 
    {
        return (type->fundType() == FT_REF && val.init == 0);
    }

    bool isEqual(EXPRCONSTANT * expr)
    {
        FUNDTYPE ft1 = type->fundType();
        FUNDTYPE ft2 = expr->type->fundType();
        if ((ft1 == FT_REF ) ^ (ft2 == FT_REF)) return false;

        if (list) realizeStringConstant();
        if (expr->list) expr->realizeStringConstant();

        if (ft1 == FT_REF) {
            if (!val.strVal) {
                return !expr->val.strVal;
            }
            return expr->val.strVal && val.strVal->length == expr->val.strVal->length && 
                !memcmp(val.strVal->text, expr->val.strVal->text, sizeof(WCHAR) * val.strVal->length );
        } else {
            return getI64Value() == expr->getI64Value() ;
        }
    }

    CONSTVALNS & getVal() 
    {
        return (CONSTVALNS&) val;
    }

    CONSTVAL & getSVal() 
    {
        if (list) realizeStringConstant();
        return val;
    }

    __int64 getI64Value() 
    {
        FUNDTYPE ft = type->fundType();

        switch (ft) {
        case FT_I8:
        case FT_U8:
            return *(val.longVal);
        case FT_U4:
            return val.uiVal;
        case FT_I1: case  FT_I2: case FT_I4: case FT_U1: case FT_U2:
            return val.iVal;
        default:
            VSFAIL("Bad fundType in getI64Value");
            return 0;
        }
    }
#endif // INCLUDE_IMPL
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## CLASS
DECLARE_EXPR( CLASS, EXPR)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## NSPACE
DECLARE_EXPR( NSPACE, EXPR)
    CHILD_SYM( nsa, NSAIDSYM)
END_EXPR()
#undef THIS_TYPE




#define THIS_TYPE( prefix) prefix ## LABEL
DECLARE_EXPR( LABEL, EXPRSTMT)
    CHILD_OTHER( block, struct BBLOCK *)            // the emitted block which follows this label
    CHILD_SYM( scope, SCOPESYM)
    CHILD_SYM( label, LABELSYM)                     // the symbol, if any (only defined on explicit, non-switch, user labels)
#ifdef INCLUDE_IMPL
    BitSet bsetEnter;
    int tsFinallyScan;

private:
    EXPRLABEL * labNext; // For temporary lists during reachability and definite assignment analysis.

public:
    void PushOnStack(EXPRLABEL ** plabTop) {
        ASSERT(this->kind == EK_LABEL);
        ASSERT(!this->labNext);

        this->labNext = *plabTop;
        *plabTop = this;
        // The bottom element of the stack has labNext set to itself
        // so we can easily check whether the label is in the stack.
        if (!this->labNext)
            this->labNext = this;
    }
    static EXPRLABEL * PopFromStack(EXPRLABEL ** plabTop) {
        EXPRLABEL * labCur = *plabTop;
        if (!labCur)
            return NULL;
        *plabTop = labCur->labNext;
        labCur->labNext = NULL;
        // The bottom element of the stack has labNext set to itself
        // so we can easily check whether the label is in the stack.
        if (*plabTop == labCur)
            *plabTop = NULL;
        return labCur;
    }
    bool InStack() {
        ASSERT(this->labNext == NULL || this->kind == EK_LABEL);
        return this->labNext != NULL;
    }
#endif // INCLUDE_IMPL
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## GOTO
DECLARE_EXPR( GOTO, EXPRSTMT)
#ifdef INCLUDE_IMPL
    union {
        NAME * labelName; // if we need to realized in the def-use pass.
#endif // INCLUDE_IMPL
        CHILD_EXPR_NO_RECURSE( label, EXPRLABEL)   // if know the code location of the dest
#ifdef INCLUDE_IMPL
    };
#endif // INCLUDE_IMPL
    CHILD_SYM( currentScope, SCOPESYM)        // the goto's location scope
    CHILD_SYM( targetScope, SCOPESYM)         // the scope of the label (only goto case, and break&cont knows that scope)
    CHILD_EXPR_NO_RECURSE( prev, EXPRGOTO)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## GOTOIF
DECLARE_EXPR( GOTOIF, EXPRGOTO)
    CHILD_EXPR( condition, EXPR)
    CHILD_OTHER( sense, bool)
#ifdef INCLUDE_IMPL
    bool FAlwaysJumps() { return !!condition->isTrueResult(sense); }
    bool FNeverJumps() { return !!condition->isFalseResult(sense); }
#endif // INCLUDE_IMPL
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## FUNCPTR
DECLARE_EXPR( FUNCPTR, EXPR)
    CHILD_WITHINST( mwi, MethWithInst)
    CHILD_EXPR_NO_RECURSE( object, EXPR)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## SWITCH
DECLARE_EXPR( SWITCH, EXPRSTMT)
    CHILD_EXPR( arg, EXPR)                      // what are we switching on?
    CHILD_OTHER( labelCount, unsigned)          // count of case statements + default statement
    CHILD_OTHER( labels, EXPRSWITCHLABEL **)    // NOT A LIST!!! this is an array of label expressions
    CHILD_EXPR( bodies, EXPRSWITCHLABEL)        // lists of switch labels
    CHILD_EXPR_NO_RECURSE( breakLabel, EXPRLABEL)
    CHILD_EXPR_NO_RECURSE( nullLabel, EXPRLABEL) // if switch on string or a nullable, and we have a "case null:"
    CHILD_OTHER( hashtableToken, mdToken)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## HANDLER
DECLARE_EXPR( HANDLER, EXPRSTMT)
    CHILD_EXPR( handlerBlock, EXPRBLOCK)
    CHILD_SYM( param, LOCVARSYM)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## TRY
DECLARE_EXPR( TRY, EXPRSTMT)
    CHILD_EXPR( tryblock, EXPRBLOCK)
    CHILD_EXPR( handlers, EXPRSTMT)  // either a block, or a chain of EXPRHANDLERs
#ifdef INCLUDE_IMPL
    bool IsFinallyBlocked() {
        if (!(flags & EXF_ISFINALLY))
            return false;
        ASSERT(handlers->FReachable());
        ASSERT(!(handlers->flags & EXF_MARKING));
        return !handlers->FReachableEnd();
    }
#endif // INCLUDE_IMPL
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## SWITCHLABEL
DECLARE_EXPR( SWITCHLABEL, EXPRLABEL)
    CHILD_EXPR( key, EXPR)          // the key of the case statement, or null if default
    CHILD_EXPR( statements, EXPRSTMT)   // statements under this label
    CHILD_OTHER( fellThrough, bool) // did we already error about this fallthrough before?
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## MULTIGET
DECLARE_EXPR( MULTIGET, EXPR)
    CHILD_EXPR_NO_RECURSE( multi, EXPRMULTI)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## MULTI
DECLARE_EXPR( MULTI, EXPR)
    CHILD_EXPR( left, EXPR)
    CHILD_EXPR( op, EXPR)
    CHILD_OTHER( pinfo, struct MultiOpInfo *)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## STTMP
DECLARE_EXPR( STTMP, EXPR)
    CHILD_EXPR( src, EXPR)
    CHILD_OTHER( slot, LOCSLOTINFO *)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## LDTMP
DECLARE_EXPR( LDTMP, EXPR)
    CHILD_EXPR_NO_RECURSE( tmp, EXPRSTTMP)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## FREETMP
DECLARE_EXPR( FREETMP, EXPR)
    CHILD_EXPR_NO_RECURSE( tmp, EXPRSTTMP)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## WRAP
DECLARE_EXPR( WRAP, EXPR)
    CHILD_EXPR_NO_RECURSE( expr, EXPR)
    CHILD_OTHER( slot, LOCSLOTINFO *)
    CHILD_OTHER_SZ( doNotFree, bool, 1)
    CHILD_OTHER_SZ( needEmptySlot, bool, 1)
    CHILD_OTHER_SZ( pinned, bool, 1)
    CHILD_OTHER( tempKind, TEMP_KIND)
    CHILD_SYM( containingScope, SCOPESYM)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## CONCAT
DECLARE_EXPR( CONCAT, EXPR)
    CHILD_EXPR( list, EXPR)
    CHILD_OTHER( pList, EXPR **)
    CHILD_OTHER( count, unsigned)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## ARRINIT
DECLARE_EXPR( ARRINIT, EXPR)
    CHILD_EXPR( args, EXPR)
    CHILD_OTHER( dimSizes, int *)
    CHILD_OTHER( dimSize, int)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## NOOP
DECLARE_EXPR( NOOP, EXPRSTMT)
END_EXPR()
#undef THIS_TYPE

#define THIS_TYPE( prefix) prefix ## DEBUGNOOP
DECLARE_EXPR( DEBUGNOOP, EXPRSTMT)
END_EXPR()
#undef THIS_TYPE

#define THIS_TYPE( prefix) prefix ## ANONMETH
DECLARE_EXPR( ANONMETH, EXPR)
    // ILGEN should never see these guys
    // They are transformed into something like this:
    // ((cachedDelegate == null) ? (cachedDelegate = new DelegateType($locals.Method)) : cachedDelegate)
    CHILD_OTHER( pInfo, AnonMethInfo *)
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## DELIM
DECLARE_EXPR( DELIM, EXPRSTMT)
    CHILD_OTHER( delim, DELIMKIND)
#ifdef INCLUDE_IMPL
    BitSet bset;
#endif // INCLUDE_IMPL
END_EXPR()
#undef THIS_TYPE


#define THIS_TYPE( prefix) prefix ## ERROR
DECLARE_EXPR( ERROR, EXPR)
    CHILD_OTHER( errorString, WCHAR *)
END_EXPR()
#undef THIS_TYPE

// Clear out all the MACROS
#undef DECLARE_EXPR
#undef DECLARE_EXPR_NO_EK
#undef CHILD_EXPR
#undef CHILD_EXPR_NO_RECURSE
#undef NEXT_EXPR
#undef CHILD_SYM
#undef CHILD_TYPESYM
#undef CHILD_TYPEARRAY
#undef CHILD_OTHER
#undef CHILD_OTHER_SZ
#undef CHILD_WITHINST
#undef CHILD_WITHTYPE
#undef END_EXPR
