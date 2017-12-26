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
// File: fncbind.h
//
// Defines the structure which contains information necessary to bind and generate
// code for a single function.
// ===========================================================================

#ifndef __fncbind_h__
#define __fncbind_h__

class FUNCBREC;

//
// Callack interface from binding
// NOTE: This is NOT a IUnknown interface!
//
interface ICompileCallback
{
public:
    // Used by extract method for reachability processing
    virtual bool IsStartStatementNode(BASENODE *pNode) = 0;
    virtual bool IsEndStatementNode(BASENODE *pNode) = 0;
    virtual void DelimCreated(EXPRDELIM * delim) = 0;
    virtual void StartPostBinding(EXPRBLOCK * block) = 0;
    virtual void ProcessExpression(EXPR *expr, BitSet & bset) = 0;
    virtual void EndPostBinding(EXPRBLOCK * block) = 0;

    //CAlled when a using node in the specified namespace decalration is bound to pSym
    virtual void ResolvedUsingNode(NSDECLSYM* pNSDecl, USINGNODE* pUsing, SYM* pSym) = 0;

    //called when we successfully bind to something in the sym provided by a using in the specified namespace declaration
    virtual void BoundToUsing(NSDECLSYM* pNSDecl, SYM* pSym) = 0;

    virtual bool CheckForNameSimplification() = 0;
    virtual void CanSimplifyName(ICSSourceData* pData, BINOPNODE* node) =0;
    virtual void CanSimplifyNameToPredefinedType(ICSSourceData* pData, BASENODE* node, TYPESYM* pType) =0;
};

//
// Callack interface from binding
// NOTE: This is NOT a IUnknown interface!
//
class CCompileCallbackAdapter : public ICompileCallback
{
public:
    virtual bool IsStartStatementNode(BASENODE *pNode) { return false; }
    virtual bool IsEndStatementNode(BASENODE *pNode) { return false; }
    virtual void DelimCreated(EXPRDELIM * delim) {}
    virtual void StartPostBinding(EXPRBLOCK * block) {}
    virtual void ProcessExpression(EXPR *expr, BitSet & bset) {}
    virtual void EndPostBinding(EXPRBLOCK * block) {}

    virtual void ResolvedUsingNode(NSDECLSYM* pNSDecl, USINGNODE* pUsing, SYM* pSym) {}
    virtual void BoundToUsing(NSDECLSYM* pNSDecl, SYM* pSym) {}
    virtual bool CheckForNameSimplification() { return false; }
    virtual void CanSimplifyName(ICSSourceData* pData, BINOPNODE* node) {}
    virtual void CanSimplifyNameToPredefinedType(ICSSourceData* pData, BASENODE* node, TYPESYM* pType) {}
};


// This member is used only in the context of ExtractMethodSignature
// to find out if the last statement of the list of selected statements
// is reachable or not.
class CCompileCallbackForward : public ICompileCallback
{
    ICompileCallback *bindCallback;

public:
    CCompileCallbackForward()
    {
        Reset();
    }
    void Reset()
    {
        bindCallback = NULL;
    }
    void Init(ICompileCallback *bindCallback)
    {
        this->bindCallback = bindCallback;
    }
    bool IsForwarding()
    {
        return (bindCallback != NULL);
    }
    ICompileCallback * GetTarget()
    {
        return bindCallback;
    }
    bool ForcePostBind()
    {
        return IsForwarding();
    }
    
    // IBindCallback forwarding
    bool IsStartStatementNode(BASENODE *pNode)
    {
        return bindCallback && bindCallback->IsStartStatementNode (pNode);
    }
    bool IsEndStatementNode(BASENODE *pNode)
    {
        return bindCallback && bindCallback->IsEndStatementNode (pNode);
    }
    void DelimCreated(EXPRDELIM * delim)
    {
        if (bindCallback != NULL)
            bindCallback->DelimCreated(delim);
    }
    void StartPostBinding(EXPRBLOCK * block)
    {
        if (bindCallback != NULL)
            bindCallback->StartPostBinding(block);
    }
    void ProcessExpression(EXPR *expr, BitSet & bset)
    {
        if (bindCallback != NULL)
            bindCallback->ProcessExpression(expr, bset);
    }
    void EndPostBinding(EXPRBLOCK * block)
    {
        if (bindCallback != NULL)
            bindCallback->EndPostBinding(block);
    }

    void ResolvedUsingNode(NSDECLSYM* pNSDecl, USINGNODE* pUsing, SYM* pSym)
    {
        if (bindCallback != NULL)
            bindCallback->ResolvedUsingNode (pNSDecl, pUsing, pSym);
    }
    void BoundToUsing(NSDECLSYM* pNSDecl, SYM* pSym)
    {
        if (bindCallback != NULL)
            bindCallback->BoundToUsing (pNSDecl, pSym);
    }
    bool CheckForNameSimplification()
    {
        if (bindCallback != NULL)
            return bindCallback->CheckForNameSimplification();
        return false;
    }
    void CanSimplifyName(ICSSourceData* pData, BINOPNODE* node)
    {
        if (bindCallback != NULL)
            bindCallback->CanSimplifyName (pData, node);
    }
    void CanSimplifyNameToPredefinedType(ICSSourceData* pData, BASENODE* node, TYPESYM* pType)
    {
        if (bindCallback != NULL)
            bindCallback->CanSimplifyNameToPredefinedType(pData, node, pType);
    }
};


//////////////////////////////////////////////////////////////////////////////////////////////
// inline functions must be included here so they can be picked-up by metaattr.cpp
// so the definitions are at the end of this header

struct LOOPLABELS {
    EXPRLABEL * contLabel;
    EXPRLABEL * breakLabel;

    LOOPLABELS(class FUNCBREC *);
    LOOPLABELS() {};
};

struct CHECKEDCONTEXT {
    bool normal;
    bool constant;

    CHECKEDCONTEXT(class FUNCBREC *, bool checked);
    CHECKEDCONTEXT() {};
    void restore(class FUNCBREC *);
};

//////////////////////////////////////////////////////////////////////////////////////////////

// Used to string together methods in the pool of available methods...
struct METHLIST {
    MethPropWithInst mpwi;

    // params is the result of type variable substitution on either mpwi.MethProp()->params or
    // an expansion of mpwi.MethProp()->params (for a param array).
    TypeArray * params;

    byte ctypeLift; // How many parameter types are lifted (for tie-breaking).
    bool fExpanded; // Whether the params came from expanding mpwi.MethProp()->params.
    METHLIST * next;
};


// One of these for each anonymous method.
struct AnonMethInfo {
    AnonMethInfo * pamiOuter;   // The outer (containing) anonymous method.
    AnonMethInfo * pamiNext;    // The next (sibling) one.
    AnonMethInfo * pamiChild;   // The first child (contained) anonymous method.

    BASENODE * tree;            // The parse tree.
    NAME * name;                // The unique method name to use.
    TypeArray * params;         // The parameter types - if given.
    EXPR * listReturns;         // The EK_LIST combined return statements.
    EXPR ** pexprListRet;       // The list building pointer.
    TYPESYM * typeRet;          // The return type of the delegate type. This is in terms of user symbols - no fabricated type variables.

    EXPRBLOCK * pBodyExpr;      // EXPR for  body of the anonymous delgate
    SCOPESYM * pArgs;           // The scope containing the names of the parameters
    EXPR * pCachedDelegate;     // The local or static used to cache this anonymous delegate
    AGGTYPESYM * pDelegateType; // The delegate type of this anonymous method

    SCOPESYM * pScope;          // The scope that will hold this AM
    LOCVARSYM * thisPointer;    // The <this> local inside the anonymous method
    bool fUsesLocals;           // Whether this anon meth uses locals / parameters from an outer method
    bool fUsesThis;             // Whether this anon meth uses the <this> pointer of the outer method
    bool fHasRetAsLeave;        // has a return inside of a try or catch (inside this anonymous method)
    bool fSeen;                 // Whether this anon method is in the final expr tree.
    bool fInCtorPreamble;       // Whether this anon method is directly in a field initializer or base ctor invocation.

    // Used for definite assignment analysis
    BitSet bsetEnter;
    int jbitMin;
    int jbitLim;

    METHSYM * meth;             // The anonymous method SYM. Created only when actually needed.
};

namespace IteratorStates { enum _Enum {
    UnusedIEnumerable = -2,
    RunningIEnumerator = -1,
    NotStartedIEnumerator = 0,
    FirstUnusedState = 1,
}; };
DECLARE_ENUM_TYPE(IteratorStates);


//////////////////////////////////////////////////////////////////////////////////////////////
// Used to distinguish compiler generated names in the EE
namespace SpecialNameKind { enum _Enum {
    None = 0,
    // Fields
    IteratorState = 1,      // <>1__state (also hard-coded into predefname table)
    CurrentField = 2,       // <>2__current (also hard-coded into predefname table)
    SavedParamOrThis = 3,   // <>3__... (hard-coded)
    HoistedThis = 4,        // <>4__this (hard-coded into predefname table)
    HoistedIteratorLocal,
    OuterscopeLocals,
    HoistedWrap,

    // Locals
    DisplayClassInstance,   // can also be a field
    CachedDelegateInstance, // can also be a field
    IteratorInstance,

    // Methods
    AnonymousMethod,

    // Types
    AnonymousMethodDisplayClass,
    IteratorClass,
    FixedBufferStruct,

    // Keep This last
    LastKind = FixedBufferStruct
}; };
DECLARE_ENUM_TYPE(SpecialNameKind);

C_ASSERT(SpecialNameKind::LastKind < 16);

//////////////////////////////////////////////////////////////////////////////////////////////
// Small wrapper for passing around argument information for the various BindGrpTo* methods
// It is used because most things only need the type, but in the case of METHGRPs and ANONMETHs
// the expr is also needed to detimine if a conversion is possible
struct ArgInfos {
    int         carg;
    TypeArray * types;
    bool        fHasExprs;
    EXPR      * prgexpr[];
};

//////////////////////////////////////////////////////////////////////////////////////////////

#define LOOKUPMASK (MASK_NSAIDSYM | MASK_AGGTYPESYM | MASK_NUBSYM | MASK_MEMBVARSYM |  MASK_TYVARSYM | MASK_LOCVARSYM | MASK_METHSYM | MASK_PROPSYM)


//////////////////////////////////////////////////////////////////////////////////////////////


#define BIND_RVALUEREQUIRED 0x01 // this is a get of expr, not an assignment to expr
#define BIND_MEMBERSET      0x02 // indicates that an lvalue is needed
#define BIND_FIXEDVALUE     0x10 // ok to take address of unfixed
#define BIND_ARGUMENTS      0x20 // this is an argument list to a call...
#define BIND_BASECALL       0x40 // this is a base method or prop call
#define BIND_USINGVALUE     0x80 // local in a using stmt decl
#define BIND_STMTEXPRONLY  0x100 // only allow expressions that are valid in a statement
#define BIND_TYPEOK        0x200 // types are ok to be returned
#define BIND_MAYBECONFUSEDNEGATIVECAST  0x400 // this may be a mistaken negative cast
#define BIND_METHODNOTOK   0x800 // naked methods are not ok to be returned
#define BIND_DECLNOTOK    0x1000 // var decls are not ok to be returned
#define BIND_NOPARAMS     0x2000 // Do not do params expansion during overload resolution
//////////////////////////////////////////////////////////////////////////////////////////////



namespace BinOpKind { enum _Enum {
    Add,
    Sub,
    Mul,
    Shift,
    Equal,
    Compare,
    Bitwise,
    BitXor,
    Logical,

    Lim
}; };
DECLARE_ENUM_TYPE(BinOpKind);


namespace BinOpMask { enum _Enum {
    None = 0,
    Add = 1 << BinOpKind::Add,
    Sub = 1 << BinOpKind::Sub,
    Mul = 1 << BinOpKind::Mul,
    Shift = 1 << BinOpKind::Shift,
    Equal = 1 << BinOpKind::Equal,
    Compare = 1 << BinOpKind::Compare,
    Bitwise = 1 << BinOpKind::Bitwise,
    BitXor = 1 << BinOpKind::BitXor,
    Logical = 1 << BinOpKind::Logical,

    // The different combinations needed in operators.cpp
    Integer     = Add | Sub | Mul | Equal | Compare | Bitwise | BitXor,
    Real        = Add | Sub | Mul | Equal | Compare,
    BoolNorm    =                   Equal |                     BitXor,

    // These are special ones.
    Delegate    = Add | Sub |       Equal,
    Enum        =       Sub |       Equal | Compare | Bitwise | BitXor,
    EnumUnder   = Add | Sub,
    UnderEnum   = Add,
    Ptr         =       Sub,
    PtrNum      = Add | Sub,
    NumPtr      = Add,
    VoidPtr     =                   Equal | Compare,
}; };
DECLARE_FLAGS_TYPE(BinOpMask);


namespace UnaOpKind { enum _Enum {
    Plus,
    Minus,
    Tilde,
    Bang,
    IncDec,

    Lim
}; };
DECLARE_ENUM_TYPE(UnaOpKind);


namespace UnaOpMask { enum _Enum {
    None = 0,
    Plus = 1 << UnaOpKind::Plus,
    Minus = 1 << UnaOpKind::Minus,
    Tilde = 1 << UnaOpKind::Tilde,
    Bang = 1 << UnaOpKind::Bang,
    IncDec = 1 << UnaOpKind::IncDec,

    // The different combinations needed in operators.cpp
    Signed      = Plus | Minus | Tilde,
    Unsigned    = Plus |         Tilde,
    Real        = Plus | Minus,
    Bool        = Bang,

}; };
DECLARE_FLAGS_TYPE(UnaOpMask);


namespace LiftFlags { enum _Enum {
    None = 0,
    Lift1 = 0x01,
    Lift2 = 0x02,
    Convert1 = 0x04,
    Convert2 = 0x08,

    LiftBoth = Lift1 | Lift2,
}; };
DECLARE_FLAGS_TYPE(LiftFlags);


namespace OpSigFlags { enum _Enum {
    None = 0,
    Convert = 0x01,   // Convert the operands before calling the bind method
    CanLift = 0x02,   // Operator has a lifted form
    AutoLift = 0x04,  // Standard nullable lifting

    // The different combinations needed in operators.cpp
    Value =     Convert | CanLift | AutoLift,
    Reference = Convert,
    BoolBit =  Convert | CanLift,
}; };
DECLARE_FLAGS_TYPE(OpSigFlags);


// Indicates what kind of conversion exists between two types.
namespace ConvKind { enum _Enum {
    Identity = 1, // Identity conversion
    Implicit = 2, // Implicit conversion
    Explicit = 3, // Explicit conversion
    Unknown  = 4, // Unknown so call canConvert
    None     = 5, // None
}; };
DECLARE_ENUM_TYPE(ConvKind);


// Indicates which type is better for overload resolution.
namespace BetterType { enum _Enum {
    Same = 0,    // They're the same
    Left = 1,    // Implicit conversion
    Right = 2,   // Explicit conversion
    Neither = 3, // Neither
}; };
DECLARE_ENUM_TYPE(BetterType);


class FUNCBREC {
    friend LOOPLABELS::LOOPLABELS(FUNCBREC *);
    friend CHECKEDCONTEXT::CHECKEDCONTEXT(FUNCBREC *, bool);
    friend void CHECKEDCONTEXT::restore(FUNCBREC *);

public:

    enum UNSAFESTATES {
        UNSAFESTATES_Unsafe,
        UNSAFESTATES_Safe,
        UNSAFESTATES_Unknown,
    };


    // Flags for bindImplicitConversion/bindExplicitConversion
    enum CONVERTTYPE {
        NOUDC               = 0x01,
        STANDARD            = 0x02, // standard only, but never pass it in, used only to check...
        ISEXPLICIT          = 0x04, // implicit conversion is really explicit
        CHECKOVERFLOW       = 0x08, // check overflow (like in a checked context).

        STANDARDANDNOUDC    = 0x03, // pass this in if you mean standard conversions only
    };


    FUNCBREC();


    EXPR * compileMethod(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo, ICompileCallback *pBindCallback = NULL);
    EXPR * compileFirstField(MEMBVARSYM * pAFSym, BASENODE * tree);
    EXPR * compileNextField(MEMBVARSYM * pAFSym, BASENODE * tree);
    bool __fastcall isThisPointer(EXPR * expr);
    bool __fastcall hasThisPointer();
    LOCSLOTINFO * getThisPointerSlot();
    void DiscardLocalState();

    EXPR * rewriteMoveNext(METHSYM * pOuterMeth, EXPR * pIterExpr, METHINFO * info);
    EXPR * makeIterGet(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo);
    EXPR * makeIterGetEnumerator(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo, METHSYM ** pmethGetEnumerator);
    EXPR * makeIterCur(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo);
    EXPR * makeIterReset(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo);
    EXPR * makeIterDispose(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo);
    EXPR * makeIterCtor(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo);

    void FixUpAnonMethInfoLists(AnonMethInfo ** ppami);
    EXPR * RewriteAnonDelegateBodies(METHSYM * meth, SCOPESYM * scpArg, AnonMethInfo * pamiFirst, EXPR * body);
    EXPR * makeAnonCtor(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo);

    bool getAttributeValue(PARENTSYM *context, EXPR * val, bool *       rval);
    bool getAttributeValue(PARENTSYM *context, EXPR * val, int *        rval);
    bool getAttributeValue(PARENTSYM *context, EXPR * val, STRCONST **  rval);
    bool getAttributeValue(PARENTSYM *context, EXPR * val, TYPESYM **   rval);

    EXPRCALL *bindAttribute(PARENTSYM * context, AGGTYPESYM * attributeClass, ATTRNODE * attribute, EXPR ** namedArgs);
    CONSTVAL createStringConstant(PCWSTR str);
    EXPR * bindSimplePredefinedAttribute(PREDEFTYPE pt);
    EXPR * bindSimplePredefinedAttribute(PREDEFTYPE pt, EXPR * arg);
    EXPR * bindStringPredefinedAttribute(PREDEFTYPE pt, PCWSTR arg);
    EXPR * bindStructLayoutArgs();
    EXPR * bindStructLayoutNamedArgs(bool hasNonZeroSize);
    EXPR * bindSkipVerifyArgs();
    EXPR * bindSkipVerifyNamedArgs();
    EXPR * bindDebuggableArgs();
    EXPR * bindFixedBufferArgs(MEMBVARSYM * field);
    void   bindCharSetNamedArg(AGGTYPESYM * cls, int defaultCharSet, EXPR ** namedArguments);
    EXPR * bindCompilationRelaxationsAttributeArgs();
    EXPR * bindRuntimeCompatibilityAttributeNamedArgs();
    EXPR * bindPropertyNamedArgument(PREDEFTYPE ptAttributeType, PREDEFNAME pnPropertyName, CONSTVAL value);
    EXPR * bindFieldNamedArgument(AGGTYPESYM *attributeType, PREDEFNAME pnFieldName, CONSTVAL value);

    void RealizeStringConcat(EXPRCONCAT * expr);
    bool isManagedType(TYPESYM * type);
    static bool isCastOrExpr(EXPR * search, EXPR * target);


    void setUnsafe(bool unsafe) {
        unsafeState = unsafe ? UNSAFESTATES_Unsafe : UNSAFESTATES_Safe;
        unsafeErrorGiven = false;
    };

    void setUnsafeState(UNSAFESTATES newState) {
        unsafeState = newState;
    }

    void resetUnsafe() {
        unsafeState = UNSAFESTATES_Unknown;
    }

    // Call only when post binding is in progress
    bool IsLocalDefinitelyAssigned(LOCVARSYM * local, BitSet & bset, bool * pfPartial = NULL);

    // call when finished compiling an outer-most class because
    // no name should 'escape' or be more visible than the user's class.
    void ResetUniqueNames() {
        uniqueNameIndex = 0;
    }

protected:
    // Persistent members:

    // Initialized w/ each function:
    CCompileCallbackForward bindCallback;  // Callback for refactoring
    METHSYM * pMSym;
    BASENODE * pTree;
    METHINFO * info;

    AnonMethInfo * m_pamiFirst; // List of top level anonymous methods.
    AnonMethInfo * m_pamiCur; // Current anonymous method being bound.

    TypeArray * taClsVarsForMethVars;
    TypeBindFlagsEnum btfFlags;       // extra flags to bind type for deprecation checking

    // If initialized for a field:
    MEMBVARSYM * pFSym; // current field to evaluate
    MEMBVARSYM * pFOrigSym; // original field to evaluate

    // The following two variables are set when pMSym and pFSym and set.
    AGGSYM * parentAgg; // The parent aggregate. Can be NULL when binding class and global attributes arguments.
    DECLSYM * parentDecl; // The parent declaration, for various name binding uses. This is either an
                          // AGGDECLSYM (if parentAgg is non-NULL), or an NSDECLSYM (if parentAgg is NULL).
                          // Note that parentAgg isn't enough for name binding if partial classes are used,
                          // because the using clauses in effect may be different and unsafe state may be different.

    AGGINFO * pClassInfo;

    LOOPLABELS initLabels;
    LOOPLABELS * loopLabels;
public:
    SCOPESYM * pOuterScope; // the argument scope
private:
    SCOPESYM * pCurrentScope; // the current scope
    SCOPESYM * pFinallyScope; // the scope of the innermost finally, or pOuterScope if none...
    SCOPESYM * pTryScope; // the scope of the innermost try, or pOuterScope if none...
    SCOPESYM * pSwitchScope; // the scope of the innermost switch, or NULL if none
    SCOPESYM * pCatchScope; // the scope of the innermose catch, or NULL if none
    class EXPRBLOCK * pCurrentBlock;
    EXPRSWITCH * exprSwitchCur;

    struct SwitchPusher
    {
    private:
        FUNCBREC * m_fnc;
        EXPRSWITCH * m_exprSwitchPrev;
    public:
        SwitchPusher(FUNCBREC * fnc, EXPRSWITCH * exprSwitchCur) {
            ASSERT(fnc);
            m_fnc = fnc;
            m_exprSwitchPrev = m_fnc->exprSwitchCur;
            m_fnc->exprSwitchCur = exprSwitchCur;
        }
        ~SwitchPusher() {
            if (m_fnc) {
                m_fnc->exprSwitchCur = m_exprSwitchPrev;
                m_fnc = NULL;
            }
        }
    };

#if defined(__GNUC__) && (__GNUC__ < 3)
    friend struct FUNCBREC::SwitchPusher;
#endif

    NRHEAP * allocator;

    LOCVARSYM * thisPointer;

    // This is only valid during transform stage and should be NULL otherwise
    LOCVARSYM * outerThisPointer; // ONLY used by anonymous methods, keeps a <this> pointer for the outermost containing method.

    int uninitedVarCount;
    int unreferencedVarCount;
    int finallyNestingCount;
    int localCount;
    int firstParentOffset;
    int uniqueNameIndex;
    int errorsBeforeBind;

    // Whether there are any forward gotos so far. This affects whether we need to
    // track the definite assignment state of a local that has an initializer.
    // It's strictly an optimization. In general, if you error, do so on the side
    // of setting this to true.
    bool m_fForwardGotos;

    UNSAFESTATES unsafeState;
    bool isUnsafeContext() {
        ASSERT(unsafeState != UNSAFESTATES_Unknown);
        return unsafeState == UNSAFESTATES_Unsafe;
    };
    bool unsafeErrorGiven;
    bool insideTryOfCatch;
    BASENODE * lastNode;

    CHECKEDCONTEXT checked;
    
    EXPR * userLabelList;
    EXPR ** pUserLabelList;
    EXPRGOTO * gotos;

    bool inFieldInitializer;  // Are we evaluating a field initializer?

    class FncBindState
    {
    private:
        FUNCBREC * fnc;
        METHSYM * meth;
        MEMBVARSYM * field;
        BASENODE * tree;
        AGGSYM * aggPar;
        DECLSYM * declPar;
        TypeBindFlagsEnum btfFlags;
        AGGINFO * infoCls;
        SCOPESYM * scopeOuter;

    public:
        FncBindState() {
            fnc = NULL;
        }
        ~FncBindState() {
            if (fnc)
                Restore();
        }
        FncBindState(FUNCBREC * fnc) {
            Save(fnc);
        }

        void Save(FUNCBREC * fnc);
        void Restore();
    };
    friend class FncBindState;

    // Predefined operator overload resolution.
    typedef EXPR * (FUNCBREC::*PfnBindBinOp)(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * op1, EXPR * op2);
    typedef EXPR * (FUNCBREC::*PfnBindUnaOp)(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * op);

    struct BinOpSig {
        PREDEFTYPE pt1;
        PREDEFTYPE pt2;
        BinOpMaskEnum grfbom;
        int cbosSkip;
        PfnBindBinOp pfn;
        OpSigFlagsEnum grfos;
    };

    struct BinOpFullSig : BinOpSig {
        TYPESYM * type1;
        TYPESYM * type2;
        LiftFlagsEnum grflt;

        void Set(TYPESYM * type1, TYPESYM * type2, PfnBindBinOp pfn, OpSigFlagsEnum grfos,
            LiftFlagsEnum grflt = LiftFlags::None);
        bool Set(FUNCBREC * fnc, BinOpSig & bos);

        bool FPreDef() { return pt1 >= 0; }
    };
    friend struct BinOpFullSig;

    struct UnaOpSig {
        PREDEFTYPE pt;
        UnaOpMaskEnum grfuom;
        int cuosSkip;
        PfnBindUnaOp pfn;
    };

    struct UnaOpFullSig : UnaOpSig {
        TYPESYM * type;
        LiftFlagsEnum grflt;

        void Set(TYPESYM * type, PfnBindUnaOp pfn, LiftFlagsEnum grflt = LiftFlags::None);
        bool Set(FUNCBREC * fnc, UnaOpSig & uos);

        bool FPreDef() { return pt >= 0; }
    };
    friend struct UnaOpFullSig;

    static BinOpSig g_rgbos[];
    static UnaOpSig g_rguos[];

    static const byte simpleTypeConversions[NUM_SIMPLE_TYPES][NUM_SIMPLE_TYPES];
    static ConvKindEnum GetConvKind(PREDEFTYPE ptSrc, PREDEFTYPE ptDst)
    {
        if (ptSrc < NUM_SIMPLE_TYPES && ptDst < NUM_SIMPLE_TYPES)
            return ConvKindEnum(simpleTypeConversions[ptSrc][ptDst] & 0x0F);
        if (ptSrc == ptDst || ptDst == PT_OBJECT && ptSrc < PT_COUNT)
            return ConvKind::Implicit;
        if (ptSrc == PT_OBJECT && ptDst < PT_COUNT)
            return ConvKind::Explicit;
        return ConvKind::Unknown;
    }

    static const byte g_mpptptBetter[NUM_EXT_TYPES + 1][NUM_EXT_TYPES + 1];
    BetterTypeEnum WhichTypeIsBetter(PREDEFTYPE pt1, PREDEFTYPE pt2, TYPESYM * typeGiven);
    BetterTypeEnum WhichTypeIsBetter(TYPESYM * type1, TYPESYM * type2, TYPESYM * typeGiven);

// The default one is good enough for us...
//    ~FUNCBREC();

    void initThisPointer();
    void InitField(MEMBVARSYM * field);
    void InitMethod(METHINFO * infoMeth, BASENODE * tree, AGGINFO * infoCls);
    SCOPESYM * initAttrBindContext(PARENTSYM * context);

    void DeclareMethodParameters(METHINFO * info, EXPR ** params = NULL);
    void addTyVarsToScope(SCOPESYM * scope);
#ifdef DEBUG
#define declareVarNoScopeCheck(tree,ident,type) declareVar(tree,ident,type,true)
    LOCVARSYM * declareVar(BASENODE * tree, NAME * ident, TYPESYM * type, bool fSkipScopeCheck = false);
#else
#define declareVarNoScopeCheck(tree,ident,type) declareVar(tree,ident,type)
    LOCVARSYM * declareVar(BASENODE * tree, NAME * ident, TYPESYM * type);
#endif
    LOCVARSYM * declareParam(NAME *ident, TYPESYM *type, unsigned flags, BASENODE * parseTree, SCOPESYM* pScope = NULL);
    LOCVARSYM * addParam(PNAME ident, TYPESYM *type, unsigned paramFlags, SCOPESYM* pScope = NULL);
    TYPESYM * bindType(TYPEBASENODE * tree);

    // Field initializers always use the parentDecl because they do not have access to constructor parameters or type parameters
    PARENTSYM * contextForTypeBinding() { 
        if (pMSym && !inFieldInitializer) {
            ASSERT(pMSym->containingDeclaration() == parentDecl);
            return pMSym;
        }
        return parentDecl;
    }

    LOCVARSYM * lookupLocalVar(NAME * name, SCOPESYM *scope);

    friend class MemberLookup;

    EXPR * SetNodeExpr(BASENODE * node, EXPR * expr) { return expr; }
    EXPRSTMT * SetNodeStmt(BASENODE * node, EXPRSTMT * stmt) { return stmt; }

    EXPR * newExprZero(BASENODE * tree, TYPESYM * type);
    EXPRCONSTANT * newExprConstant(BASENODE * tree, TYPESYM * type, CONSTVAL cv);
    EXPRLABEL * newExprLabel();
    EXPRBLOCK * newExprBlock(BASENODE * tree);
    EXPRBINOP * newExprBinop(BASENODE * tree, EXPRKIND kind, TYPESYM * type, EXPR *p1, EXPR *p2);
    EXPR *      newExprCore(BASENODE * tree, EXPRKIND kindReal, TYPESYM * type, int flags, EXPRKIND kindAlloc);
    EXPR *      newExpr(BASENODE * tree, EXPRKIND kind, TYPESYM * type) { return newExprCore(tree, kind, type, 0, kind); }
    EXPR *      newExpr(EXPRKIND kind) { return newExprCore(NULL, kind, NULL, 0, kind); }
    EXPRWRAP *  newExprWrap(EXPR * wrap, TEMP_KIND tempKind);
    EXPRSTMTAS *MakeStmt(BASENODE * tree, EXPR * expr, int flags = 0);
    void        newList(EXPR * newItem, EXPR *** list);
    EXPRERROR * newError(BASENODE * tree, TYPESYM * type);
    EXPRDELIM * newExprDelim(BASENODE * tree, DELIMKIND delim);
    EXPRMEMGRP * newExprMemGrp(BASENODE * tree, BASENODE * nodeName, MemberLookup & mem);

    EXPR *      createConstructorCall(BASENODE *tree, BASENODE * nodeCtor, AGGTYPESYM *cls, EXPR * thisExpression, EXPR *arguments, MemLookFlagsEnum flags);
    bool        isMethPropCallable(METHPROPSYM * sym, bool requireUC);
public:
    // called by CLSDREC::MakeIterator
    METHSYM * FindPredefMeth(BASENODE * tree, PREDEFNAME id, AGGTYPESYM * type, TypeArray * params,
        bool fReportError = true, MemLookFlagsEnum flags = MemLookFlags::None);
private:
    class StmtListBldr {
        EXPRSTMT ** m_pstmtNext;
    public:
        StmtListBldr(EXPRSTMT ** pstmtNext) {
            Init(pstmtNext);
        }
        void Init(EXPRSTMT ** pstmtNext) {
            ASSERT(pstmtNext && !*pstmtNext);
            m_pstmtNext = pstmtNext;
        }
        void Clear() {
            m_pstmtNext = NULL;
        }
        void Add(EXPRSTMT * stmt) {
            ASSERT(m_pstmtNext && !*m_pstmtNext);
            if (stmt) {
                ASSERT(!stmt->stmtNext);
                *m_pstmtNext = stmt;
                m_pstmtNext = &stmt->stmtNext;
            }
        }
        void Add(EXPRSTMT * stmt, int flags) {
            ASSERT(m_pstmtNext && !*m_pstmtNext);
            if (stmt) {
                ASSERT(!stmt->stmtNext);
                stmt->flags |= flags;
                *m_pstmtNext = stmt;
                m_pstmtNext = &stmt->stmtNext;
            }
        }
        void AddList(EXPRSTMT * stmt) {
            ASSERT(m_pstmtNext && !*m_pstmtNext);
            *m_pstmtNext = stmt;
            while (*m_pstmtNext)
                m_pstmtNext = &(*m_pstmtNext)->stmtNext;
        }
        EXPRSTMT ** PstmtNext() { return m_pstmtNext; }
    };

    EXPR * BindPredefMethToArgs(BASENODE * tree, PREDEFNAME id, AGGTYPESYM * type, EXPR * obj, EXPR * args, TypeArray * typeArgs = NULL);
    EXPR * BindGrpToArgs(BASENODE * tree, int bindFlags, EXPRMEMGRP * grp, EXPR * args);
    bool BindGrpToArgsCore(MethPropWithInst * pmpwi, BASENODE * tree, int bindFlags, EXPRMEMGRP * grp, ArgInfos * args,
        AGGTYPESYM * atsDelegate, bool fReportErrors, MethPropWithInst * pmpwiAmbig);
    void FillInArgInfoFromArgList(ArgInfos *argInfo, EXPR * args);

    bool BindGrpToParams(MethPropWithInst * pmpwi, BASENODE * tree, EXPRMEMGRP * grp, TypeArray * params,
        bool fReportErrors, MethPropWithInst * pmpwiAmbig, TYPESYM * typeRet = NULL);
    bool BindGrpConversion(BASENODE * tree, EXPRMEMGRP * grp, TYPESYM * typeDst, EXPR ** pexprDst, bool fReportErrors);

    void PostBindMeth(BASENODE * tree, bool fBaseCall, MethWithInst * pmwi, EXPR ** pobj, bool * pfConstrained);
    void PostBindProp(BASENODE * tree, bool fBaseCall, PropWithType pwt, /*in/out*/ EXPR ** pobj,
        /*out*/ bool * pfConstrained, /*out*/ MethWithType * pmwtGet, /*out*/ MethWithType * pmwtSet);
    void SetExternalRef(TYPESYM * type);

    bool InferTypeArgs(METHSYM * meth, AGGTYPESYM * type, TypeArray * paramsMeth, ArgInfos * argsMatch, TypeArray ** ptypeArgs);

    void RemapToOverride(SymWithType * pswt, TYPESYM * typeObj);
    METHLIST * FindBestMethod(METHLIST * list, ArgInfos * args, METHLIST ** methAmbig1, METHLIST ** methAmbig2);
    void        verifyMethodArgs(EXPR * call);
    int         WhichMethodIsBetter(METHLIST * method1, METHLIST * method2, ArgInfos * args);

    void        createNewScope();
    void        closeScope();
    static int __cdecl compareSwitchLabels(const void *l1, const void *l2);
    void __fastcall AdjustMemberObj(BASENODE * tree, SymWithType swt, EXPR ** pobj, bool * pfConstrained);
    bool __fastcall objectIsLvalue(EXPR * object);
    bool __fastcall hasCorrectType(SYM ** sym, symbmask_t mask);
    EXPRCALL *  BindPatternToMethod(BASENODE *tree, PREDEFNAME id, TYPESYM *type, EXPR *expr, TYPESYM * typeRet, int idsPattern);

    bool        isConstantInRange(EXPRCONSTANT * exprSrc, TYPESYM * typeDest, bool realsOk = false);
    bool        bindConstantCast(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeDest, EXPR ** pexprDest, bool * checkFailure);
    bool        bindSimpleCast(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeDest, EXPR ** pexprDest, unsigned exprFlags = 0);
    bool        bindAnonMethConversion(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeDest, EXPR ** pexprDest, bool fReportErrors = false);
    bool        bindUserDefinedConversion(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeSrc, TYPESYM * typeDest, 
                                          EXPR * * pexprDest, bool implicitOnly);
    int CompareSrcTypesBased(TYPESYM * type1, bool fImplicit1, TYPESYM * type2, bool fImplicit2);
    int CompareDstTypesBased(TYPESYM * type1, bool fImplicit1, TYPESYM * type2, bool fImplicit2);
    bool        bindImplicitConversion(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeSrc, TYPESYM * typeDest, 
                                       EXPR * * pexprDest, unsigned flags);
    bool        bindExplicitConversion(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeSrc, TYPESYM * typeDest, 
                                       EXPR * * pexprDest, unsigned flags);

    bool        canConvert(TYPESYM * src, TYPESYM * dest, unsigned flags = 0);
    bool        canConvert(EXPR * expr, TYPESYM * dest, unsigned flags = 0);
    EXPR *      mustConvertCore(EXPR * expr, TYPESYM * dest, BASENODE * tree, unsigned flags = 0);
    EXPR *      mustConvert(EXPR * expr, TYPESYM * dest, unsigned flags = 0) { return mustConvertCore(expr, dest, expr->tree, flags); }
    EXPR *      tryConvert(EXPR * expr, TYPESYM * dest, unsigned flags = 0);

    bool        canCast(TYPESYM * src, TYPESYM * dest, unsigned flags = 0);
    bool        canCast(EXPR * expr, TYPESYM * dest, unsigned flags = 0);
    EXPR *      mustCastCore(EXPR * expr, TYPESYM * dest, BASENODE * tree, unsigned flags = 0);
    EXPR *      mustCast(EXPR * expr, TYPESYM * dest, unsigned flags = 0) { return mustCastCore(expr, dest, expr->tree, flags); }
    EXPR *      tryCast(EXPR * expr, TYPESYM * dest, BASENODE * tree, unsigned flags = 0);

    // Determine whether this is an implicit/explicit reference conversion from typeSrc to typeDst.
    bool FImpRefConv(TYPESYM * typeSrc, TYPESYM * typeDst);
    bool FExpRefConv(TYPESYM * typeSrc, TYPESYM * typeDst);
    bool FIsSameType(TYPESYM * typeSrc, TYPESYM * typeDst);
    bool FBoxingConv(TYPESYM * typeSrc, TYPESYM * typeDst);
    bool FUnboxingConv(TYPESYM * typeSrc, TYPESYM * typeDst);
    bool FWrappingConv(TYPESYM * typeSrc, TYPESYM * typeDst);
    bool FUnwrappingConv(TYPESYM * typeSrc, TYPESYM * typeDst);

    EXPRBLOCK * bindBlock(BASENODE * tree, int scopeFlags = SF_NONE, SCOPESYM ** scope = NULL, SCOPESYM * scopeExisting = NULL);
    void BindStatement(STATEMENTNODE * sn, StmtListBldr & bldr);
    void BindStatementWorker(STATEMENTNODE *sn, StmtListBldr & bldr);

    EXPR * bindExpr(BASENODE * tree, int bindFlags = BIND_RVALUEREQUIRED); 
    EXPR * bindThisCore(BASENODE * tree, int flags);
    EXPR * bindRealThis(BASENODE * tree, int flags);
    EXPR * bindThisImplicit(BASENODE * tree) { return bindThisCore(tree, EXF_IMPLICITTHIS | EXF_CANTBENULL); }
    EXPR * bindThisExplicit(BASENODE * tree);
    EXPR * badOperatorTypesError(BASENODE * tree, EXPR * op1, EXPR * op2, TYPESYM * typeErr = NULL);
    EXPR * ambiguousOperatorError(BASENODE * tree, EXPR * op1, EXPR * op2);
    void   checkVacuousIntegralCompare(BASENODE * tree, EXPR * castOp, EXPR * constOp);
    EXPR * bindDecimalConstCast(BASENODE * tree, TYPESYM * destType, TYPESYM * srcType, EXPRCONSTANT * src);
    EXPR * bindFloatOp(BASENODE * tree, EXPRKIND kind, unsigned flags, EXPR * op1, EXPR * op2);
    bool   HasSelfCompare(TYPESYM * type, EXPRKIND kind);

    struct BinOpArgInfo {
        EXPR * arg1;
        EXPR * arg2;
        PREDEFTYPE pt1;
        PREDEFTYPE pt2;
        PREDEFTYPE ptRaw1;
        PREDEFTYPE ptRaw2;
        TYPESYM * type1;
        TYPESYM * type2;
        TYPESYM * typeRaw1;
        TYPESYM * typeRaw2;
        BinOpKindEnum bok;
        BinOpMaskEnum grfbom;
    };

    struct NubInfo {
        EXPR * rgexprCnd[2]; // condition for calling the operator
        EXPR * rgexprPre[2]; // assign to temp (or side effects)
        EXPR * rgexprTmp[2]; // load temp (or constant)
        EXPR * rgexprPst[2]; // free temp (or null)
        EXPR * rgexprVal[2]; // underlying value
        EXPR * exprCnd; // Combined condition
        bool rgfNull[2];
        bool fAlwaysNull: 1;
        bool fAlwaysNonNull: 1;
        bool fSameTemp: 1;
        bool fActive: 1;

        NubInfo() { fActive = false; }

        bool FConst(int iexpr) {
            ASSERT(0 <= iexpr && iexpr < 2);
            ASSERT(rgexprTmp[iexpr] &&
                (rgexprTmp[iexpr]->kind == EK_LDTMP ||
                rgexprTmp[iexpr]->kind == EK_CONSTANT ||
                rgexprTmp[iexpr]->kind == EK_ZEROINIT));
            return rgexprTmp[iexpr]->kind != EK_LDTMP;
        }

        EXPR * Val(int iexpr) {
            return rgexprVal[iexpr];
        }
        EXPR * TmpVal(int iexpr) {
            return rgexprTmp[iexpr] ? rgexprTmp[iexpr] : rgexprVal[iexpr];
        }
        EXPR * PreVal(int iexpr) {
            return rgexprPre[iexpr] ? rgexprPre[iexpr] : rgexprVal[iexpr];
        }

        bool FAlwaysNull() {
            ASSERT(fActive);
            ASSERT(!fAlwaysNull ||
                (rgfNull[0] || rgfNull[1]) &&
                !rgexprPst[0] && !rgexprPst[1] &&
                !exprCnd);
            return fAlwaysNull;
        }

        bool FAlwaysNonNull() {
            ASSERT(fActive);
            ASSERT(!fAlwaysNonNull ||
                !rgexprPre[0] && !rgexprPre[1] &&
                !rgexprTmp[0] && !rgexprTmp[1] &&
                !rgexprPst[0] && !rgexprPst[1] &&
                !rgexprCnd[0] && !rgexprCnd[1] &&
                !exprCnd);
            return fAlwaysNonNull;
        }

        void Init() { memset(this, 0, sizeof(NubInfo)); }
    };

    // Binary operator binding.
    EXPR * BindStdBinOp(BASENODE * tree, EXPRKIND ek, EXPR * arg1, EXPR * arg2, bool * pfUserDef = NULL, NubInfo * pnin = NULL);
    EXPR * BindNubEqOpCore(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * expr1, EXPR * expr2, BinOpFullSig & bofs, NubInfo & nin);
    EXPR * BindNubCmpOpCore(BASENODE * tree, EXPRKIND ek, uint flags, BinOpFullSig & bofs, NubInfo & nin);

    EXPR * BindIntBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);
    EXPR * BindRealBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);
    EXPR * BindDecBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);
    EXPR * BindStrBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);
    EXPR * BindShiftOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);
    EXPR * BindBoolBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);
    EXPR * BindBoolBitwiseOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);
    EXPR * BindDelBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);
    EXPR * BindEnumBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);

    EXPR * BindStrCmpOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);
    EXPR * BindRefCmpOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);
    EXPR * BindPtrBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);
    EXPR * BindPtrCmpOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2);

    // Unary operator binding.
    EXPR * BindStdUnaOp(BASENODE * tree, OPERATOR op, EXPR * arg);
    EXPR * BindIntUnaOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg);
    EXPR * BindRealUnaOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg);
    EXPR * BindDecUnaOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg);
    EXPR * BindBoolUnaOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg);
    EXPR * BindEnumUnaOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg);

    EXPR * BindIncOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg, UnaOpFullSig & uofs);

    // Used to bind predefined operators that are implemented as "user-defined" operators.
    EXPR * BindPredefOpAsUD(BASENODE * tree, EXPRKIND ek, EXPR * arg1, EXPR * arg2);

    EXPR * bindUDUnop(BASENODE * tree, EXPRKIND ek, EXPR * op1);
    EXPR * BindUDBinop(BASENODE * tree, EXPRKIND ek, EXPR * op1, EXPR * op2, bool fDontLift = false);
    EXPR * BindUserBoolOp(BASENODE * tree, EXPRKIND kind, EXPR * call);

    // Utility methods for operator overload resolution.
    bool GetBokAndFlags(EXPRKIND ek, BinOpKindEnum * pbok, uint * pflags);
    bool CanConvertArg1(BinOpArgInfo & info, TYPESYM * typeDst, LiftFlagsEnum * pgrflt, TYPESYM ** ptypeSig1, TYPESYM ** ptypeSig2);
    bool CanConvertArg2(BinOpArgInfo & info, TYPESYM * typeDst, LiftFlagsEnum * pgrflt, TYPESYM ** ptypeSig1, TYPESYM ** ptypeSig2);
    void RecordBinOpSigFromArgs(BinOpFullSig * prgbofs, int * pcbofs, BinOpArgInfo & info, PfnBindBinOp pfn);
    bool GetDelBinOpSigs(BinOpFullSig * prgbofs, int * pcbofs, BinOpArgInfo & info);
    bool GetEnumBinOpSigs(BinOpFullSig * prgbofs, int * pcbofs, BinOpArgInfo & info);
    bool GetRefEqualSigs(BinOpFullSig * prgbofs, int * pcbofs, BinOpArgInfo & info);
    bool GetPtrBinOpSigs(BinOpFullSig * prgbofs, int * pcbofs, BinOpArgInfo & info);
    int WhichBofsIsBetter(BinOpFullSig & bofs1, BinOpFullSig & bofs2, TYPESYM * typeArg1, TYPESYM * typeArg2);
    int WhichUofsIsBetter(UnaOpFullSig & uofs1, UnaOpFullSig & uofs2, TYPESYM * typeArg);

    EXPR * BindIntOp(BASENODE * tree, EXPRKIND kind, uint flags, EXPR * op1, EXPR * op2, PREDEFTYPE ptOp);
    EXPR * FoldConstI4Op(BASENODE * tree, EXPRKIND kind, EXPR * op1, EXPR * opConst1, EXPR * op2, EXPR * opConst2, PREDEFTYPE ptOp);
    EXPR * FoldConstI8Op(BASENODE * tree, EXPRKIND kind, EXPR * op1, EXPR * opConst1, EXPR * op2, EXPR * opConst2, PREDEFTYPE ptOp);

    EXPR * BindPtrToArray(BASENODE * tree, EXPRLOCAL * exprLoc, EXPR * array);
    EXPR * bindPtrToString(BASENODE * tree, EXPR * string);
    EXPR * bindAssignment(BASENODE * tree, EXPR * op1, EXPR * op2, bool allowExplicit = false);
    bool   localDeclaredOutsideTry(LOCVARSYM * local);
    EXPR * bindArrayIndex(BINOPNODE * tree, int bindFlags = BIND_RVALUEREQUIRED);
    TYPESYM * chooseArrayIndexType(BASENODE * tree, EXPR * args);
    EXPR * bindBinop(BINOPNODE * tree, int bindFlags = BIND_RVALUEREQUIRED);
    EXPR * bindCall(CALLNODE * tree, int bindFlags);
    EXPR * bindDot(BINOPNODE * tree, symbmask_t mask, int bindFlags = BIND_RVALUEREQUIRED);
    EXPR * bindAliasName(NAMENODE * tree);
    EXPR * bindEventAccess(BASENODE * tree, bool isAdd, EXPREVENT * exprEvent, EXPR * exprRHS);
    EXPR * BindToField(BASENODE * tree, EXPR * object, FieldWithType fwt, int bindFlags = BIND_RVALUEREQUIRED);
    EXPR * BindToProperty(BASENODE * tree, EXPR * object, PropWithType pwt, int bindFlags = BIND_RVALUEREQUIRED, EXPR *args = NULL, AGGTYPESYM * typeOther = NULL);
    EXPR * BindToMethod(BASENODE * tree, EXPR *object, MethWithInst mwi, EXPR *args, MemLookFlagsEnum flags = MemLookFlags::None);
    EXPR * BindToEvent(BASENODE * tree, EXPR * object, EventWithType ewt, int bindFlags = BIND_RVALUEREQUIRED);
    EXPR * bindToLocal(BASENODE * tree, LOCVARSYM * local, int bindFlags);
    AGGTYPESYM * CheckForTypeSameName(NAMENODE * node, SYM * sym, NameCacheFlagsEnum flags); // For handling the weird E.M case.
    bool BindNameToSym(SymWithType * pswt, TypeArray ** ptypeArgs, TypeArray ** ptypes, AGGTYPESYM ** ptypeOther, NAMENODE *name, symbmask_t mask, int bindFlags);
    EXPR * bindName(NAMENODE * name, symbmask_t mask = LOOKUPMASK, int bindFlags = BIND_RVALUEREQUIRED);
    bool   bindUnevaledConstantField(MEMBVARSYM * field);
    EXPR * bindUnop(UNOPNODE * tree, int bindFlags = BIND_RVALUEREQUIRED);
    EXPR * bindTypeOf(UNOPNODE * tree);
    bool   fixupTypeOfExpr(BASENODE * tree, EXPRTYPEOF * expr);
    EXPR * bindDefault(UNOPNODE * tree);
    EXPR * bindIs(BINOPNODE * tree, EXPR * op1);
    EXPR * bindAs(BINOPNODE * tree, EXPR * op1);
    void BindVarDecls(DECLSTMTNODE * tree, StmtListBldr & bldr);
    void BindFixedDecls(FORSTMTNODE * tree, StmtListBldr & bldr);
    void BindUsingDecls(FORSTMTNODE * tree, StmtListBldr & bldr);
    void BindUsingDeclsCore(FORSTMTNODE * tree, EXPR * exprFirst, StmtListBldr & bldr);
    EXPRDECL * bindVarDecl(VARDECLNODE * tree, TYPESYM * type, unsigned flags);
    void ReportTypeArgsNotAllowedError(BASENODE * node, int arity, ErrArgRef argName, ErrArgRef argKind);

    SYM  * lookupCachedName(NAMENODE * name, symbmask_t mask, TypeArray ** ptypes, NameCacheFlagsEnum * pflags);
    void   storeInCache(BASENODE * tree, NAME * name, SYM * sym, TypeArray * types, bool checkForParent = false);
    void   storeFlagsInCache(NAME * name, NameCacheFlagsEnum flags);

    void BindReturn(EXPRSTMTNODE * tree, StmtListBldr & bldr);
    void BindYield(EXPRSTMTNODE * tree, StmtListBldr & bldr);
    void BindIf(IFSTMTNODE * tree, StmtListBldr & bldr);
    void BindWhileOrDo(LOOPSTMTNODE * tree, bool asWhile, StmtListBldr & bldr);
    void BindFor(FORSTMTNODE * tree, StmtListBldr & bldr);
    void BindBreakOrContinue(BASENODE * tree, bool asBreak, StmtListBldr & bldr);
    void BindGoto(EXPRSTMTNODE * tree, StmtListBldr & bldr);
    void BindLabel(LABELSTMTNODE * tree, StmtListBldr & bldr);
    void BindChecked(LABELSTMTNODE * tree, StmtListBldr & bldr);
    void BindUnsafe(LABELSTMTNODE * tree, StmtListBldr & bldr);
    EXPR * bindCheckedExpr(UNOPNODE * tree, int bindFlags);
    void BindInitializers(bool isStatic, StmtListBldr & bldr);
    EXPRSTMT * createBaseConstructorCall(bool isThisCall);
    EXPR * bindConstructor(METHINFO * info);
    EXPR * bindDestructor(METHINFO * info);
    EXPRBLOCK *bindMethOrPropBody(BLOCKNODE *body);
    EXPR * bindMethod(METHINFO * info);
    EXPR * BindPropertyAccessor(METHINFO * info);
    EXPR * bindEventAccessor(METHINFO * info);
    EXPR * bindIfaceImpl(METHINFO *info);
    EXPR * bindNew(NEWNODE * tree, bool stmtExprOnly);
    EXPR * bindNewTyVar(NEWNODE * tree, TYVARSYM * var);
    void   checkNegativeConstant(BASENODE * tree, EXPR * expr, int id);
    EXPR * bindArrayNew(ARRAYSYM * type, NEWNODE * tree);
    METHSYM * FindDelegateCtor(AGGTYPESYM * type, BASENODE * tree, bool fReportErrors = true);
    EXPR * bindDelegateNew(AGGTYPESYM * type, NEWNODE * tree);
    EXPR * bindAnonymousMethod(ANONBLOCKNODE * tree);
    EXPR * BindAnonymousMethodInner(ANONBLOCKNODE * tree, AnonMethInfo * pInfo);
    void BindSwitch(SWITCHSTMTNODE * tree, StmtListBldr & bldr);
    void initForHashtableSwitch(BASENODE * tree, EXPRSWITCH * expr);
    void initForNonHashtableSwitch(BASENODE * tree);
    EXPRCONSTANT * VerifySwitchLabel(BASENODE * tree, TYPESYM * type, bool fGoto);
    NAME * getSwitchLabelName(EXPRCONSTANT * expr);
    NAME * GenerateSwitchLabelName(EXPRCONSTANT * expr);
    void BindTry(TRYSTMTNODE * tree, StmtListBldr & bldr);
    void BindThrow(EXPRSTMTNODE * tree, StmtListBldr & bldr);
    EXPR * bindMultiOp(BASENODE * tree, EXPRKIND op, EXPR * op1, EXPR * op2);
    bool isCastOptThis(EXPR * search);
    bool isFixedExpression(EXPR * expr, LOCVARSYM ** psymLocal);
    bool isAddressable(EXPR * expr);
    bool checkLvalue(EXPR * expr, bool isAssignment = true);
    void CheckFieldRef(EXPR * expr);
    bool CheckFieldUse(EXPR * expr, bool fReport = true);
    void checkUnsafe(BASENODE * tree, TYPESYM * type = NULL, int errCode = ERR_UnsafeNeeded, ErrArg *pArg = NULL);
    void markFieldAssigned(EXPR * expr);
    EXPR * bindQMark(BASENODE * tree, EXPR * op1, EXPRBINOP * op2);
    EXPR * bindStringConcat(BASENODE * tree, EXPR * op1, EXPR * op2);
    EXPR * bindNull(BASENODE * tree);
    EXPR * bindMethodName(BASENODE * tree);
    EXPR * bindArrayInit(UNOPNODE * tree, ARRAYSYM * type, EXPR * args);
    void   bindArrayInitList(UNOPNODE * tree, TYPESYM * elemType, int rank, int * size, EXPR *** ppList, int * totCount, int * constCount, bool * pfSideEffects);
    EXPR * bindPossibleArrayInit(BASENODE * tree, TYPESYM * type, int bindFlags = 0);
    EXPR * bindPossibleArrayInitAssg(BINOPNODE * tree, TYPESYM * type, int bindFlags = 0);
    EXPR * bindConstInitializer(MEMBVARSYM * pAFSym, BASENODE * tree);
    void BindLock(LOOPSTMTNODE * tree, StmtListBldr & bldr);
    void OnLockOrDisposeEnter(EXPR * arg);
    void OnLockOrDisposeExit(EXPR * arg);
    void OnPossibleAssignmentToArg(BASENODE * tree, EXPR * arg);
    EXPR * bindIndexer(BASENODE * tree, EXPR * object, EXPR * args, int bindFlags);
    EXPR * bindBase(BASENODE * tree);
    TypeArray * GetExpandedParams(TypeArray * params, int count);
    void noteReference(EXPR * op);
    EXPR * bindAttributeValue(BASENODE * tree);
    EXPR * bindNamedAttributeValue(AGGTYPESYM *attributeType, ATTRNODE * tree);
    EXPR * bindAttrArgs(AGGTYPESYM *attributeType, ATTRNODE * tree, EXPR **namedArgs);
    EXPR * bindMakeRefAny(BASENODE * tree, EXPR * op);
    EXPR * bindRefType(BASENODE * tree, EXPR * op);
    EXPR * bindRefValue(BINOPNODE * tree);
    EXPR * bindPtrIndirection(UNOPNODE * tree, EXPR * op);
    EXPR * bindPtrAddr(UNOPNODE * tree, EXPR * op, int bindFlags);
    EXPR * bindLocAlloc(NEWNODE * tree, PTRSYM * type);
    EXPR * bindPtrArrayIndexer(BINOPNODE * tree, EXPR * opArr, EXPR * opIndex);
    EXPR * bindPtrMul(BASENODE * tree, EXPR * times, EXPR * size);
    EXPR * bindPtrAddMul(BASENODE * tree, EXPR * ptr, EXPR * times, EXPR * size, bool ptrFirst = true, EXPRKIND kind = EK_ADD);
    EXPR * bindCastToNatural(EXPR * original);
    EXPR * bindSizeOf(BASENODE * tree, TYPESYM * type);
    EXPR * bindSizeOf(UNOPNODE * tree);
    void BindForEach(FORSTMTNODE * tree, StmtListBldr & bldr);
    AGGTYPESYM * HasIEnumerable(TYPESYM * collection, BASENODE * tree, TYPESYM * badType, PREDEFNAME badMember);
    void BindForEachInner(FORSTMTNODE * tree, EXPR ** enumerator, StmtListBldr & bldrInit, StmtListBldr & bldr);
    void BindForEachInnerCore(FORSTMTNODE * tree, EXPR ** enumerator, StmtListBldr & bldrInit, StmtListBldr & bldr);
    void BindForEachArray(FORSTMTNODE * tree, EXPR * array, StmtListBldr & bldr);
    void BindForEachString(FORSTMTNODE * tree, EXPR * string, StmtListBldr & bldr);
    bool BindForEachFinally(BASENODE *tree, EXPR * enumerator, StmtListBldr & bldr, bool *pfRemoveFinally);
    EXPR * bindBooleanValue(BASENODE * tree, EXPR * op);
    EXPR * BindValOrDefOp(BASENODE * tree, EXPR * arg1, EXPR * arg2);

    void BindNubCondValBin(BASENODE * tree, EXPR * expr1, EXPR * expr2, NubInfo & nin, LiftFlagsEnum grflt = LiftFlags::LiftBoth);
    EXPR * BindNubOpRes(BASENODE * tree, NUBSYM * nubNull, TYPESYM * typeDst, EXPR * exprVal, NubInfo & nin, bool fWarnOnNull = false);
    EXPR * BindNubOpRes(BASENODE * tree, TYPESYM * typeDst, EXPR * exprVal, EXPR * exprNull, NubInfo & nin);
    EXPR * BindNubAddTmps(BASENODE * tree, EXPR * exprRes, NubInfo & nin);

    bool BindNubSave(EXPR * exprSrc, NubInfo & nin, int iexpr, bool fLift);
    EXPR * BindNubFetchAndFree(BASENODE * tree, NubInfo & nin, int iexpr);

    EXPR * AddSideEffects(BASENODE * tree, EXPR * exprBase, EXPR * exprSide, bool fPre, bool fForceNonConst = false);
    EXPR * EnsureNonConstNonLvalue(BASENODE * tree, EXPR * exprBase);
    EXPR * BindNubConstBool(BASENODE * tree, bool fT);
    EXPR * BindQmark(BASENODE * tree, EXPR * exprCond, EXPR * exprLeft, EXPR * exprRight, bool fInvert = false);

    PROPSYM * EnsureNubHasValue(BASENODE * tree);
    METHSYM * EnsureNubGetValOrDef(BASENODE * tree);

    EXPR * BindNubHasValue(BASENODE * tree, EXPR * exprSrc, bool fCheckTrue = true);
    EXPR * BindNubValue(BASENODE * tree, EXPR * exprSrc);
    EXPR * BindNubGetValOrDef(BASENODE * tree, EXPR * exprSrc);
    EXPR * BindNubNew(BASENODE * tree, EXPR * exprSrc);
    EXPR * StripNubCtor(EXPR * expr);
    bool IsNubCtor(METHSYM * meth);
    bool IsNubCtor(EXPR * expr);
    bool BindNubConversion(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeSrc, NUBSYM * nubDst, EXPR ** pexprDst, uint flags);

    void ErrorBadSK(BASENODE * tree, SymWithType swt, symbmask_t mask, int bindFlags = 0);
    void ReportDeprecated(BASENODE * tree, SymWithType swt);

    AGGTYPESYM * GetReqPDT(PREDEFTYPE pt);
    AGGTYPESYM * GetOptPDT(PREDEFTYPE pt);
    AGGSYM * getPDO();
    AGGTYPESYM * getPDOT();
    TYPESYM * getVoidType();

public:
    // Used by CLSDREC::MakeIterator
    NAME * CreateSpecialName(SpecialNameKindEnum snk, PCWSTR szName = NULL);
    static SpecialNameKindEnum IsSpecialName(NAME * name);
    NAME * GetDisplayNameOfPossibleSpecialName(NAME * specialName);

private:
    NAME * CreateName(PCWSTR szTemplate, ...);

    //////////////////////////////////////////////////////////////////////////////////////////////
    // Base class for rewrite info. Used to store useful EXPRs and SYMs for rewriting an EXPR tree
    // and tracking state
    class RewriteInfo { 
    public:
        virtual bool RewriteFunc(FUNCBREC * funcBRec, EXPR ** expr) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////
    // Used to store useful EXPRs and SYMs for rewriting an EXPR tree in an iterator
    // and track state
    class IteratorRewriteInfo : public RewriteInfo {
    public:
        AGGSYM *    aggInnerClass;      // The compiler generated class
        EXPR *      exprLocalThis;      // EXPR for local access (really is this access on aggInnerClass)
        EXPR *      exprState;          // EXPR for aggInnerClass.$__state
        EXPR *      exprCurrent;        // EXPR for aggInnerClass.$__current
        EXPRLABEL * exprLabelFailRet;   // EXPRLABEL for the 'return false' of an iterator
        int         iNextState;         // The current state counter
        int         iFinallyState;      // The current innermost finally state

        bool RewriteFunc(FUNCBREC * funcBRec, EXPR ** expr);
    };
    friend class IteratorRewriteInfo;

    //////////////////////////////////////////////////////////////////////////////////////////////
    // Used to store useful EXPRs and SYMs for rewriting an EXPR tree in an anonymous method
    // and track state
    class AnonymousMethodRewriteInfo : public RewriteInfo {
    public:
        METHSYM *   pAMSym;            // The method that we are currently processing.
        SCOPESYM *  pArgScope;         // The scope that contains the arguments (including <this>) for the current method.
        AnonMethInfo * pamiFirst;      // The list of anon meth infos in pAMSym.

        AnonymousMethodRewriteInfo(METHSYM * meth, SCOPESYM * scope, AnonMethInfo * pamiFirst) {
            pAMSym = meth;
            pArgScope = scope;
            this->pamiFirst = pamiFirst;
        }

        bool RewriteFunc(FUNCBREC * funcBRec, EXPR ** expr);
    };
    friend class AnonymousMethodRewriteInfo;

    void RecurseAndRewriteExprTree(EXPR ** pexpr, RewriteInfo * rwInfo);
    bool RewriteIteratorFunc(EXPR ** /* in, out */ pexpr, IteratorRewriteInfo * irwInfo);
    bool RewriteAnonymousMethodFunc(EXPR ** /* in, out */ pexpr, AnonymousMethodRewriteInfo * amrwInfo);

    MEMBVARSYM * MoveLocalToField(LOCVARSYM * local, AGGSYM * aggDest, TypeArray * pSubstTypeVars, bool bAllowDupNames = true, NAME * name = NULL);
    MEMBVARSYM * MoveLocalToField(LOCVARSYM * local, EXPR * expr, TypeArray * pSubstTypeVars, bool bAllowDupNames = true)
        { return MoveLocalToField(local, expr->type->asAGGTYPESYM()->getAggregate(), pSubstTypeVars, bAllowDupNames); }
    int MoveLocalsToIter(SCOPESYM * pScope, AGGSYM * aggIter, int cIteratorLocals);
    void CorrectAnonMethScope(SCOPESYM* pTrueOutermostScope);
    bool SubstAndTestLocalUsedInAnon(SCOPESYM* pScope);
    EXPR * RewriteLocalsBlock(EXPRBLOCK * q, AnonymousMethodRewriteInfo * amrwInfo);
    EXPR * rewriteAnonMethExpr(EXPRANONMETH * q);
    void InitAnonDelegateLocals(SCOPESYM * pArgScope, StmtListBldr & bldr, TypeArray * amClsTypeVars);
    void AddAnonMethInfo(AnonMethInfo ** ppamiHead, AnonMethInfo * pami);
    EXPRSWITCH * MakeIteratorSwitch(SCOPESYM *pScope, IteratorRewriteInfo * irwInfo, EXPRLABEL * pKey, EXPRLABEL * pNoKey);
    EXPRSWITCHLABEL * MakeSwitchLabel(EXPRCONSTANT * pKey, SCOPESYM * pScope);
    // ExprFactory routines copied from MSR (todd proebstring et.al.)
    EXPR *   MakeFieldAccess(EXPR * pObject, NAME * pName, int flags = 0);
    EXPR *   MakeFieldAccess(EXPR * pObject, MEMBVARSYM * pSym, int flags = 0);
    EXPRCONSTANT *   MakeIntConst(int x) { return newExprConstant(NULL, GetReqPDT(PT_INT), ConstValInit(x)); }
    EXPRCONSTANT *   MakeBoolConst(bool b) { return newExprConstant(NULL, GetReqPDT(PT_BOOL), ConstValInit(b)); }
    EXPRRETURN * MakeReturn(SCOPESYM* pCurrentScope, EXPR* pValue, int flags = 0);
    EXPRLABEL * MakeFreshLabel();
    EXPRSTMT * MakeAssignment(BASENODE * tree, EXPR* exprLeft, EXPR* exprRight);
    EXPRSTMT * MakeAssignment(EXPR* exprLeft, EXPR* exprRight) { return MakeAssignment(NULL, exprLeft, exprRight); }
    EXPRLOCAL * MakeLocal(BASENODE * tree, LOCVARSYM * loc, bool fLVal);
    EXPRGOTO * MakeGoto(BASENODE * tree, EXPRLABEL* pLabel, int flags = 0);
    EXPRGOTOIF * MakeGotoIf(BASENODE * tree, EXPR * exprCond, EXPRLABEL * lab, bool fSense, int flags = 0);
    // End of ExprFactory routines

    void CheckForNonvirtualAccessFromClosure(EXPR* expr);
    void PostBindCompile(EXPRBLOCK * block);
    void CheckForIteratorErrors();
    void ReverseAnonMeths(AnonMethInfo ** ppami);
    void ScanAnonMeths(AnonMethInfo * pami, BitSet & bsetCur, BitSet & bsetErr);
    void ReportUnreferencedVars(SCOPESYM * scope);

    PCWSTR opName(OPERATOR op);
    NAME * ekName(EXPRKIND ek);

    COMPILER * compiler();

    static const EXPRKIND OP2EK[OP_LAST];
    static const PREDEFNAME EK2NAME[EK_ARRLEN - EK_FIRSTOP];
    static const BOOL FUNCBREC::opCanBeStatement[OP_LAST];
};


class ReachabilityChecker {
public:
    ReachabilityChecker(COMPILER * comp) {
        m_comp = comp;
    }
    void SetReachability(EXPRBLOCK * block, bool fReportWarnings);
    bool RealizeGoto(EXPRGOTO * gt, bool fFull);

    // This is valid after a call to SetReachability.
    bool HasRetAsLeave() { return m_fHasRetAsLeave; }

    static void DumpStmts(EXPRSTMT * stmtFirst, int ctab);

private:
    COMPILER * m_comp;
    bool m_fHasRetAsLeave;

    COMPILER * compiler() { return m_comp; }

    void SetParents(EXPRSTMT * stmtPar, EXPRSTMT * stmtFirst);
    void MarkReachable(EXPRSTMT * stmt);
    void MarkReachableInner(EXPRSTMT * stmt, EXPRLABEL ** plabTop);
    bool MarkBlock(EXPRBLOCK * block);
    void ReportUnreachable(EXPRSTMT * stmt, bool * pfReachable);
};


class FlowChecker {
public:
    static void CheckDefiniteAssignment(COMPILER * comp, ICompileCallback * pccb, NRHEAP * heap,
        EXPRBLOCK * block, METHINFO * info, AnonMethInfo * pamiFirst, int cvarUninit)
    {
        FlowChecker flow;
        flow.m_comp = comp;
        flow.m_pccb = pccb;
        flow.m_heap = heap;
        flow.m_info = info;
        flow.m_cvarUninit = cvarUninit;
        flow.ScanAll(block, pamiFirst);
    }

    // Get the number of bits needed for definite assignment checking.
    static int GetCbit(COMPILER * comp, TYPESYM * type);
    static int GetIbit(COMPILER * comp, MEMBVARSYM * fld, AGGTYPESYM * ats);
    static int GetIbit(COMPILER * comp, FieldWithType fwt) { return GetIbit(comp, fwt.Field(), fwt.Type()); }

private:
    COMPILER * m_comp;
    ICompileCallback * m_pccb;
    NRHEAP * m_heap;
    int m_cvarUninit;
    METHINFO * m_info;

    // Set before scanning each (anon or not) method body.
    AnonMethInfo * m_pami;
    int m_tsFinallyScanCur; // 0 when not in a finally scan.
    int m_tsFinallyScanPrev;
    LOCVARSYM * m_locThis;
    SYMLIST * m_listOutParams;

    FlowChecker() { }
    FlowChecker(FlowChecker & flow) { VSFAIL("No Copy ctor"); }

    COMPILER * compiler() { return m_comp; }

    void ScanAll(EXPRBLOCK * block, AnonMethInfo * pamiFirst);
    void ScanAnonMeths(AnonMethInfo * pami, BitSet & bsetCur, BitSet & bsetErr);
    void ReportReturnNeeded(EXPRBLOCK * block, BASENODE * tree, BitSet & bsetCur);
    void CheckOutParams(BASENODE * tree, BitSet & bsetCur);
    void BuildOutParamList(SCOPESYM * scopeParams);

    void ScanStmts(EXPRSTMT * stmt, BitSet & bsetCur, BitSet & bsetErr);
    void ScanStmtsInner(EXPRSTMT * stmt, EXPRLABEL ** plabTop, BitSet & bsetCur, BitSet & bsetErr);
    void ScanEndOfChain(EXPRSTMT * stmt, BitSet & bsetCur);
    bool ScanBlock(EXPRBLOCK * block, BitSet & bsetCur, BitSet & bsetErr);
    bool ScanGoto(EXPRGOTO * gt, BitSet & bsetCur, BitSet & bsetErr);
    bool ScanGotoIf(EXPRGOTOIF * gtif, EXPRLABEL ** plabTop, BitSet & bsetCur, BitSet & bsetErr);
    bool ScanThroughFinallys(EXPRSTMT * stmt, BitSet & bsetCur, BitSet & bsetErr);
    bool ScanLabel(EXPRLABEL * lab, BitSet & bsetCur);
    void ScanTry(EXPRTRY * tr, BitSet & bsetCur, BitSet & bsetErr);
    void ScanSwitch(EXPRSWITCH * sw, BitSet & bsetCur, BitSet & bsetErr);

    void ScanExpr(EXPR * expr, BitSet & bsetCur, BitSet & bsetErr);
    void ScanExpr(EXPR * expr, BitSet & bsetCur, BitSet & bsetErr, BitSet * pbsetTrue, BitSet * pbsetFalse, bool fSense);
    void ScanExprCond(EXPR * expr, BitSet & bsetCur, BitSet & bsetErr);
    void ScanLocal(EXPRLOCAL * loc, BitSet & bsetCur, BitSet & bsetErr);
    bool ScanField(EXPRFIELD * exprFld, BitSet & bsetCur, BitSet & bsetErr);
    void ScanAssign(EXPR * expr, EXPR * val, BitSet & bsetCur, BitSet & bsetErr, bool fValUsed);
    bool MarkAsUsed(EXPR * expr, bool fValUsed);
    void MarkAsAlias(EXPR * expr);

    int GetCbit(TYPESYM * type) { return GetCbit(compiler(), type); }
    int GetIbit(MEMBVARSYM * fld, AGGTYPESYM * ats) { return GetIbit(compiler(), fld, ats); }
    int GetIbit(FieldWithType fwt) { return GetIbit(compiler(), fwt.Field(), fwt.Type()); }
};


//////////////////////////////////////////////////////////////////////////////////////////////

#define NODELOOP(init, child, var) \
{\
    BASENODE * _nd = init; \
    while (_nd) { \
        child ## NODE * var; \
        if (_nd->kind == NK_LIST) { \
            var = _nd->asLIST()->p1->as ## child (); \
            _nd = _nd->asLIST()->p2; \
        } else { \
            var = _nd->as ## child (); \
            _nd = NULL; \
        }

//////////////////////////////////////////////////////////////////////////////////////////////

#define EXPRLOOP(init, var) \
{\
    EXPR * _nd = init; \
    while (_nd) { \
        EXPR * var; \
        if (_nd->kind == EK_LIST) { \
            var = _nd->asBIN()->p1; \
            _nd = _nd->asBIN()->p2; \
        } else { \
            var = _nd; \
            _nd = NULL; \
        } 

//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////

#define STMTLOOP(init, var) \
{\
    EXPRSTMT * _nd = init; \
    while (_nd) { \
        EXPRSTMT * var = _nd; \
        _nd = _nd->stmtNext;

//////////////////////////////////////////////////////////////////////////////////////////////

#define ENDLOOP  } }


//////////////////////////////////////////////////////////////////////////////////////////////
// Inline functions shared between metaattr.cpp and fncbind.cpp

inline CHECKEDCONTEXT::CHECKEDCONTEXT(FUNCBREC * binder, bool checked)
{
    normal = binder->checked.normal;
    constant = binder->checked.constant;

    binder->checked.normal = binder->checked.constant = checked;
}

inline void CHECKEDCONTEXT::restore(FUNCBREC * binder)
{
    binder->checked.normal = normal;
    binder->checked.constant = constant;
}

// constructor for the struct which holds the current break and continue labels...
inline LOOPLABELS::LOOPLABELS(FUNCBREC * binder)
{
    breakLabel = binder->newExprLabel();
    contLabel = binder->newExprLabel();
    binder->loopLabels = this;
}

//////////////////////////////////////////////////////////////////////////////////////////////

#endif // __fncbind_h__
