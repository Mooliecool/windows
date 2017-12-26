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
// File: clsdrec.h
//
// Defines the structure which contains information necessary to declare
// a single class
// ===========================================================================

#ifndef __clsdrec_h__
#define __clsdrec_h__

enum CONVTYPE {
    CONV_IMPL,
    CONV_EXPL,
    CONV_NONE,
    CONV_OTHER,  // for example, struct to byte,
    CONV_ERROR,  // meaning an ICE if we ever encounter it

};

class COMPILER;
class CError;

class CLSDREC {

private:
    bool isAtLeastAsVisibleAs(SYM * sym1, SYM * sym2);
    void checkConstituentVisibility(SYM * main, SYM * constituent, int errCode);

    void checkUnsafe(BASENODE * tree, TYPESYM * type, bool unsafeContext, int errCode = ERR_UnsafeNeeded);
    bool CheckForBadMemberSimple(NAME *name, BASENODE *parseTree, AGGSYM *cls, SYM* created = NULL);
    bool CheckForBadMember(NAME *name, SYMKIND symkind, TypeArray *params, BASENODE *parseTree, AGGSYM *cls, TypeArray *typeVarsMeth, SYM* created = NULL);
    bool CheckForDuplicateSymbol(NAME *name, BASENODE *parseTree, AGGSYM *cls, SYM* created = NULL);
    bool CheckForDuplicateSymbol(NAME *name, SYMKIND symkind, TypeArray *params, BASENODE *parseTree, AGGSYM *cls, TypeArray *typeVarsMeth, SYM* created = NULL);

    void CopyMethTyVarsToClass( METHPROPSYM * pMeth, AGGSYM * cls);

    void checkCLSnaming(AGGSYM *cls);
    void checkCLSnaming(NSSYM *cls);
    void CheckSigForCLS(METHSYM *method, SYM * errorSym, BASENODE * pParamTree);
    TypeArray * CLSReduceSignature(TypeArray * types);
    void CheckLinkDemandOnOverride(METHSYM * meth, MethWithType mwtBase);

    SYM *findDuplicateConversion(bool conversionsOnly, TypeArray *parameterTypes, PTYPESYM returnType, PNAME name, AGGSYM* cls);

    void declareNamespace(PNSDECLSYM declaration);
    void declareGlobalAttribute(ATTRDECLNODE *pNode, NSDECLSYM *declaration);
    void declareAggregate(AGGREGATENODE * pClassTree, DECLSYM * parent);

    void ensureBasesAreResolved(AGGSYM *cls);

    void defineAggregateMembers(AGGSYM *cls);
    void defineEnumMembers(AGGSYM *cls);
    void defineDelegateMembers(AGGSYM *cls);
    void defineProperty(PROPERTYNODE *propertyNode, AGGSYM *cls, AGGDECLSYM * clsdecl);
    EVENTSYM * defineEvent(SYM * implementingSym, BASENODE * pTree);
    PNAME createAccessorName(PNAME propertyName, PCWSTR prefix);
    bool CheckParamsType(BASENODE * tree, TYPESYM ** ptypeLast);
    METHSYM * defineMethod(METHODBASENODE * pMethodTree, AGGSYM * pClass, AGGDECLSYM * clsdecl);
    METHSYM * MakeIterator(METHINFO * info);
    bool defineFields(FIELDNODE * pFieldTree, AGGSYM * pClass, AGGDECLSYM * clsdecl);
    AGGSYM * MakeFixedBufferStruct(MEMBVARSYM * field, TYPESYM * type);
    bool defineOperator(OPERATORMETHODNODE * operatorNode, AGGSYM * cls, AGGDECLSYM * clsdecl);
    void synthesizeConstructor(AGGSYM * cls, bool isStatic);
    void checkMatchingOperator(PREDEFNAME pn, AGGSYM * cls);
    TYPESYM * StripNubs(TYPESYM * type, AGGSYM * clsCtx, int * pcnub = NULL);
    void CheckForProtectedInSealed(SYM *member);

    AGGDECLSYM * AddAggregate(BASENODE * aggregateNode, NAMENODE * nameNode, DECLSYM * parent);
    AGGDECLSYM * AddSynthAgg(PCWSTR szBasename, SpecialNameKindEnum snk, AGGDECLSYM * declPar);
    PROPSYM * FabricateExplicitImplPropertyRO(PREDEFNAME pn, AGGTYPESYM * iface, AGGDECLSYM * ads);
    METHSYM * FabricateExplicitImplMethod(PREDEFNAME pn, AGGTYPESYM * iface, AGGDECLSYM * ads);
    METHSYM * FabricateSimpleMethod(PREDEFNAME pn, AGGDECLSYM * ads, TYPESYM * typeRet);
    NSSYM * addNamespace(NAMENODE* name, NSDECLSYM* parent);
    NSDECLSYM * addNamespaceDeclaration(NAMENODE* name, NAMESPACENODE *parseTree, NSDECLSYM *containingDeclaration);

    void CheckHiddenSymbol(SYM * symNew, SymWithType swtHid);
    void checkFlags(SYM * item, unsigned allowedFlags, unsigned actualFlags);
    ACCESS GetAccessFromFlags(SYM * item, unsigned allowedFlags, unsigned actualFlags);
    
    void DefineEventAccessors(EVENTSYM *event, EventWithType evtBase);
    void DefinePropertyAccessor(PNAME name, BASENODE *parseTree, TypeArray *params, PTYPESYM retType, uint propertyFlags, PROPSYM *property, METHSYM **accessor);
    void CreateAccessor(METHSYM ** pmeth, PNAME name, BASENODE *parseTree, TypeArray *params, PTYPESYM retType, uint propertyFlags, SYM *property);
    void DefinePropertyAccessors(PROPSYM *property, PropWithType pwtBase);

    METHSYM *FindSameSignature(MethWithType mwt, AGGTYPESYM * atsSearch, bool fOverride);
    PROPSYM *findSameSignature(AGGTYPESYM *propAtTyp, PROPSYM *prop, AGGTYPESYM *typeToSearchIn);
    void BuildOrCheckAbstractMethodsList(AGGSYM * cls);
    void CheckInterfaceImplementations(AGGSYM * agg);
    void prepareClassOrStruct(AGGSYM * cls);
    void prepareInterfaceMember(METHPROPSYM * method);
    void prepareMethod(METHSYM * method);
    void prepareFields(MEMBVARSYM *field);
    void prepareConversion(METHSYM * conversion);
    void prepareOperator(METHSYM * op);
    void prepareProperty(PROPSYM * property);
    void prepareAccessor(METHSYM *accessor, PROPSYM *property, METHSYM *implAccessor);
    void CheckValidAccessorOverride(METHSYM * methAcc, PROPSYM * prop, METHSYM * methBaseAcc, int errNone);
    void SetAccessorOverride(METHSYM * methAcc, EVENTSYM * evt, METHSYM * methBaseAcc);
    void prepareEvent(EVENTSYM * property);
    void prepareInterface(AGGSYM * cls);
    void CheckExplicitImpl(SYM * sym);

    void SetOverrideConstraints(METHSYM * methOver);

    struct LayoutFrame {
        SymWithType swt;
        AGGTYPESYM * atsField;
        LayoutFrame * pframeOwner;
        LayoutFrame * pframeChild;
    };

    bool CheckStructLayout(AGGTYPESYM * ats, LayoutFrame * pframeOwner);

    NAME *getMethodConditional(METHSYM *method);
    void ReportHiding(SYM *sym, SymWithType * pswt, unsigned flags);
    void checkSimpleHiding(SYM *sym, unsigned flags);
    void checkIfaceHiding(SYM *sym, unsigned flags);
    bool FindAnyHiddenSymbol(NAME *name, AGGTYPESYM *typeStart, AGGSYM *agg, SymWithType * pswt);
    METHSYM *NeedExplicitImpl(MethWithType mwtIface, METHSYM *methImpl, AGGSYM *cls);
    void findEntryPoint(AGGSYM *cls);

    void emitTypedefsAggregate(AGGSYM *cls);
    void emitMemberdefsAggregate(AGGSYM *cls);
    void emitBasesAggregate(AGGSYM *cls);

    void compileAggregate(AGGSYM * cls);
    void CompileMember(SYM *member, AGGINFO *info);
    void compileMethod(METHSYM * method, AGGINFO * info);
    EXPR * CompileIterator(BASENODE * tree, EXPR * body, METHINFO * info, AGGINFO * aggInfo);
    void compileFabricatedMethod(BASENODE * tree, EXPR * body, METHINFO * info, AGGINFO * aggInfo);
    void CompileAnonMeths(METHINFO * info, BASENODE * tree, EXPR * body);
    void compileField(MEMBVARSYM * field, AGGINFO * info);
    void compileProperty(PROPSYM * property, AGGINFO * info);
    void compileEvent(EVENTSYM * event, AGGINFO * aggInfo);

    void CompileAggSkeleton(AGGSYM * cls);
    void CompileMemberSkeleton(SYM * sym, AGGINFO * info);

    void FillMethInfoCommon(METHSYM * meth, METHINFO * pinfo, AGGINFO * pai, bool fNoErrors);

    PARAMINFO * ReallocParams(int cntNeeded, int * maxAlreadAlloced, PARAMINFO ** ppParams);
    void EmitMemberdef(SYM * member, void * pvUnused);

    COMPILER * compiler();
    CController * controller();

    static const TOKENID accessTokens[];

    // cached values for GetLayoutKindValue();
    int m_explicitLayoutValue;
    int m_sequentialLayoutValue;

public:
    CLSDREC();

    void setOverrideBits(AGGSYM * cls);

    void declareInputfile(NAMESPACENODE * parseTree, PINFILESYM infile);
    void ResolveInheritance(NSDECLSYM *nsdecl);
    void ResolveInheritance(AGGSYM *cls);
    bool ResolveInheritanceRec(AGGSYM *cls);
    void defineBounds(NSDECLSYM *nsdecl);
    void defineBounds(AGGSYM *cls);
    void prepareNamespace(NSDECLSYM *nsDeclaration);

    void CheckForTypeErrors(NSDECLSYM * ns);
    void CheckForTypeErrors(AGGSYM * agg);
    void CheckForTypeErrors(BASENODE * tree, SYM * symCtx, METHPROPSYM * mps);
    void CheckForTypeErrors(BASENODE * tree, SYM * symCtx, TYPESYM * type);

    void defineObject();    // for our friend the symmgr
    void defineNamespace(NSDECLSYM *nsDeclaration);
    void defineAggregate(AGGSYM *cls);
    void DefineParameters(PARENTSYM * symCtx, BASENODE * nodeParams, bool fUnsafe, TypeArray ** pta, bool * pfParams);
    void emitTypedefsNamespace(NSDECLSYM *nsDeclaration);
    void emitMemberdefsNamespace(NSDECLSYM *nsDeclaration);
    void emitBasesNamespace(NSDECLSYM *nsDeclaration);
    void compileNamespace(NSDECLSYM *nsDeclaration);
    void prepareAggregate(AGGSYM * cls);
    typedef void (CLSDREC::*MEMBER_OPERATION)(SYM *aggregateMember, VOID *info);
    void EnumMembersInEmitOrder(AGGSYM *cls, VOID *info, MEMBER_OPERATION doMember);

    void ensureUsingClausesAreResolved(NSDECLSYM *nsd);

    AGGSYM * CreateAnonymousMethodClass(METHSYM * methOuter);

    bool evaluateFieldConstant(MEMBVARSYM * fieldFirst, MEMBVARSYM * fieldCurrent);
    void evaluateConstants(AGGSYM *cls);

    // Check a pointer type to make sure it's legal.
    void checkUnmanaged(BASENODE * tree, TYPESYM * type);

    SYM *findNextName(NAME *name, AGGTYPESYM **pTypeToSearchIn, SYM *current);
    SYM *findNextAccessibleName(NAME *name, AGGTYPESYM **pTypeToSearchIn, PARENTSYM *context, SYM *current, bool bAllowAllProtected, bool ignoreSpecialMethods);
    SYM *FindAnyAccessSymHiddenByMeth(METHSYM *meth, AGGTYPESYM *typeToSearchIn, AGGTYPESYM **pWhereDefined = NULL);
    SYM *FindExplicitInterfaceImplementation(AGGTYPESYM *ats, SymWithType swt);
    bool FindSymHiddenByMethPropAgg(SYM *sym, AGGTYPESYM *typeStart, AGGSYM *agg, SymWithType *pswt, SymWithType *pswtAmbig = NULL, bool *pfNeedMethImpl = NULL);

    NAMELIST * GetConditionalSymbols(METHSYM *method);
    NAMELIST * GetConditionalSymbols(AGGSYM *attribute);

    void DefineClassTypeVars(AGGSYM * agg, AGGDECLSYM *decl, BASENODE * typars);
    void DefineMethodTypeVars(METHSYM * parent, BASENODE * typars);
    TYVARSYM * CreateTypeVar(PARENTSYM * symPar, TYPEBASENODE * tree, int ivar, TYVARSYM ** prgvar, int ivarTot);
    void DefineBounds(BASENODE * tree, PARENTSYM * symPar, bool fFirst);
    void CheckBoundsVisibility(SYM * sym, TypeArray * typeVars);
    void CheckImplicitImplConstraints(MethWithType mwtImpl, MethWithType mwtBase);

    OPERATOR operatorOfName(PNAME name);

    bool isAttributeType(TYPESYM *type);
    int GetLayoutKindValue(PREDEFNAME pnLayoutKind);

    bool CheckAccess(SYM * symCheck, AGGTYPESYM * atsCheck, SYM * symWhere, TYPESYM * typeThru);
    bool CheckTypeAccess(TYPESYM * type, SYM * symWhere);

private:
    bool CheckAccessCore(SYM * symCheck, AGGTYPESYM * atsCheck, SYM * symWhere, TYPESYM * typeThru);
public:


    void ReportAccessError(BASENODE * tree, SymWithType swtBad, SYM * symWhere, TYPESYM * typeQual);

    void ReportDeprecated(BASENODE * tree, PSYM refContext, SymWithType swt);

    static const PREDEFNAME operatorNames[OP_LAST];


};

#endif // __clsdrec_h__
