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
// File: emitter.cpp
//
// Routines for emitting CLR metadata and creating executable files.
// ===========================================================================

#include "stdafx.h"

extern GUID guidSourceHashMD5;

#define SIGBUFFER_GROW 256      // Number of bytes to grow signature buffer by.
#define MAX_UTF8_CHAR  3        // max size of UTF8 char encoding
#define SAFE_FULLNAME_CHARS     (MAX_FULLNAME_SIZE/MAX_UTF8_CHAR)

/*
 * Constructor
 */
EMITTER::EMITTER()
: tokrefHeap(compiler())
{
#if DEBUG
    sigBufferInUse = false;
#endif
    sigBuffer = NULL;

    tokrefList = NULL;
    iTokrefCur = CTOKREF;   // Signal that we must allocate a new block right away.
    cgttHead = NULL;

    debugemit2 = NULL;

    pIteratorLocalsInfo = NULL;

    cSecAttrSave = 0;
    listSecAttrSave = NULL;

    pmap = NULL;
}

/*
 * Destructor
 */
EMITTER::~EMITTER()
{
    Term();
}

/*
 * Terminate everything.
 */
void EMITTER::Term()
{
    FreeSavedSecurityAttributes();

    // Free the signature buffer.
    if (sigBuffer) {
#ifdef DEBUG
        sigBufferInUse = false;
#endif
        compiler()->globalHeap.Free(sigBuffer);
        sigBuffer = NULL;
    }

    if (debugemit2 != NULL) {
        debugemit2->Release();
        debugemit2 = NULL;
    }

    cgttHead = NULL;
    tokrefHeap.Mark( &mark);
    EraseEmitTokens();  // Forget all tokens in this output file.
    tokrefHeap.FreeHeap();
}

/*
 * Semi-generic method to check for failure of a meta-data method.
 */
__forceinline void EMITTER::CheckHR(HRESULT hr)
{
    if (FAILED(hr))
        MetadataFailure(hr);
    SetErrorInfo(0, NULL); // Clear out any stale ErrorInfos
}

/*
 * Semi-generic method to check for failure of a meta-data method. Passed
 * an error ID in cases where the generic one is no good.
 */
__forceinline void EMITTER::CheckHR(int errid, HRESULT hr)
{
    if (FAILED(hr))
        MetadataFailure(errid, hr);
    SetErrorInfo(0, NULL); // Clear out any stale ErrorInfos
}

/*
 * Semi-generic method to check for failure of a debug method.
 */
__forceinline void EMITTER::CheckHRDbg(HRESULT hr)
{
    if (FAILED(hr))
        DebugFailure(hr);
    SetErrorInfo(0, NULL); // Clear out any stale ErrorInfos
}

/*
 * Handle a generic meta-data API failure.
 */
void EMITTER::MetadataFailure(HRESULT hr)
{
    MetadataFailure(FTL_MetadataEmitFailure, hr);
}

/*
 * Handle a generic debug API failure.
 */
void EMITTER::DebugFailure(HRESULT hr)
{
    WCHAR filename[MAX_PATH];

    PEFile::GetPDBFileName(compiler()->curFile->GetOutFile(), filename, lengthof(filename));
    compiler()->Error(NULL, FTL_DebugEmitFailure, compiler()->ErrHR(hr), filename);
}

/*
 * Handle an API failure. The passed error code is expected to take one insertion string
 * that is filled in with the HRESULT.
 */
void EMITTER::MetadataFailure(int errid, HRESULT hr)
{
    compiler()->Error(NULL, errid, compiler()->ErrHR(hr), compiler()->curFile->GetOutFile()->name->text);
}


/*
 * Begin emitting an output file. If an error occurs, it is reported to the user
 * and then false is returned.
 */
bool EMITTER::BeginOutputFile()
{
    // cache a local copy, don't addRef because we don't own these!
    metaemit = compiler()->curFile->GetEmit();
    metaassememit = compiler()->curFile->GetAssemblyEmitter();
    debugemit = compiler()->curFile->GetDebugEmit();
    if (!debugemit ||
        FAILED(debugemit->QueryInterface(IID_ISymUnmanagedWriter2, (void**)&debugemit2))) {
            debugemit2 = NULL;
    }
    globalTypeToken = mdTokenNil;
    tokErrAssemRef = mdTokenNil;

    // Initially, no token is set.
    tokrefHeap.Mark( &mark);

    // Create the token map.
    ASSERT(!pmap);
    pmap = TypeMemTokMap::Create(compiler(), &tokrefHeap);
    ipScopePrev = 0;

    return true;
}


/*
 * End writing an output file. If true is passed, the output file is actually written.
 * If false is passed (e.g., because an error occurred), the output file is not
 * written.
 *
 * true is returned if the output file was successfully written.
 */
bool EMITTER::EndOutputFile(bool writeFile)
{
    PAL_TRY {
        OUTFILESYM *outfile = compiler()->curFile->GetOutFile();

        int cerr = compiler()->ErrorCount();

        if (!compiler()->FEncBuild()) {
            //
            // do module attributes
            //
            compiler()->funcBRec.setUnsafe(false);
            GlobalAttrBind::Compile(compiler(), outfile, outfile->attributes, GetModuleToken());
            compiler()->funcBRec.resetUnsafe();
        }

        if (compiler()->FAbortEarly(cerr)) {
            writeFile = false;
        }

        bool fDll = outfile->isDll;

        if (!compiler()->FEncBuild()) {
            if (fDll) {
                outfile->entrySym = NULL;  // Dlls don't have entry points.
            }
            else {
                if (outfile->entrySym == NULL && writeFile) {
                    ASSERT (!outfile->isUnnamed());
                    compiler()->Error(NULL, ERR_NoEntryPoint, outfile->name->text);
                    writeFile = false;
                }
            }

            if (writeFile && !compiler()->options.m_fNOCODEGEN) {
                
                // Set output file attributes.
                compiler()->curFile->SetAttributes(fDll);

                // The rest is done by PEFile
            }
        }
    }
    PAL_FINALLY {
        // This needs to always happen, even if writing failed.

        EraseEmitTokens();  // Forget all tokens in this output file.
        metaemit = NULL;
        debugemit = NULL;
        if (debugemit2) {
            debugemit2->Release();
            debugemit2 = NULL;
        }

        globalTypeToken = mdTokenNil;
        tokErrAssemRef = mdTokenNil;

        // Free any IDocumentWriter interfaces for this output file.
        for (PINFILESYM pInfile = compiler()->curFile->GetOutFile()->firstInfile(); pInfile != NULL; pInfile = pInfile->nextInfile()) {
            if (! pInfile->isSource)
                continue;                   // Not a source file.

            if (pInfile->documentWriter) {
                pInfile->documentWriter->Release();
                pInfile->documentWriter = NULL;
            }
        }
    }
    PAL_ENDTRY

    return writeFile ? true : false;  // Never return true if writeFile was false.
}


/*
 * Begin emitting a signature. Returns a pointer to the signature that
 * should be passed to other emitting routines.
 */
PCOR_SIGNATURE EMITTER::BeginSignature()
{
    ASSERT(!sigBufferInUse);
#ifdef DEBUG
    sigBufferInUse = true;
#endif

    // If needed, create the initial allocation of the signature buffer.
    if (!sigBuffer) {
        sigBuffer = (PCOR_SIGNATURE) compiler()->globalHeap.Alloc(SIGBUFFER_GROW);
        sigEnd = sigBuffer + SIGBUFFER_GROW;
    }

    return sigBuffer;
}

/*
 * Emit a byte to the current signature.
 */
__forceinline PCOR_SIGNATURE EMITTER::EmitSignatureByte(PCOR_SIGNATURE curSig, BYTE b)
{
    ASSERT(sigBufferInUse);

    if (curSig + sizeof(BYTE) > sigEnd)
        curSig = GrowSignature(curSig);
    *curSig++ = b;
    return curSig;
}

/*
 * Emit an encoded interger to the current signature.
 */
__forceinline PCOR_SIGNATURE EMITTER::EmitSignatureUInt(PCOR_SIGNATURE curSig, ULONG u)
{
    ASSERT(sigBufferInUse);

    if (curSig + sizeof(DWORD) > sigEnd)
        curSig = GrowSignature(curSig);
    curSig += CorSigCompressData(u, curSig);
    return curSig;
}

/*
 * Emit a token to the current signature.
 */
__forceinline PCOR_SIGNATURE EMITTER::EmitSignatureToken(PCOR_SIGNATURE curSig, mdToken token)
{
    ASSERT(sigBufferInUse);

    if (curSig + sizeof(DWORD) > sigEnd)
        curSig = GrowSignature(curSig);

    curSig += CorSigCompressToken(token, curSig);
    return curSig;
}

/*
 * Finish emitting a signature. Returns the signature start and size.
 */
__forceinline PCOR_SIGNATURE EMITTER::EndSignature(PCOR_SIGNATURE curSig, int * cbSig)
{
    ASSERT(sigBufferInUse);
#ifdef DEBUG
    sigBufferInUse = false;
#endif

    *cbSig = (int)(curSig - sigBuffer);
    return sigBuffer;
}

/*
 * Ensures the signature buffer has a minimum size
 */
PCOR_SIGNATURE EMITTER::EnsureSignatureSize(ULONG cb)
{
    ASSERT(sigBufferInUse);

    ULONG cbAlloced = (ULONG)(sigEnd - sigBuffer);        // ammount allocated.

    if (cb < cbAlloced)
    {
        // Realloc the buffer and update all the pointers.
        sigBuffer = (PCOR_SIGNATURE) compiler()->globalHeap.Realloc(sigBuffer, cb);
        sigEnd = sigBuffer + cb;
    }
    return sigBuffer;
}

/*
 * Grows the signature buffer and returns a new sig pointer.
 */
PCOR_SIGNATURE EMITTER::GrowSignature(PCOR_SIGNATURE curSig)
{
    ASSERT(curSig > sigBuffer && curSig <= sigEnd);
    ASSERT(sigBufferInUse);

    int cbEmitted = (int)(curSig - sigBuffer);     // amount emitted.
    int cbAlloced = (int)(sigEnd - sigBuffer);     // ammount allocated.

    // Realloc the buffer and update all the pointers.
    sigBuffer = (PCOR_SIGNATURE) compiler()->globalHeap.Realloc(sigBuffer, SizeAdd(cbAlloced, SIGBUFFER_GROW));
    sigEnd = sigBuffer + cbAlloced + SIGBUFFER_GROW;
    curSig = sigBuffer + cbEmitted;
    return curSig;
}

PCOR_SIGNATURE EMITTER::EmitSignatureTypeVariables(PCOR_SIGNATURE sig, TypeArray * typeArgs)
{
    ASSERT(typeArgs->size > 0);

    for (int i = 0; i < typeArgs->size; i++)
        sig = EmitSignatureType(sig, typeArgs->Item(i));

    return sig;
}

/*
 * Emit a type symbol to the signature. The new current sig pointer is returned.
 * Note that out and ref params are not distinguished in the signatures, but
 * need to be distinguished in parameter properties.
 */
PCOR_SIGNATURE EMITTER::EmitSignatureType(PCOR_SIGNATURE sig, PTYPESYM type)
{
    BYTE b;

    compiler()->EnsureState(type);
    ASSERT(type->IsPrepared());

    for (;;) {
        switch (type->getKind()) {
        default:
            ASSERT(0);
            return sig;

        case SK_ARRAYSYM: {
            ARRAYSYM *arr = type->asARRAYSYM();
            if (arr->rank == 1) {
                // Single dimensional array. Emit SZARRAY, element type, size.
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_SZARRAY);
                sig = EmitSignatureType(sig, arr->elementType());
            } else {
                // Known rank > 1
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_ARRAY);
                sig = EmitSignatureType(sig, arr->elementType());
                sig = EmitSignatureUInt(sig, arr->rank);
                sig = EmitSignatureUInt(sig, 0);  // sizes.
                sig = EmitSignatureUInt(sig, arr->rank);  // lower bounds.
                for (int i = 0; i < arr->rank; ++i)
                    sig = EmitSignatureUInt(sig, 0);  // lower bound always zero.
            }
            return sig;
                          }
        case SK_VOIDSYM:
            sig = EmitSignatureByte(sig, ELEMENT_TYPE_VOID);
            return sig;

        case SK_PTRSYM:
            sig = EmitSignatureByte(sig, ELEMENT_TYPE_PTR);
            type = type->asPTRSYM()->baseType();
            break;  // continue with base type.

        case SK_PINNEDSYM:
            sig = EmitSignatureByte(sig, ELEMENT_TYPE_PINNED);
            type = type->asPINNEDSYM()->baseType();
            if (type->isPTRSYM()) {
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_BYREF);
                type = type->asPTRSYM()->baseType();
            }
            break;

        case SK_PARAMMODSYM:
            sig = EmitSignatureByte(sig, ELEMENT_TYPE_BYREF);
            type = type->asPARAMMODSYM()->paramType();
            break;  // continue with base type.

        case SK_MODOPTTYPESYM:
            {
                MODOPTSYM * opt = type->asMODOPTTYPESYM()->opt;
                if (!opt->tokEmit) {
                    BYTE sigSrc[20];
                    ULONG cbSrc = 0;
                    BYTE sigDst[20];
                    ULONG cbDst = 0;

                    sigSrc[cbSrc++] = IMAGE_CEE_CS_CALLCONV_DEFAULT;
                    sigSrc[cbSrc++] = 0; // Number of parameters
                    sigSrc[cbSrc++] = ELEMENT_TYPE_CMOD_OPT;
                    cbSrc += CorSigCompressToken(opt->tokImport, sigSrc + cbSrc);
                    sigSrc[cbSrc++] = ELEMENT_TYPE_STRING;
                    ASSERT(cbSrc <= lengthof(sigSrc));

                    const void * pvHash = NULL;
                    DWORD cbHash = 0;
                    MODULESYM * scope = opt->GetModule();
                    INFILESYM * file = opt->getInputFile();
                    if (file->GetAssemblyID() != kaidThisAssembly) {
                        CheckHR(compiler()->linker->GetAssemblyRefHash(file->mdImpFile, &pvHash, &cbHash));
                    }
                    CheckHR(metaemit->TranslateSigWithScope(
                        file->assemimport, pvHash, cbHash,
                        scope->GetMetaImport(compiler()),
                        sigSrc, cbSrc,
                        metaassememit, metaemit,
                        sigDst, lengthof(sigDst),
                        &cbDst));

                    ASSERT(sigDst[0] == IMAGE_CEE_CS_CALLCONV_DEFAULT);
                    ASSERT(sigDst[1] == 0);
                    ASSERT(sigDst[2] == ELEMENT_TYPE_CMOD_OPT);
                    PCCOR_SIGNATURE sigT = sigDst + 3;
                    opt->tokEmit = CorSigUncompressToken(sigT);
                    RecordEmitToken(&opt->tokEmit);
                }
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_CMOD_OPT);
                sig = EmitSignatureToken(sig, opt->tokEmit);
            }
            type = type->asMODOPTTYPESYM()->baseType();
            break;

        case SK_ERRORSYM:
            ASSERT(!compiler()->FAbortEarly(-1));
            ASSERT(type != compiler()->getBSymmgr().GetErrorSym());

            {
                ERRORSYM * err = type->asERRORSYM();
                ASSERT(err->parent && err->nameText && err->typeArgs);

                bool fPar = err->parent->isTYPESYM();
                if (fPar || err->typeArgs->size > 0)
                    sig = EmitSignatureByte(sig, ELEMENT_TYPE_GENERICINST);
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_CLASS);
                sig = EmitSignatureToken(sig, GetErrRef(err));
                if (fPar || err->typeArgs > 0) {
                    sig = EmitSignatureUInt(sig, err->typeArgs->size + 1);
                    if (fPar)
                        sig = EmitSignatureType(sig, err->parent->asTYPESYM());
                    else {
                        // "void *" is a sentinel meaning no parent. We can't use just void since import
                        // rejects void as a type argument.
                        sig = EmitSignatureByte(sig, ELEMENT_TYPE_PTR);
                        sig = EmitSignatureByte(sig, ELEMENT_TYPE_VOID);
                    }
                    if (err->typeArgs->size > 0)
                        sig = EmitSignatureTypeVariables(sig, err->typeArgs);
                }
            }
            return sig;

        case SK_NUBSYM:
            type = type->asNUBSYM()->GetAts();
            ASSERT(type); // Shouldn't have created the NUBSYM to begin with!
            compiler()->EnsureState(type);
            goto LAggType;

        case SK_AGGTYPESYM:
LAggType:
            {
                AGGTYPESYM *ats = type->asAGGTYPESYM();

                if (ats == compiler()->getBSymmgr().GetArglistSym()) {
                    sig = EmitSignatureByte(sig, ELEMENT_TYPE_SENTINEL);
                    return sig;
                } else if (ats == compiler()->getBSymmgr().GetNaturalIntSym()) {
                    sig = EmitSignatureByte(sig, ELEMENT_TYPE_I);
                    return sig;
                }

                b = compiler()->getBSymmgr().GetElementType(ats); 
                if (b == ELEMENT_TYPE_CLASS || b == ELEMENT_TYPE_VALUETYPE) {
                    if (ats->typeArgsAll->size)
                        sig = EmitSignatureByte(sig, ELEMENT_TYPE_GENERICINST);
                    sig = EmitSignatureByte(sig, b);
                    sig = EmitSignatureToken(sig, GetAggRef(ats->getAggregate()));
                    if (ats->typeArgsAll->size) {
                        sig = EmitSignatureUInt(sig, ats->typeArgsAll->size);
                        sig = EmitSignatureTypeVariables(sig, ats->typeArgsAll);
                    }
                } else {
                    sig = EmitSignatureByte(sig, b);
                }
            }
            return sig;

        case SK_TYVARSYM:
            sig = EmitSignatureByte(sig, (type->asTYVARSYM()->isMethTyVar) ? ELEMENT_TYPE_MVAR : ELEMENT_TYPE_VAR); 
            sig = EmitSignatureUInt(sig, type->asTYVARSYM()->indexTotal);
            return sig;
        }
    }
}


/*
 * Emit the signature of a member variable. The signature and its size
 * are returned.
 */
PCOR_SIGNATURE EMITTER::SignatureOfMembVar(PMEMBVARSYM sym, int * cbSig)
{
    PCOR_SIGNATURE sig;

    sig = BeginSignature();
    sig = EmitSignatureByte(sig, IMAGE_CEE_CS_CALLCONV_FIELD);
    if (sym->isVolatile) {
        // Mark a volatile with with a required attribute.
        AGGSYM * volatileMod = compiler()->GetOptPredefAggErr(PT_VOLATILEMOD);
        if (volatileMod) { // could be NULL if above call failed.
            sig = EmitSignatureByte(sig, ELEMENT_TYPE_CMOD_REQD);
            sig = EmitSignatureToken(sig, GetAggRef(volatileMod));
        }
    }
    sig = EmitSignatureType(sig, sym->fixedAgg ? sym->fixedAgg->getThisType() : sym->type);
    return EndSignature(sig, cbSig);
}


/*
 * Emit the signature of a method or property. The signature and its size
 * are returned.
 */
PCOR_SIGNATURE EMITTER::SignatureOfMethodOrProp(PMETHPROPSYM sym, int * pcbSig)
{
    TYPESYM * typeRet;
    TypeArray * params;

    if (sym->modOptCount) {
        SymWithType swtBase;

        if (sym->isMETHSYM() && sym->asMETHSYM()->isIfaceImpl) {
            //
            // a compiler generated explicit impl for CMOD_OPT compatibility
            //
            swtBase = sym->asMETHSYM()->asIFACEIMPLMETHSYM()->swtSlot;
        }
        else if (sym->isFAKEMETHSYM()) {
            ASSERT(sym->asFAKEMETHSYM()->parentMethSym && sym->asFAKEMETHSYM()->isVarargs);
            swtBase.Set(sym->asFAKEMETHSYM()->parentMethSym, sym->getClass()->getThisType());
        }
        else {
            // This should only happen when we're overriding an imported member with a CMOD_OPT signature.
            ASSERT(sym->getClass()->isSource);
            swtBase = sym->swtSlot;
        }

        // mpsBase shouldn't be fake.
        ASSERT(!swtBase.Sym()->isFAKEMETHSYM());
        ASSERT(!swtBase.Sym()->asMETHPROPSYM()->getClass()->isSource);

        // get existing signature
        PCCOR_SIGNATURE sig;
        ULONG cbSig;
        TypeArray * typeVarsMeth;

        if (swtBase.Sym()->isMETHSYM()) {
            ASSERT(swtBase.Meth()->typeVars->size == sym->asFMETHSYM()->typeVars->size);
            typeVarsMeth = swtBase.Meth()->typeVars;

            CheckHR(swtBase.Meth()->GetMetaImport(compiler())->GetMethodProps(
                    swtBase.Meth()->tokenImport,         // The method for which to get props.
                    NULL,                // Put method's class here.
                    NULL,    0, NULL,                // Method name
                    NULL,                    // Put flags here.
                    & sig, & cbSig,    // Method signature
                    NULL,                    // codeRVA
                    NULL));                         // Impl. Flags
        } else {
            ASSERT(sym->isPROPSYM());
            typeVarsMeth = BSYMMGR::EmptyTypeArray();

            CheckHR(swtBase.Prop()->GetMetaImport(compiler())->GetPropertyProps(
                    swtBase.Prop()->tokenImport,         // The method for which to get props.
                    NULL,                // Put method's class here.
                    NULL,    0, NULL,                // Method name
                    NULL,                    // Put flags here.
                    & sig, & cbSig,    // Method signature
                        NULL, NULL, NULL,               // Default value
                        NULL, NULL,                     // Setter, getter
                        NULL, 0, NULL));                // Other methods
        }

        int modOptCount = 0;
        compiler()->importer.ImportSignatureWithModOpts(swtBase.Sym()->GetModule(), sig, sig + cbSig, &typeRet, &params,
            &modOptCount, swtBase.Type()->getAggregate()->typeVarsAll, typeVarsMeth);
        ASSERT((uint)modOptCount == sym->modOptCount);

        ASSERT(typeRet);
        ASSERT(params);

        // Do the substitutions.
        typeRet = compiler()->getBSymmgr().SubstType(typeRet, swtBase.Type()->typeArgsAll, typeVarsMeth);
        params = compiler()->getBSymmgr().SubstTypeArray(params, swtBase.Type()->typeArgsAll, typeVarsMeth);
    }
    else {
        typeRet = sym->retType;
        params = sym->params;
    }

    PCOR_SIGNATURE sig;
    BYTE callconv;
    int cParams = params->size;

    sig = BeginSignature();

    // Set the calling convention byte.
    if (sym->isPROPSYM())
        callconv = IMAGE_CEE_CS_CALLCONV_PROPERTY;
    else if (sym->asFMETHSYM()->isVarargs)
        callconv = IMAGE_CEE_CS_CALLCONV_VARARG;
    else
        callconv = IMAGE_CEE_CS_CALLCONV_DEFAULT;

    if (! sym->isStatic)
        callconv |= IMAGE_CEE_CS_CALLCONV_HASTHIS;
    bool isGeneric = sym->isFMETHSYM() && sym->asFMETHSYM()->typeVars->size != 0;
    if (isGeneric)
        callconv |= IMAGE_CEE_CS_CALLCONV_GENERIC;
    sig = EmitSignatureByte(sig, callconv);
    if (isGeneric)
        sig = EmitSignatureUInt(sig, sym->asFMETHSYM()->typeVars->size);

    int realParams = cParams;
    if (sym->isFMETHSYM() && sym->asFMETHSYM()->isVarargs) {
        if (sym->isFAKEMETHSYM()) {
            // this is a ref 
            ASSERT(sym->asFAKEMETHSYM()->parentMethSym);
            if (sym->asFAKEMETHSYM()->parentMethSym->params->size <= cParams) {
                // the original has fewer params (or the same), so the ref must include a sentinel
                realParams--; 
            } else {
                // otherwise the original has more params, so the ref does not have a sentinel...
            }
        } else { // this is the def
            // the def paramcount does not include the sentinel, and its cParam does not count it:
            realParams--;
            cParams--;
        }
    }

    // Emit arg count.
    sig = EmitSignatureUInt(sig, realParams);

    sig = EmitSignatureType(sig, typeRet);

    // Emit args.
    for (int i = 0; i < cParams; ++i)
        sig = EmitSignatureType(sig, params->Item(i));

    // And we're done.
    return EndSignature(sig, pcbSig);
}

mdToken EMITTER::GetModuleToken()
{
    IMetaDataImport2 *metaimport= NULL;
    metaemit->QueryInterface(IID_IMetaDataImport2, (void**) &metaimport);
    mdModule token;
    CheckHR(metaimport->GetModuleFromScope(&token));
    metaimport->Release();
    return token;
}


/*
 * Emit a signature into a PE and get back a token representing it.
 */
mdToken EMITTER::GetSignatureRef(TypeArray * pta)
{
    PCOR_SIGNATURE sig;
    int cbSig;

    // This should only be called on normalized type arrays.
    ASSERT(pta == compiler()->getBSymmgr().SubstTypeArray(pta, SubstTypeFlags::NormAll));

    if (!pta->tok) {
        // Create signature for the types.
        sig = BeginSignature();
        sig = EmitSignatureByte(sig, IMAGE_CEE_CS_CALLCONV_LOCAL_SIG);
        sig = EmitSignatureUInt(sig, pta->size);  // Count of types.
        for (int i = 0; i < pta->size; ++i) {
            ASSERT(pta->Item(i) != compiler()->getBSymmgr().GetVoid());
            sig = EmitSignatureType(sig, pta->Item(i));
        }
        sig = EndSignature(sig, &cbSig);

        // Define a signature token.
        CheckHR(metaemit->GetTokenFromSig(sig, cbSig, &pta->tok));

        RecordEmitToken(&pta->tok);
    }

    return pta->tok;
}


mdToken EMITTER::GetMethodRef(METHSYM * meth, AGGTYPESYM * ats, TypeArray * typeArgsMeth)
{
    ASSERT(ats && ats->getAggregate() == meth->getClass());

    TypeArray * typeArgsCls = ats->typeArgsAll;
    if (typeArgsCls->size)
        typeArgsCls = compiler()->getBSymmgr().SubstTypeArray(typeArgsCls, SubstTypeFlags::NormAll);

    if (typeArgsMeth && typeArgsMeth->size) {
        typeArgsMeth = compiler()->getBSymmgr().SubstTypeArray(typeArgsMeth, SubstTypeFlags::NormAll);

        // A generic method instance.
        mdToken * ptok = pmap->GetTokPtr(meth, typeArgsCls, typeArgsMeth);
        if (*ptok)
            return *ptok;

        ASSERT(!meth->getBogus());
        ASSERT(!ats->getBogus());

        *ptok = GetMethodInstantiation(GetMethodRef(meth, ats, NULL), typeArgsMeth);

        return *ptok;
    }

    if (typeArgsCls->size) {
        // In a generic type.
        mdToken * ptok = pmap->GetTokPtr(meth, typeArgsCls, NULL);
        if (*ptok)
            return *ptok;

        ASSERT(!meth->getBogus());
        ASSERT(!ats->getBogus());

        *ptok = GetMethodRefGivenParent(meth, GetTypeSpec(ats));

        return *ptok;
    }

    // Not in a generic type.
    if (!meth->tokenEmit) {
        // Create a memberRef token for this symbol.
        ASSERT(!meth->getBogus());
        ASSERT(!ats->getBogus());

        // The runtime doesn't allow typedef in this case, even though we might have one
        // in the case of a "base" call where we have a memberref to a non-existent member.

        meth->tokenEmit = GetMethodRefGivenParent(meth, GetAggRef(meth->getClass(), true));
        RecordEmitToken(&meth->tokenEmit);
    }

    return meth->tokenEmit;
}


mdToken EMITTER::GetMethodRefGivenParent(PMETHSYM sym, mdToken parent) 
{
    INFILESYM *inputfile;
    mdMemberRef memberRef;
    PCOR_SIGNATURE sig;
    int cbSig;
    const WCHAR * nameText;
    WCHAR nameBuffer[MAX_FULLNAME_SIZE];
    // See if the class come from metadata or source code.
    inputfile = sym->getInputFile();
    if (inputfile->isSource || sym->isFAKEMETHSYM()) {
        ASSERT(sym->getClass()->typeVarsAll->size 
            || (inputfile->getOutputFile() != compiler()->curFile->GetOutFile())
            || (sym->isFAKEMETHSYM()));  // If it's in our file, a def token should already 
                                                // have been assigned, unless it's in a generic class

        if (sym->isFAKEMETHSYM() && sym->asFAKEMETHSYM()->parentMethSym) {
            METHSYM * meth = sym->asFAKEMETHSYM()->parentMethSym;

            // Varargs is illegal in generics so throwing away the particular containing type
            // below (parent = newParent) doesn't hurt and is required by the CLI.
            ASSERT(!meth->typeVars->size && !meth->getClass()->typeVarsAll->size);

            mdToken newParent = GetMethodRef(meth, meth->getClass()->getThisType(), NULL);
            if (TypeFromToken(newParent) == mdtMethodDef) {
                parent = newParent;
            }
        }

        // Set "nameText" to the output name.
        if (sym->name == NULL) {
            // Explicit method implementations don't have a name in the language. Synthesize 
            // a name -- the name has "." characters in it so it can't possibly collide.
            MetaDataHelper::GetExplicitImplName(sym, nameBuffer, lengthof(nameBuffer));
            nameText = nameBuffer;
        } else {
            nameText = sym->name->text;
        }

        // Symbol defined by source code. Define a member ref by name & signature.
        sig = SignatureOfMethodOrProp(sym, &cbSig);

        CheckHR(metaemit->DefineMemberRef(
            parent,                         // ClassRef or ClassDef or TypeSpec importing a member.
            nameText,                       // member's name
            sig, cbSig,                     // point to a blob value of COM+ signature
            &memberRef));
    }
    else {
        // This symbol was imported from other metadata.
        const void *pHash = NULL;
        DWORD cbHash = 0;

        if (inputfile->GetAssemblyID() != kaidThisAssembly)
            CheckHR(compiler()->linker->GetAssemblyRefHash(inputfile->mdImpFile, &pHash, &cbHash));
        CheckHR(metaemit->DefineImportMember(             
            inputfile->assemimport,             // [IN] Assembly containing the Member. 
            pHash, cbHash,                      // [IN] Assembly hash value
            sym->GetMetaImport(compiler()),     // [IN] Import scope, with member.  
            sym->tokenImport,                   // [IN] Member in import scope.   
            inputfile->assemimport ? metaassememit : NULL, // [IN] Assembly into which the Member is imported. (NULL if member isn't being imported from an assembly).
            parent,                             // [IN] Classref or classdef or TypeSpec in emit scope.    
            &memberRef));                       // [OUT] Put member ref here.   
    }

    return memberRef;
}


mdToken EMITTER::GetMethodInstantiation(mdToken parent, TypeArray *typeArgsMeth)
{
    mdToken tok;
    PCOR_SIGNATURE sig;
    int len = 0;

    sig = BeginSignature();

    sig = EmitSignatureByte(sig, IMAGE_CEE_CS_CALLCONV_INSTANTIATION);
    sig = EmitSignatureUInt(sig, typeArgsMeth->size);
    for (int i = 0; i < typeArgsMeth->size; i++) {
        sig = EmitSignatureType(sig, typeArgsMeth->Item(i));
    }

    sig = EndSignature(sig, &len);

    ASSERT(parent);
    CheckHR(metaemit->DefineMethodSpec(parent, sig, len, &tok));

    return tok;
}


/*
 * Get a member ref for a member variable for use in emitting code or metadata.
 */
mdToken EMITTER::GetMembVarRefGivenParent(PMEMBVARSYM sym, mdToken parent)
{
    PINFILESYM inputfile;
    mdMemberRef memberRef;
    PCOR_SIGNATURE sig;
    int cbSig;

    // See if the class came from metadata or source code.
    inputfile = sym->getInputFile();
    if (inputfile->isSource) {
        ASSERT(sym->getClass()->typeVarsAll->size
            || inputfile->getOutputFile() != compiler()->curFile->GetOutFile());  
                    // If it's in our file, a def token should already have been assigned,
                    // unless it's in a generic class

        // Symbol defined by source code. Define a member ref by name & signature.
        sig = SignatureOfMembVar(sym, & cbSig);

            CheckHR(metaemit->DefineMemberRef(
                    parent,                         // ClassRef or ClassDef importing a member.
                    sym->name->text,                // member's name
                    sig, cbSig,                     // point to a blob value of COM+ signature
                    &memberRef));
    }
    else {

        const void *pHash = NULL;
        DWORD cbHash = 0;

        if (inputfile->GetAssemblyID() != kaidThisAssembly)
            CheckHR(compiler()->linker->GetAssemblyRefHash(inputfile->mdImpFile, &pHash, &cbHash));
        CheckHR(metaemit->DefineImportMember(             
            inputfile->assemimport,             // [IN] Assembly containing the Member. 
            pHash, cbHash,                      // [IN] Assembly hash value 
            sym->GetMetaImport(compiler()),     // [IN] Import scope, with member.  
            sym->tokenImport,                   // [IN] Member in import scope.   
            inputfile->assemimport ? metaassememit : NULL, // [IN] Assembly into which the Member is imported. (NULL if member isn't being imported from an assembly).
            parent,                             // [IN] Classref or classdef in emit scope.    
            &memberRef));                       // [OUT] Put member ref here.   
    }
    return memberRef;
}

/*
 * Get a member ref for a member variable for use in emitting code or metadata.
 */
mdToken EMITTER::GetMembVarRef(PMEMBVARSYM sym)
{
    mdToken parent;

    if (! sym->tokenEmit) {
        // Create a memberRef token for this symbol.
        ASSERT(! sym->getBogus());

        // First we need the containing class of the method being referenced.
        parent = GetAggRef(sym->getClass(), true);    // The runtime doesn't handle typedef in this case.

        sym->tokenEmit = GetMembVarRefGivenParent(sym, parent);
        RecordEmitToken(& sym->tokenEmit);
    }

    return sym->tokenEmit;
}

/*
 * Get a member ref for a member variable for use in emitting code or metadata.
 */
mdToken EMITTER::GetMembVarRef(MEMBVARSYM * sym, AGGTYPESYM * aggType)
{
    ASSERT(aggType && aggType->getAggregate() == sym->getClass());
    ASSERT(aggType->typeArgsAll->size > 0);

    mdToken * ptok = pmap->GetTokPtr(sym, aggType->typeArgsAll, NULL);

    if (!*ptok) {
        // Create a memberRef token for this symbol.
        ASSERT(!sym->getBogus());
        ASSERT(!aggType->getBogus());

        // First we need the containing class of the method being referenced.
        mdToken parent = GetTypeSpec(aggType);

        *ptok = GetMembVarRefGivenParent(sym, parent);
    }

    return *ptok;
}

/*
 * Metadata tokens are specific to a particular metadata output file.
 * Once we finish emitting an output file, all the metadata tokens
 * for that output file must be erased. To do that, we record each
 * place that we put a metadata token for the output file into a sym
 * and erase them all at the end. A linked list of 1000 token addresses
 * at a time is used to store these.
 */

/*
 * Remember that a metadata emission token is stored at this address.
 */
void EMITTER::RecordEmitToken(mdToken * tokenAddr)
{
    if (iTokrefCur >= CTOKREF)
    {
        // We need to allocate a new block of addresses.
        TOKREFS * tokrefNew;

        tokrefNew = (TOKREFS *) tokrefHeap.Alloc(sizeof(TOKREFS));
        tokrefNew->next = tokrefList;
        tokrefList = tokrefNew;
        iTokrefCur = 0;
    }

    // Simple case, just remember the address in the current block.
    tokrefList->tokenAddrs[iTokrefCur++] = tokenAddr;
    return;
}

/*
 * Go through all the remembers token addresses and erase them.
 */
void EMITTER::EraseEmitTokens()
{
    TOKREFS * tokref;

    // Go through all the token addresses and free them
    for (tokref = tokrefList; tokref != NULL; tokref = tokref->next)
    {
        // All the blocks are full except the first.
        int cAddr = (tokref == tokrefList) ? iTokrefCur : CTOKREF;

        for (int i = 0; i < cAddr; ++i)
            * tokref->tokenAddrs[i] = 0;  // Erase each token.
    }

    pmap = NULL;

    // Free the list of token addresses.
    tokrefHeap.Free( &mark);
    tokrefList = NULL;
    iTokrefCur = CTOKREF;   // Signal that we must allocate a new block right away.
    cgttHead = NULL;
}


/*
 *
 * If this is a local type ref return nil
 * If it is in the same assembly return a ModuleRef
 * If it is in another assembly return an AssemblyRef
 *
 */
mdToken EMITTER::GetScopeForTypeRef(AGGSYM *sym)
{
    INFILESYM *in = sym->DeclFirst()->getInputFile();
    if (compiler()->curFile->GetOutFile() == in->getOutputFile())
        return TokenFromRid(1, mdtModule);

    if (in->isAddedModule || sym->GetAssemblyID() != kaidThisAssembly) {
        // Either an AddModule or a Reference, either way
        // ALink can do all the dirty work
        if (IsNilToken(in->idLocalAssembly)) {
            CheckHR(compiler()->linker->GetResolutionScope( compiler()->assemID, compiler()->curFile->GetOutFile()->idFile,
                ((sym->GetAssemblyID() != kaidThisAssembly && in->isSource) ? in->getOutputFile()->idFile : in->mdImpFile),
                &in->idLocalAssembly));
            RecordEmitToken(&in->idLocalAssembly);
        }
        return in->idLocalAssembly; // This could be a module ref or assembly ref
    } else {
        // This is a module we are currently building
        OUTFILESYM *file = in->getOutputFile();
        if (IsNilToken(file->idModRef)) {
            if (!IsNilToken(file->idFile)) {
                // It's already be built,so let ALink do it's job
                CheckHR(compiler()->linker->GetResolutionScope( compiler()->assemID, compiler()->curFile->GetOutFile()->idFile,
                    file->idFile, &file->idModRef));
            } else {
                // We haven't emitted the file yet, so we need to construct our own module-ref
                // Make sure we use the same format as for when we emit the ModuleDef
                CheckHR(metaemit->DefineModuleRef( PEFile::GetModuleName(file, compiler())->text, &file->idModRef));
            }
            RecordEmitToken(&file->idModRef);
        }
        return file->idModRef;
    }

    ASSERT(0); // we shouldn't get here
    return mdTokenNil;
}


/*
 * Get a type ref for a type for use in emitting code or metadata.
 * Returns a typeDef, typeRef or typeSpec.  If noDefAllowed is
 * set then only a typeRef or typeSpec is returned (which could be inefficient).
 */
mdToken EMITTER::GetTypeRef(PTYPESYM sym, bool noDefAllowed)
{
    ASSERT(sym && !sym->isVOIDSYM());

    switch (sym->getKind()) {
    case SK_AGGTYPESYM:
        if (!sym->asAGGTYPESYM()->typeArgsAll->size)
            return GetAggRef(sym->getAggregate(), noDefAllowed);
        // Fall through.
    case SK_ARRAYSYM:
    case SK_PTRSYM:
    case SK_NUBSYM:
    case SK_TYVARSYM:
    case SK_ERRORSYM:
        // We use typespecs instead...
        return GetTypeSpec(sym);

    default:
        VSFAIL("Bad type in GetTypeRef");
        return mdTokenNil;
    }
}

mdToken EMITTER::GetAggRef(AGGSYM * agg, bool noDefAllowed)
{
    mdToken * ptok;

    ptok = &agg->tokenEmit;

    // Runtime doesn't like def's in a few places. 
    // If requested, force to create a type ref. 
    if (noDefAllowed && *ptok && TypeFromToken(*ptok) == mdtTypeDef)
        ptok = &agg->tokenEmitRef;

    if (!*ptok) {
        // Create a new typeref token.

        // We should never be getting a typeref to a type in the same
        // output file -- they should have typedefs already assigned.
        ASSERT(noDefAllowed || agg->getOutputFile() != compiler()->curFile->GetOutFile());

        WCHAR rgch[MAX_FULLNAME_SIZE];

        // Get namespace and type name.
        if (!MetaDataHelper::GetMetaDataName(agg, rgch, lengthof(rgch)) ||
            (wcslen(rgch) >= SAFE_FULLNAME_CHARS && UTF8LengthOfUnicode(rgch, (int)wcslen(rgch)) >= MAX_FULLNAME_SIZE)) {
            compiler()->ErrorRef(agg->DeclFirst()->getParseTree(), ERR_ClassNameTooLong, agg);
        }

        mdToken scope;

        if (agg->Parent()->isNSSYM()) {
            scope = GetScopeForTypeRef(agg);
        } else {
            scope = GetAggRef(agg->Parent()->asAGGSYM(), true);
        }

        ASSERT(!IsNilToken(scope)); // We should NEVER have to emit an unscoped TypeRef!

        CheckHR(metaemit->DefineTypeRefByName(scope, rgch, ptok));

        RecordEmitToken(ptok);
    }

    return *ptok;
}

/*
 * Get a type spec for a type for use in emitting code or metadata.
 * Used when the interpretation of a typeRef/typeDef for a value type
 * is ambiguous.  Is allowed to return a typeRef/typeDef for class types, because
 * these are always unambiguous.
 */
mdToken EMITTER::GetTypeSpec(TYPESYM * type)
{
    ASSERT(type);

    mdToken * slot;

    // Before we normalize, check the constraints.
    TYPESYM * typeNaked = type->GetNakedType(false);
    if (typeNaked->isNUBSYM()) {
        typeNaked = typeNaked->asNUBSYM()->GetAts();
        ASSERT(typeNaked);
    }
    if (typeNaked->isAGGTYPESYM()) {
        AGGTYPESYM * ats = typeNaked->asAGGTYPESYM();
        if (ats->typeArgsAll->size > 0 && !ats->fConstraintsChecked) {
            TypeBind::CheckConstraints(compiler(), NULL, ats, CheckConstraintsFlags::NoDupErrors);
            ASSERT(ats->fConstraintsChecked && !ats->fConstraintError);
        }
    }

    type = compiler()->getBSymmgr().SubstType(type, SubstTypeFlags::NormAll);

    switch (type->getKind()) {
    case SK_ARRAYSYM:
        slot = &(type->asARRAYSYM()->tokenEmit);
        break;
    case SK_TYVARSYM:
        ASSERT(type->parent == NULL); // These should be standard type variables only.
        slot = &(type->asTYVARSYM()->tokenEmit);
        break;
    case SK_PTRSYM:
        slot = &(type->asPTRSYM()->tokenEmit);
        break;
    case SK_NUBSYM:
        type = type->asNUBSYM()->GetAts();
        ASSERT(type);
        compiler()->EnsureState(type);
        // Fall through.
    case SK_AGGTYPESYM:
        ASSERT(type->asAGGTYPESYM()->typeArgsAll->size > 0); // Should be calling GetTypeRef if it's not generic!
        slot = &(type->asAGGTYPESYM()->tokenEmit);
        break;
    case SK_ERRORSYM:
        slot = &(type->asERRORSYM()->tokenEmitSpec);
        break;
    default:
        VSFAIL("Bad type in GetTypeSpec");
        return mdTokenNil;
    }

    if (*slot) {
        return *slot;
    }

    PCOR_SIGNATURE sig;
    int len = 0;
    
    sig = BeginSignature();
    sig = EmitSignatureType(sig, type);
    sig = EndSignature(sig, &len);

    CheckHR(metaemit->GetTokenFromTypeSpec(
        sig,          // Namespace name.
        len,           // type name
        slot));
    
    RecordEmitToken(slot);
    
    return *slot;
}

/*
 * For accessing arrays, the COM+ EE defines four "pseudo-methods" on arrays:
 * constructor, load, store, and load address. This function gets the
 * memberRef for one of these pseudo-methods.
 */
mdToken EMITTER::GetArrayMethodRef(ARRAYSYM *sym, ARRAYMETHOD methodId)
{
    ASSERT(methodId >= 0 && methodId <= ARRAYMETH_COUNT);

    ASSERT(sym->rank > 0);

    ARRAYSYM * methodHolder;

    bool useVarargs = false;
    methodHolder = sym;

    if (! methodHolder->tokenEmitPseudoMethods[methodId]) {
        BYTE flags = useVarargs ? IMAGE_CEE_CS_CALLCONV_VARARG | IMAGE_CEE_CS_CALLCONV_HASTHIS : IMAGE_CEE_CS_CALLCONV_DEFAULT | IMAGE_CEE_CS_CALLCONV_HASTHIS;

        mdToken typeRef = GetTypeRef(sym);
        const WCHAR * name;
        PCOR_SIGNATURE sig;
        int cbSig;
        int i;

        int rank = sym->rank;
        PTYPESYM elementType = sym->elementType();

        sig = BeginSignature();
        sig = EmitSignatureByte(sig, flags);

        // Get the name and signature for the particular pseudo-method.
        switch (methodId)
        {
        case ARRAYMETH_CTOR:

            ASSERT(rank != 0 && !useVarargs);

            name = L".ctor";

            sig = EmitSignatureUInt(sig, rank);
            sig = EmitSignatureByte(sig, ELEMENT_TYPE_VOID);

            // Args are the array sizes.
            for (i = 0; i < rank; ++i)
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_I4);

            break;

        case ARRAYMETH_LOAD:
            name = L"Get";
    
            sig = EmitSignatureUInt(sig, rank);
            sig = EmitSignatureType(sig, elementType);

            if (useVarargs) {
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_SENTINEL);
            }

            // args are the array indices
            for (i = 0; i < rank; ++i)
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_I4);

            break;


        case ARRAYMETH_GETAT:
            name = L"GetAt";

            sig = EmitSignatureUInt(sig, 1);
            sig = EmitSignatureType(sig, elementType);
            sig = EmitSignatureByte(sig, ELEMENT_TYPE_I4);

            break;

        case ARRAYMETH_LOADADDR:
            name = L"Address";

            sig = EmitSignatureUInt(sig, rank);
            sig = EmitSignatureByte(sig, ELEMENT_TYPE_BYREF);
            sig = EmitSignatureType(sig, elementType);
            if (useVarargs) {
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_SENTINEL);
            }

            // args are the array indices
            for (i = 0; i < rank; ++i)
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_I4);

            break;

        case ARRAYMETH_STORE:
            name = L"Set";

            sig = EmitSignatureUInt(sig, rank + 1);
            sig = EmitSignatureByte(sig, ELEMENT_TYPE_VOID);

            if (useVarargs) {
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_SENTINEL);
            }

            // args are the array indices, plus the array element to store.
            for (i = 0; i < rank; ++i)
                sig = EmitSignatureByte(sig, ELEMENT_TYPE_I4);

            sig = EmitSignatureType(sig, elementType);

            break;

        default:
            ASSERT(0);
            return 0;
        }

        sig = EndSignature(sig, &cbSig);

        // Define the member ref for the token.
        CheckHR(metaemit->DefineMemberRef(
            typeRef,                            // ClassRef or ClassDef importing a member.
            name,                               // member's name
            sig, cbSig,                         // point to a blob value of COM+ signature
            &(methodHolder->tokenEmitPseudoMethods[methodId])));

        // Remember we created this token.
        RecordEmitToken(&(methodHolder->tokenEmitPseudoMethods[methodId]));
    }

    return methodHolder->tokenEmitPseudoMethods[methodId];
}


/*
 * Emit a constant string to the output file, and get back the
 * token of the string. Indentical strings are folded together.
 */
mdString EMITTER::GetStringRef(const STRCONST * string)
{
    mdString rval;

    CheckHR(metaemit->DefineUserString(string->text, string->length, &rval));

    return rval;
}


/***************************************************************************************************
    Get a type ref token that represents the nameText and namespace of the error sym. This doesn't
    incorporate any type args or parent type.
***************************************************************************************************/
mdToken EMITTER::GetErrRef(ERRORSYM * err)
{
    ASSERT(err->parent && err->nameText && err->typeArgs);

    mdToken * ptok = &err->tokenEmit;

    if (*ptok)
        return *ptok;

    WCHAR szName[MAX_FULLNAME_SIZE];
    StringBldrFixed str(szName, lengthof(szName));
    MetaDataHelper help;

    // Get namespace and type name.
    if (err->parent->isNSAIDSYM()) {
        NSSYM * ns = err->parent->asNSAIDSYM()->GetNS();

        if (ns != compiler()->getBSymmgr().GetRootNS()) {
            help.GetMetaDataName(ns, str);
            str.Add(L'.');
        }
    }
    str.Add(err->nameText->text);

    if (str.Error() || 
        (str.Length() >= SAFE_FULLNAME_CHARS && UTF8LengthOfUnicode(str.Str(), str.Length()) >= MAX_FULLNAME_SIZE)) {
        compiler()->Error(NULL, ERR_ClassNameTooLong, err);
    }

    mdToken tokAssem = GetErrAssemRef();
    ASSERT(!IsNilToken(tokAssem));

    CheckHR(metaemit->DefineTypeRefByName(tokAssem, str.Str(), ptok));

    RecordEmitToken(ptok);

    return *ptok;
}


mdToken EMITTER::GetErrAssemRef()
{
    if (tokErrAssemRef)
        return tokErrAssemRef;

    ASSEMBLYMETADATA amd;
    memset(&amd, 0, sizeof(amd));

    CheckHR(metaassememit->DefineAssemblyRef(NULL, 0, compiler()->namemgr->GetPredefName(PN_ERROR_ASSEM)->text,
        &amd, NULL, 0, 0, &tokErrAssemRef));

    RecordEmitToken(&tokErrAssemRef);

    return tokErrAssemRef;
}

// Create some storage for tracking iterator locals
void EMITTER::BeginIterator(int cIteratorLocals)
{
    ASSERT(pIteratorLocalsInfo == NULL);
    if (cIteratorLocals == 0)
        return;

    pIteratorLocalsInfo = new (compiler()->getLocalSymHeap()->Alloc(CDIIteratorLocalsInfo::ComputeSize(cIteratorLocals))) CDIIteratorLocalsInfo(cIteratorLocals);
}

void EMITTER::ResetIterator()
{
    if (pIteratorLocalsInfo) {
        pIteratorLocalsInfo->ClearOffsets();
    }
}

void EMITTER::EndIterator()
{
    // Don't worry about free-ing the memory, that will get taken care of by the NRHEAP!
    pIteratorLocalsInfo = NULL;
}


/*
 * Translate an access level value into flags.
 */
DWORD EMITTER::FlagsFromAccess(ACCESS access)
{
    C_ASSERT((int)fdPublic == (int)mdPublic);
    C_ASSERT((int)fdPrivate == (int)mdPrivate);
    C_ASSERT((int)fdFamily == (int)mdFamily);
    C_ASSERT((int)fdAssembly == (int)mdAssem);
    C_ASSERT((int)fdFamORAssem == (int)mdFamORAssem);

    switch (access) {
    case ACC_PUBLIC:
        return fdPublic;
    case ACC_PROTECTED:
        return fdFamily;
    case ACC_PRIVATE:
        return fdPrivate;
    case ACC_INTERNAL:
        return fdAssembly;  
    case ACC_INTERNALPROTECTED:
        return fdFamORAssem;
    default:
        ASSERT(0);
        return 0;
    }
}


/*
 * Emit an aggregate type (struct, enum, class, interface) into
 * the metadata. This does not emit any information about members
 * of the aggregrate, but must be done before any aggregate members
 * are emitted.
 */
void EMITTER::EmitAggregateDef(PAGGSYM sym)
{
    DWORD flags;
    WCHAR typeNameText[MAX_FULLNAME_SIZE];

    // If this assert triggers, we're emitting the same aggregate twice into an output scope.
    ASSERT(sym->tokenEmit == 0 || TypeFromToken(sym->tokenEmit) == mdtTypeRef);
    ASSERT(sym->IsPrepared());

    // Get namespace and type name.
    if (!MetaDataHelper::GetMetaDataName(sym, typeNameText, lengthof(typeNameText)) ||
        ((wcslen(typeNameText) >= SAFE_FULLNAME_CHARS && UTF8LengthOfUnicode(typeNameText, (int)wcslen(typeNameText)) >= MAX_FULLNAME_SIZE)) ) {
        compiler()->ErrorRef(NULL, ERR_ClassNameTooLong, sym);
    }

    // Determine flags.
    flags = MetaDataHelper::GetAggregateFlags(sym);

    if (sym->Parent()->isNSSYM()) {
        // Create the aggregate definition for a top level type.
        CheckHR(metaemit->DefineTypeDef(
                typeNameText,                   // Full name of TypeDef
                flags,                          // CustomValue flags
                NULL,                           // extends this TypeDef or typeref - GENERICS: set later due to possible recursion
                NULL,                           // Implements interfaces - GENERICS: set later due to possible recursion
                & sym->tokenEmit));
    }
    else {
        // Create the aggregate definition for a nested type.
        ASSERT(sym->Parent()->asAGGSYM()->isTypeDefEmitted);
        mdToken tokenParent = GetAggRef(sym->Parent()->asAGGSYM());

        CheckHR(metaemit->DefineNestedType(
                typeNameText,                   // Simple Name of TypeDef for nested classes.
                flags,                          // CustomValue flags
                NULL,                           // extends this TypeDef or typeref - GENERICS: set later due to possible recursion
                NULL,                           // Implements interfaces - GENERICS - set later due to possible recursion
                tokenParent,                    // Enclosing class
                & sym->tokenEmit));
    }

    // Don't emit ComType for TypeDefs in the manifest file
    if (compiler()->BuildAssembly() && (compiler()->FriendsDeclared() ? sym->hasExternalOrFriendAccess() : sym->hasExternalAccess())) {
        ASSERT(sym->isSource);
        HRESULT hr;
        if (sym->Parent()->isNSSYM())
            hr = compiler()->linker->ExportType(compiler()->assemID, sym->getOutputFile()->idFile,
                sym->tokenEmit, typeNameText, flags, &sym->tokenComType);
        else
            hr = compiler()->linker->ExportNestedType(compiler()->assemID, sym->getOutputFile()->idFile,
                sym->tokenEmit, sym->Parent()->asAGGSYM()->tokenComType, typeNameText, flags, &sym->tokenComType);
        CheckHR(FTL_InternalError, hr);
    }
    RecordEmitToken(& sym->tokenEmit);
}


void EMITTER::EmitTypeVars(SYM * sym)
{
    mdToken ** pprgtokVars;
    TypeArray * typeVars;
    mdToken tokPar;

    switch (sym->getKind()) {
    default:
        ASSERT(!"Bad SK in EmitTypeVars");
        return;
    case SK_METHSYM:
        typeVars = sym->asMETHSYM()->typeVars;
        if (typeVars->size == 0)
            return;
        pprgtokVars = &sym->asMETHSYM()->toksEmitTypeVars;
        tokPar = sym->asMETHSYM()->tokenEmit;
        break;
    case SK_AGGSYM:
        typeVars = sym->asAGGSYM()->typeVarsAll;
        if (typeVars->size == 0)
            return;
        pprgtokVars = &sym->asAGGSYM()->toksEmitTypeVars;
        tokPar = sym->asAGGSYM()->tokenEmit;
        break;
    }

    ASSERT(typeVars->size > 0);
    ASSERT(*pprgtokVars == NULL);

    mdToken rgtokBnd[8];
    int ctokMax = lengthof(rgtokBnd);
    mdToken * prgtokBnd = rgtokBnd;

    *pprgtokVars = (mdToken *)compiler()->getGlobalSymAlloc().Alloc(SizeMul(typeVars->size, sizeof(mdToken)));

    for (int ivar = 0; ivar < typeVars->size; ivar++) {
        TYVARSYM * var = typeVars->ItemAsTYVARSYM(ivar);
        ASSERT(var->FResolved());
        TypeArray * bnds = var->GetBnds();
        int cbnd = bnds->size;
        ASSERT(cbnd >= 0);

        if (cbnd >= ctokMax) {
            ctokMax += ctokMax;
            if (cbnd >= ctokMax)
                ctokMax = cbnd + 1;
            prgtokBnd = STACK_ALLOC_ZERO(mdToken, ctokMax);
        }
        ASSERT(cbnd < ctokMax);

        for (int ibnd = 0; ibnd < cbnd; ibnd++) {
            prgtokBnd[ibnd] = GetTypeRef(bnds->Item(ibnd));
        }
        ASSERT(cbnd < ctokMax);
        StoreAtIndex(prgtokBnd, cbnd, ctokMax, 0); // NULL terminate.

        uint flags = 0;

        if (var->FNewCon())
            flags |= gpDefaultConstructorConstraint;
        if (var->FRefCon())
            flags |= gpReferenceTypeConstraint;
        if (var->FValCon())
            flags |= gpNotNullableValueTypeConstraint | gpDefaultConstructorConstraint;

        CheckHR(metaemit->DefineGenericParam(
            tokPar, var->indexTotal, flags, var->name->text, 0, prgtokBnd,
            (*pprgtokVars) + ivar));
    }

    ASSERT(sizeof(*pprgtokVars) % sizeof(mdToken) == 0);
    mdToken * ptok = (mdToken *)pprgtokVars;
    for (size_t i = 0; i < sizeof(*pprgtokVars) / sizeof(mdToken); i++) {
        RecordEmitToken(ptok + i);
    }
}


void EMITTER::EmitAggregateBases(PAGGSYM sym)
{
    mdToken tokenBaseClass;
    mdToken * tokenInterfaces;
    int cInterfaces;

    // If any type variables, emit them.
    if (sym->typeVarsAll->size)
        EmitTypeVars(sym);

    // Determine base class.
    tokenBaseClass = mdTypeRefNil;
    if (sym->baseClass) {
        tokenBaseClass = GetTypeRef(sym->baseClass);
    }

    // Determine base interfaces.

    // First, count the number of interfaces.
    cInterfaces = sym->ifacesAll->size;

    // If any interfaces, allocate array and fill it in.
    if (cInterfaces) {
        tokenInterfaces = STACK_ALLOC(mdToken, cInterfaces + 1);
        for (int i = 0; i < cInterfaces; i++) {
            tokenInterfaces[i] = GetTypeRef(sym->ifacesAll->Item(i));
        }
        tokenInterfaces[cInterfaces] = mdTypeRefNil;
    }
    else {
        // No interfaces.
        tokenInterfaces = NULL;
    }

    CheckHR(metaemit->SetTypeDefProps(
            sym->tokenEmit,                 // TypeDef
            (DWORD)-1,                      // do not reset flags, it will overwrite any pseudoCAs in an incremental build
            tokenBaseClass,                 // base class
            tokenInterfaces));              // interfaces
}


/*
 * Emit any "special fields" associated with an aggregate. This can't be done
 * in EmitAggregateDef or EmiAggregateInfo due to the special order rules
 * for metadata emitting.
 */
void EMITTER::EmitAggregateSpecialFields(PAGGSYM sym)
{
    if (sym->IsEnum()) {
        // The underlying type of an enum is represented as a
        // non-static field of that type. Its name is "value__".

        PCOR_SIGNATURE sig;
        int cbSig;
        mdToken tokenTemp;

        // Get the signature from the field from enum enum underlying type.
        sig = BeginSignature();
        sig = EmitSignatureByte(sig, IMAGE_CEE_CS_CALLCONV_FIELD);
        sig = EmitSignatureType(sig, sym->underlyingType);
        sig = EndSignature(sig, &cbSig);

        // Create the field definition in the metadata.
        CheckHR(metaemit->DefineField(
                    sym->tokenEmit,             // Parent TypeDef
                    compiler()->namemgr->GetPredefName(PN_ENUMVALUE)->text, // Name of member
                    fdPublic | fdSpecialName,   // Member attributes
                    sig, cbSig,                 // COM+ signature
                    ELEMENT_TYPE_VOID,          // const type
                    NULL, 0,                    // value of constant
                    & tokenTemp));
    }
}

/*
 * Give a member variable that has a constant value, the COM+ representation
 * of that constant value is returned as an ELEMENT_TYPE, pointer, and
 * size (in characters, only for strings). tempBuff is an 8-byte temp buffer that is available if desired
 * to store the value.
 */
BYTE EMITTER::GetConstValue(PMEMBVARSYM sym, LPVOID tempBuff, LPVOID * ppValue, size_t * pcch)
{
    BYTE elementType;
    
    if (sym->type->isAGGTYPESYM()) {
        // Enums use constant types of their element type.
        PAGGTYPESYM type = sym->type->underlyingType()->asAGGTYPESYM();
        elementType = compiler()->getBSymmgr().GetElementType(type);
    } else if (sym->type->isARRAYSYM()) {
        elementType = ELEMENT_TYPE_CLASS;
    } else {
        ASSERT(!"Unknown constant type");
        elementType = ELEMENT_TYPE_VOID;
    }

    *pcch = 0;

    // This code needs to support little endian and big endian
    // The metaemit api's will store in the appropriate format
    switch (elementType) {
    case ELEMENT_TYPE_STRING:
        if (! sym->constVal.strVal) {
            // The "null" string (not the empty string).
            elementType = ELEMENT_TYPE_CLASS;
            *ppValue = NULL;
        }
        else {
            // Return length and text of the string.
            *ppValue = sym->constVal.strVal->text;
            *pcch = sym->constVal.strVal->length;
            break;
        }
        break;

    case ELEMENT_TYPE_BOOLEAN:
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:        
        *(__int8 *)tempBuff = (__int8)sym->constVal.iVal;
        *ppValue = tempBuff;
        break;
    
    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:            
        *(__int16 *)tempBuff = (__int16)sym->constVal.iVal;
        *ppValue = tempBuff;
        break;

    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
        C_ASSERT(sizeof(sym->constVal.iVal) == 4);
        *ppValue = &sym->constVal.iVal;
        break;

    case ELEMENT_TYPE_I8: 
    case ELEMENT_TYPE_U8:
        *ppValue = sym->constVal.longVal;
        break;

    case ELEMENT_TYPE_R4:        
        *(float *)tempBuff = (float) *sym->constVal.doubleVal;
        *ppValue = tempBuff;
        break;

    case ELEMENT_TYPE_R8:
        *ppValue = sym->constVal.doubleVal;
        break;

    case ELEMENT_TYPE_OBJECT:
        elementType = ELEMENT_TYPE_CLASS;
        // FALL THRU.
    case ELEMENT_TYPE_CLASS:
        // Only valid constant of class type is NULL.
        *ppValue = NULL; 
        break;

    default:
        ASSERT(0);      // This shouldn't happen.
        elementType = ELEMENT_TYPE_VOID;
        *ppValue = NULL;
        break;
    }

    return elementType;
}

/*
 * given a constval structure associated with a given type, return a variant that has the same stuff.
 * returns true on success, false on failure. v is assume uninitialized.
 * This does not need to be in little endian but should be in native endian format
 */
bool EMITTER::VariantFromConstVal(AGGTYPESYM * type, CONSTVAL * cv, VARIANT * v)
{
    if (type->isEnumType())
        type = type->underlyingEnumType();

    switch (compiler()->getBSymmgr().GetElementType(type)) {
    // Note: The next few cases depend on a little endian representation.
    case ELEMENT_TYPE_U1:
        V_VT(v) = VT_UI1;
        V_UI1(v) = (unsigned char)cv->iVal;
        return true;

    case ELEMENT_TYPE_I1:
        V_VT(v) = VT_I1;
        V_I1(v) = (signed char)cv->iVal;
        return true;

    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
        V_VT(v) = VT_UI2;
        V_UI2(v) = (unsigned short)cv->iVal;
        return true;

    case ELEMENT_TYPE_I2:
        V_VT(v) = VT_I2;
        V_I2(v) = (signed short)cv->iVal;
        return true;

    case ELEMENT_TYPE_I4:
        V_VT(v) = VT_I4;
        V_I4(v) = cv->iVal;
        return true;

    case ELEMENT_TYPE_U4:
        V_VT(v) = VT_UI4;
        V_UI4(v) = cv->uiVal;
        return true;

    case ELEMENT_TYPE_I8:
        V_VT(v) = VT_I8;
        V_I8(v) = * (cv->longVal);
        return true;

    case ELEMENT_TYPE_U8:
        V_VT(v) = VT_UI8;
        V_UI8(v) = *(cv->ulongVal);
        return true;

    case ELEMENT_TYPE_BOOLEAN:
        V_VT(v) = VT_BOOL;
        V_BOOL(v) = (cv->iVal) ? VARIANT_TRUE : VARIANT_FALSE;
        return true;

    case ELEMENT_TYPE_R4:
        V_VT(v) = VT_R4;
        V_R4(v) = (float) *cv->doubleVal;
        return true;

    case ELEMENT_TYPE_R8:
        V_VT(v) = VT_R8;
        V_R8(v) = *cv->doubleVal; 
        return true;

    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_OBJECT:
        V_VT(v) = VT_I4;
        V_I4(v) = 0;
        return true;

    case ELEMENT_TYPE_VALUETYPE:
        if (type->isPredefType(PT_DECIMAL)) {
            V_DECIMAL(v) = *cv->decVal;
            V_VT(v) = VT_DECIMAL;
            return true;
        }
        break;

    case ELEMENT_TYPE_STRING:
        if (! cv->strVal) {
            V_VT(v) = VT_I4;
            V_I4(v) = 0;
            return true;
        }
        else {
            // Return length and text of the string.
            V_VT(v) = VT_BSTR;
            V_BSTR(v) = SysAllocStringLen(cv->strVal->text, cv->strVal->length);
            return true;
        }
        break;

    }

    return false;
}


/*
 * getthe flags for a field
 */
DWORD EMITTER::GetMembVarFlags(MEMBVARSYM *sym)
{
    DWORD flags;
    // Determine the flags.
    flags = FlagsFromAccess(sym->GetAccess());
    if (sym->isStatic)
        flags |= fdStatic;
    if (sym->isConst) {
        if (sym->type->isPredefType(PT_DECIMAL)) {
            // COM+ doesn;t support decimal constants
            // they are initialized with field initializers in the static constructor
            flags |= (fdStatic | fdInitOnly);
        } else {
            flags |= (fdStatic | fdLiteral);
        }
    }
    else if (sym->isReadOnly)
        flags |= fdInitOnly;

    return flags;
}

/*
 * Emitted field def into the metadata. The parent aggregate must
 * already have been emitted into the current metadata output file.
 */
void EMITTER::EmitMembVarDef(PMEMBVARSYM sym)
{
    mdTypeDef parentToken;      // Token of the parent aggregrate.
    PCCOR_SIGNATURE sig;
    int cbSig;
    BYTE tempBuff[8];
    DWORD constType = ELEMENT_TYPE_VOID;
    void * constValue = NULL;
    size_t cchConstValue = 0;

    // Get typedef token for the containing class/enum/struct. This must
    // be present because the containing class must have been emitted already.
    parentToken = sym->getClass()->tokenEmit;
    ASSERT(TypeFromToken(parentToken) == mdtTypeDef && parentToken != mdTypeDefNil);

    // Determine the flags.
    if (sym->isConst && !sym->fixedAgg) {
        if (! sym->type->isPredefType(PT_DECIMAL)) {

            // Get the value of the constant.
            constType = GetConstValue(sym, tempBuff, &constValue, &cchConstValue);
        }
    }

    int len = (int) wcslen(sym->name->text);
    if (len >= SAFE_FULLNAME_CHARS && UTF8LengthOfUnicode(sym->name->text, len) >= MAX_FULLNAME_SIZE) {
        compiler()->ErrorRef(NULL, ERR_IdentifierTooLong, sym);
    }

    // Get the signature from the field from its type.
    sig = SignatureOfMembVar(sym, & cbSig);

    // Create the field definition in the metadata.
    CheckHR(metaemit->DefineField(
                parentToken,            // Parent TypeDef
                sym->name->text,        // Name of member
                GetMembVarFlags(sym),   // Member attributes
                sig, cbSig,             // COM+ signature
                constType,              // const type Flag
                constValue, (ULONG)cchConstValue,  // value of constant
                & sym->tokenEmit));

    RecordEmitToken(& sym->tokenEmit);
}

/*
 * Allocate the metadata token for a parameter
 */
void EMITTER::DefineParam(mdToken tokenMethProp, int index, mdToken *paramToken)
{
    ASSERT(*paramToken == mdTokenNil);

    CheckHR(metaemit->DefineParam(
            tokenMethProp,                      // Owning method/property
            index,                              // Which param
            NULL,                               // Param name
            0,                                  // param flags
            0,                                  // C++ Type Flag
            NULL, 0,                            // Default value
            paramToken));                       // [OUT] Put param token here
}

/*
 * Set or Reset the parameter properties for a method.
 * The parameter properties is only the parameter name.
 * The parameter mode (in, out, in/out) are set by PCAs
 * index is the parameter index (0 == return value, 1 is first parameter)
 * NOTE: all parameters must be named
 */
void EMITTER::EmitParamProp(mdToken tokenMethProp, int index, TYPESYM *type, PARAMINFO *paramInfo, bool hasDefaultValue, DWORD etDefaultValue, BlobBldr &blob)
{
    ASSERT((index == 0 && !paramInfo->Name()) || (index != 0 && paramInfo->Name()));

    PNAME name;

    // Get parameter name.
    name = paramInfo->Name();

    // NOTE: param flags are done through Pseudo-CAs which come after ParamProps
    ASSERT(!paramInfo->isIn && !paramInfo->isOut);

    // Param mode.
    if (compiler()->options.m_fNOCODEGEN)
        return;

    // Set param properties if not the default.
    if (name || hasDefaultValue)
    {
        int len = (int) wcslen(name->text);
        if (len >= SAFE_FULLNAME_CHARS && UTF8LengthOfUnicode(name->text, len) >= MAX_FULLNAME_SIZE) {
            compiler()->ErrorRef(NULL, ERR_IdentifierTooLong, name);
        }

        void *pValue;
        ULONG cchValue;
        if (hasDefaultValue && blob.Length() > 0) {
            pValue = blob.Buffer();
            cchValue = blob.Length();
            if (etDefaultValue == ELEMENT_TYPE_STRING)
                cchValue /= sizeof(WCHAR);
        } else {
            pValue = 0;
            cchValue = 0;
        }
        
        if (mdTokenNil == paramInfo->tokenEmit) {
            CheckHR(metaemit->DefineParam(
                    tokenMethProp,                      // Owning method/property
                    index,                              // Which param
                    name->text,                         // Param name
                    0,                                  // param flags
                    etDefaultValue,                     // C++ Type flags
                    pValue, cchValue,                   // Default value
                    &paramInfo->tokenEmit));            // [OUT] Put param token here
        } else {
            CheckHR(metaemit->SetParamProps(
                    paramInfo->tokenEmit,
                    name->text,                         // Param name
                    ULONG_MAX,                          // param flags - preserve the old value for pdOptionalValue
                    etDefaultValue,                     // C++ type flags
                    pValue, cchValue));                 // Default value
        }
    }
}

/*
 * Returns the flags for a method
 */
DWORD EMITTER::GetMethodFlags(METHSYM *sym)
{
    DWORD flags;

    // Determine the flags.
    flags = FlagsFromAccess(sym->GetAccess());

    if (!sym->isHideByName) {
        flags |= mdHideBySig;
    }
    if (sym->isStatic) {
        flags |= mdStatic;
    }
    if (sym->isCtor()) {
        flags |= mdSpecialName;
    }

    if (sym->isVirtual) {
        ASSERT(! sym->isCtor());
        flags |= mdVirtual;
    }
    else if (sym->isMetadataVirtual) {
        // Non-virtual in the language, but be virtual in the metadata. Also emit 
        // mdFinal so we read it in as non-virtual/sealed.
        flags |= mdVirtual | mdFinal;
    }

    if (sym->isVirtual || sym->isMetadataVirtual) {
        if (sym->isOverride && !sym->isNewSlot) {
            flags |= mdReuseSlot;
        } else {
            flags |= mdNewSlot;
        }
    }

    if (sym->isAbstract) {
        flags |= mdAbstract;
    }

    if (sym->isOperator || sym->isAnyAccessor()) {
        flags |= mdSpecialName;
    }
    ASSERT(!sym->swtSlot.Sym() || sym->swtSlot.Type() && sym->isMetadataVirtual);

    // Enforce C#'s notion of internal virtual
    // If the method is private or internal and virtual but not final
    // Set the new bit ot indicate that it can only be overriden
    // by classes that can normally access this member.
    if (((flags & mdMemberAccessMask) == mdPrivate ||
        (flags & mdMemberAccessMask) == mdFamANDAssem ||
        (flags & mdMemberAccessMask) == mdAssem) &&
        ((flags & (mdVirtual | mdFinal)) == mdVirtual))
        flags |= mdCheckAccessOnOverride;

    return flags;
}

mdToken EMITTER::GetGlobalFieldDef(METHSYM * sym, unsigned int count, unsigned int size, BYTE **pBuffer)
{
    mdToken token = GetGlobalFieldDef(sym, count, NULL, size);

    ULONG rva;
    *pBuffer = (BYTE*) compiler()->curFile->AllocateRVABlock(size, 8, &rva);

    CheckHR(metaemit->SetFieldRVA(token, rva));

    return token;
}

mdToken EMITTER::GetGlobalFieldDef(METHSYM * sym, unsigned int count, TYPESYM * type, unsigned int size)
{
    ASSERT(!compiler()->FEncBuild());

    WCHAR bufferW[255];
    ASSERT(type || size);

    if (mdTokenNil == globalTypeToken) {
        const WCHAR szGlobalFieldPrefix[] = L"<PrivateImplementationDetails>";
        const int cchGlobalFieldPrefix = lengthof(szGlobalFieldPrefix) - 1;
        GUID guidModule;
        CComPtr<IMetaDataImport> metaimport;

        CheckHR(metaemit->QueryInterface(IID_IMetaDataImport, (void**) &metaimport));
        CheckHR(metaimport->GetScopeProps( NULL, 0, NULL, &guidModule));
        CheckHR(StringCchCopyW(bufferW, lengthof(bufferW), szGlobalFieldPrefix));
        if (!StringFromGUID2( guidModule, bufferW + cchGlobalFieldPrefix, lengthof(bufferW) - cchGlobalFieldPrefix))
            CheckHR(STRSAFE_E_INSUFFICIENT_BUFFER);

        CheckHR(metaemit->DefineTypeDef(
                bufferW,               // Full name of TypeDef
                tdClass | tdNotPublic,          // CustomValue flags
                GetTypeRef(compiler()->GetReqPredefType(PT_OBJECT)),   // extends this TypeDef or typeref
                NULL,                           // Implements interfaces
                & globalTypeToken));
        CompilerGeneratedAttrBind::EmitAttribute(compiler(), globalTypeToken);
    }

    mdToken dummyToken = mdTokenNil;
    if (!type) {
        ASSERT(size);

        switch (size) {
        case 1:
            type = compiler()->GetReqPredefType(PT_BYTE);
            break;
        case 2:
            type = compiler()->GetReqPredefType(PT_SHORT);
            break;
        case 4:
            type = compiler()->GetReqPredefType(PT_INT);
            break;
        case 8:
            type = compiler()->GetReqPredefType(PT_LONG);
            break;
        default:
            {
                CompGenTypeToken ** pcgttSearch = &cgttHead;
                while (*pcgttSearch && (*pcgttSearch)->size < size)
                    pcgttSearch = &(*pcgttSearch)->next;

                if (*pcgttSearch && (*pcgttSearch)->size == size) {
                    dummyToken = (*pcgttSearch)->tkTypeDef;
                    break;
                }

                CompGenTypeToken * cgttNew = (CompGenTypeToken *) tokrefHeap.Alloc(sizeof(CompGenTypeToken));
                StringCchPrintfW(bufferW, lengthof(bufferW), L"__StaticArrayInitTypeSize=%u", size);
 
                CheckHR(metaemit->DefineNestedType(
                    bufferW,                        // Simple Name of TypeDef for nested classes.
                    tdExplicitLayout | tdNestedPrivate | tdSealed,     // CustomValue flags
                    GetTypeRef(compiler()->GetReqPredefType(PT_VALUE)),   // extends this TypeDef or typeref
                    NULL,                // Implements interfaces
                    globalTypeToken,
                    & dummyToken));
    
                CheckHR(metaemit->SetClassLayout(
                    dummyToken,
                    1,
                    NULL,
                    size));
                cgttNew->next = *pcgttSearch;
                cgttNew->size = size;
                cgttNew->tkTypeDef = dummyToken;
                *pcgttSearch = cgttNew;
                break;
            }
        }
    }

    StringCchPrintfW(bufferW, lengthof(bufferW), L"$$method%#x-%d", sym->tokenEmit, count);

    PCOR_SIGNATURE sig;
    int cbSig;

    mdToken tokenTemp;

    sig = BeginSignature();
    sig = EmitSignatureByte(sig, IMAGE_CEE_CS_CALLCONV_FIELD);
    if (dummyToken != mdTokenNil) {
        sig = EmitSignatureByte(sig, ELEMENT_TYPE_VALUETYPE);
        sig = EmitSignatureToken(sig, dummyToken);
    } else {
        sig = EmitSignatureType(sig, type);
    }
    sig = EndSignature(sig, &cbSig);
    
    CheckHR(metaemit->DefineField(
                globalTypeToken,             // Parent TypeDef
                bufferW, // Name of member
                fdAssembly | fdStatic | fdPrivateScope,  // Member attributes
                sig, cbSig,                 // COM+ signature
                ELEMENT_TYPE_VOID,          // const type
                NULL, 0,                    // value of constant
                & tokenTemp));

    return tokenTemp; 
}

/*
 * Emit the methoddef for a method into the metadata.
 */
void EMITTER::EmitMethodDef(METHSYM * sym)
{
    mdTypeDef parentToken;      // Token of the parent aggregrate.
    DWORD flags;
    PCCOR_SIGNATURE sig;
    int cbSig;
    const WCHAR * nameText;
    WCHAR nameBuffer[MAX_FULLNAME_SIZE];

    // Get typedef token for the containing class/enum/struct. This must
    // be present because the containing class must have been emitted already.
    parentToken = sym->getClass()->tokenEmit;
    ASSERT(TypeFromToken(parentToken) == mdtTypeDef && parentToken != mdTypeDefNil);

    // Set "nameText" to the output name.
    if (sym->name == NULL) {
        // Explicit method implementations don't have a name in the language. Synthesize 
        // a name -- the name has "." characters in it so it can't possibly collide.
        // force truncation using a character size limit that won't exceed our UTF8 bytes max
        MetaDataHelper::GetExplicitImplName(sym, nameBuffer, SAFE_FULLNAME_CHARS);
        nameText = nameBuffer;
    }
    else {
        nameText = sym->name->text;
        int len = (int) wcslen(nameText);
        if (len >= SAFE_FULLNAME_CHARS && UTF8LengthOfUnicode(nameText, len) >= MAX_FULLNAME_SIZE) {
            compiler()->ErrorRef(NULL, ERR_IdentifierTooLong, sym);
        }
    }

    // Determine the flags.
    flags = GetMethodFlags(sym);

    // Get signature of method.
    sig = SignatureOfMethodOrProp(sym, &cbSig);

    // Define the method.
    CheckHR(metaemit->DefineMethod(
                parentToken,        // Parent TypeDef
                nameText,           // name of member
                flags,              // Member attributes
                sig, cbSig,         // point to a blob value of COM+ signature
                0,                  // RVA of method code (will be set in EmitMethodInfo)
                0,                  // implementation flags (will be set in EmitMethodInfo)
                & sym->tokenEmit));


    // If any typarams, allocate array and fill it in.
    ASSERT(!sym->toksEmitTypeVars);
    if (sym->typeVars->size)
        EmitTypeVars(sym);

    RecordEmitToken(&sym->tokenEmit);

    EmitMethodImpl(sym);
}

/*
 * Emit additional method information into the metadata.
 *
 */
void EMITTER::EmitMethodInfo(METHSYM * sym, METHINFO * info)
{
    DWORD implFlags;

    // A method def must already have been assigned via EmitMethodDef.
    ASSERT(compiler()->options.m_fNOCODEGEN || (sym->tokenEmit && TypeFromToken(sym->tokenEmit) == mdtMethodDef));

    //
    // set impl flags
    //
    if (info->isMagicImpl) {
        if (sym->isSysNative) {
            // COM classic coclass constructor
            ASSERT(sym->isCtor() && !sym->isStatic);

            implFlags = miManaged | miRuntime | miInternalCall;
        } else {
            // Magic method with implementation supplied by run-time.
            // delegate construcotr and Invoke
            ASSERT(sym->getClass()->IsDelegate());

            implFlags = miManaged | miRuntime;
        }
    } else if (sym->isAbstract) {
        implFlags = 0;
    } else {
        implFlags = miManaged | miIL;       // Our code is always managed IL.
    }

    if (info->isSynchronized)
        implFlags |= miSynchronized;

    if (compiler()->options.m_fNOCODEGEN)
        return;

    // Set the impl flags.
    CheckHR(metaemit->SetMethodImplFlags(sym->tokenEmit, implFlags));
}

//
// rewrites the flags for a method
//
void EMITTER::ResetMethodFlags(METHSYM *sym)
{
    ASSERT(sym->tokenEmit != 0);

    CheckHR(metaemit->SetMethodProps(sym->tokenEmit, GetMethodFlags(sym), UINT_MAX, UINT_MAX));
}


void EMITTER::SetEntryPoint(METHSYM *sym)
{
    ASSERT(sym->name);

    // Normal method
    POUTFILESYM outfile = sym->getInputFile()->getOutputFile();

    // Is this an entry point for the program? We only allow one.
    if (! outfile->isDll) {
        if (outfile->entrySym != NULL) {
            if (!outfile->multiEntryReported) {
                // If multiple entry points, we want to report all the duplicate entry points, including
                // the first one we found.
                compiler()->ErrorRef(NULL, ERR_MultipleEntryPoints, outfile->entrySym, outfile->name->text);
                outfile->multiEntryReported = true;
            }

            compiler()->ErrorRef(NULL, ERR_MultipleEntryPoints, sym, outfile->name->text);
        }

        outfile->entrySym = sym;

        if (outfile->isUnnamed()) {
            compiler()->getCSymmgr().SetOutFileName(sym->getInputFile());
        }
    }
}


// If an output file has a "/main" option
// find the class specified and then find the Main method
// report errors as needed
void EMITTER::FindEntryPoint(OUTFILESYM *outfile)
{
    if (outfile->entryClassName == NULL)
        return;

    WCHAR * pszName = outfile->entryClassName;
    int cchName = (int)wcslen(pszName);
    WCHAR * pchCur = pszName;
    WCHAR * pchLim = pchCur + cchName;

    // Try in this assembly first.
    int aid = kaidThisAssembly;
    BAGSYM * bagFound;

    for (BAGSYM * bagCur = compiler()->getBSymmgr().GetRootNS(); ; pchCur++) {
        WCHAR * pchStart = pchCur;

        // Advance p to the end of the next segment.
        pchCur += wcscspn(pchCur, L"./");
        if (pchCur <= pchStart)
            goto LNotFound;

        NAME * name; name = compiler()->namemgr->LookupString(pchStart, (int)(pchCur - pchStart));
        if (!name)
            goto LNotFound;

LRetry:
        for (bagFound = compiler()->LookupInBagAid(name, bagCur, aid, MASK_NSSYM | MASK_AGGSYM)->asBAGSYM();
            bagFound;
            bagFound = compiler()->LookupNextInAid(bagFound, aid, MASK_NSSYM | MASK_AGGSYM)->asBAGSYM())
        {
            if (!bagFound->isAGGSYM() || bagFound->asAGGSYM()->typeVarsAll->size == 0)
                break;
        }
        if (!bagFound) {
            if (aid == kaidThisAssembly) {
                aid = kaidGlobal;
                goto LRetry;
            }
            ASSERT(aid == kaidGlobal);
            if (pchCur < pchLim)
                goto LNotFound;
        }
        if (pchCur >= pchLim) {
            SYM * symOther;

            if (bagFound) {
                if (bagFound->isAGGSYM() && (bagFound->asAGGSYM()->IsClass() || bagFound->asAGGSYM()->IsStruct()))
                    break;
                symOther = bagFound;
            }
            else {
                // Didn't find any AGGSYMs or NSSYMs. Look for anything.
                symOther = compiler()->LookupInBagAid(name, bagCur, aid, MASK_ALL);
            }

            if (symOther) {
                compiler()->ErrorRef(NULL, ERR_MainClassNotClass, symOther);
                return;
            }

LNotFound:
            compiler()->Error(NULL, ERR_MainClassNotFound, pszName);
            return;
        }

        bagCur = bagFound;
    }


    ASSERT(bagFound->isAGGSYM() && (bagFound->asAGGSYM()->IsClass() || bagFound->asAGGSYM()->IsStruct()));

    AGGSYM * agg = bagFound->asAGGSYM();
    OUTFILESYM *other;

    if ((other = agg->getOutputFile()) != outfile) {
        if (agg->isSource) {
            compiler()->ErrorRef(NULL, ERR_MainClassWrongFile, agg);
        } else
            if (outfile->isUnnamed()) {
                compiler()->getCSymmgr().SetOutFileName(outfile->firstInfile());
            }
            compiler()->Error(NULL, ERR_MainClassIsImport, outfile->entryClassName, outfile->name->text);
        return;
    }

    FindEntryPointInClass(agg);
}


void EMITTER::FindEntryPointInClass(AGGSYM *parent)
{
    OUTFILESYM *outfile = parent->getOutputFile();

    // We've found the specified class, now let's look for a Main
    SYM *sym, *next;
    METHSYM *meth = NULL;

    sym = compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_MAIN), parent, MASK_METHSYM);

    // No sense going through this loop if the class is generic....
    if (sym && parent->typeVarsAll->size == 0) {
        meth = sym->asMETHSYM();
        while (meth != NULL) {
            // Must be public, static, void/int Main ()
            // with no args or String[] args
            // If you change this code also change the code in CLSDREC::defineMethod (it does basically the same thing)
            if (meth->isStatic &&
                !meth->isPropertyAccessor() &&
                meth->typeVars->size == 0 &&
                (meth->retType == compiler()->getBSymmgr().GetVoid() || meth->retType->isPredefType(PT_INT)) &&
                    (meth->params->size == 0 ||
                    (meth->params->size == 1 && meth->params->Item(0)->isARRAYSYM() &&
                        meth->params->Item(0)->asARRAYSYM()->elementType()->isPredefType(PT_STRING))))
            {
                SetEntryPoint(meth);
            }
            next = meth->nextSameName;
            meth = NULL;
            while (next != NULL) {
                if (next->isMETHSYM()) {
                    meth = next->asMETHSYM();
                    break;
                }
                next = next->nextSameName;
            }
        }
    }

    if (outfile->entrySym == NULL) {
        // We didn't find an entry point. Warn on all the Mains.
        compiler()->ErrorRef(NULL, ERR_NoMainInClass, parent);
        if (sym) {
            meth = sym->asMETHSYM();

            while (meth != NULL) {
                // Report anything that looks like Main () - even instance methods.
                if (!meth->isPropertyAccessor()) {
                    compiler()->ErrorRef(NULL,
                        (meth->typeVars->size > 0 || parent->typeVarsAll->size > 0) ? WRN_MainCantBeGeneric : WRN_InvalidMainSig, sym);
                }

                next = meth->nextSameName;
                meth = NULL;
                while (next != NULL) {
                    if (next->isMETHSYM()) {
                        meth = next->asMETHSYM();
                        break;
                    }
                    next = next->nextSameName;
                }
            }
        }
    }
}


void *EMITTER::EmitMethodRVA(PMETHSYM sym, ULONG cbCode, ULONG alignment)
{
    ULONG codeRVA;
    void * codeLocation;

    // Only call this if you are really emitting code
    ASSERT(cbCode > 0);

    // A method def must already have been assigned via EmitMethodDef.
    ASSERT(sym->tokenEmit &&
           (TypeFromToken(sym->tokenEmit) == mdtMethodDef));

    codeLocation = compiler()->curFile->AllocateRVABlock(cbCode, alignment, &codeRVA);

    // Set the code location.
    CheckHR(metaemit->SetRVA(sym->tokenEmit, codeRVA));

    compiler()->RecordEncMethRva(sym, codeRVA);

    // Return the block of code allocated for this method.
    return codeLocation;
}



/*
 * Emit a "namespace" directive into the debug info
 * for this namespace
 */
bool EMITTER::EmitDebugNamespace(NSAIDSYM * nsaid)
{
    NRHeapMarker heapMarker(compiler()->getLocalSymHeap());
    StringBldrNrHeap strBuilder(compiler()->getLocalSymHeap());

    if (!ComputeDebugNamespace(nsaid, strBuilder)) {  
        return false;
    }

    CheckHRDbg(debugemit->UsingNamespace(strBuilder.Str()));
    
    return true;
}


bool EMITTER::ComputeDebugNamespace(NSAIDSYM * nsa, StringBldr & bldr)
{
    MetaDataHelper helper;
    SYM * aidSym;

    int aid = nsa->aid;
    if (aid == kaidErrorAssem || aid == kaidUnresolved) {
        return false;
    } else if (aid == kaidThisAssembly) {
        goto Outfile;
    }
    aidSym = compiler()->getBSymmgr().GetSymForAid(aid);
    if (aidSym) {
        if (aidSym->isINFILESYM()) {
            bldr.Add(L"I");
            helper.GetFullName(nsa->GetNS(), bldr);
            bldr.Add(L" ");
            bldr.Add(compiler()->importer.GetAssemblyName(aidSym));
            return true;
        } else if (aidSym->isEXTERNALIASSYM()) {
            bldr.Add(L"E");
            helper.GetFullName(nsa->GetNS(), bldr);
            bldr.Add(L" ");
            bldr.Add(aidSym->asEXTERNALIASSYM()->name->text);
            return true;
        } else if (aidSym->isOUTFILESYM()) {
Outfile:
            bldr.Add(L"O");
            helper.GetFullName(nsa->GetNS(), bldr);
            return true;
        } else {
            ASSERT(!"Bad aid sym");
            // intentional fall through...
        }
    }
    bldr.Add(L"U");
    helper.GetFullName(nsa->GetNS(), bldr);
    return true;
}


bool EMITTER::EmitDebugAlias(ALIASSYM * as)
{

    NRHeapMarker heapMarker(compiler()->getLocalSymHeap());

    StringBldrNrHeap strBuilder(compiler()->getLocalSymHeap());

    if (as->fExtern) {
        strBuilder.Add(L"X");
        strBuilder.Add(as->name->text);
    } else {

        ASSERT(as->sym->isNSAIDSYM() || as->sym->isTYPESYM());

        strBuilder.Add(L"A");
        strBuilder.Add(as->name->text);
        strBuilder.Add(L" ");

        if (as->sym->isNSAIDSYM()) {
            if (!ComputeDebugNamespace(as->sym->asNSAIDSYM(), strBuilder)) {
                return false;
            }
        } else {
            strBuilder.Add(L"T");
            TypeNameSerializer tns(compiler());
            CComBSTR bstr;
            bstr.Attach(tns.GetAssemblyQualifiedTypeName(as->sym->asTYPESYM(), false));
            if (!bstr)
                return false;
            strBuilder.Add(bstr, bstr.Length());
        }
    }
    CheckHRDbg(debugemit->UsingNamespace(strBuilder.Str()));
    
    return true;
}

void EMITTER::EmitForwardedDebugInfo(METHSYM * methFrom, METHSYM * methTo)
{
    ASSERT(methFrom != methTo);

    CDIGlobalInfo gInfo(pIteratorLocalsInfo ? 2 : 1);
    CDIForwardingInfo fInfo(methTo->tokenEmit);

    DWORD fIteratorSize = pIteratorLocalsInfo ? pIteratorLocalsInfo->size : 0;

    BYTE * buffer, * bufferNow = buffer = STACK_ALLOC(BYTE, CDIGlobalInfo::Size() + fInfo.size + fIteratorSize);
    gInfo.CopyInto(&bufferNow);
    fInfo.CopyInto(&bufferNow);
    if (pIteratorLocalsInfo) {
        pIteratorLocalsInfo->CopyInto(&bufferNow);
    }

    ASSERT((size_t) (bufferNow - buffer) == CDIGlobalInfo::Size() + fInfo.size + fIteratorSize);
    CheckHRDbg(debugemit->SetSymAttribute(methFrom->tokenEmit, MSCUSTOMDEBUGINFO, (ULONG32) (bufferNow - buffer), buffer));
}

void EMITTER::EmitExternAliasNames(METHSYM * methsym)
{
    FOREACHCHILD(compiler()->GetExternAliasContainer(), extAlias)
        if (extAlias->isEXTERNALIASSYM()) {
            FOREACHSYMLIST(extAlias->asEXTERNALIASSYM()->infileList, infile, INFILESYM)
                EmitExternalAliasName(extAlias->name, infile);
            ENDFOREACHSYMLIST
        }
    ENDFOREACHCHILD
}

void EMITTER::EmitExternalAliasName(NAME * name, INFILESYM * infile)
{
    NRHeapMarker heapMarker(compiler()->getLocalSymHeap());

    StringBldrNrHeap strBuilder(compiler()->getLocalSymHeap());

    strBuilder.Add(L"Z");
    strBuilder.Add(name->text);
    strBuilder.Add(L" ");
    strBuilder.Add(compiler()->importer.GetAssemblyName(infile));
    
    CheckHRDbg(debugemit->UsingNamespace(strBuilder.Str()));
}

/*
 * Begin emitting debug information for a method.
 */
void EMITTER::EmitCustomDebugInfo(METHSYM * sym)
{
    DECLSYM * containingDecl = sym->containingDeclaration();

    // Add all the using statements
    NSSYM * myNS = NULL;
    NSDECLSYM * psym;
    unsigned short impliedCount = 1;
    NSDECLSYM * innerMostNamespace = NULL;

    psym = containingDecl->GetNsDecl();
    
    if (psym && psym->inputfile->isSource) {
        innerMostNamespace = psym;
        if (innerMostNamespace->methodWithEmittedUsings && !compiler()->FEncBuild()) {
            EmitForwardedDebugInfo(sym, psym->methodWithEmittedUsings);
            return;
        }
    }

    NSSYM * root = compiler()->getBSymmgr().GetRootNS();
    if (innerMostNamespace) {
        myNS = innerMostNamespace->NameSpace(); 
    }
    for (NSSYM * ns = myNS; ns != root && ns != NULL; ns = ns->Parent())
    {
        impliedCount++;
    }

    BYTE * usingInfoSpace = STACK_ALLOC_ZERO(BYTE, CDIUsingInfo::ComputeSize(impliedCount));
    CDIUsingInfo * usingInfo = new (usingInfoSpace) CDIUsingInfo(impliedCount);
    int cursor = 0;
    
    for (; psym != NULL; psym = containingDecl->asNSDECLSYM())
    {
        containingDecl = psym->DeclPar();

        // Emit all the using clauses in this ns declaration.
        if (psym->usingClausesResolved) 
        {
            FOREACHSYMLIST(psym->usingClauses, usingSym, SYM)
                bool emitted = false;
                if (usingSym->isALIASSYM()) {
                    emitted = EmitDebugAlias(usingSym->asALIASSYM());
                } else if (usingSym->asNSAIDSYM()) {
                    emitted = EmitDebugNamespace(usingSym->asNSAIDSYM());
                }
                if (emitted) {
                    usingInfo->usingCounts[cursor].countOfUsing++;
                }
            ENDFOREACHSYMLIST
        }
        if (containingDecl) {
            while (myNS && myNS != containingDecl->asNSDECLSYM()->NameSpace()) {
                cursor++;
                myNS = myNS->Parent();
            }
        }
    }

    CDIGlobalInfo ginfo(pIteratorLocalsInfo ? 2 : 1);
    CDIForwardToModuleInfo fInfo;
    CDIForwardToModuleInfo * pfInfo = NULL;

    if (sym->getInputFile()->getOutputFile()->methodWithEmittedModuleInfo && !compiler()->FEncBuild()) {
        fInfo.tokenOfModuleInfo = sym->getInputFile()->getOutputFile()->methodWithEmittedModuleInfo->tokenEmit;
        pfInfo = &fInfo;
        ginfo.count++;
    } else {
        EmitExternAliasNames(sym);
        sym->getInputFile()->getOutputFile()->methodWithEmittedModuleInfo = sym;
    }

    DWORD fInfoSize = pfInfo ? fInfo.size : 0;
    DWORD fIteratorSize = pIteratorLocalsInfo ? pIteratorLocalsInfo->size : 0;
    size_t bufferSize = CDIGlobalInfo::Size() + usingInfo->size + fInfoSize + fIteratorSize;

    BYTE * buffer, * bufferNow = buffer = (BYTE*)compiler()->getLocalSymHeap()->Alloc(bufferSize);
    ginfo.CopyInto(&bufferNow);
    usingInfo->CopyInto(&bufferNow);
    if (pfInfo) {
        pfInfo->CopyInto(&bufferNow);
    }
    if (pIteratorLocalsInfo) {
        pIteratorLocalsInfo->CopyInto(&bufferNow);
    }

    ASSERT((size_t)(bufferNow - buffer) == bufferSize);
    CheckHRDbg(debugemit->SetSymAttribute(sym->tokenEmit, MSCUSTOMDEBUGINFO, (ULONG32)bufferSize, buffer));
    
    if (innerMostNamespace && !innerMostNamespace->methodWithEmittedUsings) {
        innerMostNamespace->methodWithEmittedUsings = sym;
    }
        
}



void EMITTER::DumpIteratorLocals(METHSYM * meth)
{
#ifdef DEBUG
    ASSERT(pIteratorLocalsInfo && meth);
    AGGSYM * agg = meth->getClass();
    ASSERT(agg->isFabricated);

    printf("\t\tIterator Locals hoisted to Fields:\n");
    SYM * symChild = agg->firstChild;

    for (int i = 0; i < pIteratorLocalsInfo->cBuckets; i++) {
       while(!symChild->isMEMBVARSYM() || symChild->asMEMBVARSYM()->iIteratorLocal == -1)
            symChild = symChild->nextChild;
        ASSERT(symChild && symChild->asMEMBVARSYM()->iIteratorLocal == i);
        printf("\t\t\tIndex %d: ", i);
        compiler()->getBSymmgr().DumpType(symChild->asMEMBVARSYM()->type);
        printf(" %ls\n\t\t\t\t(0x%08x-0x%08x)\n", symChild->name->text,
            pIteratorLocalsInfo->rgBuckets[i].ilOffsetStart, pIteratorLocalsInfo->rgBuckets[i].ilOffsetEnd);
        symChild = symChild->nextChild;
    }

    while(symChild && (!symChild->isMEMBVARSYM() || symChild->asMEMBVARSYM()->iIteratorLocal == -1))
        symChild = symChild->nextChild;
    ASSERT(symChild == NULL);
#endif // DEBUG
}


void EMITTER::EmitForwardedIteratorDebugInfo(METHSYM * methFrom, METHSYM * methTo)
{
    if (debugemit != NULL) {
        WCHAR szIterClassNameText[MAX_FULLNAME_SIZE];
        // Get the arity in the name
        if (!MetaDataHelper::GetMetaDataName(methTo->getClass(), szIterClassNameText, lengthof(szIterClassNameText)))
            return;

        // Create the custom debug info structures
        DWORD size = (DWORD)CDIGlobalInfo::Size() + CDIForwardIteratorInfo::ComputeSize(szIterClassNameText);
        BYTE * buffer = STACK_ALLOC_ZERO(BYTE, size);
        new (buffer) CDIGlobalInfo(1);
        new (buffer + CDIGlobalInfo::Size()) CDIForwardIteratorInfo(szIterClassNameText);

        // Save them (yes we have to open the method to set its attributes)
        CheckHRDbg(debugemit->OpenMethod(methFrom->tokenEmit));
        CheckHRDbg(debugemit->SetSymAttribute(methFrom->tokenEmit, MSCUSTOMDEBUGINFO, size, buffer));
        CheckHRDbg(debugemit->CloseMethod());
    }
}

/*
 * Begin emitting debug information for a method.
 */
void EMITTER::EmitDebugMethodInfoStart(METHSYM * sym)
{
    CheckHRDbg(debugemit->OpenMethod(sym->tokenEmit));

    // The using statements MUST belong to a scope
    // So open the default scope
    EmitDebugScopeStart(0);
}



/*
 * Stop emitting debug information for a method.
 */
void EMITTER::EmitDebugMethodInfoStop(METHSYM * sym, int ilOffsetEnd)
{
    EmitCustomDebugInfo(sym);

    // Close the default scope
    EmitDebugScopeEnd(ilOffsetEnd);

    CheckHRDbg(debugemit->CloseMethod());
}

/*
 *  Emit a block of debug info (line/col , il offset pairs) associated
 *  with the given method
 */
void EMITTER::EmitDebugBlock(METHSYM * sym, int count, unsigned int * offsets, SourceExtent * extents)
{

    ASSERT(debugemit);

    if (count == 0)
        return;

    // This is the infile that parents the METHSYM.
    INFILESYM * infile = sym->getInputFile();
    
    // Loop through each filename, line, column set and remap them
    for (int i = 0; i < count; i++) {
        CSourceModuleBase * pModule = extents[i].infile == NULL ? NULL : extents[i].infile->pData->GetModule();
        NAME * Filename = NULL;
        ASSERT(extents[i].IsValid());

        if (extents[i].fNoDebugInfo) {
HIDE_LINE:
            // Don't emit sequential/redundant NO_DEBUG_LINE sequence points.
            // If the current and previous sequence point are NO_DEBUG_LINE,
            // ignore the current sequence point by making it invalid.
            // This makes the next loop think the sequence point has already
            // been processed.
            if (i > 0 && extents[i-1].fNoDebugInfo && extents[i].IsMergeAllowed() && extents[i-1].IsMergeAllowed())
                extents[i].SetHiddenInvalidSource();
            else
                extents[i].infile = infile; // Hidden lines are always in the current source
            continue;
        }

        ASSERT(pModule);
        if (pModule->hasMap()) {
            bool bHidden = false;
            pModule->MapLocation(&extents[i].begin, &Filename, &bHidden, NULL);
            if (bHidden) {
                extents[i].fNoDebugInfo = true;
                goto HIDE_LINE;
            }

            pModule->MapLocation(&extents[i].end, (NAME**)NULL, NULL, NULL);
            if (Filename != NULL) {
                ASSERT(infile->getOutputFile() == extents[i].infile->getOutputFile());
                INFILESYM* sym = compiler()->LookupGlobalSym( Filename, infile->getOutputFile(), MASK_INFILESYM)->asINFILESYM();
                if (!sym) {
                    // Non-source files 'created' via #line and used basically to store the documentWriter interface
                    sym = compiler()->LookupGlobalSym( Filename, infile->getOutputFile(), MASK_SYNTHINFILESYM)->asANYINFILESYM();
                }
                if (!sym) {
                    extents[i].infile = compiler()->getCSymmgr().CreateSynthSourceFile( Filename->text, infile->getOutputFile());
                } else {
                    extents[i].infile = sym;
                }
            }
        }

        // We need to check the lines (but not the hidden ones) for various PDB limitations
        CheckExtent( extents[i]);
    }

    // Arrays for each of the constiuent files
    NRMARK markPreArrays;
    uint * newOffsets;
    uint * newLines;  
    uint * newCols;   
    uint * newEndLines;  
    uint * newEndCols;   

    // Allocate the arrays
    size_t cb = SizeMul(count, sizeof(uint));
    compiler()->localSymAlloc.Mark( &markPreArrays);
    newOffsets = (uint *)compiler()->localSymAlloc.Alloc(cb);
    newLines = (uint *)compiler()->localSymAlloc.Alloc(cb);
    newCols = (uint *)compiler()->localSymAlloc.Alloc(cb);
    newEndLines = (uint *)compiler()->localSymAlloc.Alloc(cb);
    newEndCols = (uint *)compiler()->localSymAlloc.Alloc(cb);

    // Make multiple passes over the list of sequence points - one for each unique file
    // Each pass initializes the documentWriter (if needed) and collects all sequence points
    // for that file into the new* arrays and emits that set of sequence points
    // newCount is set to the next non-NULL infile in the list
    // processes entries are set to NULL
    // newCount is set to -1 when all entries have been processed
    for (int newCount = 0; newCount >= 0; ) {
        INFILESYM * infileCur = extents[newCount].infile;
        InitDocumentWriter(infileCur);

        int j = 0;
        int i = newCount;
        newCount = -1;
        for (; i < count; i++) {
            if (extents[i].infile == infileCur) {
                newOffsets[j] = offsets[i];
                if (extents[i].fNoDebugInfo) {
                    newEndLines[j] = newLines[j] = NO_DEBUG_LINE;
                    newCols[j] = newEndCols[j] = 0;
                }
                else {
                    newLines[j] = extents[i].begin.iLine + 1;  // make them 1 based
                    newEndLines[j] = extents[i].end.iLine + 1;
                    newCols[j] = extents[i].begin.iChar + 1;
                    newEndCols[j] = extents[i].end.iChar + 1;
                }

                j++;
                extents[i].SetInvalid();
            } else if (newCount == -1 && extents[i].IsValidSource()) {
                newCount = i;
            }
        }

        CheckHRDbg(debugemit->DefineSequencePoints(
            infileCur->documentWriter,
            j,
            newOffsets,
            newLines,
            newCols,
            newEndLines,
            newEndCols));
    }

    // cleanup
    compiler()->localSymAlloc.Free(&markPreArrays);
}

// Cheeck this extent for various PDB limitations
void EMITTER::CheckExtent(SourceExtent & extent)
{
    if (extent.fNoDebugInfo)
        return;

    // PDB limitation: 0x00FEEFEE is used to mean "don't step here"
    // PDB limitation: only 24-bits are stored for line info, warn if that is exceeded
    if (!extent.infile->fTooManyLinesReported && 
        (extent.begin.iLine >= NO_DEBUG_LINE || extent.end.iLine >= NO_DEBUG_LINE))
    {
        static const POSDATA start(NO_DEBUG_LINE, 0), end(NO_DEBUG_LINE, 1);
        compiler()->Error(ERRLOC( extent.infile->name->text, start, end), WRN_TooManyLinesForDebugger);
        extent.infile->fTooManyLinesReported = true;
    }

    ASSERT (extent.end.iLine >= extent.begin.iLine);

    //EDMAURER There used to be additional checks here that would test if extents
    //were below maximums imposed by the PDB format. One of these was incorrectly 
    //limiting data that would, in fact, fit within PDB constraints. If the compiler 
    //is not going to give warnings based on these limitations, then let downstream 
    //consumers of this data deal with their own limits. Or even better, give 
    //indication that the submitted data is outside of limits.
}

void EMITTER::InitDocumentWriter(INFILESYM* infile)
{
    if (infile->documentWriter != NULL)
        return;

    // No ISymDocumentWriter interface yet obtained for this input file.
    PCWSTR name;

    if (infile->pData) {
        name = infile->pData->GetModule()->GetFileName();
    } else {
        name = infile->name->text;
    }

    CheckHRDbg(debugemit->DefineDocument(
        (WCHAR*)name,
        (GUID *) & CorSym_LanguageType_CSharp,
        (GUID *) & CorSym_LanguageVendor_Microsoft,
        (GUID *) & CorSym_DocumentType_Text,
        &(infile->documentWriter)));

}

/*
 * Emit a signature into a PE and get back a token representing it.
 * used exclusively for debugemit2 interface
 */
mdSignature EMITTER::GetDebugVarSig(PTYPESYM type)
{
    // Normalize the type.
    type = compiler()->getBSymmgr().SubstType(type, SubstTypeFlags::NormAll);

    TYPESYM * types[] = { type, compiler()->getBSymmgr().GetVoid() };

    // Look up in hash table to see if we have seen this signature before.
    // If we have, just return the previously obtained token.
    TypeArray * params = compiler()->getBSymmgr().AllocParams(lengthof(types), types);
    mdToken * pToken = &params->tok;

    if (! *pToken) {
        PCOR_SIGNATURE sig;
        int cbSig;

        // Create signature for the types.
        sig = BeginSignature();
        // The debugger expects to see a FIELD calling convention!!!!
        sig = EmitSignatureByte(sig, IMAGE_CEE_CS_CALLCONV_FIELD);
        sig = EmitSignatureType(sig, type);
        sig = EndSignature(sig, &cbSig);

        // Define a signature token.
        CheckHR(metaemit->GetTokenFromSig(sig, cbSig, pToken));

        RecordEmitToken(pToken);
    }

    return *pToken;
}


/*
 * Emit a local variable into a given debug scope
 */
void EMITTER::EmitDebugTemporary(TYPESYM * type, PCWSTR name, mdToken tkLocalVarSig, unsigned slot)
{
    if (debugemit2 != NULL) {
        CheckHRDbg(debugemit2->DefineLocalVariable2(
            (WCHAR *) name,
            VAR_IS_COMP_GEN,
            tkLocalVarSig,
            ADDR_IL_OFFSET,
            slot, 0, 0,
            0, 0));
    } else {
        PCOR_SIGNATURE sig;
        int cbSig;

        // Create signature for the type
        sig = BeginSignature();
        sig = EmitSignatureType(sig, type);
        sig = EndSignature(sig, &cbSig);

        CheckHRDbg(debugemit->DefineLocalVariable(
            (WCHAR *) name,
            VAR_IS_COMP_GEN,
            cbSig,
            (BYTE *) sig,
            ADDR_IL_OFFSET,
            slot, 0, 0,
            0, 0));
    }
}


/*
 * Emit a local variable into a given debug scope
 */
void EMITTER::EmitDebugLocal(LOCVARSYM * sym, mdToken tkLocalVarSig, int ilOffsetStart, int ilOffsetEnd)
{
    // ilOffsetEnd is inclusive, not exclusive as passed in.
    if (ilOffsetEnd != 0)
        ilOffsetEnd--;

    if (sym->fIsIteratorLocal) {
        ASSERT(sym->slot.Index() == (uint)-1);
        ASSERT(sym->movedToField && sym->movedToField->iIteratorLocal >= 0);
        ASSERT(pIteratorLocalsInfo);
        ASSERT(pIteratorLocalsInfo->cBuckets > sym->movedToField->iIteratorLocal);
        CDIIteratorLocalBucket * pBucket = &pIteratorLocalsInfo->rgBuckets[sym->movedToField->iIteratorLocal];
        if (pBucket->ilOffsetStart == 0 || pBucket->ilOffsetStart > ilOffsetStart)
            pBucket->ilOffsetStart = ilOffsetStart;

        if (pBucket->ilOffsetEnd == 0 || pBucket->ilOffsetEnd < ilOffsetEnd)
            pBucket->ilOffsetEnd = ilOffsetEnd;

        return;
    }

    ASSERT(sym->slot.Index() != (uint)-1);
    if (debugemit2 != NULL) {
        CheckHRDbg(debugemit2->DefineLocalVariable2(
            (WCHAR *) sym->name->text,
            0,
            tkLocalVarSig,
            ADDR_IL_OFFSET,
            sym->slot.Index(), 0, 0,
            ilOffsetStart, ilOffsetEnd));
    } else {
        PCOR_SIGNATURE sig;
        int cbSig;

        // Create signature for the type
        sig = BeginSignature();
        sig = EmitSignatureType(sig, sym->type);
        sig = EndSignature(sig, &cbSig);

        CheckHRDbg(debugemit->DefineLocalVariable(
            (WCHAR *)sym->name->text,
            0,
            cbSig,
            (BYTE *) sig,
            ADDR_IL_OFFSET,
            sym->slot.Index(), 0, 0,
            ilOffsetStart, ilOffsetEnd));
    }

}

/*
 * Emit a local constant into a given debug scope
 */
void EMITTER::EmitDebugLocalConst(LOCVARSYM * sym)
{
    VARIANT value;
    HRESULT HR;
    VariantInit(&value);

    if (VariantFromConstVal(sym->type->asAGGTYPESYM(), & sym->constVal, & value)) {
        if (debugemit2 != NULL) {
            HR = debugemit2->DefineConstant2(
                sym->name->text,
                value,
                GetDebugVarSig(sym->type));
            if (HR == E_INVALIDARG) {
                HR = S_OK;
            }
            CheckHRDbg(HR);
        } else {
            PCOR_SIGNATURE sig;
            int cbSig;

            // Create signature for the type
            sig = BeginSignature();
            sig = EmitSignatureType(sig, sym->type);
            sig = EndSignature(sig, &cbSig);

            HR = debugemit->DefineConstant(
                sym->name->text,
                value,
                cbSig,
                (BYTE *) sig);
            if (HR == E_INVALIDARG) {
                HR = S_OK;
            }
            CheckHRDbg(HR);
        }
    }

    VariantClear(&value);
}

/*
 * Emit a scope for local variables starting at the given IL offset
 */
void EMITTER::EmitDebugScopeStart(int ilOffsetStart)
{
    unsigned int dummy;

    CheckHRDbg(debugemit->OpenScope(ilOffsetStart, &dummy));
}

/*
 * Emit a scope for local variables ending at the given IL offset (the passed
 * offset is the next IL instruction.
 */
void EMITTER::EmitDebugScopeEnd(int ilOffsetEnd)
{
    CheckHRDbg(debugemit->CloseScope(ilOffsetEnd));
}


/*
 * Returns the flags to be set for a property.
 */
DWORD EMITTER::GetPropertyFlags(PROPSYM *sym)
{
    DWORD flags = 0;

    return flags;
}


/*
 * Emit a property into the metadata. The parent aggregate, and the
 * property accessors, must already have been emitted into the current
 * metadata output file.
 */
void EMITTER::EmitPropertyDef(PPROPSYM sym)
{
    mdTypeDef parentToken;          // Token of the parent aggregrate.
    mdMethodDef getToken, setToken; // Tokens of getter and setter accessors.
    PCCOR_SIGNATURE sig;
    int cbSig;

    const WCHAR * nameText;
    WCHAR nameBuffer[MAX_FULLNAME_SIZE];

    // Set "nameText" to the output name.
    if (sym->name == NULL) {
        // Explicit method implementations don't have a name in the language. Synthesize 
        // a name -- the name has "." characters in it so it can't possibly collide.
        // force truncation using a character size limit that won't exceed our UTF8 bytes max
        MetaDataHelper::GetExplicitImplName(sym, nameBuffer, SAFE_FULLNAME_CHARS);
        nameText = nameBuffer;
    }
    else {
        nameText = sym->getRealName()->text;
        int len = (int) wcslen(nameText);
        if (len >= SAFE_FULLNAME_CHARS && UTF8LengthOfUnicode(nameText, len) >= MAX_FULLNAME_SIZE) {
            compiler()->ErrorRef(NULL, ERR_IdentifierTooLong, sym);
        }
    }

    // Get typedef token for the containing class/enum/struct. This must
    // be present because the containing class must have been emitted already.
    parentToken = sym->getClass()->tokenEmit;
    ASSERT(TypeFromToken(parentToken) == mdtTypeDef && parentToken != mdTypeDefNil);

    // Determine getter/setter methoddef tokens.
    getToken = setToken = mdMethodDefNil;
    if (sym->methGet) {
        getToken = sym->methGet->tokenEmit;
        ASSERT((!sym->name || TypeFromToken(getToken) == mdtMethodDef) && getToken != mdMethodDefNil);
    }
    if (sym->methSet) {
        setToken = sym->methSet->tokenEmit;
        ASSERT((!sym->name || TypeFromToken(setToken) == mdtMethodDef) && setToken != mdMethodDefNil);
    }

    // Get signature of property.
    sig = SignatureOfMethodOrProp(sym, &cbSig);

    // Define the propertydef.
    CheckHR(metaemit->DefineProperty(
                parentToken,                    // the class/interface on which the property is being defined
                nameText,                       // name of property
                GetPropertyFlags(sym),          // property attributes (CorPropertyAttr)
                sig, cbSig,                     // blob value of COM+ signature
                0,                              // C++ type flags
                NULL, 0,                        // Constant/default value
                setToken, getToken,             // Getter and setter accessors
                NULL,                           // Other accessors
                & sym->tokenEmit));

    RecordEmitToken(& sym->tokenEmit);
}

/*
 * Returns the flags to be set for an event.
 */
DWORD EMITTER::GetEventFlags(EVENTSYM *sym)
{
    DWORD flags = 0;

    return flags;
}

/*
 * Emit an event into the metadata. The parent aggregate, and the
 * event accessors, and the event field/property must already have been 
 * emitted into the current metadata output file.
 */
void EMITTER::EmitEventDef(PEVENTSYM sym)
{
    mdTypeDef parentToken;             // Token of parent aggregate
    mdToken typeToken;                 // Token for type of the event (a delegate type).
    mdMethodDef addToken, removeToken; // Token of add/remove accessors

    const WCHAR * nameText;
    WCHAR nameBuffer[MAX_FULLNAME_SIZE];

    // Set "nameText" to the output name.
    if (sym->name == NULL) {
        // Explicit method implementations don't have a name in the language. Synthesize 
        // a name -- the name has "." characters in it so it can't possibly collide.
        // force truncation using a character size limit that won't exceed our UTF8 bytes max
        MetaDataHelper::GetExplicitImplName(sym, nameBuffer, SAFE_FULLNAME_CHARS);
        nameText = nameBuffer;
    }
    else {
        nameText = sym->name->text;
        int len = (int) wcslen(nameText);
        if (len >= SAFE_FULLNAME_CHARS && UTF8LengthOfUnicode(nameText, len) >= MAX_FULLNAME_SIZE) {
            compiler()->ErrorRef(NULL, ERR_IdentifierTooLong, sym);
        }
    }

    // Get typedef token for the containing class/enum/struct. This must
    // be present because the containing class must have been emitted already.
    parentToken = sym->getClass()->tokenEmit;
    ASSERT(TypeFromToken(parentToken) == mdtTypeDef && parentToken != mdTypeDefNil);

    // Get typeref token for the delegate type of this event.
    typeToken = GetTypeRef(sym->type);

    // Determine adder/remover methoddef tokens.
    addToken = sym->methAdd->tokenEmit;
    ASSERT((!sym->name || TypeFromToken(addToken) == mdtMethodDef) && addToken != mdMethodDefNil);
    removeToken = sym->methRemove->tokenEmit;
    ASSERT((!sym->name || TypeFromToken(removeToken) == mdtMethodDef) && removeToken != mdMethodDefNil);

    // Define the eventdef.
    CheckHR(metaemit->DefineEvent(
        parentToken,                        // the class/interface on which the event is being defined 
        nameText,                           // Name of the event   
        GetEventFlags(sym),                 // Event attributes (CorEventAttr)
        typeToken,                          // a reference to the delegate class
        addToken,                           // required add method 
        removeToken,                        // required remove method  
        mdMethodDefNil,                     // optional fire method    
        NULL,                               // optional array of other methods associate with the event    
        &sym->tokenEmit));                  //  output event token 

    RecordEmitToken(& sym->tokenEmit);
}


/* Handle an error on a Define Attribute call. */
void EMITTER::HandleAttributeError(HRESULT hr, BASENODE *parseTree, METHSYM *method)
{
    CComPtr<IErrorInfo> errorInfo;
    PCWSTR descString;

    // Try to get a really good error string with GetErrorInfo. Otherwise, use the HRESULT to get one.
    // ErrHR does both.
    descString = compiler()->ErrHR(hr, true);

    compiler()->Error(parseTree, ERR_CustomAttributeError, descString, method->getClass());
}
 
/*
 * Emit a user defined custom attribute.
 */
void EMITTER::EmitCustomAttribute(BASENODE *parseTree, mdToken token, METHSYM *method, BYTE *buffer, unsigned bufferSize)
{
    ASSERT(method->getClass()->typeVarsAll->size == 0);
    ASSERT(method->typeVars->size == 0);

    mdToken ctorToken = GetMethodRef(method, method->getClass()->getThisType(), NULL);
    ASSERT(ctorToken != mdTokenNil);
    HRESULT hr = S_OK;

    if (token == mdtAssembly) {
        AGGSYM *cls = method->getClass();
        hr = compiler()->linker->EmitAssemblyCustomAttribute(compiler()->assemID, compiler()->curFile->GetOutFile()->idFile,
            ctorToken, buffer, bufferSize, cls->isSecurityAttribute, cls->isMultipleAttribute);
    } else if (method->getClass()->isSecurityAttribute) {
        // Security attributes must be accumulated for later emitting.
        SaveSecurityAttribute(token, ctorToken, method, buffer, bufferSize);
    } else {
        hr = metaemit->DefineCustomAttribute(token, ctorToken, buffer, bufferSize, NULL);
    }

    if (FAILED(hr)) {
        HandleAttributeError(hr, parseTree, method);
    }
}

/*
 * Save a security attribute on a symbol for later emitted (all security attributes for a 
 * given symbol must be emitted in one call).
 */
void EMITTER::SaveSecurityAttribute(mdToken token, mdToken ctorToken, METHSYM * method, BYTE * buffer, unsigned bufferSize)
{
    SECATTRSAVE * pSecAttrSave;
    ASSERT(token == tokenSecAttrSave || cSecAttrSave == 0);

    if (!compiler()->options.m_fCompileSkeleton) {
        // Allocate new structure to save the information.
        pSecAttrSave = (SECATTRSAVE *) compiler()->globalHeap.Alloc(sizeof(SECATTRSAVE));
        pSecAttrSave->next = listSecAttrSave;
        pSecAttrSave->ctorToken = ctorToken;
        pSecAttrSave->method = method;
        pSecAttrSave->bufferSize = bufferSize;
        pSecAttrSave->buffer = (BYTE *) compiler()->globalHeap.Alloc(bufferSize);
        memcpy(pSecAttrSave->buffer, buffer, bufferSize);

        // Link into the global list of security attributes.
        listSecAttrSave = pSecAttrSave;
        tokenSecAttrSave = token;
        ++cSecAttrSave;
    }
}

/*
 * Emit pending security attributes on a symbol.
 */
void EMITTER::EmitSecurityAttributes(BASENODE *parseTree, mdToken token)
{
    if (cSecAttrSave == 0)
        return;  // nothing to do.

    ASSERT(token == tokenSecAttrSave);

    COR_SECATTR * rSecAttrs = STACK_ALLOC(COR_SECATTR, cSecAttrSave);
    SECATTRSAVE * pSecAttrSave;
    int i;

    // Put all the saved attributes into one array.
    pSecAttrSave = listSecAttrSave;
    i = 0;
    while (pSecAttrSave) {
        rSecAttrs[i].tkCtor = pSecAttrSave->ctorToken;
        rSecAttrs[i].pCustomAttribute = pSecAttrSave->buffer;
        rSecAttrs[i].cbCustomAttribute = pSecAttrSave->bufferSize;
        ++i;
        pSecAttrSave = pSecAttrSave->next;
    }
    ASSERT(i == (int)cSecAttrSave);

    HRESULT hr;
    hr = metaemit->DefineSecurityAttributeSet(token, rSecAttrs, cSecAttrSave, NULL);
    if (FAILED(hr)) {
        HandleAttributeError(hr, parseTree, listSecAttrSave->method); // use first attribute for error reporting.
    }

    FreeSavedSecurityAttributes();
}

/*
 * Emit a type forwarder (exported type pointing to a seperate assembly) for the type specified.
 */
void EMITTER::EmitTypeForwarder(AGGTYPESYM *type)
{
    WCHAR wszName[MAX_FULLNAME_SIZE];
    MetaDataHelper::GetFullName(type->getAggregate(), wszName, lengthof(wszName));
    PAGGSYM pagSym = type->getAggregate();

    mdToken tkAssemblyRef = GetScopeForTypeRef(pagSym);
    ASSERT(TypeFromToken(tkAssemblyRef) == mdtAssemblyRef);
    compiler()->linker->ExportTypeForwarder(tkAssemblyRef, wszName, 0, &pagSym->tokenComType);

    EmitNestedTypeForwarder(pagSym, tkAssemblyRef);
}

/*
 *  Recursively add entries to the exported type table for any base types of 'agg'.  If a type is forwarded, all 
 *  nested types must have an extry in the exported type table which points to their parent's index in the exported type table.
 */
void EMITTER::EmitNestedTypeForwarder(AGGSYM *agg, mdAssemblyRef tkAssemblyRef)
{
    ASSERT(!agg->isSource);

    if (agg->isNested()) {
        // A type forwarder for a nested type is just an exported type pointing to the type forwarder of the parent type.
        CheckHR(compiler()->linker->ExportNestedTypeForwarder(compiler()->assemID, 
            tkAssemblyRef,                            // Nested types are guaranteed to be in the same assembly as the parent.
            mdTypeDefNil,                             // This is only a "hint" of which type def is the parent,  
                                                      // since we don't have type typedefs for an imported assembly, we cannot emit anything.
            agg->parent->asAGGSYM()->tokenComType,    // Add pointer to our forwarded type as the parent 
            agg->name->text,                          // Use the simple name, we do no use the fully qualified name for nested types.
            0,                                        // Do no specify any flags for nested type forwarders.
            &agg->tokenComType));
    }

    // Walk the children of this type:  we need entries in the exported type table for each nested type.
    ASSERT(TypeFromToken(agg->tokenComType) == mdtExportedType);
    FOREACHCHILD(agg, elem)
        SETLOCATIONSYM(elem);

        if (elem->getKind() != SK_AGGSYM) 
            continue;

        EmitNestedTypeForwarder(elem->asAGGSYM(), tkAssemblyRef);

    ENDFOREACHCHILD

}

void EMITTER::FreeSavedSecurityAttributes()
{
    SECATTRSAVE * pSecAttrSave;

    // Free the saved attrbutes.
    pSecAttrSave = listSecAttrSave;
    while (pSecAttrSave) {
        SECATTRSAVE *pNext;
        pNext = pSecAttrSave->next;
        compiler()->globalHeap.Free(pSecAttrSave->buffer);
        compiler()->globalHeap.Free(pSecAttrSave);
        pSecAttrSave = pNext;
    }

    cSecAttrSave = 0;
    listSecAttrSave = NULL;
}


void EMITTER::EmitMethodImpl(PMETHSYM sym)
{
    ASSERT(!sym->swtSlot.Type() == !sym->swtSlot.Sym());

    // Explicit interface method implementations have null name.
    if (sym->fNeedsMethodImp && sym->swtSlot)
        EmitMethodImpl(sym, sym->swtSlot);
}


void EMITTER::EmitMethodImpl(PMETHSYM sym, SymWithType swtExpImpl)
{
    ASSERT(swtExpImpl && !swtExpImpl.Type() == !swtExpImpl.Sym());

    // Don't emit method impls if there were any errors since it's not guaranteed that the needed
    // method def has been emitted (and we don't need them for skeleton compiles).
    if (compiler()->ErrorCount())
        return;

    HRESULT hr = metaemit->DefineMethodImpl(
                sym->getClass()->tokenEmit,     // The class implementing the method
                sym->tokenEmit,                 // our methoddef token
                GetMethodRef(
                    swtExpImpl.Meth(), 
                    swtExpImpl.Type(), 
                    NULL));  // method being implemented
    CheckHR(hr);
}

TypeMemTokMap * TypeMemTokMap::Create(COMPILER * compiler, NRHEAP * heap)
{
    TypeMemTokMap * pmap = (TypeMemTokMap *)heap->AllocZero(sizeof(TypeMemTokMap));

    pmap->heap = heap;
    pmap->compiler = compiler;
    pmap->centHash = 64;
    pmap->prgent = (Entry **)heap->AllocZero(pmap->centHash * sizeof(Entry *));
    ASSERT(pmap->centTot == 0);

    return pmap;
}


TypeMemTokMap::Entry * TypeMemTokMap::Find(Key & key, Entry *** ppentIns)
{
    Entry ** pent = prgent + (key.hash & (centHash - 1));

    *ppentIns = pent;
    while (*pent) {
        if ((*pent)->key == key)
            return *pent;
        pent = &(*pent)->next;
    }

    return NULL;
}


void TypeMemTokMap::GrowTable()
{
    // centHash is a power of 2.
    ASSERT(centHash > 0 && (centHash & (centHash - 1)) == 0);
    ASSERT(centTot >= 2 * centHash);

    // Go from average density 2 to average density 1/2.
    int centHashNew = centHash * 4;
    Entry ** prgentNew = (Entry **)heap->AllocZero(SizeMul(centHashNew, sizeof(Entry *)));

    // For each old slot in the hash table....
    for (Entry ** pent = prgent + centHash; pent-- > prgent; ) {
        // For each entry in the slot....
        while (*pent) {
            Entry * ent = *pent;

            // Remove it from the source chain.
            *pent = ent->next;

            // Find the new slot.
            ASSERT(pent == prgent + (ent->key.hash & (centHash - 1)));
            Entry ** pentIns = prgentNew + (ent->key.hash & (centHashNew - 1));

            // Insert it at the head of the new chain.
            ent->next = *pentIns;
            *pentIns = ent;
        }
    }

    prgent = prgentNew;
    centHash = centHashNew;
}


TypeMemTokMap::Entry * TypeMemTokMap::New(Key & key, Entry ** pentIns)
{
    Entry * ent = (Entry *)heap->Alloc(sizeof(Entry));
    ent->key = key;
    ent->tok = 0;
    ent->next = *pentIns;
    *pentIns = ent;
    if (++centTot >= 2 * centHash)
        GrowTable();

    return ent;
}


mdToken * TypeMemTokMap::GetTokPtr(SYM * sym, TypeArray * typeArgsCls, TypeArray * typeArgsMeth)
{
    Key key(sym, typeArgsCls, typeArgsMeth);

    Entry ** pentIns;
    Entry * ent = Find(key, &pentIns);

    if (!ent) {
        ent = New(key, pentIns);
        ASSERT(!ent->tok);
    }

    return &ent->tok;
}
