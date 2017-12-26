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
// File: metahelp.cpp
//
// Helper routines for importing/emitting CLR metadata.
// ===========================================================================

#include "stdafx.h"

// Returns a fully-qualified name (open type, do not pass TYPESYMs)
// With inner types denoted with m_chNested (defaults to '+')
// And everything else separated by dots (.)
void MetaDataHelper::GetFullName(SYM * sym, StringBldr & str, SYM * symInner)
{
    // Special case -- the root namespace.
    if (!sym->parent) {
        // At the root namespace.
        return;
    }

    if (sym->isTYPESYM()) {
        ASSERT(!"MetaDataHelper::GetFullName should not be called on TYPESYMs");
        return;
    }

    PPARENTSYM parent = sym->parent;

    // If Our parent isn't the root, get the parent name and separator and advance beyond it.
    if (parent->parent) {
        GetFullName(parent, str, sym);
        str.Add(parent->isAGGSYM() ? m_chNested : L'.');
    }

    // Get the current name and add it on
    NAME * name;
    if (sym->isPROPSYM())
        name = sym->asPROPSYM()->getRealName();
    else
        name = sym->name;

    int ichMin = str.Length();
    if (name == NULL)
        GetExplicitImplName(sym, str);
    else
        str.Add(name->text);

    EscapeSpecialChars(str, ichMin);
    GetTypeVars(sym, str, symInner);
}

// Returns a class/type name as it should appear in metadata
// Basically the same as GetFullName except inner classes
// are NOT qualified by their outer class or namespace
void MetaDataHelper::GetMetaDataName(BAGSYM * sym, StringBldr & str)
{
    // Special case -- the root namespace.
    if (!sym->parent) {
        // At the root namespace.
        return;
    }

    BAGSYM  *bagPar = sym->parent->asBAGSYM();

    // If Our parent isn't the root or an outer type, get the parent name and seperator and advance beyond it.
    if (bagPar->isNSSYM() && bagPar->parent) {
        GetMetaDataName(bagPar, str);
        str.Add(L'.');
    }

    // Get the current name and add it on
    NAME * name = sym->name;
    int ichMin = str.Length();
    str.Add(name->text);

    EscapeSpecialChars(str, ichMin);
    GetTypeVars(sym, str, NULL);
}


/*
 * Determine the visibility flags for a typedef definition in metadata
 */
DWORD MetaDataHelper::GetTypeAccessFlags(SYM * sym)
{
    DWORD flags = 0;

    ASSERT(sym->isAGGSYM());

    // Set access flags.
    if (sym->parent->isNSSYM()) {
        // "Top-level" aggregate. Can only be public or internal.
        ASSERT(sym->GetAccess() == ACC_PUBLIC || sym->GetAccess() == ACC_INTERNAL);
        if (sym->GetAccess() == ACC_PUBLIC)
            flags |= tdPublic;
        else
            flags |= tdNotPublic;
    }
    else {
        // nested aggregate. Can be any access.
        switch (sym->GetAccess()) {
        case ACC_PUBLIC:     flags |= tdNestedPublic;   break;
        case ACC_INTERNAL:   flags |= tdNestedAssembly; break;
        case ACC_PROTECTED:  flags |= tdNestedFamily;   break;
        case ACC_INTERNALPROTECTED:
                             flags |= tdNestedFamORAssem; break;
        case ACC_PRIVATE:    flags |= tdNestedPrivate;  break;
        default: ASSERT(!"Bad access flag");            break;
        }
    }

    return flags;
}

/*
 * Determine the flags for a typedef definition in metadata
 */
DWORD MetaDataHelper::GetAggregateFlags(AGGSYM * sym)
{
    DWORD flags = 0;
    // Determine flags.

    // Set access flags.
    flags |= GetTypeAccessFlags(sym);

    // Set other flags
    switch (sym->AggKind()) {
    case AggKind::Class:
        if (sym->isSealed)
            flags |= tdSealed;
        if (sym->isAbstract)
            flags |= tdAbstract;
        if (!sym->hasUDStaticCtor)
            flags |= tdBeforeFieldInit;
        break;

    case AggKind::Interface:
        flags |= tdInterface | tdAbstract;
        break;

    case AggKind::Enum:
        ASSERT(sym->isSealed);
        flags |= tdSealed;
        break;

    case AggKind::Struct:
        ASSERT(sym->isSealed);
        flags |= tdSealed;
        if (!sym->hasUDStaticCtor)
            flags |= tdBeforeFieldInit;
        if (sym->isFabricated)
            flags |= tdSequentialLayout; // Fabricated structs are always sequential
        break;

    case AggKind::Delegate:
        ASSERT(sym->isSealed);
        flags |= tdSealed;
        break;

    default:
        ASSERT(0);
    }

    switch (sym->getOutputFile()->defaultCharSet) {
    default:
        VSFAIL("A new value was added to System.Runtime.InteropServices.CharSet that we need to handle");
    case 0: // Unset
    case 1: // CharSet.None
        break;
    case 2: // CharSet.Ansi
        flags |= tdAnsiClass;
        break;
    case 3: // CharSet.Unicode
        flags |= tdUnicodeClass;
        break;
    case 4: // CharSet.Auto
        flags |= tdAutoClass;
        break;
    }

    return flags;
}


/*
 * Get a synthesized name for explicit interface implementations. The name we use is:
 * "InterfaceName.MethodName", where InterfaceName is the fully qualified name of the
 * interface containing the implemented method. This name has a '.' in it, so it can't
 * conflict with any "real" name or be confused with one.
 *
 * Returns true if the buffer had enough space for the name.
 * If not enough space, then adds as much of name as possible to buffer.
 * Always NULL terminates buffer.
 */
void MetaDataHelper::GetExplicitImplName(SYM * sym, StringBldr & str)
{
    ASSERT(sym->isEVENTSYM() || sym->isMETHPROPSYM());

    SymWithType swtImpl;
    ERRORSYM * err = NULL;
    PCWSTR pszName = NULL;
    PCWSTR pszAlias = NULL;

    switch (sym->getKind()) {
    case SK_EVENTSYM:
        ASSERT(sym->asEVENTSYM()->IsExpImpl());
        swtImpl = sym->asEVENTSYM()->ewtSlot;
        err = sym->asEVENTSYM()->errExpImpl;
        break;

    case SK_METHSYM:
    case SK_PROPSYM: {
        ASSERT(sym->asMETHPROPSYM()->IsExpImpl());
        swtImpl = sym->asMETHPROPSYM()->swtSlot;
        err = sym->asMETHPROPSYM()->errExpImpl;
        BASENODE *pName = NULL;
        if (sym->isMETHSYM() && sym->asMETHSYM()->parseTree && sym->asMETHSYM()->parseTree->kind == NK_METHOD) {
            pName = sym->asMETHSYM()->parseTree->asMETHOD()->pName;
        }
        else if(sym->isPROPSYM() && sym->asPROPSYM()->parseTree && sym->asPROPSYM()->parseTree->kind == NK_PROPERTY) {
            pName = sym->asPROPSYM()->parseTree->asPROPERTY()->pName;
        }
        while (pName && pName->kind == NK_DOT) {
            pName = pName->asDOT()->p1;
        }
        if (pName && pName->kind == NK_ALIASNAME) {
            pszAlias = pName->asANYNAME()->pName->text;
        }
        if (!sym->isPROPSYM() || !sym->asPROPSYM()->isIndexer())
            break;
        pszName = swtImpl ? swtImpl.Sym()->asPROPSYM()->getRealName()->text : L"Item";
        // fish out any user specified alias
        break;
      }

    default:
        // gcc -Wall (all warnings enabled) complains if all cases
        // aren't handled, so we explicitly handle default and assert
        ASSERT(false);
    }

    ASSERT(swtImpl || err);

    if (pszAlias) {
        str.Add(pszAlias);
        str.Add(L"::");
    }

    if (swtImpl) {
        GetExplicitImplTypeName(swtImpl.Type(), str);
        if (!pszName)
            pszName = swtImpl.Sym()->name->text;
    }
    else {
        GetExplicitImplTypeName(err, str);
    }

    if (pszName) {
        // Add dot seperator.
        str.Add(L'.');
        str.Add(pszName);
    }
}


void MetaDataHelper::GetExplicitImplTypeName(TYPESYM * type, StringBldr & str)
{
    ASSERT(type);

    TYPESYM * typeNaked = type->GetNakedType();
    TypeArray * typeArgs = NULL;

    switch (typeNaked->getKind()) {
    default:
        VSFAIL("Unhandled type in GetExplicitImplTypeName");
        return;

    case SK_TYVARSYM:
        str.Add(typeNaked->name->text);
        break;

    case SK_NUBSYM:
        typeNaked = typeNaked->asNUBSYM()->GetAts();
        if (!typeNaked) {
            VSFAIL("Why did GetAts return null?");
            return;
        }
        // Fall through.
    case SK_AGGTYPESYM:
        {
            AGGTYPESYM * typeOuter = typeNaked->asAGGTYPESYM()->outerType;
            AGGSYM * agg = typeNaked->getAggregate();

            if (typeOuter) {
                GetExplicitImplTypeName(typeOuter, str);
                str.Add(L'.');
            }
            else {
                ASSERT(agg->Parent() && !agg->Parent()->isAGGSYM());
                int cch = str.Length();
                GetFullName(agg->Parent(), str, agg);
                if (cch < str.Length())
                    str.Add(L'.');
            }
            str.Add(agg->name->text);

            typeArgs = typeNaked->asAGGTYPESYM()->typeArgsThis;
        }
        break;

    case SK_ERRORSYM:
        {
            ERRORSYM * err = typeNaked->asERRORSYM();
            SYM * symPar = err->parent;

            if (symPar && symPar->isTYPESYM()) {
                GetExplicitImplTypeName(symPar->asTYPESYM(), str);
                str.Add(L'.');
            }
            else if (symPar && symPar->isNSAIDSYM()) {
                symPar = symPar->asNSAIDSYM()->GetNS();
                int cch = str.Length();
                GetFullName(symPar, str, err);
                if (cch < str.Length())
                    str.Add(L'.');
            }
            str.Add(err->nameText->text);

            typeArgs = err->typeArgs;
        }
        break;
    }

    if (typeArgs && typeArgs->size > 0) {
        str.Add(L'<');
        for (int i = 0; i < typeArgs->size; i++) {
            if (i > 0)
                str.Add(L',');
            GetExplicitImplTypeName(typeArgs->Item(i), str);
        }
        str.Add(L'>');
    }

    // Add ptr and array modifiers
    AddTypeModifiers(type, str);
}


// appends array and ptr modifiers
void MetaDataHelper::AddTypeModifiers(TYPESYM *type, StringBldr & str)
{
    switch (type->getKind()) {
    case SK_AGGTYPESYM:
    case SK_TYVARSYM:
    case SK_ERRORSYM:
    case SK_NUBSYM:
        break;
    case SK_ARRAYSYM:
        // NOTE: In C# a 2-dim array of 1-dim array of int is int[,][].
        // This produces int[][,] as metadata requires.
        AddTypeModifiers(type->asARRAYSYM()->elementType(), str);
        str.Add(L'[');
        for (int i = 1; i < type->asARRAYSYM()->rank; i++)
            str.Add(L',');
        str.Add(L']');
        break;
    case SK_PTRSYM:
        AddTypeModifiers(type->asPTRSYM()->baseType(), str);
        str.Add(L'*');
        break;
    default:
        ASSERT(!"Unknown symbol type");
        break;
    }
}

void MetaDataHelper::GetTypeVars(SYM * sym, StringBldr & str, SYM * symInner)
{
    if (sym->isAGGSYM() && sym->asAGGSYM()->isArityInName) {
        TypeArray * typeVars = sym->asAGGSYM()->typeVarsThis;
        ASSERT(typeVars->size > 0); // isArityInName shouldn't be set otherwise.
        str.Add(L'`');
        str.AddNum(typeVars->size);
    }
}


void MetaDataHelperXml::EscapeSpecialChars(StringBldr & str, int ichMin)
{
    // Replace:
    // . with #
    // , with @
    // < with {
    // > with }
    for (PWCH pch = str.Str() + ichMin; *pch; pch++) {
        switch (*pch) {
        case L'.':
            *pch = L'#';
            break;
        case L',':
            *pch = L'@';
            break;
        case L'<':
            *pch = L'{';
            break;
        case L'>':
            *pch = L'}';
            break;
        }
    }
}


void MetaDataHelperXml::GetTypeVars(SYM * sym, StringBldr & str, SYM * symInner)
{
    if (sym->isAGGSYM()) {
        TypeArray * typeVars = sym->asAGGSYM()->typeVarsThis;
        if (typeVars->size) {
            str.Add(L'`');
            str.AddNum(typeVars->size);
        }
    }
    else if (sym->isMETHSYM()) {
        TypeArray * typeVars = sym->asMETHSYM()->typeVars;
        if (typeVars->size) {
            str.Add(L"``");
            str.AddNum(typeVars->size);
        }
    }
}

void MetaDataHelperXml::GetTypeName(TYPESYM * type, StringBldr & str)
{
    m_compiler->EnsureState(type, AggState::DefinedMembers);
    ASSERT(type->AggState() >= AggState::DefinedMembers);

    switch (type->getKind()) {
    default:
        ASSERT(0);
        return;
	case SK_NUBSYM:
        GetTypeName(type->asNUBSYM()->GetAts(), str);
        return;

    case SK_ARRAYSYM:
        GetTypeName(type->asARRAYSYM()->elementType(), str);
        if (type->asARRAYSYM()->rank == 1) {
            // Single dimensional array.
            str.Add(L"[]");
        } else {
            // Known rank > 1
            str.Add(L'[');
            for (int i = 1; i < type->asARRAYSYM()->rank; ++i)    // Do 1 less
                str.Add(L"0:,");
            str.Add(L"0:]");
        }
        return;

    case SK_PTRSYM:
        GetTypeName(type->asPTRSYM()->baseType(), str);
        str.Add(L'*');
        return;

    case SK_TYVARSYM:
        str.Add(L'`');
        if (type->asTYVARSYM()->isMethTyVar)
            str.Add(L'`');
        str.AddNum(type->asTYVARSYM()->indexTotal);
        return;

    case SK_PINNEDSYM:
        GetTypeName(type->asPINNEDSYM()->baseType(), str);
        str.Add(L'^');
        return;

    case SK_PARAMMODSYM:
        GetTypeName(type->asPARAMMODSYM()->paramType(), str);
        str.Add(L'@');
        return;

    case SK_MODOPTTYPESYM:
        GetTypeName(type->asMODOPTTYPESYM()->baseType(), str);
        return;

    case SK_VOIDSYM:
        type = m_compiler->GetReqPredefType(PT_SYSTEMVOID, false);
        // fall-through to SK_AGGTYPESYM
    case SK_AGGTYPESYM:
        {
            AGGSYM * agg = type->asAGGTYPESYM()->getAggregate();

            if (!agg->Parent()) { 
                // __arglist syms do not have a parent, so just return. There's no spec on how to handle these parameters
                // in xml doc comments, but in v7.0 and v7.1 we have just left the parameter nbbame blank.
                ASSERT(type == m_compiler->getBSymmgr().GetArglistSym());
                return;
            }

            AGGTYPESYM * typeOuter = type->asAGGTYPESYM()->outerType;

            if (typeOuter) {
                GetTypeName(typeOuter, str);
                str.Add(L'.');
            }
            else {
                ASSERT(agg->Parent()->isNSSYM());
                int cch = str.Length();
                GetFullName(agg->Parent(), str, agg);
                if (cch < str.Length())
                    str.Add(L'.');
            }
            str.Add(agg->name->text);

            TypeArray * typeArgs = type->asAGGTYPESYM()->typeArgsThis;

            if (typeArgs->size > 0) {
                str.Add(L'{');
                for (int i = 0; i < typeArgs->size; i++) {
                    if (i > 0)
                        str.Add(L',');
                    GetTypeName(typeArgs->Item(i), str);
                }
                str.Add(L'}');
            }
        }
        return;
    }
}

TypeNameSerializer::TypeNameSerializer(COMPILER * compiler)
{
    m_compiler = compiler;
    m_qbldr = compiler->GetTypeNameBuilder();
}

TypeNameSerializer::~TypeNameSerializer()
{
    m_compiler->ReleaseTypeNameBuilder();
}

// returns NULL for failure (after reporting the error)
BSTR TypeNameSerializer::GetAssemblyQualifiedTypeName(TYPESYM * type, bool fOpenType)
{
    BSTR bstr = NULL;
    if (type->pszAssemblyQualifiedName != NULL)
        return SysAllocString(type->pszAssemblyQualifiedName);


    if (!GetAssemblyQualifiedTypeNameCore(type, fOpenType)) {
        m_qbldr->Clear();
        return NULL;
    }

    HRESULT hr = m_qbldr->ToString(&bstr);
    
    // Always call clear even if something else fails
    HRESULT hr2 = m_qbldr->Clear();

    if (CheckHR(type, hr) && CheckHR(type, hr2))
    {
        // cache this for later
        type->pszAssemblyQualifiedName = m_compiler->getGlobalSymAlloc().AllocStr(bstr);
        return bstr;
    }

    if (bstr)
        SysFreeString(bstr);
    return NULL;
}

bool TypeNameSerializer::CheckHR(TYPESYM * type, HRESULT hr)
{
    if (SUCCEEDED(hr))
        return true;
    m_compiler->Error( NULL, FTL_TypeNameBuilderError, type, m_compiler->ErrHR(hr, true));
    return false;
}

bool TypeNameSerializer::GetAssemblyQualifiedTypeNameCore(TYPESYM * type, bool fOpenType)
{
    TYPESYM * temp = type->GetNakedType();
    if (temp->isNUBSYM()) {
        // Convert the NUBSYM to a Nullable AGGTYPESYM
        temp = temp->asNUBSYM()->GetAts();
    }
    else if (temp->isVOIDSYM()) {
        temp = m_compiler->GetReqPredefType(PT_SYSTEMVOID, false);
    }
    if (temp == NULL || !temp->isAGGTYPESYM()) {
        ASSERT(temp && temp->isERRORSYM());
        return false;
    }
    AGGTYPESYM * ats = temp->asAGGTYPESYM();
    AGGSYM * agg = ats->getAggregate();
    HRESULT hr;

    if (!GetAggName(agg))
        return false;

    if (!fOpenType && ats->typeArgsAll->size > 0) {
        hr = m_qbldr->OpenGenericArguments();
        if (!CheckHR(type, hr))
            return false;

        for (int i = 0; i < ats->typeArgsAll->size; i++) {
            hr = m_qbldr->OpenGenericArgument();
            if (!CheckHR(type, hr) || !GetAssemblyQualifiedTypeNameCore( ats->typeArgsAll->Item(i), false))
                return false;

            hr = m_qbldr->CloseGenericArgument();
            if (!CheckHR(type, hr))
                return false;
        }

        hr = m_qbldr->CloseGenericArguments();
        if (!CheckHR(type, hr))
            return false;
    }

    if (!AddTypeModifiers(type))
        return false;

    PCWSTR pszAssem = m_compiler->importer.GetAssemblyName(agg);
    if (!pszAssem)
        return true;

    hr = m_qbldr->AddAssemblySpec(pszAssem);
    return CheckHR(type, hr);
}

bool TypeNameSerializer::GetAggName(AGGSYM * agg)
{
    WCHAR szName[MAX_FULLNAME_SIZE];

    if (agg->isNested()) {
        if (!GetAggName(agg->GetOuterAgg()))
            return false;
    }
    if (!MetaDataHelper::GetMetaDataName(agg, szName, lengthof(szName)))
        return false;

    HRESULT hr = m_qbldr->AddName(szName);
    return CheckHR(agg->getThisType(), hr);
}

bool TypeNameSerializer::AddTypeModifiers(TYPESYM * type)
{
    HRESULT hr = S_OK;

    switch (type->getKind()) {
    case SK_AGGTYPESYM:
    case SK_TYVARSYM:
    case SK_ERRORSYM:
    case SK_NUBSYM:
    case SK_VOIDSYM:
        break;
    case SK_ARRAYSYM:
        // NOTE: In C# a 2-dim array of 1-dim array of int is int[,][].
        // This produces int[][,] as metadata requires.
        if (!AddTypeModifiers(type->asARRAYSYM()->elementType()))
            return false;
        if (type->asARRAYSYM()->rank == 1) {
            hr = m_qbldr->AddSzArray();
        }
        else {
            hr = m_qbldr->AddArray(type->asARRAYSYM()->rank);
        }
        break;

    case SK_PTRSYM:
        if (!AddTypeModifiers(type->asPTRSYM()->baseType()))
            return false;
        hr = m_qbldr->AddPointer();
        break;

    default:
        VSFAIL("Unknown symbol type");
        break;
    }

    return CheckHR(type, hr);
}


CAggSymNameEncImportIter::CAggSymNameEncImportIter(COMPILER *compiler, AGGSYM *agg, NAME *name) :
    m_compiler(compiler), m_agg(agg), m_name(name), m_curSym(NULL), m_fSimpleName(true)
{
}

bool CAggSymNameEncImportIter::MoveNext()
{
    if (m_curSym == NULL)
    {
        // First, try to look for the given name directly
        SYM *sym = m_compiler->getBSymmgr().LookupAggMember(m_name, m_agg, MASK_ALL);

        if (sym == NULL)
        {
            sym = NextExplImplWithName(m_agg->firstChild);
            if (sym == NULL)
            {
                sym = m_compiler->getBSymmgr().LookupAggMember(m_compiler->namemgr->GetPredefName(PN_INDEXERINTERNAL), m_agg, MASK_ALL);
            }
            else
            {
                m_fSimpleName = false;
            }
        }

        m_curSym = sym;
    }
    else
    {
        if (m_fSimpleName)
        {
            m_curSym = m_curSym->nextSameName;
        }
        else
        {
            m_curSym = NextExplImplWithName(m_curSym->nextChild);
        }
    }
    return (m_curSym != NULL);
}

SYM *CAggSymNameEncImportIter::Current()
{
    ASSERT(m_curSym != NULL);
    return m_curSym;
}

bool CAggSymNameEncImportIter::IsExplImpl(SYM *sym)
{
    switch (sym->getKind()) 
    {
        case SK_EVENTSYM:
            return sym->asEVENTSYM()->IsExpImpl();

        case SK_METHSYM:
        case SK_PROPSYM:
            return sym->asMETHPROPSYM()->IsExpImpl();

        default:
            break;
    }
    return false;
}
bool CAggSymNameEncImportIter::IsEplImplWithName(SYM *sym)
{
    if (!IsExplImpl(sym))
        return false;

    WCHAR nameBuffer[MAX_FULLNAME_SIZE];
    MetaDataHelper::GetExplicitImplName(sym, nameBuffer, lengthof(nameBuffer));
    return (wcscmp(m_name->text, nameBuffer) == 0);
}

SYM *CAggSymNameEncImportIter::NextExplImplWithName(SYM *pSym)
{
    for (SYM *curSym = pSym; curSym != NULL; curSym = curSym->nextChild)
    {
        if (IsEplImplWithName(curSym))
            return curSym;
    }
    return NULL;
}

