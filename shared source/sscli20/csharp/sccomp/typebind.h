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
// File: typebind.h
//
// Defines the structure used when binding types
// ===========================================================================

#ifndef __typebind_h__
#define __typebind_h__

// Type bind flags.
namespace TypeBindFlags { enum _Enum {
    None = 0x00,
    CallPreHook         = 0x01, // Call PreLookup before doing normal lookup - used for XML references.
    AllowInaccessible   = 0x02, // If an accessible item isn't found, use an inaccessible one instead - used for XML references.
    NoDeprecated        = 0x04, // Don't check for deprecated.
    SuppressErrors      = 0x08, // Suppress errors. This may cause NULL to be returned when non-NULL (with errors) would have been returned.
    AllowMissing        = 0x10, // If the lookout failed because the name was missing, just return NULL (don't error).
    NoBogusCheck        = 0x20, // Don't check for bogus when deciding accessibility.
    AvoidEnsureState    = 0x40, // Avoid calling EnsureState at all.
}; };
DECLARE_FLAGS_TYPE(TypeBindFlags)


// CheckConstraints options.
namespace CheckConstraintsFlags { enum _Enum {
    None = 0x00,
    Outer = 0x01,
    NoDupErrors = 0x02,
    NoErrors = 0x04,
}; };
DECLARE_FLAGS_TYPE(CheckConstraintsFlags)


/******************************************************************************
    TypeBind class.

    TypeBind has static methods to accomplish most tasks.

    For some of these tasks there are also instance methods. The instance
    method versions don't report not found errors (they may report others) but
    instead record error information in the TypeBind instance. Call the
    ReportErrors method to report recorded errors.
******************************************************************************/
class TypeBind
{
public:
    TypeBind() { DebugOnly(m_fValid = false); }

    /***** Instance Methods *****/

    // SearchNamespacesInst searches the namespaces starting at decl. If decl is an AGGDECLSYM, the search starts at the
    // NSDECLSYM containing decl.
    SYM * SearchNamespacesInst(COMPILER * cmp, BASENODE * node, NAME * name, TypeArray * typeArgs, DECLSYM * decl, SYM * symAccess,
        TypeBindFlagsEnum flags = TypeBindFlags::None);

    bool HasIntelligentErrorInfo() {
        ASSERT(m_fValid);
        return m_symInaccess || m_symBogus || m_symBadKind || m_symBadArity;
    }
    void ReportErrors(BASENODE * node, NAME * name, SYM * symLeft, TypeArray * typeArgs, SYM ** psym);

    /***** Static Methods *****/

    // BindName resolves an NK_DOT, NK_NAME, NK_GENERICNAME or NK_ALIASNAME to a NSAIDSYM or AGGTYPESYM.
    // symCtx is used for accessibility checking and the place to search.
    static SYM * BindName(COMPILER * cmp, BASENODE * node, SYM * symCtx, TypeBindFlagsEnum flags = TypeBindFlags::None);

    // BindNameToType calls BindName then errors (if not TypeBindFlags::SuppressErrors) and returns NULL if the result is not a type.
    // symCtx is used for accessibility checking and the place to search.
    static TYPESYM * BindNameToType(COMPILER * cmp, BASENODE * node, SYM * symCtx, TypeBindFlagsEnum flags = TypeBindFlags::None);

    // BindType resolves a TYPEBASENODE to a type. symCtx is used for accessibility checking and the place to search.
    // symCtx is used for accessibility checking and the place to search.
    static TYPESYM * BindType(COMPILER * cmp, TYPEBASENODE * node, SYM * symCtx, TypeBindFlagsEnum flags = TypeBindFlags::None);

    // BindTypeAggDeclExt resolves a TYPEBASENODE to a type, searching only the "exterior" of a particular AGGDECLSYM. This means, type variables
    // of the AGG are considered, but members are not. The AGGSYM is use for accessibility. The search starts with the type variables and
    // continues to the enclosing types and namespaces.
    static TYPESYM * BindTypeAggDeclExt(COMPILER * cmp, TYPEBASENODE * node, AGGDECLSYM * ads, TypeBindFlagsEnum flags = TypeBindFlags::None);

    // BindTypeWithTypeVars first looks in the type variables of symTypeVars, then looks in symStart and its base and outer
    // contexts.
    static TYPESYM * BindTypeWithTypeVars(COMPILER * cmp, TYPEBASENODE * node, SYM * symStart, SYM * symAccess, SYM * symTypeVars,
        TypeBindFlagsEnum flags = TypeBindFlags::None);

    // BindTypeArgs resolves type arguments for a generic name. It accepts ANYNAME.
    static TypeArray * BindTypeArgs(COMPILER * cmp, NAMENODE * node, SYM * symCtx, TypeBindFlagsEnum flags = TypeBindFlags::None);

    // Looks for an attribute type.
    static TYPESYM * BindAttributeType(COMPILER * cmp, BASENODE * node, SYM * symCtx, TypeBindFlagsEnum flags = TypeBindFlags::None);

    // Gets the SYM associated with a using alias.
    static SYM * BindUsingAlias(COMPILER * cmp, ALIASSYM * alias);

    // Checks deprecated, bogus, constraints, etc.
    static void CheckType(COMPILER * cmp, BASENODE * node, TYPESYM * type, SYM * symCtx, TypeBindFlagsEnum flags = TypeBindFlags::None);

    // SearchNamespaces looks for the given identifier in the namespace declaration and any enclosing declarations. Using clauses are honored.
    // symAccess is used for accessibility checking. The typeArgs specify the arity (and are applied to any found type). node is just used
    // for error reporting.
    static SYM * SearchNamespaces(COMPILER * cmp, BASENODE * node, NAME * name, TypeArray * typeArgs, NSDECLSYM * nsd, SYM * symAccess,
        TypeBindFlagsEnum flags = TypeBindFlags::None);

    // SearchSingleNamespace looks for the given identifier in the namespace. symAccess is used for accessibility checking. The typeArgs specify
    // the arity (and are applied to any found type).
    static SYM * SearchSingleNamespace(COMPILER * cmp, BASENODE * node, NAME * name, TypeArray * typeArgs, NSAIDSYM * nsa, SYM * symAccess,
        TypeBindFlagsEnum flags = TypeBindFlags::None);

    // SearchNamespacesForAlias looks for an alias with the given name in the namespace declaration and any enclosing declarations.
    static SYM * SearchNamespacesForAlias(COMPILER * cmp, BASENODE * node, NAME * name, NSDECLSYM * nsd, TypeBindFlagsEnum flags = TypeBindFlags::None);

    static bool CheckConstraints(COMPILER * cmp, BASENODE * tree, TYPESYM * type, CheckConstraintsFlagsEnum flags);
    static void CheckMethConstraints(COMPILER * cmp, BASENODE * tree, MethWithInst mwi);

protected:
    // Outputs for error reporting. These are relevant only if the symbol wasn't found.
    SYM * m_symInaccess;
    SYM * m_symBogus;
    SYM * m_symBadKind;
    SYM * m_symBadArity;

    // Inputs.
    COMPILER * m_cmp;
    SYM * m_symCtx; // Context for error messages.
    SYM * m_symStart; // Lookup on a single name starts here (unless alias qualified).
    SYM * m_symAccess; // Accessibility is checked from here.
    SYM * m_symTypeVars; // Type variables belong to this SYM are checked first.
    TypeBindFlagsEnum m_flags : 16;
    bool m_fUsingAlias;
    DebugOnly( bool m_fValid; )

    COMPILER * compiler() { ASSERT(m_fValid); return m_cmp; }

    TypeBind(COMPILER * cmp, SYM * symCtx, SYM * symStart, SYM * symAccess, SYM * symTypeVars, TypeBindFlagsEnum flags) {
        Init(cmp, symCtx, symStart, symAccess, symTypeVars, flags);
    }
    void Init(COMPILER * cmp, SYM * symCtx, SYM * symStart, SYM * symAccess, SYM * symTypeVars, TypeBindFlagsEnum flags);

    void ClearErrorInfo() {
        // Outputs for error reporting.
        m_symInaccess = NULL;
        m_symBogus = NULL;
        m_symBadKind = NULL;
        m_symBadArity = NULL;
    }

    bool FSuppressErrors() { return (m_flags & TypeBindFlags::SuppressErrors) != 0; }
    bool FAllowMissing() { return (m_flags & TypeBindFlags::AllowMissing) != 0; }

    // These update the error info and modify *psym.
    void CheckAccess(SYM ** psym);
    void CheckArity(SYM ** psym, TypeArray * typeArgs, AGGTYPESYM * typeOuter);

    // These are instance methods and call ClearErrorInfo and ReportErrors as appropriate.
    SYM * BindNameCore(BASENODE * node);
    SYM * BindNameCore(BASENODE * node, NAME * name);
    TYPESYM * BindNameToTypeCore(BASENODE * node);
    SYM * BindSingleNameCore(NAMENODE * node, NAME * name, TypeArray * typeArgs = NULL);
    SYM * BindAliasNameCore(NAMENODE * node);
    TypeArray * BindTypeArgsCore(NAMENODE * node);
    SYM * BindDottedNameCore(BINOPNODE * node, NAME * name);
    SYM * BindRightSideCore(SYM * symLeft, BASENODE * nodePar, NAMENODE * nodeName, NAME * name, TypeArray * typeArgs = NULL);
    TYPESYM * BindTypeCore(TYPEBASENODE * node);
    TYPESYM * BindAttributeTypeCore(BASENODE * node);

    // These are instance methods to perform the indicated search. They don't call ClearErrorInfo or
    // ReportErrors. They update the error info (m_symInaccess, etc).
    SYM * SearchClassCore(AGGTYPESYM * ats, NAME * name, TypeArray * typeArgs, bool fOuterAndTypeVars);
    TYVARSYM * SearchTypeVarsCore(SYM * symOwner, NAME * name, TypeArray * typeArgs);
    SYM * SearchSingleNamespaceCore(NSAIDSYM * nsa, BASENODE * node, NAME * name, TypeArray * typeArgs, bool fAggOnly = false);
    SYM * SearchSingleNamespaceCore(NSSYM * ns, int aid, BASENODE * node, NAME * name, TypeArray * typeArgs, bool fAggOnly = false);
    SYM * SearchNamespacesCore(NSDECLSYM * nsd, BASENODE * node, NAME * name, TypeArray * typeArgs);
    SYM * SearchNamespacesForAliasCore(NSDECLSYM * nsd, BASENODE * node, NAME * name);
    SYM * SearchUsingAliasesCore(NSDECLSYM * nsd, BASENODE * node, NAME * name, TypeArray * typeArgs);
    SYM * SearchUsingClausesCore(NSDECLSYM * nsd, BASENODE * node, NAME * name, TypeArray * typeArgs);

    // Used to determine whether a using alias is currently available. This depends on whether
    // m_fUsingAlias is set (indicating that we're currently resolving an alias), whether the alias is
    // extern, and whether alias->parent == m_symStart.
    bool FAliasAvailable(ALIASSYM * alias) {
        ASSERT(!m_fUsingAlias || m_symStart->isNSDECLSYM());
        return !m_fUsingAlias || alias->fExtern || alias->parent != m_symStart;
    }

    static bool CheckConstraintsCore(COMPILER * cmp, BASENODE * tree, SYM * symErr,
        TypeArray * typeVars, TypeArray * typeArgs, TypeArray * typeArgsCls, TypeArray * typeArgsMeth, 
        CheckConstraintsFlagsEnum flags);
    static bool SatisfiesBound(COMPILER * cmp, TYPESYM * arg, TYPESYM * typeBnd);

    // Misc. utililites.
    NAME * AppendAttrSuffix(NAMENODE * name);
    bool IsAttributeType(SYM * sym);
    int RankSym(SYM * sym);

    // Only called if the CallPreHook flag is set. Called by BindSingleNameCore before any other
    // lookup is performed.
    virtual SYM * PreLookup(NAME * name, TypeArray * typeArgs) { return NULL; }
};

#endif //__typebind_h__
