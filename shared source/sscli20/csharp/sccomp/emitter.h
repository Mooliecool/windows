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
// File: emitter.h
//
// Defines the structures used to emit COM+ metadata and create executable files.
// ===========================================================================

#ifndef __emitter_h__
#define __emitter_h__

#include <iceefilegen.h>

#define CTOKREF 1000   // Number of token refs per block. Fits nicely in a page.

// structure for saving security attributes.
struct SECATTRSAVE {
    SECATTRSAVE * next;
    mdToken ctorToken;
    METHSYM * method;
    BYTE * buffer;
    unsigned bufferSize;
};


// Maps from the triple (agg-type-sym, member, type-args) to token.
class TypeMemTokMap
{
private:
    struct Key
    {
        SYM * sym;
        TypeArray * typeArgsCls;
        TypeArray * typeArgsMeth;
        unsigned hash;

        Key(SYM * sym, TypeArray * typeArgsCls, TypeArray * typeArgsMeth) {
            ASSERT(typeArgsCls && typeArgsCls->size > 0 || typeArgsMeth && typeArgsMeth->size > 0);
            if (typeArgsCls && !typeArgsCls->size)
                typeArgsCls = NULL;
            if (typeArgsMeth && !typeArgsMeth->size)
                typeArgsMeth = NULL;
            ASSERT(offsetof(Key, sym) == 0 && offsetof(Key, hash) == 3 * sizeof(void *));
            this->sym = sym;
            this->typeArgsCls = typeArgsCls;
            this->typeArgsMeth = typeArgsMeth;
            this->hash = NAMEMGR::HashString(reinterpret_cast<PCWSTR>(this), offsetof(Key, hash) / sizeof(WCHAR));
        }

        bool operator==(Key & key) {
            return key.hash == hash && key.sym == sym && key.typeArgsCls == typeArgsCls && key.typeArgsMeth == typeArgsMeth;
        }
    };

    struct Entry
    {
        Key key;
        mdToken tok;
        Entry * next;
    };

    NRHEAP * heap;
    COMPILER * compiler;
    Entry ** prgent;
    int centHash;
    int centTot;

    Entry * Find(Key & key, Entry *** ppentIns);
    Entry * New(Key & key, Entry ** pentIns);
    void GrowTable();

    TypeMemTokMap() { }

public:
    static TypeMemTokMap * Create(COMPILER * compiler, NRHEAP * heap);
    mdToken * GetTokPtr(SYM * sym, TypeArray * typeArgsCls, TypeArray * typeArgsMeth);
};


/*
 *  The class that handles emitted PE files, and generating the metadata
 *  database within the files.
 */
class EMITTER
{
public:
    EMITTER();
    ~EMITTER();
    void Term();

    COMPILER * compiler();

    bool BeginOutputFile();
    bool EndOutputFile(bool writeFile);

    void SetEntryPoint(METHSYM *sym);
    void FindEntryPoint(OUTFILESYM *outfile);
    void FindEntryPointInClass(AGGSYM *parent);

    void EmitTypeVars(SYM * sym);
    void EmitAggregateDef(PAGGSYM sym);
    void EmitAggregateSpecialFields(PAGGSYM sym);
    void EmitAggregateBases(PAGGSYM sym);
    void EmitMembVarDef(PMEMBVARSYM sym);
    void EmitPropertyDef(PPROPSYM sym);
    void EmitEventDef(PEVENTSYM sym);
    void EmitMethodDef(PMETHSYM sym);
    void EmitMethodInfo(METHSYM * sym, METHINFO * info);
    void EmitMethodImpl(PMETHSYM sym);
    void EmitMethodImpl(PMETHSYM sym, SymWithType swtExpImpl);
    void DefineParam(mdToken tokenMethProp, int index, mdToken *paramToken);
    void EmitParamProp(mdToken tokenMethProp, int index, TYPESYM *type, PARAMINFO *paramInfo, bool hasDefaultValue, DWORD etDefaultValue, BlobBldr &blob);
    void *EmitMethodRVA(PMETHSYM sym, ULONG cbCode, ULONG alignment);
    void ResetMethodFlags(METHSYM *sym);
    DWORD GetMethodFlags(METHSYM *sym);
    DWORD GetMembVarFlags(MEMBVARSYM *sym);
    DWORD GetPropertyFlags(PROPSYM *sym);
    DWORD GetEventFlags(EVENTSYM *sym);
    void EmitDebugMethodInfoStart(METHSYM * sym);
    void EmitDebugMethodInfoStop(METHSYM * sym, int ilOffsetEnd);
    void EmitDebugBlock(METHSYM * sym, int count, unsigned int * offsets, SourceExtent * extents);
    void EmitDebugTemporary(TYPESYM * type, PCWSTR name, mdToken tkLocalVarSig, unsigned slot);
    void EmitDebugLocal(LOCVARSYM * sym, mdToken tkLocalVarSig, int ilOffsetStart, int ilOffsetEnd);
    void EmitDebugLocalConst(LOCVARSYM * sym);
    void EmitDebugScopeStart(int ilOffsetStart);
    void EmitDebugScopeEnd(int ilOffsetEnd);
    void EmitCustomAttribute(BASENODE *parseTree, mdToken token, METHSYM *method, BYTE *buffer, unsigned bufferSize);
    bool HasSecurityAttributes() { return cSecAttrSave > 0; }
    void EmitSecurityAttributes(BASENODE *parseTree, mdToken token);

    void EmitTypeForwarder(AGGTYPESYM * type);
    void EmitNestedTypeForwarder(AGGSYM *agg, mdAssemblyRef tkAssemblyRef);

    mdToken GetMethodRef(METHSYM * sym, AGGTYPESYM * aggType, TypeArray * typeArgsMeth);
    mdToken GetMethodInstantiation(mdToken parent, TypeArray * typeArgsMeth);
    mdToken GetMembVarRef(PMEMBVARSYM sym);
    mdToken GetMembVarRef(PMEMBVARSYM sym, AGGTYPESYM * aggType);
    mdToken GetTypeRef(PTYPESYM sym, bool noDefAllowed = false);
    mdToken GetAggRef(PAGGSYM sym, bool noDefAllowed = false);
    mdToken GetArrayMethodRef(PARRAYSYM sym, ARRAYMETHOD methodId);
    mdToken GetSignatureRef(TypeArray * pta);
    mdString GetStringRef(const STRCONST * string);
    mdToken GetModuleToken();
    mdToken GetGlobalFieldDef(METHSYM * sym, unsigned int count, TYPESYM * type, unsigned int size = 0);
    mdToken GetGlobalFieldDef(METHSYM * sym, unsigned int count, unsigned int size, BYTE ** pBuffer);

    mdToken GetErrRef(ERRORSYM * err);
    mdToken GetErrAssemRef();

    void BeginIterator(int cIteratorLocals);
    void ResetIterator();
    void EndIterator();
    void EmitForwardedIteratorDebugInfo(METHSYM * methFrom, METHSYM * methTo);

protected:

    void CheckHR(HRESULT hr);
    void CheckHR(int errid, HRESULT hr);
    void CheckHRDbg(HRESULT hr);
    void MetadataFailure(HRESULT hr);
    void DebugFailure(HRESULT hr);
    void MetadataFailure(int errid, HRESULT hr);
    DWORD FlagsFromAccess(ACCESS access);
    PCOR_SIGNATURE BeginSignature();
    PCOR_SIGNATURE EmitSignatureByte(PCOR_SIGNATURE curSig, BYTE b);
    PCOR_SIGNATURE EmitSignatureUInt(PCOR_SIGNATURE curSig, ULONG b);
    PCOR_SIGNATURE EmitSignatureToken(PCOR_SIGNATURE curSig, mdToken token);
    PCOR_SIGNATURE EmitSignatureType(PCOR_SIGNATURE sig, PTYPESYM type);
    PCOR_SIGNATURE EmitSignatureTypeVariables(PCOR_SIGNATURE sig, TypeArray * typeArgs);

    mdToken GetTypeSpec(PTYPESYM sym);
    PCOR_SIGNATURE GrowSignature(PCOR_SIGNATURE curSig);
    PCOR_SIGNATURE EnsureSignatureSize(ULONG cb);
    PCOR_SIGNATURE EndSignature(PCOR_SIGNATURE curSig, int * cbSig);
    PCOR_SIGNATURE SignatureOfMembVar(PMEMBVARSYM sym, int * cbSig);
    PCOR_SIGNATURE SignatureOfMethodOrProp(PMETHPROPSYM sym, int * cbSig);
    mdSignature GetDebugVarSig(PTYPESYM type);
    BYTE GetConstValue(PMEMBVARSYM sym, LPVOID tempBuff, LPVOID * ppValue, size_t * pcb);
    bool VariantFromConstVal(AGGTYPESYM * type, CONSTVAL * cv, VARIANT * v);
    mdToken GetScopeForTypeRef(AGGSYM *sym);
    void EmitCustomDebugInfo(METHSYM * sym);
    bool EmitDebugNamespace(NSAIDSYM * ns);
    bool ComputeDebugNamespace(NSAIDSYM *ns, StringBldr & bldr);
    void EmitForwardedDebugInfo(METHSYM * methFrom, METHSYM * methTo);
    bool EmitDebugAlias(ALIASSYM * as);
    void EmitExternAliasNames(METHSYM * methsym);
    void EmitExternalAliasName(NAME * name, INFILESYM * infile);
    void InitDocumentWriter(INFILESYM * infile);
    void CheckExtent(SourceExtent & extent);
    mdToken GetMethodRefGivenParent(PMETHSYM sym, mdToken parent);
    mdToken GetMembVarRefGivenParent(PMEMBVARSYM sym, mdToken parent);
    void DumpIteratorLocals(METHSYM * meth);

    void RecordEmitToken(mdToken * tokref);
    void EraseEmitTokens();

    void HandleAttributeError(HRESULT hr, BASENODE *parseTree, METHSYM *method);
    void SaveSecurityAttribute(mdToken token, mdToken ctorToken, METHSYM * method, BYTE * buffer, unsigned bufferSize);
    void FreeSavedSecurityAttributes();

    // For accumulating security attributes
    SECATTRSAVE * listSecAttrSave;
    unsigned cSecAttrSave;
    mdToken tokenSecAttrSave;

    // cache a local copy of these
    IMetaDataEmit2 * metaemit;
    IMetaDataAssemblyEmit *metaassememit;
    ISymUnmanagedWriter * debugemit;
    ISymUnmanagedWriter2 * debugemit2;

    // Scratch area for signature creation.
#ifdef DEBUG
    bool           sigBufferInUse;
#endif
    PCOR_SIGNATURE sigBuffer;
    PCOR_SIGNATURE sigEnd;    // End of allocated area.

    mdToken globalTypeToken;
    mdToken tokErrAssemRef;

    struct CDIIteratorLocalsInfo * pIteratorLocalsInfo;

    // Heap for storing token addresses.
    struct TOKREFS {
        struct TOKREFS * next;
        mdToken * tokenAddrs[CTOKREF];
    };
    NRHEAP tokrefHeap;                  // Heap to allocate from.
    TOKREFS * tokrefList;               // Head of the tokref list.
    int iTokrefCur;                     // Current index within tokrefList.

    struct CompGenTypeToken {
        struct CompGenTypeToken *   next;
        mdToken                     tkTypeDef;
        unsigned int                size;
    };
    CompGenTypeToken * cgttHead;

    NRMARK mark;

    TypeMemTokMap * pmap;
    int ipScopePrev;
};

#endif //__emitter_h__
