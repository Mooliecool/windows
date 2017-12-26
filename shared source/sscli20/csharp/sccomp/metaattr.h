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
// File: metaattr.h
//
// ===========================================================================

#ifndef __metaattr_h__
#define __metaattr_h__

const CorAttributeTargets catReturnValue = (CorAttributeTargets) 0x2000;
const CorAttributeTargets catAllOld = catAll;
#define catAll ((CorAttributeTargets) (catAllOld | catReturnValue))

/***************************************************************************************************
    Base class for all attribute binders.
***************************************************************************************************/
class AttrBind
{
protected:
    AttrBind(COMPILER *compiler, bool fEarly);

    void Init(SYM * sym);
    void Init(CorAttributeTargets ek, PARENTSYM * context);

    void ProcessAll(BASENODE * attributes);
    void ProcessAll(AGGSYM * agg);
    void ProcessSingleAttr(ATTRNODE * attr);
    void PostProcess(BASENODE * errorLocation);
    void ProcessSynthAttr(AGGTYPESYM * attributeType, EXPR * ctorExpression, EXPR * namedArgs);
    void CompileFabricatedAttr();
    virtual void ValidateAttrs();

    //
    // the main loop methods
    //
    virtual bool    BindAttr(ATTRNODE *attr);
    void            VerifyAndEmit(ATTRNODE *attr);
    virtual void    VerifyAndEmitCore(ATTRNODE *attr);
    virtual void    VerifyAndEmitPredef(ATTRNODE *attr);
    virtual mdToken GetToken() { mdToken token = sym->getTokenEmit(); ASSERT(token != 0); return token; }
    void            AddAttrArg(EXPR * arg, BlobBldr & blob);
    bool            IsConditionalFalse();
    unsigned        CountNamedArgs();

    //
    // helpers for predefined attributes which can occur on more than one SK
    //
    void ProcessObsoleteEarly(ATTRNODE * attr);
    void ProcessCLSEarly(ATTRNODE * attr);
    void VerifyCLS(ATTRNODE * attr);
    bool isLinkDemand(ATTRNODE * attr);
    void ProcessConditionalEarly(ATTRNODE * attr);
    virtual void AddConditionalName(NAME * name) { }
    void AddDefaultCharSet();
    OUTFILESYM *getOutputFile();

    void ErrorBadSymbolKind(BASENODE *tree);

    // stuff for the symbol being attributed
    SYM *               sym;
    CorAttributeTargets ek;
    ATTRLOC             attrloc;
    PARENTSYM *         context;
    SYMLIST *           customAttributeList;
    bool hasLinkDemand;
    bool fEarly;

    // stuff for a single attribute
    AGGTYPESYM *    attributeType;
    EXPR *          ctorExpression;
    EXPR *          namedArguments;
    PREDEFATTR      predefinedAttribute;

    COMPILER *      compiler;

    // return the given predefined type (including void)
    NAME *          GetPredefName(PREDEFNAME pn);
    bool            isAttributeType(TYPESYM * type);

    bool            getValue(EXPR *argument, bool * rval);
    bool            getValue(EXPR *argument, int * rval);
    bool            getValue(EXPR *argument, STRCONST ** rval);
    bool            getValue(EXPR *argument, TYPESYM **type);

    static NAME *   getNamedArgumentName(EXPR *expr);
    static EXPR *   getNamedArgumentValue(EXPR *expr);

    static STRCONST *   getKnownString(EXPR *expr);
    static bool         getKnownBool(EXPR *expr);

private:

    bool VerifyAttrArg(EXPR *arg);

};


/***************************************************************************************************
    Default attribute binder. CompileEarly handles obsolete and cls.
***************************************************************************************************/
class DefaultAttrBind : public AttrBind
{
public:
    static void CompileEarly(COMPILER * compiler, SYM * sym);
    static void CompileAndEmit(COMPILER * compiler, SYM * sym);

protected:
    DefaultAttrBind(COMPILER * compiler, SYM * sym, bool fEarly) : AttrBind(compiler, fEarly)
    {
        Init(sym);
    }

    bool BindAttr(ATTRNODE * attr);
};


/***************************************************************************************************
    Early attribute binder for global attributes.
***************************************************************************************************/
class EarlyGlobalAttrBind : public AttrBind
{
public:
    static void Compile(COMPILER *compiler, GLOBALATTRSYM *globalAttribute);

protected:
    EarlyGlobalAttrBind(COMPILER * compiler, GLOBALATTRSYM *globalAttribute);
    bool BindAttr(ATTRNODE *attr);
    void VerifyAndEmitPredef(ATTRNODE *attr);
};


/***************************************************************************************************
    Attribute binder for global attributes.
***************************************************************************************************/
class GlobalAttrBind : public AttrBind
{
public:
    static void Compile(COMPILER *compiler, PARENTSYM * context, GLOBALATTRSYM *globalAttribute, mdToken tokenEmit);

protected:
    GlobalAttrBind(COMPILER * compiler, GLOBALATTRSYM *globalAttribute, mdToken tokenEmit);
    GlobalAttrBind(COMPILER * compiler, mdToken tokenEmit, PARENTSYM * context);
    void VerifyAndEmitPredef(ATTRNODE *attr);
    mdToken GetToken() { return tokenEmit; }

    GLOBALATTRSYM *     globalAttribute;
    mdToken             tokenEmit;
    bool		fHadDebuggable;
};


/***************************************************************************************************
    Early attribute binder for AGGSYMs. This handles:
        usage, obsolete, cls, coclass, comimport, conditional
***************************************************************************************************/
class EarlyAggAttrBind : public AttrBind
{
public:
    static void Compile(COMPILER *compiler, AGGSYM *cls);

protected:
    EarlyAggAttrBind(COMPILER * compiler, AGGSYM *cls);
    bool BindAttr(ATTRNODE *attr);
    void VerifyAndEmitPredef(ATTRNODE *attr);
    void ValidateAttrs();
    void AddConditionalName(NAME * name);

    void ProcessCoClass(ATTRNODE * attr);

    AGGSYM * cls;
    NAMELIST ** pnlstCond;
};


/***************************************************************************************************
    Attribute binder for AGGSYMs.
***************************************************************************************************/
class AggAttrBind : public AttrBind
{
public:
    static void Compile(COMPILER *compiler, AGGSYM *cls, AGGINFO *info);

protected:
    AggAttrBind(COMPILER * compiler, AGGSYM * cls, AGGINFO * info, NAME * defaultMemberName);
    void VerifyAndEmitPredef(ATTRNODE * attr);
    void ValidateAttrs();

    void ProcessStructLayout(ATTRNODE * attr);

    AGGSYM * cls;
    AGGINFO * info;
    NAME * defaultMemberName;

private:
    void CheckSequentialOnPartialType(ATTRNODE * attr);

};


/***************************************************************************************************
    Attribute binder for a set of type variables. Note that one TYVARSYM may map to many generic
    parameters in meta-data, since in C# nested types inherit outer type parameters but in
    metadata nested types do not inherit generic parameters from the outer type.
***************************************************************************************************/
class TypeVarAttrBind : public AttrBind
{
public:
    static void CompileParamList(COMPILER *compiler, PARENTSYM *declFirst, TypeArray *tyvars, mdToken *tokens);

protected:
    TypeVarAttrBind(COMPILER * compiler, TYVARSYM * var, PARENTSYM * context, mdToken tok);
    virtual mdToken GetToken() { ASSERT(tok != 0); return tok; }

private:
    mdToken tok;
};


/***************************************************************************************************
    Early attribute binder for INDEXERSYMs. Handles indexer-name.
***************************************************************************************************/
class IndexerNameAttrBind : public AttrBind
{
public:
    static void Compile(COMPILER *compiler, INDEXERSYM *propertyNode);

protected:
    IndexerNameAttrBind(COMPILER *compiler, INDEXERSYM * prop);
    bool BindAttr(ATTRNODE * attr);
    void VerifyAndEmitPredef(ATTRNODE * attr);

    INDEXERSYM * prop;
};


/***************************************************************************************************
    Attribute binder for parameters.
***************************************************************************************************/
class ParamAttrBind : public AttrBind
{
public:
    static void CompileParamList(COMPILER *compiler, METHINFO *info);

protected:
    // for regular parameter lists
    ParamAttrBind(COMPILER *compiler, METHSYM *method);
    void Init(TYPESYM *type, PARAMINFO *info, int index);

    mdToken GetToken();
    void VerifyAndEmitPredef(ATTRNODE *attr);
    void ValidateAttrs();

    void EmitParamProps();
    void EmitPredefAttrs();

    METHSYM *       method;
    PARAMINFO *     paramInfo;
    TYPESYM *       parameterType;
    int             index;
    bool            hasDefaultValue;
    DWORD           etDefaultValue;
    BYTE            rgb[256];
    BlobBldrNrHeap  blob;

};


/***************************************************************************************************
    Attribute binder for methods.
***************************************************************************************************/
class MethAttrBind : public AttrBind
{
public:
    static void CompileEarly(COMPILER * cmp, METHSYM * meth);
    static void CompileAndEmit(COMPILER * cmp, METHSYM * meth, bool fDebuggerHidden);

protected:
    MethAttrBind(COMPILER * cmp, METHSYM * meth, bool fEarly) : AttrBind(cmp, fEarly)
    {
        this->meth = meth;
        this->pnlstCond = fEarly ? &meth->conditionalSymbols : NULL;
        Init(meth);
    }
    void VerifyAndEmitPredef(ATTRNODE *attr);
    bool BindAttr(ATTRNODE *attr);
    void ValidateAttrs();
    void AddConditionalName(NAME * name);

    void ProcessDllImport(ATTRNODE * attr);

    METHSYM * meth;
    NAMELIST ** pnlstCond;
};


/***************************************************************************************************
    Attribute binder for properties. This does not deal with attributes that belong on the
    accessors.
***************************************************************************************************/
class PropAttrBind : public AttrBind
{
public:
    static void Compile(COMPILER * compiler, PROPSYM *prop);

protected:
    PropAttrBind(COMPILER * compiler, PROPSYM * prop);
    void VerifyAndEmitPredef(ATTRNODE * attr);

    PROPSYM * prop;
};


/***************************************************************************************************
    Attribute binder for fields.
***************************************************************************************************/
class FieldAttrBind : public AttrBind
{
public:
    static void Compile(COMPILER *compiler, MEMBVARSYM *field, MEMBVARINFO *info, AGGINFO *agginfo);

protected:
    FieldAttrBind(COMPILER *compiler, MEMBVARSYM *field, MEMBVARINFO *info, AGGINFO *agginfo);
    void VerifyAndEmitPredef(ATTRNODE *attr);
    void ValidateAttrs();

    MEMBVARSYM * field;
    MEMBVARINFO * info;
    AGGINFO * agginfo;
};


/***************************************************************************************************
    Attribute binder for attributes with an unknown target. The parser will have produced a
    warning on any of these.
***************************************************************************************************/
class UnknownAttrBind : public AttrBind
{
public:
    static void Compile(COMPILER *compiler, PARENTSYM *context, BASENODE *attributes);
    static void Compile(COMPILER *compiler, GLOBALATTRSYM *sym);

protected:
    UnknownAttrBind(COMPILER * compiler, PARENTSYM *context);
    void VerifyAndEmitPredef(ATTRNODE *attr) { }
    void VerifyAndEmitCore(ATTRNODE *attr) { }
};

/***************************************************************************************************
    Attribute binder for emitting the CompilerGeneratedAttribute.
***************************************************************************************************/
class CompilerGeneratedAttrBind : public AttrBind
{
public:
    CompilerGeneratedAttrBind(COMPILER *compiler, bool fEarly) : AttrBind(compiler,fEarly) {}

    static void EmitAttribute(COMPILER * cmp, mdToken tok);
};

/***************************************************************************************************
    Attribute binder for emitting the UnsafeValueTypeAttribute.
***************************************************************************************************/
class UnsafeValueTypeAttrBind : public AttrBind
{
public:
    UnsafeValueTypeAttrBind(COMPILER *compiler, bool fEarly) : AttrBind(compiler,fEarly) {}

    static void EmitAttribute(COMPILER * cmp, mdToken tok);
};

#endif //__metaattr_h__
