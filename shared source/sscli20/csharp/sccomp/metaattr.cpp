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
// File: metaattr.cpp
//
// Routines for converting the attribute information of an item
// ===========================================================================

#include "stdafx.h"

//-----------------------------------------------------------------------------

EXPR * FUNCBREC::bindAttributeValue(BASENODE * tree)
{
    CHECKEDCONTEXT checked(this, compiler()->GetCheckedMode());
    this->checked.constant = true;

    EXPR * val = bindExpr(tree);

    checked.restore(this);

    return val;
}

CONSTVAL FUNCBREC::createStringConstant(PCWSTR str)
{
    CONSTVAL rval;
    
    rval.strVal = (STRCONST*) allocator->Alloc(sizeof(STRCONST));
    if (str) {
        int len = (int) wcslen(str);
        rval.strVal->length = len;
        rval.strVal->text = (WCHAR*) allocator->Alloc(sizeof(WCHAR) * rval.strVal->length);
        memcpy(rval.strVal->text, str, sizeof(WCHAR) * rval.strVal->length);
    } else {
        rval.strVal = NULL;
    }
    return rval;
}


EXPR * FUNCBREC::bindNamedAttributeValue(AGGTYPESYM *attributeType, ATTRNODE * attr)
{
    //
    // bind name to public non-readonly field or property
    //
    NAME * name = attr->pName->asNAME()->pName;
    TYPESYM * type = NULL;
    AGGTYPESYM *typeToSearch = attributeType;
    bool badModifiers = false;
    bool badType = false;
    bool badSymbol = false;

    MemberLookup mem;
    if (!mem.Lookup(compiler(), typeToSearch, NULL, parentDecl, name, 0, MemLookFlags::UserCallable)) {
        mem.ReportErrors(attr);
        return NULL;
    }
    mem.ReportWarnings(attr);

    SymWithType swt = mem.SwtFirst();

    if (swt.Sym()->isMEMBVARSYM()) {
        if (swt.Sym()->GetAccess() != ACC_PUBLIC || swt.Field()->isReadOnly || swt.Field()->isStatic || swt.Field()->isConst) {
            badModifiers = true;
        } else {
            if (!compiler()->clsDeclRec.isAttributeType(swt.Field()->type)) {
                badType = true;
            } else {
                type = swt.Field()->type;
                swt.Field()->isAssigned = true;
            }
        }
    }
    else if (swt.Sym()->isPROPSYM()) {

        ASSERT(!swt.Prop()->isIndexer());

        // check modifiers on property and accessors.  Both accessors must exist and be public.
        // This may be called before we create the accessor methods though, so we also have to check the parsetree as well.
        if (swt.Prop()->GetAccess() != ACC_PUBLIC || swt.Prop()->isStatic)
            badModifiers = true;
        else {
            if (swt.Prop()->methSet) {
                if (swt.Prop()->methSet->GetAccess() != ACC_PUBLIC)
                    badModifiers = true;
            } else if (!swt.Prop()->parseTree || !swt.Prop()->parseTree->asANYPROPERTY()->pSet || 
                (swt.Prop()->parseTree->asANYPROPERTY()->pGet->flags & NF_MOD_ACCESSMODIFIERS) != 0 &&
                (swt.Prop()->parseTree->asANYPROPERTY()->pSet->flags & NF_MOD_PUBLIC) == 0)
            {
                badModifiers = true;
            } 

            if (swt.Prop()->methGet) {
                if (swt.Prop()->methGet && swt.Prop()->methGet->GetAccess() != ACC_PUBLIC)
                    badModifiers = true;
            } else if (!swt.Prop()->parseTree || !swt.Prop()->parseTree->asANYPROPERTY()->pGet ||
                (swt.Prop()->parseTree->asANYPROPERTY()->pGet->flags & NF_MOD_ACCESSMODIFIERS) != 0 &&
                (swt.Prop()->parseTree->asANYPROPERTY()->pGet->flags & NF_MOD_PUBLIC) == 0)
            {
                badModifiers = true;
            } 
        }

        if (!badModifiers) {
            if (!compiler()->clsDeclRec.isAttributeType(swt.Prop()->retType)) {
                badType = true;
            }
            else {
                type = swt.Prop()->retType;
            }
        }
    }
    else {
        badSymbol = true;
    }

    if (!type) {
        //
        // didn't find an accessible field
        // report what we did find ...
        //
        compiler()->ErrorRef(attr->pName, badType ? ERR_BadNamedAttributeArgumentType : ERR_BadNamedAttributeArgument,
            ErrArgNameNode(attr->pName), swt);

        return NULL;
    }

    if (swt.Sym()->IsDeprecated()) {
        ReportDeprecated(attr, swt);
    }

    //
    // bind value
    //
    EXPR * value = bindAttributeValue(attr->pArgs);
    if (!value || !value->isOK()) {
        return NULL;
    }
    value = mustConvert(value, type);
    if (!value || !value->isOK()) {
        return NULL;
    }

    //
    // package it up as an assignment
    //
    EXPR * op1;
    if (swt.Sym()->isMEMBVARSYM()) {
        op1 = newExpr(attr->pName, EK_FIELD, type);
        op1->asFIELD()->fwt = swt;
    } else {
        ASSERT(swt.Sym()->isPROPSYM());
        PROPSYM * prop = swt.Sym()->asPROPSYM();
        op1 = newExpr(attr->pName, EK_PROP, type);
        op1->asPROP()->pwtSlot.Set(prop, swt.Type());
        if (prop->methGet)
            op1->asPROP()->mwtGet.Set(prop->methGet, swt.Type());
        if (prop->methSet)
            op1->asPROP()->mwtSet.Set(prop->methSet, swt.Type());
    }
    return SetNodeExpr(attr, newExprBinop(attr, EK_ASSG, op1->type, op1, value));
}

EXPR * FUNCBREC::bindAttrArgs(AGGTYPESYM *attributeType, ATTRNODE * attr, EXPR **namedArgs)
{
    EXPR *rval = NULL;
    EXPR ** prval = &rval;
    NODELOOP(attr->pArgs, BASE, node)

        ATTRNODE *argNode = node->asATTRARG();

        if (argNode->pName) {
            //
            // check for duplicate named argument
            //
            NODELOOP(attr->pArgs, BASE, nodeInner)
                ATTRNODE *argNodeInner = nodeInner->asATTRARG();
                if (argNodeInner->pName) {
                    if (argNodeInner->pName->asNAME()->pName == argNode->pName->asNAME()->pName) {
                        if (argNodeInner == argNode) {
                            break;
                        } else {
                            compiler()->Error(argNode, ERR_DuplicateNamedAttributeArgument, argNode->pName->asNAME()->pName->text);
                        }
                    }
                }
            ENDLOOP

            EXPR * item = bindNamedAttributeValue(attributeType, argNode);
            if (item) {
                newList(item, &namedArgs);
            }
        } else {
            EXPR * item = bindAttributeValue(argNode->pArgs);
            newList(item, &prval);
        }
    ENDLOOP;

    return rval;
}

// Initialize the state of the FUNCBREC to prepare for binding an attribute
SCOPESYM * FUNCBREC::initAttrBindContext(PARENTSYM * context)
{
    ASSERT(context && !context->isBAGSYM() && !context->isTYPESYM());

    pMSym = NULL;
    pFSym = NULL;
    pFOrigSym = NULL;
    m_pamiFirst = NULL;
    m_pamiCur = NULL;

    DECLSYM * declCtx;

    if (context->isDECLSYM())
        declCtx = context->asDECLSYM();
    else 
        declCtx = context->containingDeclaration();

    if (parentDecl != declCtx) {
        parentDecl = declCtx;
        if (parentDecl->isAGGDECLSYM())
            parentAgg = parentDecl->asAGGDECLSYM()->Agg();
        else
            parentAgg = NULL;
        pClassInfo = NULL;
        btfFlags = (parentAgg && parentAgg->isContainedInDeprecated()) ? TypeBindFlags::NoDeprecated : TypeBindFlags::None;
    }

    pCurrentScope = compiler()->getLSymmgr().CreateLocalSym(SK_SCOPESYM, NULL, NULL)->asSCOPESYM();
    pCurrentScope->nestingOrder = 1;

    // We null out the scope so as not to get any cache hits
    SCOPESYM * scope = pOuterScope;
    pOuterScope = NULL;
    return scope;
}

EXPRCALL *FUNCBREC::bindAttribute(PARENTSYM * context, AGGTYPESYM * attributeType, ATTRNODE * attribute, EXPR ** namedArgs)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    SCOPESYM * scope = initAttrBindContext(context);

    // attributes are always in a safe context
    UNSAFESTATES oldUnsafeContext = unsafeState;
    setUnsafe(false);

    *namedArgs = NULL;
    EXPR * args = bindAttrArgs(attributeType, attribute, namedArgs);
    if (args && !args->isOK()) {
        return NULL;
    }
    EXPR *call = createConstructorCall(attribute, attribute, attributeType, NULL, args, MemLookFlags::NewObj);
    if (call->kind != EK_CALL) {
        call = NULL;
    }

    setUnsafeState(oldUnsafeContext);

    // restore it (useful if we are doing param attrs...)
    pOuterScope = scope;

    return SetNodeExpr(attribute, call)->asCALL();
}

bool FUNCBREC::getAttributeValue(PARENTSYM *context, EXPR * val, STRCONST ** rval)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    initAttrBindContext(context);
    
    val = mustConvert(val, GetReqPDT(PT_STRING));

    if (!val || !val->isOK()) return false;

    if (val->kind != EK_CONSTANT) {
        compiler()->Error(val->tree, ERR_BadAttributeParam);
        return false;
    }

    *rval = val->asCONSTANT()->getSVal().strVal;
    return true;

}

bool FUNCBREC::getAttributeValue(PARENTSYM *context, EXPR * val, bool * rval)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    initAttrBindContext(context);

    val = mustConvert(val, GetReqPDT(PT_BOOL));

    if (!val) return false;

    if (val->kind != EK_CONSTANT) {
        compiler()->Error(val->tree, ERR_BadAttributeParam);
        return false;
    }

    *rval = val->asCONSTANT()->getVal().iVal ? true : false;
    return true;
}


bool FUNCBREC::getAttributeValue(PARENTSYM *context, EXPR * val, int * rval)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    initAttrBindContext(context);

    val->setType(val->type->underlyingType());
    val = mustConvert(val, GetReqPDT(PT_INT));

    if (!val) return false;

    if (val->kind != EK_CONSTANT) {
        compiler()->Error(val->tree, ERR_BadAttributeParam);
        return false;
    }

    *rval = val->asCONSTANT()->getVal().iVal;
    return true;
}

bool FUNCBREC::getAttributeValue(PARENTSYM *context, EXPR * val, TYPESYM ** rval)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    initAttrBindContext(context);

    val = mustConvert(val, GetReqPDT(PT_TYPE));

    if (!val) return false;

    if (val->kind == EK_CONSTANT) {
        *rval = 0;
    } else if (val->kind == EK_TYPEOF) {
        *rval = val->asTYPEOF()->sourceType;
    } else {
        compiler()->Error(val->tree, ERR_BadAttributeParam);
        return false;
    }

    return true;
}

// Bind a parameterless predefined attribute
inline EXPR * FUNCBREC::bindSimplePredefinedAttribute(PREDEFTYPE pt)
{
    return bindSimplePredefinedAttribute(pt, NULL);
}

// Bind a predefined attribute with a single string parameter.
EXPR * FUNCBREC::bindStringPredefinedAttribute(PREDEFTYPE pt, PCWSTR arg)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);

    STRCONST * str = (STRCONST*) allocator->Alloc(sizeof(STRCONST));
    int cch = (int)wcslen(arg);

    str->length = cch;
    str->text = (WCHAR*) allocator->Alloc(sizeof(WCHAR) * cch);
    memcpy(str->text, arg, sizeof(WCHAR) * cch);

    return bindSimplePredefinedAttribute(pt, newExprConstant(NULL, compiler()->GetReqPredefType(PT_STRING), ConstValInit(str)));
}


// Bind a predefined attribute with a single parameter.
EXPR * FUNCBREC::bindSimplePredefinedAttribute(PREDEFTYPE pt, EXPR * args)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);

    pMSym = NULL;
    pFSym = NULL;
    pFOrigSym = NULL;

    // We null out the scope so as not to get any cache hits
    SCOPESYM * scope = pOuterScope;
    pOuterScope = NULL;

    AGGTYPESYM * cls = compiler()->GetOptPredefType(pt);
    if (!cls)
        return NULL;
    EXPR * expr = createConstructorCall(NULL, NULL, cls, NULL, args, MemLookFlags::NewObj);

    // restore it (useful if we are doing param attrs...)
    pOuterScope = scope;

    return expr;
}

EXPR * FUNCBREC::bindSkipVerifyArgs()
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    // this is just one arg, the value of SecurityAction.RequestMinumum

    AGGTYPESYM * cls = compiler()->GetOptPredefType(PT_SECURITYACTION);
    ASSERT(cls);
    NAME * name = compiler()->namemgr->GetPredefName(PN_REQUESTMINIMUM);

    MemberLookup mem;

    if (!mem.Lookup(compiler(), cls, NULL, parentDecl, name, 0, MemLookFlags::UserCallable) || !mem.SymFirst()->isMEMBVARSYM()) {
        compiler()->Error(NULL, ERR_MissingPredefinedMember, cls->name->text, name->text);
        return newError(NULL, NULL);
    }

    EXPR * rval = newExprConstant(NULL, cls, mem.SwtFirst().Field()->constVal);

    return rval;
}

EXPR * FUNCBREC::bindStructLayoutArgs()
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    
    // LayoutKind.Sequential
    AGGTYPESYM * cls = compiler()->GetOptPredefType(PT_LAYOUTKIND);
    ASSERT(cls);
    return newExprConstant(NULL, cls, ConstValInit(compiler()->clsDeclRec.GetLayoutKindValue(PN_SEQUENTIAL)));
}

EXPR * FUNCBREC::bindStructLayoutNamedArgs(bool hasNonZeroSize)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);

    if (hasNonZeroSize) {
        return NULL;
    }

    // add Size = 1
    return bindFieldNamedArgument(compiler()->GetOptPredefType(PT_STRUCTLAYOUT), PN_Size, ConstValInit(1));
}

EXPR * FUNCBREC::bindDebuggableArgs()
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);

    AGGSYM * aggDebuggable = compiler()->GetOptPredefAgg(PT_DEBUGGABLEATTRIBUTE);
    if (!aggDebuggable)
        return NULL;

    NAME * name = compiler()->namemgr->GetPredefName(PN_DEBUGGINGMODES_CLASS);
    AGGSYM * aggDebuggingModesEnum = compiler()->getBSymmgr().LookupAggMember(name, aggDebuggable, MASK_AGGSYM)->asAGGSYM();
    if (aggDebuggingModesEnum && !aggDebuggingModesEnum->nextSameName && aggDebuggingModesEnum->IsEnum()) {
        compiler()->EnsureState(aggDebuggingModesEnum);
        AGGTYPESYM * atsDebuggingModesEnum = aggDebuggingModesEnum->getThisType();

        // Get the enum members we care about
        MEMBVARSYM * membNoOptimize = NULL;
        MEMBVARSYM * membTrackJit = NULL;
        MEMBVARSYM * membIgnorePdb = NULL;
        MEMBVARSYM * membEnableENC = NULL;
        name = compiler()->namemgr->GetPredefName(PN_DISABLEOPTIMIZATIONS);
        membNoOptimize = compiler()->getBSymmgr().LookupAggMember(name, aggDebuggingModesEnum, MASK_MEMBVARSYM)->asMEMBVARSYM();
        name = compiler()->namemgr->GetPredefName(PN_DEFAULT);
        membTrackJit = compiler()->getBSymmgr().LookupAggMember(name, aggDebuggingModesEnum, MASK_MEMBVARSYM)->asMEMBVARSYM();
        name = compiler()->namemgr->GetPredefName(PN_IGNORESYMBOLSTORESEQUENCEPOINTS);
        membIgnorePdb = compiler()->getBSymmgr().LookupAggMember(name, aggDebuggingModesEnum, MASK_MEMBVARSYM)->asMEMBVARSYM();
        name = compiler()->namemgr->GetPredefName(PN_ENABLEEDITANDCONTINUE);
        membEnableENC = compiler()->getBSymmgr().LookupAggMember(name, aggDebuggingModesEnum, MASK_MEMBVARSYM)->asMEMBVARSYM();

        if (!membNoOptimize || !membNoOptimize->isConst || membNoOptimize->type != atsDebuggingModesEnum ||
            !membTrackJit || !membTrackJit->isConst || membTrackJit->type != atsDebuggingModesEnum ||
            !membIgnorePdb || !membIgnorePdb->isConst || membIgnorePdb->type != atsDebuggingModesEnum ||
            !membEnableENC || !membEnableENC->isConst || membEnableENC->type != atsDebuggingModesEnum ||
            !FindPredefMeth(NULL, PN_CTOR, aggDebuggable->getThisType(),
                compiler()->getBSymmgr().AllocParams(1, (TYPESYM**)&atsDebuggingModesEnum), false))
            goto OLD_STYLE_DEBUGGABLE;

        if (!compiler()->options.m_fEMITDEBUGINFO)
        {
            // NOTE: on the new style debuggable attribute, we need to emit it even for the pdbOnly case
            // This is all the default behavior (or the JIT will ignore it)
            return NULL;
        }

        CONSTVAL cv = membIgnorePdb->constVal;

        if (!compiler()->options.m_fOPTIMIZATIONS)
            cv.iVal |= membEnableENC->constVal.iVal;

        if (compiler()->options.m_fEMITDEBUGINFO && !compiler()->options.pdbOnly)
            cv.iVal |= membTrackJit->constVal.iVal;

        if (!compiler()->options.m_fOPTIMIZATIONS)
            cv.iVal |= membNoOptimize->constVal.iVal;

        return newExprConstant(NULL, atsDebuggingModesEnum, cv);
    }
    else {
OLD_STYLE_DEBUGGABLE:
        if (!compiler()->options.m_fEMITDEBUGINFO || compiler()->options.pdbOnly)
        {
            // This is all the default behavior (or the JIT will ignore it)
            return NULL;
        }

        CONSTVAL cv;
        cv.iVal = (compiler()->options.m_fEMITDEBUGINFO && !compiler()->options.pdbOnly);

        EXPR * eDebug = newExprConstant(NULL, compiler()->GetReqPredefType(PT_BOOL), cv);

        cv.iVal = !compiler()->options.m_fOPTIMIZATIONS;

        EXPR * eDisableOpt = newExprConstant(NULL, compiler()->GetReqPredefType(PT_BOOL), cv);

        return newExprBinop(NULL, EK_LIST, NULL, eDebug, eDisableOpt);
    }
}


EXPR * FUNCBREC::bindSkipVerifyNamedArgs()
{
    return bindPropertyNamedArgument(PT_SECURITYPERMATTRIBUTE, PN_SKIPVERIFICATION, ConstValInit(true));
}

EXPR * FUNCBREC::bindFixedBufferArgs(MEMBVARSYM * field)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);

    // The first argument is the underlying element type of the
    // fixed buffer field, the second is the count of such elements
    EXPRTYPEOF *type_of;
    type_of = newExpr(NULL, EK_TYPEOF, GetReqPDT(PT_TYPE))->asTYPEOF();
    type_of->sourceType = field->type->asPTRSYM()->baseType();
    type_of->flags |= EXF_CANTBENULL;

    TYPESYM * param = GetReqPDT(PT_TYPEHANDLE);
    TypeArray * paramList = compiler()->getBSymmgr().AllocParams(1, &param);
    type_of->method = FindPredefMeth(NULL, PN_GETTYPEFROMHANDLE, GetReqPDT(PT_TYPE), paramList);

    if (type_of->method == NULL)
        return newError(NULL, NULL);

    EXPR * length = newExprConstant(NULL, GetReqPDT(PT_INT), field->constVal);

    return newExprBinop(NULL, EK_LIST, NULL, type_of, length);
}

void FUNCBREC::bindCharSetNamedArg(AGGTYPESYM * cls, int defaultCharSet, EXPR ** namedArguments)
{
    newList(bindFieldNamedArgument(cls, PN_CharSet, ConstValInit(defaultCharSet)), &namedArguments);
}

EXPR * FUNCBREC::bindCompilationRelaxationsAttributeArgs()
{
    // NoStringInterning
    return newExprConstant(NULL, compiler()->GetReqPredefType(PT_INT), ConstValInit(8));
}

EXPR * FUNCBREC::bindFieldNamedArgument(AGGTYPESYM *attributeType, PREDEFNAME pnFieldName, CONSTVAL value)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    ASSERT(attributeType);
    
    NAME * name = compiler()->namemgr->GetPredefName(pnFieldName);

    MemberLookup mem;

    if (!mem.Lookup(compiler(), attributeType, NULL, parentDecl, name, 0, MemLookFlags::UserCallable) || !mem.SymFirst()->isMEMBVARSYM()) {
        compiler()->Error(NULL, ERR_MissingPredefinedMember, attributeType->name->text, name->text);
        return newError(NULL, NULL);
    }

    FieldWithType fwt = mem.SwtFirst();

    EXPRFIELD * fieldExpr = newExpr(NULL, EK_FIELD, fwt.Field()->type)->asFIELD();
    fieldExpr->fwt = fwt;
    fieldExpr->object = NULL;

    return newExprBinop(NULL, EK_ASSG, fieldExpr->type, fieldExpr,
        newExprConstant(NULL, fieldExpr->type, value));
}

EXPR * FUNCBREC::bindPropertyNamedArgument(PREDEFTYPE ptAttributeType, PREDEFNAME pnPropertyName, CONSTVAL value)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    
    AGGTYPESYM * cls = compiler()->GetOptPredefType(ptAttributeType);
    ASSERT(cls);
    NAME * name = compiler()->namemgr->GetPredefName(pnPropertyName);

    MemberLookup mem;

    if (!mem.Lookup(compiler(), cls, NULL, parentDecl, name, 0, MemLookFlags::UserCallable) || !mem.SymFirst()->isPROPSYM()) {
        compiler()->Error(NULL, ERR_MissingPredefinedMember, cls->name->text, name->text);
        return newError(NULL, NULL);
    }

    PropWithType pwt = mem.SwtFirst();

    EXPRPROP * propExpr = newExpr(NULL, EK_PROP, pwt.Prop()->retType)->asPROP();
    propExpr->pwtSlot = pwt;
    if (pwt.Prop()->methGet)
        propExpr->mwtGet.Set(pwt.Prop()->methGet, pwt.Type());
    if (pwt.Prop()->methSet)
        propExpr->mwtSet.Set(pwt.Prop()->methSet, pwt.Type());
    propExpr->args = NULL;
    propExpr->object = NULL;

    EXPR * exprValue = newExprConstant(NULL, propExpr->type, value);
    EXPR * rval = newExprBinop(NULL, EK_ASSG, propExpr->type, propExpr, exprValue);

    return rval;
}

EXPR * FUNCBREC::bindRuntimeCompatibilityAttributeNamedArgs()
{
    return bindPropertyNamedArgument(PT_RUNTIMECOMPATIBILITY, PN_WRAPNONEXCEPTIONTHROWS, ConstValInit(true));
}


//-----------------------------------------------------------------------------
//
// returns true if a type is an attribute type
//
bool CLSDREC::isAttributeType(TYPESYM *type)
{
    compiler()->EnsureState(type);

    switch (type->getKind()) {
    case SK_AGGTYPESYM:
        if (type->getAggregate()->IsEnum()) {
            return true;
        }
        if (type->getAggregate()->isPredefined) {
            switch (type->getAggregate()->iPredef) {

            case PT_BYTE:
            case PT_SHORT:
            case PT_INT:
            case PT_LONG:
            case PT_FLOAT:
            case PT_DOUBLE:
            case PT_CHAR:
            case PT_BOOL:
            case PT_OBJECT:
            case PT_STRING:
            case PT_TYPE:

            case PT_SBYTE:
            case PT_USHORT:
            case PT_UINT:
            case PT_ULONG:

                return true;

            }
        }
        break;

    case SK_ARRAYSYM:
        if (type->asARRAYSYM()->rank == 1 && isAttributeType(type->asARRAYSYM()->elementType()) &&
            !type->asARRAYSYM()->elementType()->isARRAYSYM()) {

            return true;
        }

    default:
        break;
    }

    return false;
}

//-----------------------------------------------------------------------------

inline NAME * AttrBind::GetPredefName(PREDEFNAME pn)
{
    return compiler->namemgr->GetPredefName(pn);
}

inline bool AttrBind::isAttributeType(TYPESYM *type)
{
    return compiler->clsDeclRec.isAttributeType(type);
}

AttrBind::AttrBind(COMPILER * compiler, bool fEarly)
{
    this->sym = NULL;
    this->ek = (CorAttributeTargets)0;
    this->attrloc = AL_UNKNOWN;
    this->context = 0;
    this->attributeType = 0;
    this->customAttributeList = 0;
    this->hasLinkDemand = false;
    this->fEarly = fEarly;
    this->compiler = compiler;
}


void AttrBind::Init(SYM * sym)
{
    ASSERT(!sym->isAGGDECLSYM());
    Init(sym->getElementKind(), sym->isAGGSYM() ? NULL : sym->containingDeclaration()); // AGGSYMs don't have a unique context.
    this->sym = sym;
}

void AttrBind::Init(CorAttributeTargets ek, PARENTSYM * context)
{
    ASSERT(context == NULL || !context->isAGGSYM());
    
    this->ek = ek;
    this->context = context;

    // set the attrloc
    switch (ek)
    {
    case catReturnValue:
        this->attrloc = AL_RETURN;
        break;
    case catParameter:
        this->attrloc = AL_PARAM;
        break;
    case catEvent:
        this->attrloc = AL_EVENT;
        break;
    case catField:
        this->attrloc = AL_FIELD;
        break;
    case catProperty:
        this->attrloc = AL_PROPERTY;
        break;
    case catConstructor:
    case catMethod:
        this->attrloc = AL_METHOD;
        break;
    case catStruct:
    case catClass:
    case catEnum:
    case catInterface:
    case catDelegate:
        this->attrloc = AL_TYPE;
        break;
    case catModule:
        this->attrloc = AL_MODULE;
        break;
    case catAssembly:
        this->attrloc = AL_ASSEMBLY;
        break;
    case catGenericParameter:
        this->attrloc = AL_TYPEVAR;
        break;
    default:
        ASSERT(!"Bad CorAttributeTargets");
        break;
    }
}

inline static void StoreUINT64(BlobBldr & blob, UINT64 x)
{
    x = VAL64(x);
    blob.Add(&x, sizeof(x));
}

inline static void StoreDWORD(BlobBldr & blob, UINT32 x)
{
    x = VAL32(x);
    blob.Add(&x, sizeof(x));
}

inline static void StoreUSHORT(BlobBldr & blob, UINT16 x)
{
    x = VAL16(x);
    blob.Add(&x, sizeof(x));
}

//
// Stores a Packed string in blob
//
static void StoreString(BlobBldr & blob, PCWSTR str, int cchLen)
{
    // check for null string
    if (!str) {
        blob.Add(0xFF);
        return;
    }

    // convert the string to UTF8.
    int cbUTF8 = UTF8LengthOfUTF16(str, cchLen);

    // get the real packed length of the string.
    char packedLen[4];
    int cbPackedLen = CorSigCompressData(cbUTF8, (void*)packedLen);

    // copy in the packed string length
    blob.Add(packedLen, cbPackedLen);

    // copy in the converted string
    BYTE * buffer = blob.AddBuf(cbUTF8);
    if (buffer)
        UnicodeToUTF8(str, &cchLen, (char *)buffer, cbUTF8);
}

static void StoreString(BlobBldr & blob, NAME *name)
{
    PCWSTR sz;
    int length;
    if (name) {
        sz = name->text;
        length = (int)wcslen(sz);
    } else {
        sz = NULL;
        length = 0;
    }
    StoreString(blob, sz, length);
}

static void StoreString(BlobBldr & blob, STRCONST *constVal)
{
    PCWSTR sz;
    int length;
    if (constVal) {
        sz = constVal->text;
        length = constVal->length;
    } else {
        sz = NULL;
        length = 0;
    }
    StoreString(blob, sz, length);
}

//
// Stores a type name into a blob
//
static void StoreTypeName(BlobBldr & blob, TYPESYM *type, COMPILER * compiler, bool fOpenType)
{
    if (type) {
        TypeNameSerializer tns(compiler);
        BSTR bstr = tns.GetAssemblyQualifiedTypeName(type, fOpenType);

        StoreString(blob, bstr, SysStringLen(bstr));
        SysFreeString(bstr);
    }
    else
        StoreString(blob, NULL, 0);
}

#define PREDEFTYPEDEF(id, name, isRequired, isSimple, isNumeric, kind, ft, et, nicename, zero, qspec, asize, st, attr, arity, inmscorlib) \
    (CorSerializationType)st,
CorSerializationType serializationTypes[] =
{
#include "predeftype.h"
};

static void StoreEncodedType(BlobBldr & blob, TYPESYM *type, COMPILER *compiler)
{
    switch (type->getKind()) {
    case SK_ARRAYSYM:
        ASSERT(type->asARRAYSYM()->rank == 1);
        blob.Add(SERIALIZATION_TYPE_SZARRAY);
        StoreEncodedType(blob, type->asARRAYSYM()->elementType(), compiler);
        break;

    case SK_AGGTYPESYM:
        if (type->isEnumType()) {
            ASSERT(!type->asAGGTYPESYM()->typeArgsAll->size);
            blob.Add(SERIALIZATION_TYPE_ENUM);
            StoreTypeName(blob, type, compiler, false);
        } else {
            ASSERT(type->isPredefined() && !type->asAGGTYPESYM()->typeArgsAll->size);
            BYTE b = serializationTypes[type->getPredefType()];
            ASSERT(b != 0);
            blob.Add(b);
        }
        break;

    default:
        ASSERT(!"unrecognized attribute type");
        break;
    }
}

void AttrBind::ProcessAll(BASENODE *attributes)
{
    NODELOOP(attributes, ATTRDECL, decl)
        if (decl->location == attrloc)
        {
            NODELOOP(decl->pAttr, ATTR, attr)
                ProcessSingleAttr(attr);
            ENDLOOP;
        }
    ENDLOOP;

    PostProcess(attributes);
}

// Compile all the attributes on all declarations of an aggsym.
void AttrBind::ProcessAll(AGGSYM * aggsym)
{
    BASENODE * errorLocation = NULL;
    FOREACHAGGDECL(aggsym, aggdecl)
        context = aggdecl;
        
        BASENODE * attributes = aggdecl->getAttributesNode();

        if (errorLocation == NULL && attributes != NULL)
            errorLocation = attributes;  // Pick first attribute location for error reporting
        
        NODELOOP(attributes, ATTRDECL, decl)
            if (decl->location == attrloc)
            {
                NODELOOP(decl->pAttr, ATTR, attr)
                    ProcessSingleAttr(attr);
                ENDLOOP;
            }
        ENDLOOP;
    ENDFOREACHAGGDECL
    
    PostProcess(errorLocation);
}



// This is stuff that is done after compiling all the individual attributes.
void AttrBind::PostProcess(BASENODE * errorLocation)
{
    ValidateAttrs();

    // GetToken() forces us to emit a token, even in cases where none is needed
    // like for param tokens.  So if there are no security attributes, don't emit a token!
    if((!sym || !sym->isGLOBALATTRSYM()) && compiler->emitter.HasSecurityAttributes())
        compiler->emitter.EmitSecurityAttributes(errorLocation, GetToken());
}

void AttrBind::CompileFabricatedAttr()
{
    if ((sym->isFabricated || (sym->isMETHSYM() && sym->asMETHSYM()->isAnonymous())) && !sym->parent->isFabricated && sym->getTokenEmit()) {
#ifdef DEBUG
        // Verify that none of the parents are fabricated.
        for(SYM* temp = sym->parent; temp != NULL; temp = temp->parent)
            ASSERT(!temp->isFabricated);
#endif
        CompilerGeneratedAttrBind::EmitAttribute(compiler, GetToken());
    }
}

void AttrBind::ProcessSynthAttr(AGGTYPESYM * attributeType, EXPR* ctorExpression, EXPR * namedArguments)
{
    this->attributeType = attributeType;
    this->ctorExpression = ctorExpression;
    this->namedArguments = namedArguments;

    VerifyAndEmitCore(NULL);
}

void AttrBind::ProcessSingleAttr(ATTRNODE * attr)
{
    ASSERT(context);
    ASSERT(attr);       // if this hits you want to call VerifyAndEmitCore() instead

    attributeType = NULL;
    ctorExpression = NULL;
    namedArguments = NULL;

    //
    // need to resolve the attribute as regular class
    //
    BASENODE * name = attr->pName;
    TYPESYM * type = TypeBind::BindAttributeType(compiler, name, context);
	ASSERT(type);
    if (type->isERRORSYM())
        return;

    attributeType = type->asAGGTYPESYM();
    if (attributeType->getAggregate()->isAbstract) {
        compiler->Error(name, ERR_AbstractAttributeClass, ErrArgNameNode(name));
    }

    //
    // map from the attribute class back to a possible predefined attribute
    //
    predefinedAttribute = compiler->getBSymmgr().GetPredefAttr(attributeType);

    //
	if (BindAttr(attr) && !IsConditionalFalse()) {
        VerifyAndEmit(attr);
    }
}

// Tests whether this is a conditional attribute, and if so whether those conditional symbols are defined for the current input file.
// Returns true iff this is a conditional attribute which should not be emitted (i.e. the attribute is conditional on some symbols but none of them are defined at the current location).
bool AttrBind::IsConditionalFalse()
{
    INFILESYM * inputFile = context->getInputFile(); 
    ASSERT(inputFile);  // only way this can fire is if the context is set incorrectly

    bool fIsConditional = false;
    for (NAMELIST * list = compiler->clsDeclRec.GetConditionalSymbols(attributeType->getAggregate()); list; list = list->next) {
        fIsConditional = true;
        if (inputFile->isSymbolDefined(list->name))
            return false;
    }

    // return true only in the case where we actually saw some conditional symbols and none were defined in the current scope.
    return fIsConditional;    
}

bool AttrBind::BindAttr(ATTRNODE *attr)
{
    ASSERT(attributeType);
    ASSERT(attr);

    EXPRCALL * call;
    EXPR * namedArgs;

    AGGSYM * agg = attributeType->getAggregate();
    compiler->EnsureState(agg);

    if (!agg->isAttribute) {
        compiler->ErrorRef(attr->pName, ERR_NotAnAttributeClass, agg);
        return false;
    }

    // Don't check for allowable targets and multiple before the prepare stage.
    // This is since the attributeClass might not be set yet and we should always
    // go through the attributes during the compile phase anyway, so we should
    // catch all the errors then.
    if (compiler->AggStateMax() >= AggState::Prepared) {
        // Check whether the attribute can target this symbol type.
        if (!(agg->attributeClass & ek)) {
            ASSERT(agg->attributeClass);
            ErrorBadSymbolKind(attr->pName);
            return false;
        }

        // Check for invalid duplicate attributes.
        if (compiler->AggStateMax() >= AggState::Prepared && !agg->isMultipleAttribute) {
            SYMLIST ** plist = &customAttributeList;
            for ( ; *plist; plist = &(*plist)->next) {
                if ((*plist)->sym == attributeType) {
                    compiler->Error(attr->pName, ERR_DuplicateAttribute, ErrArgNameNode(attr->pName));
                    return false;
                }
            }

            // Not found, add it to our list of attributes for this symbol.
            compiler->getLSymmgr().AddToLocalSymList(attributeType, &plist);
        }
    }

    //
    // bind ctor args
    //
    namedArgs = NULL;
    call = compiler->funcBRec.bindAttribute(context, attributeType, attr, &namedArgs);

    // verify that args are constants
    bool badArg = false;
    if (call && call->mwi.Meth()) {
        EXPRLOOP(call->args, arg)
            if (!VerifyAttrArg(arg))
                badArg = true;
        ENDLOOP;
    }

    // verify named args
    EXPRLOOP(namedArgs, arg)
        if (!VerifyAttrArg(arg->asBIN()->p2))
            badArg = true;
    ENDLOOP;

    if (badArg || !call || !call->mwi.Meth()) {
        return false;
    }

    ctorExpression = call;
    namedArguments = namedArgs;

    hasLinkDemand |= isLinkDemand(attr);

    return true;
}

unsigned AttrBind::CountNamedArgs()
{
    unsigned numberOfNamedArguments = 0;
    EXPRLOOP(namedArguments, arg)
        numberOfNamedArguments += 1;
    ENDLOOP;
    
    return numberOfNamedArguments;
}

void AttrBind::VerifyAndEmit(ATTRNODE *attr)
{
    // If this assert fires you probably want to call VerifyAndEmitCore() instead.
    ASSERT(attr);

    if (predefinedAttribute != PA_COUNT)
        VerifyAndEmitPredef(attr);
    else
        VerifyAndEmitCore(attr);
}

void AttrBind::VerifyAndEmitPredef(ATTRNODE *attr)
{
    switch (predefinedAttribute) {
    case PA_KEYFILE:
        {
            if (!compiler->options.m_fCompileSkeleton || ctorExpression->kind != EK_CALL) break;
            EXPR *argument = ctorExpression->asCALL()->args;
            if (!argument || !argument->type->isPredefType(PT_STRING) || argument->kind != EK_CONSTANT || argument->asCONSTANT()->isNull() || argument->asCONSTANT()->getSVal().strVal->length == 0) break;
            CComBSTR bstrTemp(argument->asCONSTANT()->getSVal().strVal->length, argument->asCONSTANT()->getSVal().strVal->text);
            if (!PathIsRelativeW(bstrTemp)) break;
            CComBSTR bstrPrefixed = L"..\\";
            bstrPrefixed.Append(bstrTemp);
            argument->asCONSTANT()->getSVal() = compiler->funcBRec.createStringConstant(bstrPrefixed);
            break;
        }
    case PA_OBSOLETE:
        if (fEarly)
            ProcessObsoleteEarly(attr);
        break;

    case PA_CLSCOMPLIANT:
        if (fEarly)
            ProcessCLSEarly(attr);
        else
            VerifyCLS(attr);
        break;

    case PA_CONDITIONAL:
        if (fEarly)
            ProcessConditionalEarly(attr);
        break;

    case PA_TYPEFORWARDER:
        {
            // if a TypeForwardedToAttribute is specified, then emit an entry to the exported type table, instead of 
            // emitting the assembly level attribute.
            EXPR *argument = ctorExpression->asCALL()->args;
            ASSERT(argument->type->isPredefType(PT_TYPE) || !argument->isOK());

            TYPESYM * type = NULL;
            if (!getValue(argument, &type) || !type || !type->isAGGTYPESYM()) {
                compiler->Error(attr, ERR_InvalidFwdType);
                return;
            }

            AGGSYM *agg = type->asAGGTYPESYM()->getAggregate();
            if (agg->InAlias(kaidThisAssembly)) {
                // cannot emit a type forwarder to a type that is defined in this assembly.
                CError * err;
                err = compiler->MakeError(attr, ERR_ForwardedTypeInThisAssembly, type);
                if (!err)
                    return;

                if (agg->isSource)
                    compiler->AddLocationToError( err, ERRLOC(& compiler->getBSymmgr(), agg->DeclFirst()->getParseTree()) );
                else
                    compiler->AddLocationToError( err, ERRLOC(agg->DeclOnly()->getInputFile()) );
                compiler->SubmitError(err);
            } else if (agg->isNested())
                compiler->Error(attr, ERR_ForwardedTypeIsNested, agg, agg->parent);
            else if (agg->typeVarsThis->size > 0) { 
                compiler->Error(attr, ERR_FwdedGeneric, type);
            } else
                compiler->emitter.EmitTypeForwarder(type->asAGGTYPESYM());

            // We do not emit the actual attribute, so we just return instead.
            return;
        }
    default:
        break;
    }

    VerifyAndEmitCore(attr);
}


void AttrBind::VerifyAndEmitCore(ATTRNODE *attr)
{
    if (fEarly)
        return;

    BYTE rgb[256];
    BlobBldrNrHeap blob(&this->compiler->localSymAlloc, rgb, lengthof(rgb));

    // Serialize the ctor arguments into the buffer
    StoreUSHORT(blob, 1);
    EXPRLOOP(ctorExpression->asCALL()->args, arg)
        AddAttrArg(arg, blob);
    ENDLOOP;

    // Serialize the named arguments to the buffer
    StoreUSHORT(blob, (USHORT) CountNamedArgs());
    EXPRLOOP(namedArguments, arg)
        EXPRBINOP * assg = arg->asBIN();
        NAME *name = NULL;
        TYPESYM *type = NULL;

        // Get the name, type and store the field/property byte
        if (assg->p1->kind == EK_FIELD) {
            MEMBVARSYM * field = assg->p1->asFIELD()->fwt.Field();
            name = field->name;
            type = field->type;
            blob.Add(SERIALIZATION_TYPE_FIELD);
        } else {
            PROPSYM * property = assg->p1->asPROP()->pwtSlot.Prop();
            name = property->name;
            type = property->retType;
            blob.Add(SERIALIZATION_TYPE_PROPERTY);
        }

        // Member type, name and value
        StoreEncodedType(blob, type, compiler);
        StoreString(blob, name);
        AddAttrArg(assg->p2, blob);

    ENDLOOP;

    // Write the attribute to the metadata
    if (!compiler->options.m_fNOCODEGEN &&
        (!compiler->options.m_fCompileSkeleton || attributeType->isPredefined()))
    {
        compiler->emitter.EmitCustomAttribute(attr, GetToken(), ctorExpression->asCALL()->mwi.Meth(), blob.Buffer(), blob.Length());
    }
}

// Returns false on error.
bool AttrBind::VerifyAttrArg(EXPR *arg)
{
    TYPESYM *type = arg->type;

REDO:
    switch (type->getKind()) {
    case SK_AGGTYPESYM:
    {
        if (type->asAGGTYPESYM()->typeArgsAll->size > 0) {
            compiler->Error(arg->tree, ERR_BadAttributeParam);
            return false;
        }

        AGGSYM *cls = type->getAggregate();
        if (cls->IsEnum()) {
            // REVIEW ShonK: What are these UNDONEs about?
            // Where do we verify that the enum is public as stated in section 17.1.3?

            type = type->underlyingType();
            goto REDO;
        } else if (!cls->isPredefined || compiler->getBSymmgr().GetAttrArgSize((PREDEFTYPE)cls->iPredef) == 0) {
            compiler->Error(arg->tree, ERR_BadAttributeParam);
            return false;
        }

        if (compiler->getBSymmgr().GetAttrArgSize((PREDEFTYPE)cls->iPredef) < 0) {
            switch (cls->iPredef)
            {
            case PT_STRING:
                if (arg->kind != EK_CONSTANT) {
                    compiler->Error(arg->tree, ERR_BadAttributeParam);
                    return false;
                }
                break;

            case PT_OBJECT:
                if (arg->kind != EK_CAST) {
                    if (arg->kind == EK_CONSTANT) {
                        ASSERT(arg->isNull());
                        break;
                    }
                    compiler->Error(arg->tree, ERR_BadAttributeParam);
                    return false;
                }

                // implicit cast of something to object
                // need to encode the underlying object(enum, type, string)
                return VerifyAttrArg(arg->asCAST()->p1);

            case PT_TYPE:
                switch (arg->kind) {
                case EK_TYPEOF:
                    {
                        TYPESYM * typeSrc = arg->asTYPEOF()->sourceType;
                        // Can't contain type variables!
                        if (!(arg->flags & EXF_OPENTYPE) && BSYMMGR::TypeContainsTyVars(typeSrc, NULL)) {
                            compiler->ErrorRef(arg->tree, ERR_AttrArgWithTypeVars, typeSrc);
                            return false;
                        }
                    }
                    break;

                case EK_CONSTANT:
                    break;

                default:
                    compiler->Error(arg->tree, ERR_BadAttributeParam);
                    return false;
                }
                break;

            default:
                ASSERT(!"bad variable size attribute argument type");
            }
        } else if (arg->kind != EK_CONSTANT) {
            compiler->Error(arg->tree, ERR_BadAttributeParam);
            return false;
        }
        break;
    }

    case SK_ARRAYSYM:
    {
        PARRAYSYM arr;
        arr = type->asARRAYSYM();

        if (arr->rank != 1 ||                                   // Single Dimension
            arr->elementType()->isARRAYSYM() ||          // Of non-array
            !compiler->clsDeclRec.isAttributeType(arr->elementType()) ||             // valid attribute argument type
            (arg->kind != EK_ARRINIT && arg->kind != EK_CONSTANT)) {                    // which is constant

            compiler->Error(arg->tree, ERR_BadAttributeParam);
            return false;
        }

        if (arg->kind == EK_ARRINIT) {
            EXPR * param, *next = arg->asARRINIT()->args;
            while (next) {
                if (next->kind == EK_LIST) {
                    param = next->asBIN()->p1;
                    next = next->asBIN()->p2;
                } else {
                    param = next;
                    next = NULL;
                }
                if (!VerifyAttrArg(param))
                    return false;
            }
        } else {
            ASSERT(arg->kind == EK_CONSTANT);
        }

        // can't use array types in CLS compliant attributes
        if (!fEarly && compiler->AllowCLSErrors() && compiler->CheckSymForCLS(sym, false)) {
            compiler->Error(arg->tree, WRN_CLS_ArrayArgumentToAttribute);
        }
        break;
    }

    default:
        compiler->Error(arg->tree, ERR_BadAttributeParam);
        return false;
    }

    return true;
}

NAME * AttrBind::getNamedArgumentName(EXPR *expr)
{
    ASSERT(expr->kind == EK_ASSG);

    expr = expr->asBIN()->p1;

    switch (expr->kind) {                
    case EK_FIELD:
        return expr->asFIELD()->fwt.Field()->name;
    case EK_PROP:
        return expr->asPROP()->pwtSlot.Prop()->name;
    default:
        ASSERT(!"bad expr kind");
        return NULL;
    };
}

EXPR * AttrBind::getNamedArgumentValue(EXPR *expr)
{
    ASSERT(expr->kind == EK_ASSG);
    
    return expr->asBIN()->p2;
}

const PCWSTR attributeTargetStrings[] =
{
    L"assembly",            // catAssembly      = 0x0001, 
    L"module",              // catModule        = 0x0002,
    L"class",               // catClass         = 0x0004,
    L"struct",              // catStruct        = 0x0008,
    L"enum",                // catEnum          = 0x0010,
    L"constructor",         // catConstructor   = 0x0020,
    L"method",              // catMethod        = 0x0040,
    L"property, indexer",   // catProperty      = 0x0080,
    L"field",               // catField         = 0x0100,
    L"event",               // catEvent         = 0x0200,
    L"interface",           // catInterface     = 0x0400,
    L"param",               // catParameter     = 0x0800,
    L"delegate",            // catDelegate      = 0x1000,
    L"return",              // catReturn        = 0x2000,
    L"type parameter",      // catGenericParameter = 0x4000,

    0
};

//
// convert valid targets to string
//
void BuildAttrTargetString(__out_ecount(cchBuffer) PWSTR buffer, size_t cchBuffer, CorAttributeTargets validTargets)
{
    ASSERT(cchBuffer > 1);
    buffer[0] = '\0';

    const PCWSTR *target = attributeTargetStrings;
    while (validTargets && *target)
    {
        if (validTargets & 1) {
            if (*buffer) {
                StringCchCatW(buffer, cchBuffer, L", ");
            }
            StringCchCatW(buffer, cchBuffer, *target);
        }

        validTargets = (CorAttributeTargets) (validTargets >> 1);
        target += 1;
    }
}

void AttrBind::ErrorBadSymbolKind(BASENODE *tree)
{
    //
    // get valid targets
    //
    CorAttributeTargets validTargets; 
    validTargets = this->attributeType->getAggregate()->attributeClass;

    // Check to make sure we aren't incorrectly reporting an error
    ASSERT((ek & validTargets) == 0);

    //
    // convert valid targets to string
    //
    WCHAR buffer[1024];
    BuildAttrTargetString(buffer, lengthof(buffer), validTargets);
    compiler->Error(tree, ERR_AttributeOnBadSymbolType, ErrArgNameNode(tree), buffer);
}

void AttrBind::ValidateAttrs()
{
    // default implementation does nothing
    // may be overridden  
}

void AttrBind::AddAttrArg(EXPR * arg, BlobBldr & blob)
{
    TYPESYM *type = arg->type;
    unsigned int iPredef;

    switch (type->getKind()) {
    case SK_AGGTYPESYM:
    {
        if (type->isEnumType()) {
            iPredef = type->underlyingEnumType()->getPredefType();
        } else {
            ASSERT(type->isPredefined());
            iPredef = type->getPredefType();
        }

        ASSERT(compiler->getBSymmgr().GetAttrArgSize((PREDEFTYPE)iPredef) != 0);
        switch (iPredef)
        {
        case PT_BOOL:
        case PT_SBYTE:
        case PT_BYTE:   
            _ASSERTE(compiler->getBSymmgr().GetAttrArgSize((PREDEFTYPE)iPredef) == 1);
            blob.Add(arg->asCONSTANT()->getVal().iVal);
            break;

        case PT_SHORT:
        case PT_CHAR:
        case PT_USHORT:
            _ASSERTE(compiler->getBSymmgr().GetAttrArgSize((PREDEFTYPE)iPredef) == 2);
            StoreUSHORT(blob, arg->asCONSTANT()->getVal().iVal);
            break;

        case PT_INT:
        case PT_UINT:
            _ASSERTE(compiler->getBSymmgr().GetAttrArgSize((PREDEFTYPE)iPredef) == 4);
            StoreDWORD(blob, arg->asCONSTANT()->getVal().iVal);
            break;

        case PT_FLOAT:
            {
                FLOAT flt = (FLOAT) *arg->asCONSTANT()->getVal().doubleVal;
                _ASSERTE(compiler->getBSymmgr().GetAttrArgSize((PREDEFTYPE)iPredef) == 4);
                StoreDWORD(blob, (DWORD&)flt);
                break;
            }

        case PT_LONG:
        case PT_ULONG:
            _ASSERTE(compiler->getBSymmgr().GetAttrArgSize((PREDEFTYPE)iPredef) == 8);
            StoreUINT64(blob, *arg->asCONSTANT()->getVal().longVal);
            break;

        case PT_DOUBLE:
            {
                DOUBLE dbl = *arg->asCONSTANT()->getVal().doubleVal;
                _ASSERTE(compiler->getBSymmgr().GetAttrArgSize((PREDEFTYPE)iPredef) == 8);
                StoreUINT64(blob, (UINT64&)dbl);
                break;
            }

        case PT_STRING:
            StoreString(blob, arg->asCONSTANT()->getSVal().strVal);
            break;

        case PT_OBJECT:
            // This must be a constant argument to an Variant.OpImplicit call or a null.
            if (arg->kind == EK_CAST) {
                // implicit cast of something to object
                // need to encode the underlying object(enum, type, string)
                arg = arg->asCAST()->p1;
                StoreEncodedType(blob, arg->type, compiler);
                AddAttrArg(arg, blob);
            }
            else {
                ASSERT(arg->isNull());
                blob.Add(SERIALIZATION_TYPE_STRING);
                StoreString(blob, NULL, 0);
            }
            break;

        case PT_TYPE:
            switch (arg->kind) {
            default:
                ASSERT(!"Unknown constant of type type");
                // Fall through
            case EK_CONSTANT:
                // this handles the 'null' type constant
                StoreTypeName(blob, NULL, compiler, false);
                break;

            case EK_TYPEOF:
                {
                    TYPESYM * type = arg->asTYPEOF()->sourceType;
                    if (arg->flags & EXF_OPENTYPE) {
                        ASSERT(type->isAGGTYPESYM() && type->asAGGTYPESYM()->typeArgsAll->size > 0 &&
                            type->asAGGTYPESYM()->typeArgsAll->Item(type->asAGGTYPESYM()->typeArgsAll->size - 1)->isUNITSYM());
                    }
                    else {
                        ASSERT(!BSYMMGR::TypeContainsTyVars(type, NULL));
                    }
                    StoreTypeName(blob, type, compiler, !!(arg->flags & EXF_OPENTYPE));
                }
                break;
            }
            break;

        default:
            ASSERT(!"bad variable size attribute argument type");
        }
        break;
    }

    case SK_ARRAYSYM:
    {
        PARRAYSYM arr;
        arr = type->asARRAYSYM();

        ASSERT(arr->rank == 1 &&
                    (arr->elementType()->isPredefined() || arr->elementType()->isEnumType()));

        if (arg->kind == EK_CONSTANT) {
            StoreDWORD(blob, 0xFFFFFFFF);
        } else {
            StoreDWORD(blob, arg->asARRINIT()->dimSize);
            EXPR * param, *next = arg->asARRINIT()->args;
            while (next) {
                if (next->kind == EK_LIST) {
                    param = next->asBIN()->p1;
                    next = next->asBIN()->p2;
                } else {
                    param = next;
                    next = NULL;
                }
                AddAttrArg(param, blob);
            }
        }
        break;
    }

    default:
        ASSERT(!"unexpected attribute argument type");
        break;
    }
}

bool AttrBind::getValue(EXPR *argument, bool * rval)
{
    return compiler->funcBRec.getAttributeValue(context, argument, rval);
}

bool AttrBind::getValue(EXPR *argument, int * rval)
{
    return compiler->funcBRec.getAttributeValue(context, argument, rval);
}

bool AttrBind::getValue(EXPR *argument, STRCONST ** rval)
{
    return compiler->funcBRec.getAttributeValue(context, argument, rval);
}

bool AttrBind::getValue(EXPR *argument, TYPESYM **type)
{
    return compiler->funcBRec.getAttributeValue(context, argument, type);
}

STRCONST * AttrBind::getKnownString(EXPR *expr)
{
    return expr->asCONSTANT()->getSVal().strVal;
}

bool AttrBind::getKnownBool(EXPR *expr)
{
    return expr->asCONSTANT()->getVal().iVal ? true : false;
}


void AttrBind::ProcessObsoleteEarly(ATTRNODE * attr)
{
    ASSERT(fEarly);

    //
    // crack the ctor arguments to get the obsolete information
    //
    PCWSTR msg = NULL;
    bool fError = false;
    bool fFirst = true;

    EXPRLOOP(ctorExpression->asCALL()->args, argument)
        if (fFirst) {
            STRCONST * message = NULL;
            if (!getValue(argument, &message))
                break;
            if (message) {
                // add string to global heap so that it doesn't go away
                msg = compiler->namemgr->AddString(message->text, message->length)->text;
            }
            fFirst = false;
        }
        else {
            getValue(argument, &fError);
            break;
        }
    ENDLOOP;

    sym->SetDeprecated(true, fError, msg);
}


void AttrBind::ProcessCLSEarly(ATTRNODE * attr)
{
    ASSERT(fEarly);

    sym->hasCLSattribute = true;

    // Mark it according to the argument
    EXPRLOOP(ctorExpression->asCALL()->args, argument)
        if (argument->type->isPredefType(PT_BOOL)) {
            bool value;
            if (!getValue(argument, &value)) {
                break;
            }
            sym->isCLS = value;
        }
    ENDLOOP;
}

OUTFILESYM *AttrBind::getOutputFile()
{
    return sym->GetSomeInputFile()->getOutputFile();
}

void AttrBind::VerifyCLS(ATTRNODE * attr)
{
    if (sym->isGLOBALATTRSYM() || !compiler->AllowCLSErrors())
        return;

    OUTFILESYM * outfile = getOutputFile();

    if (!outfile->hasCLSattribute) {
        if (sym->isCLS) {
            // It's illegal to have CLSCompliant(true) when the assembly is not marked
            compiler->Error(attr, WRN_CLS_AssemblyNotCLS, sym);
        } else {
            compiler->Error(attr, WRN_CLS_AssemblyNotCLS2, sym);
        }
    } else if (!sym->hasExternalAccess()) {
        // Why put CLSCompliant on a private thing?
        compiler->Error(attr, WRN_CLS_MeaninglessOnPrivateType, sym);
    } else if (sym->isCLS) {
        PARENTSYM * parent = sym->parent;
        if (parent->isAGGSYM() && !compiler->CheckSymForCLS(parent, false)) {
            // It's illegal to have CLSCompliant(true) inside a non-compliant type
            // We know the parent is non-compliant because the assembly/module is.
            compiler->Error(attr, WRN_CLS_IllegalTrueInFalse, sym, parent);
            sym->isCLS = false;
        }
    }
}

bool AttrBind::isLinkDemand(ATTRNODE * attr)
{
    if (attributeType->isSecurityAttribute()) {
        EXPRLOOP(ctorExpression->asCALL()->args, argument)

            if (argument->type->isPredefType(PT_SECURITYACTION)) {
                int value;
                if (!getValue(argument, &value)) {
                    break;
                }
                return (value == dclLinktimeCheck);
            }
            break;
        ENDLOOP;
    }
    return false;
}

void AttrBind::ProcessConditionalEarly(ATTRNODE * attr)
{
    ASSERT(fEarly);

    // Validate arguments ...
    STRCONST * conditionalValue = NULL;
    EXPRLOOP(ctorExpression->asCALL()->args, argument)
        if (conditionalValue) {
            VSFAIL("We bound to a ConditionalAttribute constructor that takes more than 1 argument.");
            break;
        }
        if (!getValue(argument, &conditionalValue))
            break;

        // Convert the string to a name and return it
        if (compiler->checkForValidIdentifier(conditionalValue, true, argument->tree, ERR_BadArgumentToAttribute, attributeType)) {
            NAME *name = compiler->namemgr->AddString(conditionalValue->text, conditionalValue->length);
            AddConditionalName(name);
        }
    ENDLOOP;
}

void AttrBind::AddDefaultCharSet()
{
    OUTFILESYM *outfile = getOutputFile();
    if (outfile->defaultCharSet > 1) {
        // This means it is CharSet.Ansi, CharSet.Unicode, or CharSet.Auto

        // Can't use EXPRLOOP because we want to keep the end-of-list pointer
        // so we can add the CharSet property if needed
        EXPR * cur = namedArguments;
        EXPR ** lastArg = &namedArguments;
        while (cur) {
            EXPR * arg;
            if (cur->kind == EK_LIST) {
                arg = cur->asBIN()->p1;
                lastArg = &cur->asBIN()->p2;
                cur = cur->asBIN()->p2;
            } else {
                arg = cur;
                cur = NULL;
            } 

            NAME *name = getNamedArgumentName(arg);
            if (name == GetPredefName(PN_CharSet))
                return;
        }

        compiler->funcBRec.bindCharSetNamedArg(attributeType, outfile->defaultCharSet, lastArg);
    }
}

/***************************************************************************************************
    Methods for DefaultAttrBind.
***************************************************************************************************/

void DefaultAttrBind::CompileEarly(COMPILER * compiler, SYM * sym)
{
    BASENODE * nodeAttrs = NULL;

    if (!sym->isAGGSYM()) {
        nodeAttrs = sym->getAttributesNode();
        if (!nodeAttrs)
            return;
    }

    DefaultAttrBind attrbind(compiler, sym, true);

    if (sym->isAGGSYM()) {
        // AGGSYMs must accumulate from all the declarations.
        attrbind.ProcessAll(sym->asAGGSYM());
    }
    else {
        attrbind.ProcessAll(nodeAttrs);
    }
}

void DefaultAttrBind::CompileAndEmit(COMPILER * compiler, SYM * sym)
{
    ASSERT(!sym->isAGGSYM()); // Can't call getAttributesNode on AGGSYMs.

    BASENODE * nodeAttrs = sym->getAttributesNode();
    if (!nodeAttrs)
        return;

    UnknownAttrBind::Compile(compiler, sym->containingDeclaration(), nodeAttrs);
    DefaultAttrBind attrbind(compiler, sym, false);
    attrbind.ProcessAll(nodeAttrs);
}

bool DefaultAttrBind::BindAttr(ATTRNODE *attr)
{
    if (!fEarly ||
        predefinedAttribute == PA_OBSOLETE ||
        predefinedAttribute == PA_CLSCOMPLIANT)
    {
        return AttrBind::BindAttr(attr);
    }

    return false;
}


//-----------------------------------------------------------------------------

void AggAttrBind::Compile(COMPILER *compiler, AGGSYM *cls, AGGINFO *info)
{
    NAME * defaultMemberName = NULL;
    // Do we have any indexers? If so, we must emit a default member name.
    if (cls->IsStruct() || cls->IsClass() || cls->IsInterface()) {
        PROPSYM * indexer = compiler->getBSymmgr().LookupAggMember(compiler->namemgr->GetPredefName(PN_INDEXERINTERNAL), 
                                                             cls, MASK_PROPSYM)->asPROPSYM();
        while (indexer) {
            NAME * indexerName = indexer->getRealName();

            // All indexers better have the same metadata name. 
            if (defaultMemberName && indexerName != defaultMemberName) 
                compiler->Error(indexer->getParseTree(), ERR_InconsistantIndexerNames);
            defaultMemberName = indexerName;

            indexer = compiler->getBSymmgr().LookupNextSym(indexer, cls, MASK_PROPSYM)->asPROPSYM();
        }
    }
        

    //
    // do we have attributes
    //

    // Handle unknown attribute locations.
    FOREACHAGGDECL(cls, clsdecl)    
        BASENODE *attributes = clsdecl->getAttributesNode();
        UnknownAttrBind::Compile(compiler, clsdecl->DeclPar(), attributes);
    ENDFOREACHAGGDECL

    // Bind attributes.
    AggAttrBind attrbind(compiler, cls, info, defaultMemberName);
    attrbind.ProcessAll(cls);

    // If we have any indexers in us, emit the "DefaultMember" attribute with the name of the indexer.
    if (defaultMemberName) {
        attrbind.defaultMemberName = NULL;
        attrbind.ProcessSynthAttr(compiler->GetReqPredefType(PT_DEFAULTMEMBER), 
            compiler->funcBRec.bindStringPredefinedAttribute(PT_DEFAULTMEMBER, defaultMemberName->text), NULL);
    }

    //bind attributes on type variables
    if (cls->typeVarsAll->size > 0)
        TypeVarAttrBind::CompileParamList(compiler, cls->DeclFirst(), cls->typeVarsAll, cls->toksEmitTypeVars);

    attrbind.CompileFabricatedAttr();
}

AggAttrBind::AggAttrBind(COMPILER *compiler, AGGSYM *cls, AGGINFO *info, NAME * defaultMemberName) :
    AttrBind(compiler, false)
{
    this->cls = cls;
    this->info = info;
    this->defaultMemberName = defaultMemberName;
    Init(cls);
}

void AggAttrBind::VerifyAndEmitPredef(ATTRNODE *attr)
{
    BASENODE *name = attr->pName;

    switch (predefinedAttribute) {
    case PA_DEFAULTMEMBER:
        if (defaultMemberName) {
            compiler->Error(attr->pName, ERR_DefaultMemberOnIndexedType);
            return;
        }
        break;

    case PA_ATTRIBUTEUSAGE:
        if (!cls->isAttribute) {
            compiler->Error(name, ERR_AttributeUsageOnNonAttributeClass, ErrArgNameNode(name));
            return;
        }
        break;

    case PA_STRUCTLAYOUT:
        ProcessStructLayout(attr);
        return;

    case PA_COMIMPORT:
        info->isComimport = true;
        cls->isComImport = true;
        if (cls->IsClass()) {
            if (!cls->baseClass->isPredefType(PT_OBJECT)) {
                compiler->ErrorRef(NULL, ERR_ComImportWithBase, cls);
            }

            // Can only have a compiler generated constructor
            for (METHSYM *ctor = compiler->getBSymmgr().LookupAggMember(GetPredefName(PN_CTOR), cls, MASK_METHSYM)->asMETHSYM();
                ctor;
                ctor = ctor->nextSameName->asMETHSYM())
            {
                if (ctor->isCompilerGeneratedCtor()) {
                    // Whose implementation is supplied by COM+
                    ctor->isExternal = true;
                    ctor->isSysNative = true;

                    // We need to reset the method's flags
                    compiler->emitter.ResetMethodFlags(ctor);
                }
                else {
                    compiler->ErrorRef(NULL, ERR_ComImportWithUserCtor, ctor);
                }
            }
        }
        break;

    case PA_GUID:             
        info->hasUuid = true;
        break;

    case PA_REQUIRED:
        // Predefined attribute which is not allowed in C#
        compiler->Error(name, ERR_CantUseRequiredAttribute, ErrArgNameNode(name));
        return;

    case PA_UNMANAGEDFUNCTIONPOINTER:
        AddDefaultCharSet();
        break;

    default:
        break;
    }

    AttrBind::VerifyAndEmitPredef(attr);
}

// Check for SequentialLayout attribute on a partial class, and give a warning if it exists.
void AggAttrBind::CheckSequentialOnPartialType(ATTRNODE * attr)
{
    AGGDECLSYM *declFirst = NULL;
    bool fGiveWarning = false;
    FOREACHCHILD(cls, memb)
        if (memb->isMEMBVARSYM() && !memb->asMEMBVARSYM()->isStatic) {
            AGGDECLSYM *decl = memb->containingDeclaration()->asAGGDECLSYM();
            ASSERT(decl);
            if (!declFirst)
                declFirst = decl;
            else if (declFirst != decl) {
                fGiveWarning = true;
                break;
            }
        
        }
    ENDFOREACHCHILD

    // only give a warning if there are multiple aggdecls with fields in them.
    if (!fGiveWarning)
        return;

    CError * err;
    err = compiler->MakeError((attr ? attr->pName : NULL), WRN_SequentialOnPartialClass, cls);
    if (!err)
        return;

    FOREACHAGGDECL(cls, decl)
        ASSERT(decl->isPartial);
        compiler->AddLocationToError( err, ERRLOC(decl->getInputFile(), decl->getParseTree()));
    ENDFOREACHAGGDECL
    compiler->SubmitError(err);
}

void AggAttrBind::ProcessStructLayout(ATTRNODE * attr)
{
    if (!cls->IsClass() && !cls->IsStruct()) {
        ErrorBadSymbolKind(attr->pName);
        return;
    }

    info->hasStructLayout = true;

    int layoutKind;
    EXPRLOOP(ctorExpression->asCALL()->args, argument)
        getValue(argument, &layoutKind);
        break;
    ENDLOOP;

    if (layoutKind == compiler->clsDeclRec.GetLayoutKindValue(PN_EXPLICIT))
        info->hasExplicitLayout = true;
    else if ((cls->DeclFirst()->isPartial) && (layoutKind == compiler->clsDeclRec.GetLayoutKindValue(PN_SEQUENTIAL)))
        CheckSequentialOnPartialType(attr);


    //
    // if the class has structlayout then it is
    // expected to be used in Interop
    //
    // Disable warnings for unassigned members for all interop structs here
    //
    cls->hasExternReference = true;

    VerifyAndEmitCore(attr);
}

void AggAttrBind::ValidateAttrs()
{
    if (info->isComimport && !info->hasUuid) {
        compiler->ErrorRef(NULL, ERR_ComImportWithoutUuidAttribute, cls);
    }

    // struct layout defaults to sequential for structs, if the user didn't explicitly specify
    if (!info->hasStructLayout && cls->IsStruct())
    {
        if (cls->DeclFirst()->isPartial)
            CheckSequentialOnPartialType(NULL);
        // structs with 0 instance fields must be given an explicit size of 0
        bool hasInstanceVar = false;
        FOREACHCHILD(cls, child)
            if (child->isMEMBVARSYM() && !child->asMEMBVARSYM()->isStatic) {
                hasInstanceVar = true;
                break;
            }
        ENDFOREACHCHILD

        AGGTYPESYM *symStructLayout = compiler->GetOptPredefType(PT_STRUCTLAYOUT);
        if (symStructLayout != NULL) {
            ProcessSynthAttr(symStructLayout,
                compiler->funcBRec.bindSimplePredefinedAttribute(PT_STRUCTLAYOUT, compiler->funcBRec.bindStructLayoutArgs()),
                compiler->funcBRec.bindStructLayoutNamedArgs(hasInstanceVar));
        }
    }
}

//-----------------------------------------------------------------------------

TypeVarAttrBind::TypeVarAttrBind(COMPILER * compiler, TYVARSYM * var, PARENTSYM * context, mdToken tok) : AttrBind(compiler, false)
{
    this->tok = tok;
    this->sym = var;
    Init(catGenericParameter, context);
}


void TypeVarAttrBind::CompileParamList(COMPILER *compiler, PARENTSYM *declFirst, TypeArray *tyvars, mdToken *tokens)
{
    // loop through tyvars
    for (int i = 0; i < tyvars->size; i++)  {
        TYVARSYM *var = tyvars->ItemAsTYVARSYM(i);

        TypeVarAttrBind attrbind(compiler, var, NULL, tokens[var->indexTotal]);

        // loop through all attribute nodes for each tyvar
        FOREACHATTRLIST(var->attributeList, listNode)
            // there was an error when we tried to bind this attribute initially, so we will not attempt again.
            if (listNode->fHadError)
                continue;

            attrbind.context = listNode->context;

            int ec = compiler->ErrorCount();
            attrbind.ProcessAll(listNode->attr);
            if (ec != compiler->ErrorCount()) {
                // there was an error, mark it so we don't try and bind this attribute again for nested types
                listNode->fHadError = true;
            }

        ENDFOREACHATTRLIST
    }
}

//-----------------------------------------------------------------------------

void EarlyAggAttrBind::Compile(COMPILER *compiler, AGGSYM *cls)
{
    //
    // do we have attributes
    //
    EarlyAggAttrBind attrbind(compiler, cls);
    attrbind.ProcessAll(cls);
}

EarlyAggAttrBind::EarlyAggAttrBind(COMPILER *compiler, AGGSYM *cls) :
    AttrBind(compiler, true)
{
    this->cls = cls;
    this->pnlstCond = &cls->conditionalSymbols;
    Init(cls);
}

bool EarlyAggAttrBind::BindAttr(ATTRNODE *attr)
{
    // Primarily attributes on attributes.
    switch (predefinedAttribute) {
    default:
        if (!attributeType || !attributeType->isSecurityAttribute())
            return false;
        break;

    case PA_ATTRIBUTEUSAGE:
    case PA_OBSOLETE:
    case PA_CLSCOMPLIANT:
    case PA_COCLASS:
    case PA_COMIMPORT:
    case PA_CONDITIONAL:
        break;
    }

    return AttrBind::BindAttr(attr);
}

void EarlyAggAttrBind::ValidateAttrs()
{
    cls->hasLinkDemand = hasLinkDemand;

    if (!cls->isComImport && cls->IsInterface() && cls->comImportCoClass) {
        compiler->ErrorRef(NULL, WRN_CoClassWithoutComImport, cls);
        cls->underlyingType = NULL;
        cls->comImportCoClass = NULL;
    }
}

void EarlyAggAttrBind::VerifyAndEmitPredef(ATTRNODE *attr)
{
    switch (predefinedAttribute)
    {
    case PA_COCLASS:
        ProcessCoClass(attr);
        return;

    case PA_COMIMPORT:
        cls->isComImport = true;
        return;

    case PA_CONDITIONAL:
        if (!cls->isAttribute)
            compiler->Error(attr->pName, ERR_ConditionalOnNonAttributeClass, attributeType);
        break;

    case PA_ATTRIBUTEUSAGE:
        {
            BASENODE *name = attr->pName;
            if (cls->attributeClass) {
                if (cls->iPredef != PT_ATTRIBUTEUSAGE &&
                    cls->iPredef != PT_CONDITIONAL &&
                    cls->iPredef != PT_OBSOLETE &&
                    cls->iPredef != PT_CLSCOMPLIANT)
                {
                    compiler->Error(name, ERR_DuplicateAttribute, ErrArgNameNode(name));
                    return;
                }

                // attributeusage, conditional and obsolete attributes are 'special'
                cls->attributeClass = (CorAttributeTargets) 0;
            }
            bool foundAllowMultiple = false;

            EXPRLOOP(ctorExpression->asCALL()->args, argument)
                if (cls->attributeClass) {
                    // using a constructor which should be removed
                    compiler->Error(attr, ERR_DeprecatedSymbolStr,
                        ctorExpression->asCALL()->mwi, L"Use single argument contsructor instead");
                    return;
                }
                int val;
                if (getValue(argument, &val)) {
                    if (val == 0 || (val & ~catAll)) {
                        compiler->Error(argument->tree, ERR_InvalidAttributeArgument, ErrArgNameNode(attr->pName));
                        cls->attributeClass = catAll;
                    }
                    else
                        cls->attributeClass = (CorAttributeTargets)val;
                }
            ENDLOOP;

            EXPRLOOP(namedArguments, argument)
                NAME *name = getNamedArgumentName(argument);
                EXPR *value = getNamedArgumentValue(argument);
                int val;

                if (name == GetPredefName(PN_VALIDON)) {
                    if (cls->attributeClass != 0) {
                        compiler->Error(argument->tree, ERR_DuplicateNamedAttributeArgument, name);
                    } else if (!getValue(value, &val)) {
                        // error already reported
                    } else if (val == 0 || (val & ~catAll)) {
                        compiler->Error(argument->tree, ERR_InvalidNamedArgument, name);
                        cls->attributeClass = catAll;
                    }
                    else
                        cls->attributeClass = (CorAttributeTargets)val;
                } else if (name == GetPredefName(PN_ALLOWMULTIPLE)) {
                    bool isMultiple = false;

                    if (foundAllowMultiple) {
                        compiler->Error(argument->tree, ERR_DuplicateNamedAttributeArgument, name);
                    } else if (!getValue(value, &isMultiple)) {
                        // error already reported
                    } else {
                        cls->isMultipleAttribute = isMultiple;
                        foundAllowMultiple = true;
                    }
                } else if (name == GetPredefName(PN_INHERITED)) {
                } else {
                    ASSERT(!"unknown named argument to attributeusage attribute");
                }
            ENDLOOP;

            // Set default allow on
            if (cls->attributeClass == 0) {
                cls->attributeClass = catAll;
            }
        }
        return;

    default:
        break;
    }

    AttrBind::VerifyAndEmitPredef(attr);
}

void EarlyAggAttrBind::AddConditionalName(NAME * name)
{
    compiler->getBSymmgr().AddToGlobalNameList(name, &pnlstCond);
}

void EarlyAggAttrBind::ProcessCoClass(ATTRNODE * attr)
{
    //
    // if the attribute is on an interface, then we do some special processing
    //
    if (cls->IsInterface()) {
        //
        // set the baseClass to point to the CoClass
        // and fill in the comImportCoClass string
        //
        EXPRLOOP(ctorExpression->asCALL()->args, argument)

            if (argument->type->isPredefType(PT_TYPE)) {
                TYPESYM * value;
                if (!getValue(argument, &value)) {
                    break;
                }
                if (value && value->isClassType()) {
                    WCHAR buffer[MAX_FULLNAME_SIZE];
                    if (MetaDataHelper::GetFullName(value->getAggregate(), buffer, lengthof(buffer))) {
                        cls->underlyingType = value->asAGGTYPESYM();
                        cls->comImportCoClass = compiler->getGlobalSymAlloc().AllocStr(buffer);
                    }
                }
            }
        ENDLOOP;
    }
}

/***************************************************************************************************
    MethAttrBind methods
***************************************************************************************************/

void MethAttrBind::CompileEarly(COMPILER * cmp, METHSYM * meth)
{	
    BASENODE * nodeAttr = meth->getAttributesNode();

    if (!nodeAttr)
        return;

    MethAttrBind attrbind(cmp, meth, true);
    attrbind.ProcessAll(nodeAttr);
}


void MethAttrBind::CompileAndEmit(COMPILER * cmp, METHSYM * meth, bool fDebuggerHidden)
{
    BASENODE * nodeAttr = meth->getAttributesNode();

    if (nodeAttr || fDebuggerHidden || meth->isFabricated || meth->isAnonymous()) {
        MethAttrBind attrbind(cmp, meth, false);

        if (nodeAttr) {
            UnknownAttrBind::Compile(cmp, meth->containingDeclaration(), nodeAttr);
            attrbind.ProcessAll(nodeAttr);
        }

        // Sometimes we need to really hide some methods.
        if (fDebuggerHidden) {
            AGGTYPESYM * atsHidden = cmp->GetOptPredefType(PT_DEBUGGERHIDDEN);
            if (atsHidden) {
                attrbind.ProcessSynthAttr(atsHidden,
                    cmp->funcBRec.bindSimplePredefinedAttribute(PT_DEBUGGERHIDDEN), NULL);
            }
        }

        if (meth->isFabricated || meth->isAnonymous() || fDebuggerHidden)
            attrbind.CompileFabricatedAttr();
    }

    if (meth->typeVars->size > 0)
        TypeVarAttrBind::CompileParamList(cmp, meth, meth->typeVars, meth->toksEmitTypeVars);
}


bool MethAttrBind::BindAttr(ATTRNODE *attr)
{
    // For early processing only do security, conditonal, obsolete and cls.
    if (!fEarly ||
        (attributeType && attributeType->isSecurityAttribute()) ||
        (predefinedAttribute == PA_CONDITIONAL ||
            predefinedAttribute == PA_OBSOLETE ||
            predefinedAttribute == PA_CLSCOMPLIANT))
    {
        return AttrBind::BindAttr(attr);
    }

    return false;
}


void MethAttrBind::VerifyAndEmitPredef(ATTRNODE *attr)
{
    BASENODE *name; name = attr->pName;

    switch (predefinedAttribute) {
    case PA_OBSOLETE:
    case PA_CLSCOMPLIANT:
        if (meth->isAnyAccessor())
            goto LBadAccAttr;
        break;

    case PA_CONDITIONAL:
        if (meth->isAnyAccessor()) {
LBadAccAttr:
            if (fEarly)
                return;

            // Predefined attribute which is not valid on accessors
            WCHAR buffer[1024];
            BuildAttrTargetString(buffer, lengthof(buffer), attributeType->getAggregate()->attributeClass);
            compiler->Error(attr->pName, ERR_AttributeNotOnAccessor, ErrArgNameNode(attr->pName), buffer);
            return;
        }

        if (!fEarly) {
            // Call base VerifyAndEmitPredef.
            break;
        }

        if (meth->getClass()->IsInterface()) {
            compiler->Error(attr, ERR_ConditionalOnInterfaceMethod);
        } else if (!meth->isUserCallable() || !meth->name || meth->isCtor() || meth->isDtor()) {
            compiler->Error(attr, ERR_ConditionalOnSpecialMethod, meth);
        } else if (meth->isOverride) {
            compiler->Error(attr, ERR_ConditionalOnOverride, meth);
        } else if (meth->retType != compiler->getBSymmgr().GetVoid()) {
            compiler->Error(attr, ERR_ConditionalMustReturnVoid, meth);
        } else {
            // Conditional method cannot have out parameters.
            for (int i = 0; i < meth->params->size; i++) {
                if (meth->params->Item(i)->isPARAMMODSYM() && meth->params->Item(i)->asPARAMMODSYM()->isOut) {
                    compiler->Error(attr, ERR_ConditionalWithOutParam, meth);
                }
            }
            // Call base VerifyAndEmitPredef.
            break;
        }
        // In error cases, don't call base VerifyAndEmitPredef
        return;

    case PA_DLLIMPORT:
        if (!fEarly)
            ProcessDllImport(attr);
        return;

    default:
        break;
    }

    AttrBind::VerifyAndEmitPredef(attr);
}

//
// compiles a dllimport attribute on a method
//
void MethAttrBind::ProcessDllImport(ATTRNODE * attr)
{
    if (!meth->isStatic || !meth->isExternal) {
        compiler->Error(attr->pName, ERR_DllImportOnInvalidMethod);
        return;
    }

    AddDefaultCharSet();

    VerifyAndEmitCore(attr);
}

void MethAttrBind::ValidateAttrs()
{
    if (fEarly)
        meth->hasLinkDemand = hasLinkDemand;
}

void MethAttrBind::AddConditionalName(NAME * name)
{
    if (pnlstCond)
        compiler->getBSymmgr().AddToGlobalNameList(name, &pnlstCond);
}

//-----------------------------------------------------------------------------
void IndexerNameAttrBind::Compile(COMPILER * compiler, INDEXERSYM * propertyNode) 
{
    BASENODE *attributes = propertyNode->getAttributesNode();
    if (!attributes) {
        return;
    }

    IndexerNameAttrBind attrbind(compiler, propertyNode);
    attrbind.ProcessAll(attributes);
}

IndexerNameAttrBind::IndexerNameAttrBind(COMPILER *compiler, INDEXERSYM * prop) :
    AttrBind(compiler, true)
{
    this->prop = prop;
    Init(prop);
}

bool IndexerNameAttrBind::BindAttr(ATTRNODE *attr)
{
    switch (predefinedAttribute) {
    default:
        return false;

    case PA_NAME:
        break;
    }

    return AttrBind::BindAttr(attr);
}

void IndexerNameAttrBind::VerifyAndEmitPredef(ATTRNODE *attr)
{
    switch (predefinedAttribute) {
    case PA_NAME:
        {
            ASSERT(!namedArguments);

            if (prop->isOverride)
                compiler->Error(attr->pName, ERR_NameAttributeOnOverride, ErrArgNameNode(attr->pName));

            STRCONST * nameValue = NULL;
            EXPRLOOP(ctorExpression->asCALL()->args, argument)
                ASSERT(!nameValue);  // there can be only one
                getValue(ctorExpression->asCALL()->args, &nameValue);

                if (compiler->checkForValidIdentifier(nameValue, false, argument->tree, 
                        ERR_BadArgumentToAttribute, attributeType))
                {
                    // convert the string to a name
                    prop->realName = compiler->namemgr->AddString(nameValue->text, nameValue->length);
                }
            ENDLOOP;
        }
        return;

    default:
        // Unlike most early binders, this shouldn't call the base VerifyAndEmitPredef.
        // This is because another early binder has already been invoked on this property
        // to handle cls, obsolete, etc.
        return;
    }
}

//-----------------------------------------------------------------------------

void PropAttrBind::Compile(COMPILER * compiler, PROPSYM * prop)
{
    BASENODE * attributes = prop->getAttributesNode();
    if (!attributes) {
        return;
    }

    UnknownAttrBind::Compile(compiler, prop->containingDeclaration(), attributes);

    PropAttrBind attrbind(compiler, prop);
    if (attributes)
        attrbind.ProcessAll(attributes);

    attrbind.CompileFabricatedAttr();
}

PropAttrBind::PropAttrBind(COMPILER * compiler, PROPSYM * prop) :
    AttrBind(compiler, false)
{
    this->prop = prop;
    Init(prop);
}

void PropAttrBind::VerifyAndEmitPredef(ATTRNODE *attr)
{
    switch (predefinedAttribute) {
    case PA_NAME:
        // Name attributes are checked in the define stage for non-explicit impl indexers,
        // otherwise they are an error.
        if (!prop->isIndexer() || prop->IsExpImpl())
            compiler->Error(attr->pName, ERR_BadIndexerNameAttr, ErrArgNameNode(attr->pName));
        return;

    default:
        break;
    }

    AttrBind::VerifyAndEmitPredef(attr);
}

//-----------------------------------------------------------------------------

void FieldAttrBind::Compile(COMPILER *compiler, MEMBVARSYM *field, MEMBVARINFO *info, AGGINFO *agginfo)
{
    BASENODE * attributes = field->getAttributesNode();

    if (attributes)
        UnknownAttrBind::Compile(compiler, field->containingDeclaration(), attributes);

    // Do this whether or not there are attributes, so ValidateAttributes gets called either way.
    FieldAttrBind attrbind(compiler, field, info, agginfo);
    attrbind.ProcessAll(attributes);
    attrbind.CompileFabricatedAttr(); 
}

FieldAttrBind::FieldAttrBind(COMPILER *compiler, MEMBVARSYM *field, MEMBVARINFO *info, AGGINFO *agginfo) :
    AttrBind(compiler, false)
{
    this->field = field;
    this->info = info;
    this->agginfo = agginfo;
    Init(field);
}

void FieldAttrBind::VerifyAndEmitPredef(ATTRNODE *attr)
{
    switch (predefinedAttribute) {
    case PA_STRUCTOFFSET:
        // Must be explicit layout kind on aggregate.
        if (!agginfo->hasExplicitLayout) {
            compiler->Error(attr->pName, ERR_StructOffsetOnBadStruct);
            return;
        }
        if (field->isConst || field->isStatic) {
            compiler->Error(attr->pName, ERR_StructOffsetOnBadField);
            return;
        }
        info->foundOffset = true;
        break;

    case PA_FIXED:
        compiler->Error(attr->pName, ERR_DoNotUseFixedBufferAttr);
        return;

    default:
        break;
    }

    AttrBind::VerifyAndEmitPredef(attr);
}

void FieldAttrBind::ValidateAttrs()
{
    if (agginfo->hasExplicitLayout && !field->isConst && !field->isStatic && !info->foundOffset) {
        compiler->ErrorRef(NULL, ERR_MissingStructOffset, field);
    }

    if (field->isConst && field->type->isPredefType(PT_DECIMAL)) {
        DecimalConstantBuffer buffer;

        buffer.format   = VAL16(1);
        buffer.scale    = DECIMAL_SCALE(*(field->constVal.decVal));
        buffer.sign     = DECIMAL_SIGN(*(field->constVal.decVal));
        buffer.hi       = VAL32(DECIMAL_HI32(*(field->constVal.decVal)));
        buffer.mid      = VAL32(DECIMAL_MID32(*(field->constVal.decVal)));
        buffer.low      = VAL32(DECIMAL_LO32(*(field->constVal.decVal)));
        buffer.cNamedArgs = 0;

        AGGSYM * aggDecConst = compiler->GetOptPredefAggErr(PT_DECIMALCONSTANT);
        METHSYM * ctor;
        if (aggDecConst &&
            (ctor = compiler->getBSymmgr().LookupAggMember(compiler->namemgr->GetPredefName(PN_CTOR),
                aggDecConst, MASK_METHSYM)->asMETHSYM()) != NULL)
        {
            compiler->emitter.EmitCustomAttribute(
                field->getParseTree(), field->tokenEmit, ctor,
                (BYTE*)&buffer, sizeof(buffer));
        }
    }
    else if (field->fixedAgg && field->constVal.iVal > 0) {
        // Create an attribute to persist the element type and length of the fixed buffer
        AGGTYPESYM * atsFixed = compiler->GetOptPredefTypeErr(PT_FIXEDBUFFER);
        if (atsFixed) {
            FieldAttrBind attrbind(compiler, field, info, agginfo);
            attrbind.ProcessSynthAttr(atsFixed,
                compiler->funcBRec.bindSimplePredefinedAttribute(PT_FIXEDBUFFER, compiler->funcBRec.bindFixedBufferArgs(field)),
                NULL);
        }
    }
}

//-----------------------------------------------------------------------------

ParamAttrBind::ParamAttrBind(COMPILER *compiler, METHSYM *method) :
    AttrBind(compiler, false),
    blob(&compiler->localSymAlloc, rgb, lengthof(rgb))
{
    this->method = method;
}

void ParamAttrBind::Init(TYPESYM *type, PARAMINFO *info, int index)
{
    this->parameterType = type;
    this->paramInfo = info;
    this->index = index;
    this->customAttributeList = 0;
    this->sym = method;
    this->hasDefaultValue = false;
    this->etDefaultValue = 0;
    this->blob.Reset();
    AttrBind::Init((index == 0 ? catReturnValue : catParameter), method->declaration);
    EmitParamProps();
}

mdToken ParamAttrBind::GetToken()
{
    if (paramInfo->tokenEmit == mdTokenNil) {
        compiler->emitter.DefineParam(method->tokenEmit, index, &paramInfo->tokenEmit);
    }

    return paramInfo->tokenEmit;
}

void ParamAttrBind::VerifyAndEmitPredef(ATTRNODE *attr)
{
    switch (predefinedAttribute) {
    case PA_IN:
        if (parameterType->isPARAMMODSYM() && parameterType->asPARAMMODSYM()->isOut) {
            // "out" parameter cannot have the "in" attribute.
            compiler->Error(attr, ERR_InAttrOnOutParam);
            return;
        }
        paramInfo->isIn = true;
        break;

    case PA_OUT:
        paramInfo->isOut = true;
        break;

    case PA_PARAMARRAY:
        compiler->Error(attr, ERR_ExplicitParamArray);
        break;

    case PA_CLSCOMPLIANT:
        if ((!method->getClass()->IsDelegate() || method->isInvoke()) && compiler->AllowCLSErrors()) {
            if (attrloc == AL_PARAM) {
                compiler->Error(attr, WRN_CLS_MeaninglessOnParam);
            } else if (attrloc == AL_RETURN) {
                compiler->Error(attr, WRN_CLS_MeaninglessOnReturn);
            }
        }
        break;
        
    case PA_DEFAULTVALUE:
        EXPRLOOP(ctorExpression->asCALL()->args, arg)
            ASSERT(!this->hasDefaultValue);
            TYPESYM *underlyingParameterType = parameterType->isPARAMMODSYM() ? parameterType->asPARAMMODSYM()->paramType() : parameterType;

            if (arg->kind == EK_CAST) {
                EXPR *value = arg->asCAST()->p1;
                TYPESYM *valueType = value->type;
                
                if (valueType != underlyingParameterType && !underlyingParameterType->isPredefType(PT_OBJECT)) {
                    compiler->Error(attr, ERR_DefaultValueTypeMustMatch);
                } else if (valueType->getKind() == SK_ARRAYSYM || valueType->isPredefType(PT_TYPE)) {
                    if (underlyingParameterType->isPredefType(PT_OBJECT))
                        compiler->Error(attr, ERR_DefaultValueBadValueType, valueType);
                    else
                        compiler->Error(attr, ERR_DefaultValueBadParamType, underlyingParameterType);
                } else {
                    this->hasDefaultValue = true;
                    
                    if (valueType->isPredefType(PT_STRING))
                        blob.Add(value->asCONSTANT()->getSVal().strVal->text, value->asCONSTANT()->getSVal().strVal->length * sizeof(WCHAR));
                    else
                        AddAttrArg(value, blob);
                    this->etDefaultValue = compiler->getBSymmgr().GetElementType(valueType->underlyingType()->asAGGTYPESYM());
                }
            } else {
                // null value of object type
                if (underlyingParameterType->getKind() == SK_ARRAYSYM || underlyingParameterType->isPredefType(PT_TYPE)) {
                    compiler->Error(attr, ERR_DefaultValueBadParamType, underlyingParameterType);
                } else if (underlyingParameterType->getKind() != SK_AGGTYPESYM || !underlyingParameterType->asAGGTYPESYM()->getAggregate()->IsRefType()) {
                    compiler->Error(attr, ERR_DefaultValueTypeMustMatch);
                } else {
                    this->hasDefaultValue = true;
                    this->etDefaultValue = ELEMENT_TYPE_CLASS;
                }
            }
        ENDLOOP;
        return; // don't emit real attribute to the metadata

    default:
        break;
    }

    AttrBind::VerifyAndEmitPredef(attr);
}

void ParamAttrBind::ValidateAttrs()
{
    if (paramInfo->isOut && !paramInfo->isIn && parameterType->isPARAMMODSYM() && parameterType->asPARAMMODSYM()->isRef) {
        compiler->ErrorRef(NULL, ERR_OutAttrOnRefParam, method);
    }
}

void ParamAttrBind::EmitPredefAttrs()
{
    // emit the synthetized parameter...
    if (paramInfo->isParamArray) {
        AttrBind::ProcessSynthAttr(compiler->GetReqPredefType(PT_PARAMS), 
            compiler->funcBRec.bindSimplePredefinedAttribute(PT_PARAMS), NULL);
    }

    if (parameterType->isPARAMMODSYM() && parameterType->asPARAMMODSYM()->isOut) {
        AttrBind::ProcessSynthAttr(compiler->GetReqPredefType(PT_OUT), 
            compiler->funcBRec.bindSimplePredefinedAttribute(PT_OUT), NULL);
    }
    
    if (hasDefaultValue) {
        EmitParamProps();
    }
}

void ParamAttrBind::EmitParamProps()
{
    compiler->emitter.EmitParamProp(method->tokenEmit, index, parameterType, paramInfo, hasDefaultValue, etDefaultValue, blob);
}

void ParamAttrBind::CompileParamList(COMPILER *compiler, METHINFO *info)
{
    ParamAttrBind attrbind(compiler, info->meth);

    // If the delegate previously had attribute errors, then do not continue processing attributes on each of the delegate methods because they will
    // bind the same attribute and give duplicate error messages.  This method is not called during refactoring because we use the RefactoringAttrBind instead.
    // If this ever changes, this statement should be updated because it will keep us from binding attributes on delegate Invoke methods in error cases.
    if (info->meth->getClass()->fHadAttributeError) {
        ASSERT(info->meth->getClass()->IsDelegate() && compiler->ErrorCount() > 0);
        // it is safe to return because the only way we can put attributes on any of the parameters of this method is via the delegate, so they will all be duplicates
        // and at least one of them will have given an error.
        return;   
    }

    if (info->nodeAttr) {
        attrbind.Init(info->meth->retType, &info->returnValueInfo, 0);
        attrbind.ProcessAll(info->nodeAttr);
    }

    PARAMINFO *ppin = info->rgpin;
    for (int i = 0; i < info->cpin; i++, ppin++) {

        // if the property previously had bad attributes on the parameters, don't give another error here
        // We can still parse non-indexer accessors because the only way to get attributes on the parameters is with the param: attribute target

        if(info->meth->isPropertyAccessor() && info->meth->getProperty()->fHadAttributeError) {
            ASSERT(info->meth->getProperty()->isIndexer() && compiler->ErrorCount() > 0);
            continue;
        }

        attrbind.Init(info->meth->params->Item(i), ppin, i + 1);

        if (ppin->nodeAttr) {
            UnknownAttrBind::Compile(compiler, info->meth->containingDeclaration(), ppin->nodeAttr);
            attrbind.ProcessAll(ppin->nodeAttr);
        }

        // Emit predefined parameter properties(name, marshaling).
        attrbind.EmitPredefAttrs();
    }
}


//-----------------------------------------------------------------------------

void GlobalAttrBind::Compile(COMPILER *compiler, PARENTSYM *context, GLOBALATTRSYM *globalAttributes, mdToken tokenEmit)
{
    bool fHadDebuggable = false;
    //
    // do we have attributes
    //
    if (globalAttributes) {

        GlobalAttrBind attrbind(compiler, globalAttributes, tokenEmit);
        while (1)
        {
            int ec = compiler->ErrorCount();
            if (!globalAttributes->fHadAttributeError)
                attrbind.ProcessAll(globalAttributes->parseTree);
            if (ec != compiler->ErrorCount()) {
                // mark that we got an error processing this attribute and we shouldn't process it again
                // NOTE: this interacts with similar loop in EarlyGlobalAttrBind::Compile()
                ASSERT(!globalAttributes->fHadAttributeError);
                globalAttributes->fHadAttributeError = true;
            }

            globalAttributes = globalAttributes->nextAttr;
            if (!globalAttributes) 
            {
                break;
            }
            attrbind.Init(globalAttributes);
        }
        fHadDebuggable = attrbind.fHadDebuggable;
    }

    // synthetized attributes
    GlobalAttrBind attrbind(compiler, tokenEmit, context);
    if (compiler->options.m_fUNSAFE) {
        if (TypeFromToken(tokenEmit) == mdtModule) {
            if (compiler->GetOptPredefType(PT_UNVERIFCODEATTRIBUTE, false))
                attrbind.ProcessSynthAttr(compiler->GetOptPredefType(PT_UNVERIFCODEATTRIBUTE),
                    compiler->funcBRec.bindSimplePredefinedAttribute(PT_UNVERIFCODEATTRIBUTE),
                    NULL);
        } else {
            if (compiler->GetOptPredefType(PT_UNVERIFCODEATTRIBUTE, false) &&
                compiler->GetOptPredefType(PT_SECURITYPERMATTRIBUTE, false) &&
                compiler->GetOptPredefType(PT_SECURITYACTION, false))
            {
                attrbind.ProcessSynthAttr(compiler->GetOptPredefType(PT_SECURITYPERMATTRIBUTE),
                    compiler->funcBRec.bindSimplePredefinedAttribute(PT_SECURITYPERMATTRIBUTE, compiler->funcBRec.bindSkipVerifyArgs()),
                    compiler->funcBRec.bindSkipVerifyNamedArgs());
            }
        }
    }

    if (TypeFromToken(tokenEmit) != mdtModule) {
        if (!fHadDebuggable && compiler->BuildAssembly()) {
            EXPR * args = compiler->funcBRec.bindDebuggableArgs();
            // This returns NULL if the args are the default values (meaning no attribute is needed)
            if (args) {
                attrbind.ProcessSynthAttr(compiler->GetOptPredefType(PT_DEBUGGABLEATTRIBUTE),
                    compiler->funcBRec.bindSimplePredefinedAttribute(PT_DEBUGGABLEATTRIBUTE, args), NULL);
            }
        }

        // Emit the CompilationRelaxationsAttribute
        if (compiler->EmitRelaxations() && compiler->GetOptPredefType(PT_COMPILATIONRELAXATIONS, false))
        {
            attrbind.ProcessSynthAttr(compiler->GetOptPredefType(PT_COMPILATIONRELAXATIONS),
                compiler->funcBRec.bindSimplePredefinedAttribute(PT_COMPILATIONRELAXATIONS, compiler->funcBRec.bindCompilationRelaxationsAttributeArgs()),
                NULL);
        }

        // Emit the RuntimeCompatibilityAttribute
        if (compiler->EmitRuntimeCompatibility())
        {
            attrbind.ProcessSynthAttr(compiler->GetOptPredefType(PT_RUNTIMECOMPATIBILITY),
                compiler->funcBRec.bindSimplePredefinedAttribute(PT_RUNTIMECOMPATIBILITY, NULL),
                compiler->funcBRec.bindRuntimeCompatibilityAttributeNamedArgs());
        }
    }
}

GlobalAttrBind::GlobalAttrBind(COMPILER *compiler, GLOBALATTRSYM *globalAttributes, mdToken tokenEmit) :
    AttrBind(compiler, false)
{
    this->globalAttribute = globalAttributes;
    this->tokenEmit = tokenEmit;
    fHadDebuggable = false;
    Init(globalAttribute);
}

GlobalAttrBind::GlobalAttrBind(COMPILER *compiler, mdToken tokenEmit, PARENTSYM *context) :
    AttrBind(compiler, false)
{
    globalAttribute = NULL;
    fHadDebuggable = false;
    this->tokenEmit = tokenEmit;
    ASSERT(TypeFromToken(tokenEmit) == mdtAssembly || TypeFromToken(tokenEmit) == mdtModule);
    Init((TypeFromToken(tokenEmit) == mdtAssembly) ? catAssembly : catModule, context);
}

void GlobalAttrBind::VerifyAndEmitPredef(ATTRNODE *attr)
{
    BASENODE * name = attr->pName;

    PCWSTR szOptionName = NULL;
    switch (predefinedAttribute) {
    case PA_KEYFILE:
        szOptionName = COptionData::GetOptionDef(COptionData::GetOptionIndex(OPTID_KEYFILE))->pszDescSwitch;
        break;
    case PA_KEYNAME:
        szOptionName = COptionData::GetOptionDef(COptionData::GetOptionIndex(OPTID_KEYNAME))->pszDescSwitch;
        break;
    case PA_DELAYSIGN:
        szOptionName = COptionData::GetOptionDef(COptionData::GetOptionIndex(OPTID_DELAYSIGN))->pszDescSwitch;
        break;
    case PA_DEBUGGABLE:
        fHadDebuggable = true;
        goto SKIP_WARNING;
    case PA_FRIENDASSEMBLY:
        {
            // Verify the attribute contains a well-formed assembly reference
            STRCONST * strValue = NULL;
            EXPRLOOP(ctorExpression->asCALL()->args, argument)
                ASSERT(!strValue);   // how did we get two parameters to the InternalsVisibleToAttribute?
                getValue(argument, &strValue);
                WCHAR *szValue = STACK_ALLOC (WCHAR, strValue->length + 1);
                StringCchCopyNW(szValue, strValue->length+1, strValue->text, strValue->length);            // the Strsafe API will null terminate.
                if (!compiler->importer.CheckFriendAssemblyName(argument->tree, szValue, sym->getInputFile()->getOutputFile()))
                    return;
            ENDLOOP;

            goto SKIP_WARNING;
        }
    default:
        goto SKIP_WARNING;
    }

    // Only report the warning if they have non-default values
    EXPRLOOP(ctorExpression->asCALL()->args, argument)
        if (argument->type->isPredefType(PT_BOOL)) {
            bool value;
            if (!getValue(argument, &value) || !value)
                goto SKIP_WARNING;
        } else if (argument->type->isPredefType(PT_STRING)) {
            STRCONST * value = NULL;
            if (!getValue(argument, &value) || !value || value->length == 0)
                goto SKIP_WARNING;
        }
    ENDLOOP;
    compiler->Error(name, WRN_UseSwitchInsteadOfAttribute, szOptionName, ErrArgNameNode(name));

SKIP_WARNING:
    AttrBind::VerifyAndEmitPredef(attr);
}

//-----------------------------------------------------------------------------

void EarlyGlobalAttrBind::Compile(COMPILER *compiler, GLOBALATTRSYM *globalAttributes)
{
    //
    // do we have attributes
    //
    if (!globalAttributes) {
        return;
    }

    EarlyGlobalAttrBind attrbind(compiler, globalAttributes);
    while (1)
    {
        int ec = compiler->ErrorCount();
        // Only compile the assembly-level attributes and the ones that don't already have errors
        if (!globalAttributes->fHadAttributeError)
            attrbind.ProcessAll(globalAttributes->parseTree);
        if (ec != compiler->ErrorCount()) {
            // Set to indicate that we got an error
            // processing this attribute and we shouldn't process it again
            ASSERT(!globalAttributes->fHadAttributeError);
            globalAttributes->fHadAttributeError = true;
        }
        globalAttributes = globalAttributes->nextAttr;
        if (!globalAttributes) 
        {
            break;
        }
        attrbind.Init(globalAttributes);
    }
}

EarlyGlobalAttrBind::EarlyGlobalAttrBind(COMPILER *compiler, GLOBALATTRSYM *globalAttributes) :
    AttrBind(compiler, true)
{
    Init(globalAttributes);
}

bool EarlyGlobalAttrBind::BindAttr(ATTRNODE *attr)
{
    if (predefinedAttribute == PA_CLSCOMPLIANT ||
        predefinedAttribute == PA_DEFAULTCHARSET ||
        predefinedAttribute == PA_COMPILATIONRELAXATIONS ||
        predefinedAttribute == PA_RUNTIMECOMPATIBILITY ||
        predefinedAttribute == PA_FRIENDASSEMBLY ||
        predefinedAttribute == PA_KEYFILE ||
        predefinedAttribute == PA_KEYNAME ||
        predefinedAttribute == PA_DELAYSIGN)
    {
        return AttrBind::BindAttr(attr);
    }

    return false;
}

void EarlyGlobalAttrBind::VerifyAndEmitPredef(ATTRNODE *attr)
{
    switch(predefinedAttribute) {
    case PA_DEFAULTCHARSET:
        {
            OUTFILESYM * module = sym->getInputFile()->getOutputFile();

            ASSERT(attrloc == AL_MODULE);
            ASSERT(module);

            EXPRLOOP(ctorExpression->asCALL()->args, argument)
                getValue(argument, &module->defaultCharSet);
                if (module->defaultCharSet < 0 || module->defaultCharSet > 4)
                {
                    compiler->Error(argument->tree, ERR_InvalidDefaultCharSetValue);
                    module->defaultCharSet = 0;
                }
                break;
            ENDLOOP;
        }
        break;

    case PA_COMPILATIONRELAXATIONS:
        if (attrloc == AL_ASSEMBLY)
            compiler->SuppressRelaxations();
        break;

    case PA_RUNTIMECOMPATIBILITY:
        if (attrloc == AL_ASSEMBLY)
        {
            compiler->SuppressRuntimeCompatibility();
            EXPRLOOP(namedArguments, argument)
                NAME *name = getNamedArgumentName(argument);
                EXPR *value = getNamedArgumentValue(argument);
                bool val = true;

                if (name == GetPredefName(PN_WRAPNONEXCEPTIONTHROWS) && getValue(value, &val) && !val)
                    compiler->SuppressWrapNonExceptionThrows();
            ENDLOOP;
        }
        break;

    case PA_FRIENDASSEMBLY:
        compiler->SetFriendsDeclared();
        break;

    case PA_KEYFILE:
    case PA_KEYNAME:
    case PA_DELAYSIGN:
        // If there is a manifest then we set the flag on it since any module linked into that manifest with
        // any of these attributes will cause ALink to sign the final manifest.
        if (compiler->GetManifestOutFile())
            compiler->GetManifestOutFile()->fHasSigningAttribute = true;
        else {
            OUTFILESYM *module = sym->getInputFile()->getOutputFile();
            module->fHasSigningAttribute = true;
        }
        break;

    default:
        // do nothing for other attributes early
        break;
    }

    AttrBind::VerifyAndEmitPredef(attr);
}

//-----------------------------------------------------------------------------

void UnknownAttrBind::Compile(COMPILER *compiler, PARENTSYM *context, BASENODE *attributes)
{
    if (!attributes) {
        return;
    }

    UnknownAttrBind attrbind(compiler, context);
    attrbind.ProcessAll(attributes);
}

void UnknownAttrBind::Compile(COMPILER *compiler, GLOBALATTRSYM *sym)
{
    while (sym) {
        Compile(compiler, sym->parent, sym->parseTree);
        sym = sym->nextAttr;
    }
}

UnknownAttrBind::UnknownAttrBind(COMPILER *compiler, PARENTSYM *context) :
    AttrBind(compiler, false)
{
    this->ek = catAll;
    this->context = context;
    this->attrloc = AL_UNKNOWN;
}

void CompilerGeneratedAttrBind::EmitAttribute(COMPILER *compiler, mdToken token)
{
    ASSERT(token);
    if (!compiler->options.m_fNOCODEGEN && !compiler->options.m_fCompileSkeleton)
    {
        DWORD blob = 0x00000001;  // store the version number only in the blob (no attributes).
        EXPR* ctorExpression =  compiler->funcBRec.bindSimplePredefinedAttribute(PT_COMPILERGENERATED);
        if (ctorExpression)
            compiler->emitter.EmitCustomAttribute(NULL, token, ctorExpression->asCALL()->mwi.Meth(), (BYTE *)&blob, sizeof(DWORD));
    }
}

void UnsafeValueTypeAttrBind::EmitAttribute(COMPILER *compiler, mdToken token)
{
    ASSERT(token);
    if (!compiler->options.m_fNOCODEGEN && !compiler->options.m_fCompileSkeleton)
    {
        DWORD blob = 0x00000001;  // store the version number only in the blob (no attributes).
        EXPR* ctorExpression =  compiler->funcBRec.bindSimplePredefinedAttribute(PT_UNSAFEVALUETYPE);
        if (ctorExpression)
            compiler->emitter.EmitCustomAttribute(NULL, token, ctorExpression->asCALL()->mwi.Meth(), (BYTE *)&blob, sizeof(DWORD));
    }
}

