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
/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XX                                                                           XX
XX                            FJit.cpp                                       XX
XX                                                                           XX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
*/

#include "jitpch.h"
#include "openum.h"
#include "clrtypes.h"

// Stores the stack before an operation that may cause GC. This is necessary for GC to find roots
// of local variables.
#define LABELSTACK(pcOffset, numOperandsToIgnore)                       \
        _ASSERTE(opStack_len >= numOperandsToIgnore);             \
        stacks.append((unsigned)(pcOffset), opStack, opStack_len-numOperandsToIgnore, 0)

#if defined(_DEBUG) || defined(LOGGING)
void logMsg(ICorJitInfo* info, unsigned logLevel, char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    info->logMsg(logLevel, fmt, args);
}
#endif

#if defined(_DEBUG) || defined(LOGGING)
ICorJitInfo* logCallback = 0;              // where to send the logging mesages
extern const char *opname[];
#endif

#define DECLARE_HELPERS      // causes the helpers to be declared
#include "fjitcore.h"

//
// OpType Constructors
// These were placed here to prevent inlining of the constructors
// Inlining of constructors exposed a bug in gcc on osx. If
// this is known to be fixed, it will be safe to move them back to the header
// file.
//

OpType::OpType()
{
#ifdef _DEBUG
    type_enum = typeError;
#endif
    readonly    = 0;
    type_handle = 0;
}

OpType::OpType(OpTypeEnum opEnum)
{
    type_enum    = opEnum;
    readonly     = 0;
    type_handle  = 0;
    _ASSERTE(type_enum < typeCount);
}


OpType::OpType(CORINFO_CLASS_HANDLE valClassHandle)
{
    type_handle = valClassHandle;
    readonly    = 0;
    type_enum   = typeValClass;
}

OpType::OpType(CORINFO_METHOD_HANDLE mHandle)
{
    type_enum     = typeMethod;
    readonly      = 0;
    method_handle = mHandle;
}

OpType::OpType(OpTypeEnum opEnum,
               CORINFO_CLASS_HANDLE valClassHandle,
               bool setClassHandle,
               bool isReadOnly)
{
    type_enum    = opEnum;
    readonly     = (unsigned) isReadOnly;
    if ( !isPrimitive() || isRef() || isByRef() || setClassHandle)
        type_handle  = valClassHandle;
    else
        type_handle  = NULL;
    _ASSERTE(type_enum < typeCount);
}

OpType::OpType(OpTypeEnum opEnum, OpTypeEnum childEnum)
{
    type_enum    = opEnum;
    readonly     = 0;
    child_type   = childEnum;
    _ASSERTE(type_enum == typeByRef && child_type > typeRef ||
             type_enum == typeRef   && child_type == typeRef );
}

OpType::OpType(CorInfoType jitType, CORINFO_CLASS_HANDLE valClassHandle, bool setClassHandle)
{
    type_enum   = OpType(jitType).enum_();
    readonly       = 0;
    if ( !isPrimitive() || isRef() || isByRef() || setClassHandle )
        type_handle  = valClassHandle;
    else
        type_handle  = NULL;
    _ASSERTE(type_enum < typeCount);
}


OpType::OpType(CorInfoType jitType)
{
    _ASSERTE(jitType < CORINFO_TYPE_COUNT);
    _ASSERTE((typeI4 > typeI2) && (typeI1 > typeU1) && (typeU2 > typeU1));
    _ASSERTE(toOpStackType[CORINFO_TYPE_REFANY] == typeRefAny);  //spot check table
    _ASSERTE(toOpStackType[CORINFO_TYPE_BYREF] == typeByRef);    //spot check table
    _ASSERTE(toOpStackType[CORINFO_TYPE_LONG] == typeI8);        //spot check table
    //_ASSERTE(sizeof(toOpStackType) == CORINFO_TYPE_COUNT);
    type_enum = (OpTypeEnum) toOpStackType[jitType];
    readonly          = 0;
    type_handle       = 0;
    if (type_enum == typeError)
    {
        _ASSERTE(!"Bad input to the constructor");
        RaiseException(SEH_JIT_REFUSED,EXCEPTION_NONCONTINUABLE,0,NULL); //_ASSERTE(type_enum != typeError);
    }
}

//
// FJit Class
//


/*  Either an available FJit or NULL
    We are assuming that on average we will finish this jit before another starts up.
    If that proves to be untrue, we'll just allocate new FJit's as necessary.
    We delete the extra ones in FJit::ReleaseContext()
    */
FJit* next_FJit;

// This is the same as New special cased for FJit since the caller
// has an SEH __try block which is not allowed by the compiler.
void NewFJit(FJit** pNewContext, ICorJitInfo* comp)
{
    if ((*pNewContext = new FJit(comp)) == NULL)
        RaiseException(SEH_NO_MEMORY,EXCEPTION_NONCONTINUABLE,0,NULL);
}


FJit::FJit(ICorJitInfo* comp) {

#if defined(_DEBUG) || defined(LOGGING)
    szDebugClassName = new char[255];
#endif

    New(mapping,FJit_Encode());
    New(state, FJitState[0]);
    New(localsMap,stackItems[0]);
    New(argsMap,stackItems[0]);
    New(opStack,OpType[0]);
    New(tempOpStack, OpType[0]);
    New(localsGCRef,unsigned char[0]);
    New(interiorGC,unsigned char[0]);
    New(pinnedGC,unsigned char[0]);
    New(pinnedInteriorGC,unsigned char[0]);
    New(gcHdrInfo,unsigned char[0]);

    codeBuffer = (unsigned char*)VirtualAlloc(NULL,
                              MIN_CODE_BUFFER_RESERVED_SIZE,
                              MEM_RESERVE,
                              PAGE_READWRITE);

    if (codeBuffer) {
        codeBufferReservedSize = MIN_CODE_BUFFER_RESERVED_SIZE;
        codeBufferCommittedSize = 0;
    }
    else
    {
        codeBufferReservedSize = 0;
        codeBufferCommittedSize = 0;
#ifdef _DEBUG
        DWORD errorcode = GetLastError();
        LOGMSG((jitInfo, LL_INFO1000, "Virtual alloc failed. Error code = %#x", errorcode));
#endif
    }

    New(fixupTable,FixupTable());
    gcHdrInfo_len = 0;
    gcHdrInfo_size = 0;
    interiorGC_len = 0;
    localsGCRef_len = 0;
    pinnedGC_len = 0;
    pinnedInteriorGC_len = 0;
    interiorGC_size = 0;
    localsGCRef_size = 0;
    pinnedGC_size = 0;
    pinnedInteriorGC_size = 0;
    EHBuffer_size = 256;    // start with some reasonable size, it is grown more if needed
    New(EHBuffer,unsigned char[EHBuffer_size]);
    opStack_len = 0;
    opStack_size = 0;
    tempOpStack_size = 0;
    state_size = 0;
    locals_size = 0;
    args_size = 0;
    ver_failure = 0;

    jitInfo = NULL;
    flags = 0;

    // initialize cached constants
    CORINFO_EE_INFO CORINFO_EE_INFO;
    comp->getEEInfo(&CORINFO_EE_INFO);
    offsetOfEEClass = CORINFO_EE_INFO.offsetOfEEClass;
    offsetOfInterfaceTable = CORINFO_EE_INFO.offsetOfInterfaceTable;
}

FJit::~FJit() {

#if defined(_DEBUG) || defined(LOGGING)
    delete [] szDebugClassName;
    szDebugClassName = NULL;
#endif

    if (mapping) delete mapping;
    mapping = NULL;
    if (state)  delete [] state;
    state = NULL;
    if (argsMap) delete [] argsMap;
    argsMap = NULL;
    if (localsMap) delete [] localsMap;
    localsMap = NULL;
    if (opStack) delete [] opStack;
    opStack = NULL;
    if (tempOpStack) delete [] tempOpStack;
    tempOpStack = NULL;
    if (localsGCRef) delete [] localsGCRef;
    localsGCRef = NULL;
    if (interiorGC) delete [] interiorGC;
    interiorGC = NULL;
    if (pinnedGC) delete [] pinnedGC;
    pinnedGC = NULL;
    if (pinnedInteriorGC) delete [] pinnedInteriorGC;
    pinnedInteriorGC = NULL;
    if (gcHdrInfo) delete [] gcHdrInfo;
    gcHdrInfo = NULL;
    if (EHBuffer) delete [] EHBuffer;
    EHBuffer = NULL;
    _ASSERTE(codeBuffer || codeBufferCommittedSize == 0 && codeBufferReservedSize == 0);
    _ASSERTE(codeBuffer || codeBufferReservedSize == 0);
    if (codeBufferReservedSize>0 || codeBufferCommittedSize>0)
        VirtualFree(codeBuffer, 0, MEM_RELEASE);
    codeBufferReservedSize = 0;
    if (fixupTable) delete fixupTable;
    fixupTable = NULL;
}

/* initialize the compilation context with the method data */
void FJit::setup() {
    unsigned size;
    unsigned char* outPtr;

    CORINFO_CLASS_HANDLE classHnd = jitInfo->getMethodClass(methodInfo->ftn);
    methodAttributes = jitInfo->getMethodAttribs(methodInfo->ftn, methodInfo->ftn);

    _ASSERTE(((methodAttributes & CORINFO_FLG_STATIC) == 0) == (methodInfo->args.hasThis()));

    /* set up the labled stacks */
    stacks.reset();
    stacks.jitInfo = jitInfo;

    /* initialize the fixup table */
    fixupTable->setup();

    /* set gcHdrInfo compression buffer empty */
    gcHdrInfo_len = 0;
    if (methodInfo->EHcount) {
        JitGeneratedLocalsSize = (methodInfo->EHcount*2+2)*sizeof(void*);  // two locals for each EHclause,1 for localloc, and one for end marker
    }
    else {
        JitGeneratedLocalsSize = sizeof(void*);  // no eh clause, but there might be a localloc
    }
    /* compute local offsets */
    computeLocalOffsets(); // should be replaced by an exception?
    /* encode the local gc refs and interior refs into the gcHdrInfo */
    //make sure there's room
    //Compression ratio 8:1 (1 BYTE = 8 bits gets compressed to 1 bit)
    size = ( localsGCRef_len + 7 +
             interiorGC_len + 7 +
             pinnedGC_len + 7 +
             pinnedInteriorGC_len + 7) / 8
              + 2*4 /* bytes to encode size of each*/;

    if (gcHdrInfo_len+size > gcHdrInfo_size) {
        gcHdrInfo_size = growBuffer(&gcHdrInfo, gcHdrInfo_len, gcHdrInfo_len+size);
    }
    //drop the pieces in
    size = FJit_Encode::compressBooleans(localsGCRef, localsGCRef_len);
    outPtr = &gcHdrInfo[gcHdrInfo_len];
    gcHdrInfo_len += FJit_Encode::encode(size, &outPtr);
    memcpy(outPtr, localsGCRef, size);
    gcHdrInfo_len += size;

    size = FJit_Encode::compressBooleans(interiorGC, interiorGC_len);
    outPtr = &gcHdrInfo[gcHdrInfo_len];
    gcHdrInfo_len += FJit_Encode::encode(size, &outPtr);
    memcpy(outPtr, interiorGC, size);
    gcHdrInfo_len += size;

    size = FJit_Encode::compressBooleans(pinnedGC, pinnedGC_len);
    outPtr = &gcHdrInfo[gcHdrInfo_len];
    gcHdrInfo_len += FJit_Encode::encode(size, &outPtr);
    memcpy(outPtr, pinnedGC, size);
    gcHdrInfo_len += size;

    size = FJit_Encode::compressBooleans(pinnedInteriorGC, pinnedInteriorGC_len);
    outPtr = &gcHdrInfo[gcHdrInfo_len];
    gcHdrInfo_len += FJit_Encode::encode(size, &outPtr);
    memcpy(outPtr, pinnedInteriorGC, size);
    gcHdrInfo_len += size;

    _ASSERTE(gcHdrInfo_len <= gcHdrInfo_size);

    /* set up the operand stack */
    size = methodInfo->maxStack+1; //+1 since for a new obj intr, +1 for exceptions
#ifdef _DEBUG
    size++; //to allow writing TOS marker beyond end;
#endif
    if (size > opStack_size) {
        if (opStack) delete [] opStack;
        opStack_size = size+4; //+4 to cut down on reallocations
        New(opStack,OpType[opStack_size]);
    }
    /* Validate that we didn't overflow on the stack allocation */
    if (methodInfo->maxStack > opStack_size )
        { RaiseException(SEH_JIT_REFUSED,EXCEPTION_NONCONTINUABLE,0,NULL); }

    resetOpStack();  //stack starts empty

    /* clear the verification stack table */
    ver_stacks.reset();

    /* clear the flow analysis and verification structures */
    SplitOffsets.reset();

    /* compute arg offsets, note offsets <0 imply enregistered args */
    args_len = methodInfo->args.numArgs;
    if (methodInfo->args.hasThis()) args_len++;     //+1 since we treat <this> as arg 0, if <this> is present
    if (methodInfo->args.hasTypeArg()) args_len++;
    if (args_len > args_size) {
        if (argsMap) delete [] argsMap;
        args_size = args_len+4; //+4 to cut down on reallocating.
        New(argsMap,stackItems[args_size]);
    }

    // Get layout information on the arguments
    C_ASSERT(sizeof(stackItems) == sizeof(argInfo));
    argInfo* argsInfo = (argInfo*) argsMap;

    unsigned enregisteredSize = 0;
    argsFrameSize = computeArgInfo(&methodInfo->args, argsInfo, classHnd, enregisteredSize);

    // Convert the sizes to offsets (assumes the stack grows down)
    // Note we are reusing the same memory in place!
    unsigned offset = ( methodInfo->args.isVarArg() ? 0 : argsFrameSize + sizeof(prolog_frame) );
    unsigned varArgCookieOff = sizeof(prolog_frame);

    // The varargs frame starts just above the first argument but past the this pointer and the return buffer for
    // emulated fastcall calling convention

    if (PushEnregArgs || PARAMETER_SPACE )
    {
       offset = ARGS_RIGHT_TO_LEFT ? offset + enregisteredSize : sizeof(prolog_frame);

       // The return buffer is not mentioned in the signature so the offset has to be
       // adjusted if it is present and enregistered
       offset += ( (!ARGS_RIGHT_TO_LEFT && methodInfo->args.hasRetBuffArg() && EnregReturnBuffer) ? sizeof(void *) : 0 );

       // Different method of access is used for vararg functions so the offset need to be negated
       if (methodInfo->args.isVarArg())
         if (!PARAMETER_SPACE )
           offset = 0 - offset;
         else
           varArgCookieOff += ( (methodInfo->args.hasThis()) + (methodInfo->args.hasRetBuffArg()) )*sizeof(void *);
    }

    for (unsigned i = 0; i < args_len; i++) {
        unsigned argSize = argsInfo[i].size;

        // Skip the vararg cookie, which is not explicitly mentioned in the signature
        if ( PARAMETER_SPACE && methodInfo->args.isVarArg() && varArgCookieOff == offset )
            offset     += sizeof(void *);

        // If a pointer to a value type is passed instead of the value type - change the size
        if ( PASS_VALUETYPE_BYREF && (argsInfo[i].type.enum_() == typeRefAny || argsInfo[i].type.enum_() == typeValClass) )
            argSize = sizeof(void *);

        if (argsInfo[i].isReg ) {
          if (!PushEnregArgs)
          {
            if ( !EnregArgumentsFP || (argsInfo[i].type != typeR4 && argsInfo[i].type != typeR8 ) )
                 argsMap[i].offset = offsetOfRegister(argsInfo[i].regNum);
            else
            {
                _ASSERTE( !(ARGS_RIGHT_TO_LEFT) );  // This code assumes an ordering
                ON_X86_ONLY(_ASSERTE(false);)
                argsMap[i].offset = offset;
            }
            argsMap[i].isReg  = 1;
            argsMap[i].regNum = argsInfo[i].regNum;
            offset = ARGS_RIGHT_TO_LEFT ? offset : offset + argSize;
          }
          else
          {
            _ASSERTE( ARGS_RIGHT_TO_LEFT );
            argsMap[i].offset = argsInfo[i].regNum*4 + 8;
            argsMap[i].isReg  = 0;
          }
        }
        else {
            offset = ARGS_RIGHT_TO_LEFT ? offset - argSize : offset;
            argsMap[i].offset = offset;
            offset = ARGS_RIGHT_TO_LEFT ? offset : offset + argSize;
        }

        // Fill out the rest of the structure
        argsMap[i].type = argsInfo[i].type;

        // Allocate local space for unaligned arguments
        if (ALIGN_ARGS && (argsMap[i].type.enum_() == typeR8 || argsMap[i].type.enum_() == typeI8)
                       && (argsMap[i].offset % SIZE_STACK_SLOT))
       localsFrameSize += 8;
    }

    ON_X86_ONLY(_ASSERTE(PushEnregArgs || offset == sizeof(prolog_frame) || methodInfo->args.isVarArg());)

    /* build the method header info for the code manager */
    mapInfo.hasThis      = methodInfo->args.hasThis();
    mapInfo.isValueClass = (jitInfo->getClassAttribs(classHnd, methodInfo->ftn) & CORINFO_FLG_VALUECLASS) != 0;
    mapInfo.hasRetBuff   = methodInfo->args.hasRetBuffArg();
    mapInfo.savedIP      = false;
    mapInfo.EnCMode      = (flags & CORJIT_FLG_DEBUG_EnC) ? true : false;

    mapInfo.methodArgsSize = PARAMETER_SPACE ?
                             (argsFrameSize + enregisteredSize) :
                             (PushEnregArgs ? argsFrameSize + enregisteredSize:  argsFrameSize);

    #if FIXED_ENREG_BUFFER
    if (  mapInfo.methodArgsSize < FIXED_ENREG_BUFFER )
      mapInfo.methodArgsSize = FIXED_ENREG_BUFFER;
    #endif

    mapInfo.methodFrame = (unsigned short)((localsFrameSize + sizeof(prolog_data))/sizeof(void*));
    //mapInfo.hasSecurity = (methodAttributes & CORINFO_FLG_SECURITYCHECK) ? TRUE : FALSE;
    mapInfo.methodJitGeneratedLocalsSize = JitGeneratedLocalsSize;


}

/******************************************************************************/
// Given a type token, generate code that will evaluate to the correct type
// handle representation of that token. This might require run-time lookup if the
// enclosing method is shared between instantiations and the token refers to a
// type spec that contains type variables.
//
// Usage: This emits the handle as argument 1 for a handler call.
void FJit::TokenToHandle(unsigned annotatedToken,
                         CORINFO_CLASS_HANDLE& tokenType,
                         CorInfoTokenKind tokenKind /* = CORINFO_TOKENKIND_Default */,
                         bool emitCode /*=true*/)
{

    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    tokenType = jitInfo->getTokenTypeAsHandle(tokenScope, annotatedToken);

    BOOL isTypeTok = (TypeFromToken(annotatedToken) == mdtTypeDef ||
                      TypeFromToken(annotatedToken) == mdtTypeRef ||
                      TypeFromToken(annotatedToken) == mdtTypeSpec);
     
    CORINFO_GENERICHANDLE_RESULT embedInfo;
    jitInfo->embedGenericHandle(tokenScope, // module containing the IL for the code where the token comes from (would be different if inlining)
                                isTypeTok ? (annotatedToken | CORINFO_ANNOT_PERMITUNINSTDEFORREF) : annotatedToken,
                                methodInfo->ftn, // parameter to give exact type context for code (would be different if inlining)
                                tokenKind,
                                &embedInfo);

    // this should always hold for non-shared code
    _ASSERTE(!embedInfo.lookup.lookupKind.needsRuntimeLookup);

    // check we're not returned an indirection (which should only occur if precompiling)
    _ASSERTE(embedInfo.lookup.constLookup.handle);

    if (emitCode) 
    {
        emit_LDC_I(embedInfo.lookup.constLookup.handle);
    }
}

/* get and initialize a compilation context to use for compiling */
FJit* FJit::GetContext(ICorJitInfo* comp, CORINFO_METHOD_INFO* methInfo, DWORD dwFlags) {
    FJit* next;

    next = (FJit*)InterlockedExchangePointer((PVOID*)&next_FJit,NULL);

    BOOL gotException = TRUE;
    PAL_TRY
    {
        /*if the list was empty, make a new one to use */
        if (!next)
        {
            NewFJit(&next,comp);
        }
#if defined(_DEBUG) || defined(LOGGING)
        const char * temp;
        next->szDebugMethodName = comp->getMethodName(methInfo->ftn, &temp );
        strncpy((char *)next->szDebugClassName, temp,  254 );
#endif

        /* set up this one for use */
        next->jitInfo = comp;
        next->methodInfo = methInfo;
        next->flags = dwFlags;
        next->ensureMapSpace();
        next->setup();
        gotException = FALSE;

    }
    PAL_FINALLY
    {
        // cleanup if we get here because of an exception
        if (gotException && (next != NULL))
        {
            next->ReleaseContext();
            next = NULL;
        }
    }
    PAL_ENDTRY

    return next;
}

/* return a compilation context to the free list */
/* xchange with the next_FJit and if we get one back, delete the one we get back
   The assumption is that the steady state case is almost no concurrent jits
   */
void FJit::ReleaseContext() {
    FJit* next;

    /* mark this context as not in use */
    jitInfo = NULL;
    methodInfo = NULL;
    _ASSERTE(this != next_FJit);     // I should not be on the free 'list'

    next = (FJit*)InterlockedExchangePointer((PVOID*) &next_FJit, this);

    _ASSERTE(this != next);                 // I was not on the free 'list'
    if (next) delete next;
}

/* make sure list of available compilation contexts is initialized at startup */
BOOL FJit::Init() {
    next_FJit = NULL;
    return TRUE;
}

void FJit::Terminate() {
    FJit* next;
    next = (FJit*)InterlockedExchangePointer((PVOID*) &next_FJit, NULL);
    if (next) delete next;

    return;
}

/* Reset state of context to state at the start of jitting. Called when we need to abort and rejit a method */
void FJit::resetContextState(bool ResetVerFlags)
{
    fixupTable->setup();
    mapping->reset();
    stacks.reset();
    resetOpStack();
    SplitOffsets.reset();

    // Reset the verification flags if necessary
    if ( ResetVerFlags )
    {
      ver_failure = 0;
      ver_failure_offset = 0;
    }
}
void FJit::resetStateArray()
{
   memset(state, 0, state_size * sizeof(FJitState));
}

/*adjust the internal mem structs as needed for the size of the method being jitted*/
void FJit::ensureMapSpace() {

    if (methodInfo->ILCodeSize + 1 > state_size) {
        delete [] state;
        state_size = methodInfo->ILCodeSize + 1;
        New(state,FJitState[state_size]);
    }
    // We add an extra slot for the epilog
    memset(state, 0, (methodInfo->ILCodeSize + 1) * sizeof(FJitState));
    mapping->ensureMapSpace(methodInfo->ILCodeSize+1);
}

/* compress the gc info into gcHdrInfo and answer the size in bytes */
unsigned int FJit::compressGCHdrInfo(){
    stacks.compress(&gcHdrInfo, &gcHdrInfo_len, &gcHdrInfo_size);
    return gcHdrInfo_len;
}

void FJit::initializeExceptionHandling()
{
    // insert the handler entry points
    if (methodInfo->EHcount) {
        /*lets drop the exception handler entry points into our label table.
          but first we have to make a dummy stack with an object on it. */
        for ( unsigned int except = 0; except < methodInfo->EHcount; except++) {
            CORINFO_EH_CLAUSE clause;
            jitInfo->getEHinfo(methodInfo->ftn, except, &clause);
            state[clause.TryOffset].isTry                       = true;
            state[clause.TryOffset+clause.TryLength].isEndBlock = true;
            state[clause.HandlerOffset].isHandler                         = true;
            state[clause.HandlerOffset + clause.HandlerLength].isEndBlock = true;
            if (clause.Flags & ( CORINFO_EH_CLAUSE_FINALLY |  CORINFO_EH_CLAUSE_FAULT) )
            {
                // Associate an empty stack with the entry point
                ver_stacks.add(clause.HandlerOffset, opStack, 0);
            }
            else { // This has to be a catch clause
                // Make sure the stack is empty
                resetOpStack();
                state[clause.HandlerOffset].isTOSInReg = true;

                if (clause.Flags & CORINFO_EH_CLAUSE_FILTER) {
                   // Push System.Object onto the stack
                   pushOp( OpType(typeRef, jitInfo->getBuiltinClass(CLASSID_SYSTEM_OBJECT)));
                   ver_stacks.add(clause.FilterOffset, opStack, 1);
                   ver_stacks.add(clause.HandlerOffset, opStack, 1);
                   state[clause.FilterOffset].isTOSInReg = true;
                   state[clause.FilterOffset].isHandler  = true;
                   state[clause.FilterOffset].isFilter   = true;
                }
                else
                {
                   // Push the specified exception object onto the stack ( the token has already been validated )
                   pushOp( OpType(typeRef, jitInfo->findClass(methodInfo->scope,clause.ClassToken, methodInfo->ftn)) );
                   ver_stacks.add(clause.HandlerOffset, opStack, 1);
                }
            }
        }
        // drop the dummy stack entry
        resetOpStack();
    }
}

void appendGCArray(unsigned local_word, unsigned* pGCArray_size, unsigned* pGCArray_len,__deref_inout_ecount(*pGCArray_len) unsigned char** ppGCArray)
{
    if (local_word + 1 > *pGCArray_size) {
        *pGCArray_size = FJit::growBuffer(ppGCArray, *pGCArray_len, local_word+1);
    }

    memset(&((*ppGCArray)[*pGCArray_len]), 0, local_word- *pGCArray_len);
    *pGCArray_len = local_word;
    (*ppGCArray)[(*pGCArray_len)++] = true;

}
/* compute the locals offset map for the method being compiled */
void FJit::computeLocalOffsets() {
    /* compute the number of locals and make sure we have the space */
    if (methodInfo->locals.numArgs > locals_size) {
        if (localsMap) delete [] localsMap;
        locals_size = methodInfo->locals.numArgs+16;    // +16 to cut down on reallocation
        New(localsMap,stackItems[locals_size]);
    }

    /* assign the offsets, starting with the ones in defined in the il header */
    interiorGC_len = 0;
    localsGCRef_len = 0;
    pinnedGC_len = 0;
    pinnedInteriorGC_len = 0;
    unsigned local = 0;
    unsigned offset = JitGeneratedLocalsSize;
    unsigned local_word = 0;    //offset in words to a local in the local frame

    /* process the local var sig */
    CORINFO_ARG_LIST_HANDLE sig = methodInfo->locals.args;
    while (local < methodInfo->locals.numArgs) {
        CORINFO_CLASS_HANDLE cls;
        CorInfoTypeWithMod corArgType = jitInfo->getArgType(&methodInfo->locals, sig, &cls);
        CorInfoType argType = strip(corArgType);
        unsigned size = computeArgSize(argType, sig, &methodInfo->locals);
        OpType locType;
        if (argType == CORINFO_TYPE_VALUECLASS) {
            _ASSERTE(cls);
            locType = OpType(cls);

            CorInfoType tempCorType = jitInfo->asCorInfoType(cls);
            OpType tempType = OpType(tempCorType, cls);
            if ( !tempType.isValClass() )
               RaiseException(SEH_JIT_REFUSED,EXCEPTION_NONCONTINUABLE,0,NULL);

            unsigned words = size / sizeof(void*);  //#of void* sized words in local
            if ( ALIGN_LOCALS && (localOffset(offset, size) % SIZE_STACK_SLOT) && words > 1)
        {
            local_word++;
                    offset += sizeof(void *);
        }
            if (local_word + words > localsGCRef_size)
                localsGCRef_size = growBuffer(&localsGCRef, localsGCRef_len, local_word+words);
            memset(&localsGCRef[localsGCRef_len], 0, local_word-localsGCRef_len);
            localsGCRef_len = local_word;
            jitInfo->getClassGClayout(cls, &localsGCRef[localsGCRef_len]);
            // the GC layout needs to be reversed since local offsets are negative with respect to EBP
            if (words > 1) {
                for (unsigned index = 0; index < words/2; index++){
                    unsigned char temp = localsGCRef[localsGCRef_len+index];
                    localsGCRef[localsGCRef_len+index] = localsGCRef[localsGCRef_len+words-1-index];
                    localsGCRef[localsGCRef_len+words-1-index] = temp;
                }
            }
            localsGCRef_len = local_word + words;
        }
        else {
        // Check if the type is actually a valuetype that is represented as a primitive
        locType = createOpType( argType, cls, true );
            switch (locType.enum_()) {
                default:
                    _ASSERTE(!"Bad Type");
                    RaiseException(SEH_JIT_REFUSED,EXCEPTION_NONCONTINUABLE,0,NULL);

                case typeI8:
                case typeR8:
          if ( ALIGN_LOCALS && (localOffset(offset, size) % SIZE_STACK_SLOT) )
          {
            local_word++;
                    offset += sizeof(void *);
          }
                case typeU1:
                case typeU2:
                case typeI1:
                case typeI2:
                case typeI4:
                case typeR4:

                    break;
            case typeRef: {
                    // Store the object handle for the class object
#if !defined(FJIT_NO_VALIDATION)
                    cls = jitInfo->getArgClass(&methodInfo->locals, sig);
                    _ASSERTE(cls != NULL);
                    if ( cls == NULL )
                       RaiseException(SEH_JIT_REFUSED,EXCEPTION_NONCONTINUABLE,0,NULL);
                    locType.setHandle(cls);
#endif
                    if (corArgType & CORINFO_TYPE_MOD_PINNED)
                    {
                        appendGCArray(local_word,&pinnedGC_size,&pinnedGC_len,&pinnedGC);
                    }
                    else
                    {
                        appendGCArray(local_word,&localsGCRef_size,&localsGCRef_len,&localsGCRef);
                    }
                }    break;

                case typeRefAny: {
                    if ( ALIGN_LOCALS && (localOffset(offset, size) % SIZE_STACK_SLOT) )
                    {
                        local_word++;
                        offset += sizeof(void *);
                    }
                    if (local_word + 2 > interiorGC_size) {
                        interiorGC_size = growBuffer(&interiorGC, interiorGC_len, local_word+2);
                    }
                    memset(&interiorGC[interiorGC_len], 0, local_word-interiorGC_len);
                    interiorGC_len = local_word;
                    interiorGC[interiorGC_len++] = true;
                    interiorGC[interiorGC_len++] = false;
                    } break;
                case typeByRef:
                    // Store the object handle for the managed pointer
#if !defined(FJIT_NO_VALIDATION)
                    cls = jitInfo->getArgClass(&methodInfo->locals,sig);
                    // Make sure that the pointer is typed
                    _ASSERTE(cls != NULL);
                    if ( cls == NULL )
                       RaiseException(SEH_JIT_REFUSED,EXCEPTION_NONCONTINUABLE,0,NULL);
                    CORINFO_CLASS_HANDLE childClassHandle;
                    CorInfoType childType = jitInfo->getChildType(cls, &childClassHandle);
                    locType.setTarget(childType,childClassHandle);
#endif
                    if (corArgType & CORINFO_TYPE_MOD_PINNED)
                    {
                        appendGCArray(local_word,&pinnedInteriorGC_size,&pinnedInteriorGC_len,&pinnedInteriorGC);
                    }
                    else
                    {
                        appendGCArray(local_word,&interiorGC_size,&interiorGC_len,&interiorGC);
                    }
                    break;
            }
        }
        localsMap[local].isReg = false;
        localsMap[local].offset = localOffset(offset, size);
        localsMap[local].type = locType;
        local_word += size/sizeof(void*);
        local++;
        offset += size;
        sig = jitInfo->getArgNext(sig);
    }
    localsFrameSize = offset;

    // Pad the managed frame size to get the proper alignment for the first entry of the evaluation stack
    if ( SIZE_STACK_SLOT != sizeof(void *) && ((localsFrameSize + sizeof(prolog_data))% SIZE_STACK_SLOT ) )
      localsFrameSize += ( (localsFrameSize + sizeof(prolog_data)) % SIZE_STACK_SLOT );
}

/* answer true if this arguement type is enregisterable on a machine chip */
bool FJit::enregisteredArg(CorInfoType argType) {
    _ASSERTE( argType < CORINFO_TYPE_COUNT && (int)argType >= 0 );
    return typeEnregisterMap[(int)argType];
}

/* compute the size of an argument based on machine chip */
unsigned int FJit::computeArgSize(CorInfoType argType, CORINFO_ARG_LIST_HANDLE argSig, CORINFO_SIG_INFO* sig)
{
    _ASSERTE( argType < CORINFO_TYPE_COUNT  && (int)argType >= 0 );
    if (argType == CORINFO_TYPE_VALUECLASS)
    {
        CORINFO_CLASS_HANDLE cls;
        cls = jitInfo->getArgClass(sig, argSig);
        return (int) (cls ?  typeSizeInSlots(jitInfo,cls) *sizeof(void*) : 0);
    }
    else
        return typeSizeMap[(int)argType];
}

/* compute the argument types and sizes based on the jitSigInfo and place them in 'map'
   return the total stack size of the arguments.  Note that although this function takes into
   calling conventions (varargs), and possible hidden params (returning valueclasses)
   only parameters visible at the IL level (declared + this ptr) are in the map.
   Note that 'thisCls' can be zero, in which case, we assume the this pointer is a typeRef */

unsigned FJit::computeArgInfo(CORINFO_SIG_INFO* jitSigInfo, argInfo* map, CORINFO_CLASS_HANDLE thisCls, unsigned & enregSize)
{
    unsigned curGPReg  = 0;  // Index to the next available general purpose register
    unsigned curFPReg  = 0;  // Index to the next available floating point register
    unsigned totalSize = 0;  // Size of the arguments on the stack
    unsigned int arg   = 0;  // Index to the current argument in the map
    enregSize = 0;           // Cumilative size of enregistered arguments

    // Do we have a hidden return value buff parameter, if so it will use up a reg  (goes in front of this)
    if (jitSigInfo->hasRetBuffArg() && EnregReturnBuffer && ReturnBufferFirst) {
        _ASSERTE(curGPReg < MAX_GP_ARG_REGISTER);
        curGPReg++;
        enregSize += sizeof(void *);
    }

    if (jitSigInfo->hasThis())
    {
        map[arg].isReg  = true;      //this is always enregistered
        map[arg].regNum = curGPReg++;
        map[arg].size   = sizeof(void *);
        enregSize      += sizeof(void *);

        if (thisCls != 0)
        {
            unsigned attribs = jitInfo->getClassAttribs(thisCls, methodInfo->ftn);
            if (attribs & CORINFO_FLG_VALUECLASS) {
                if (attribs & CORINFO_FLG_UNMANAGED)
                    map[arg].type = OpType(typeI);      // <this> not in the GC heap, just and int
                else
                    map[arg].type = OpType(typeByRef, thisCls);
                    // <this> was an unboxed value class, it really is passed byref
            }
            else
            {
                map[arg].type = OpType(typeRef, thisCls);
            }
        }
        else
           map[arg].type = OpType(typeI);
        arg++;
    }

    // Do we have a hidden return value buff parameter, if so it will use up a reg (goes behind this)
    if (jitSigInfo->hasRetBuffArg() && EnregReturnBuffer && !ReturnBufferFirst) {
        _ASSERTE(curGPReg < MAX_GP_ARG_REGISTER);
        curGPReg++;
        enregSize += sizeof(void *);
    }

    if (jitSigInfo->isVarArg())     // only 'this' and 'retbuff', are enregistered for varargs on x86
    {
        curGPReg  = PARAMETER_SPACE ? curGPReg + 1 : MAX_GP_ARG_REGISTER;
        enregSize = PARAMETER_SPACE ? enregSize + sizeof(void *): enregSize;
    }

    // because we don't know the total size, we compute the number
    // that needs to be subtracted from the total size to get the correct arg
    CORINFO_ARG_LIST_HANDLE args = jitSigInfo->args;
    for (unsigned  i = 0; i < jitSigInfo->numArgs; i++)
    {
        // Obtain the argument type from the signature
        CORINFO_CLASS_HANDLE cls;
        CorInfoType argType = strip(jitInfo->getArgType(jitSigInfo, args, &cls));

        // Store the argument type in the map
        OpType argJitType = createOpType( argType, cls );
        map[arg].type = argJitType;

        // For managed pointers and objects obtain the exact types
        if ( map[arg].type.enum_() == typeByRef )
        {
            cls = jitInfo->getArgClass(jitSigInfo, args);
            _ASSERTE(cls != NULL);
            CORINFO_CLASS_HANDLE childClassHandle;
            CorInfoType childType = jitInfo->getChildType(cls, &childClassHandle);
            map[arg].type.setTarget(childType,childClassHandle);
        }
        else if ( map[arg].type.enum_() == typeRef )
        {
            cls = jitInfo->getArgClass(jitSigInfo, args);
            map[arg].type.setHandle(cls);
        }

        // Check if the current argument can be enregistered
        if( enregisteredArg(argType) &&
            ((curGPReg < MAX_GP_ARG_REGISTER) ||
             (floatEnregisterMap[map[arg].type.enum_()] && curFPReg < MAX_FP_ARG_REGISTER)) )
        {
            map[arg].isReg  = true;
            map[arg].size   = computeArgSize(argType, args, jitSigInfo);
            if ( PASS_VALUETYPE_BYREF && (map[arg].type.enum_() == typeRefAny || map[arg].type.enum_() == typeValClass) )
                enregSize = enregSize + sizeof(void *) > MAX_GP_ARG_REGISTER*sizeof(void *) ?
                             MAX_GP_ARG_REGISTER*sizeof(void *) : enregSize + sizeof(void *) ;
            else
                enregSize = enregSize + map[arg].size > MAX_GP_ARG_REGISTER*sizeof(void *) ?
                             MAX_GP_ARG_REGISTER*sizeof(void *) : enregSize + map[arg].size ;

            // Select either floating point or general purpose register
            if ( floatEnregisterMap[ map[arg].type.enum_() ] )
            {
               map[arg].regNum = curFPReg++;

               // Update the general purpose register or stack information if necessary
               int GPSpace = map[arg].size - (MAX_GP_ARG_REGISTER - curGPReg)*sizeof(void *);
               if ( GPSpace <= 0 )
                 curGPReg += (map[arg].size / sizeof(void *));
               else
               {
                 totalSize += GPSpace;
                 curGPReg = MAX_GP_ARG_REGISTER;
               }
            }
            else
            {
               map[arg].regNum = curGPReg++;

               if ( PASS_VALUETYPE_BYREF && (map[arg].type.enum_() == typeRefAny || map[arg].type.enum_() == typeValClass) )
               {
         //totalSize += sizeof(void *);
               }
               else if ( map[arg].size > sizeof(void *) )
               {
                  int GPSpace = map[arg].size - sizeof(void *) - (MAX_GP_ARG_REGISTER - curGPReg)*sizeof(void *);
                  if ( GPSpace <= 0 )
                    curGPReg += ((map[arg].size - sizeof(void *))/ sizeof(void *));
                  else
                  {
                    totalSize += GPSpace;
                    curGPReg = MAX_GP_ARG_REGISTER;
                  }
               }
            }
        }
        else
        {
            map[arg].isReg = false;
            map[arg].size  = computeArgSize(argType, args, jitSigInfo);

            if ( PASS_VALUETYPE_BYREF && (map[arg].type.enum_() == typeRefAny || map[arg].type.enum_() == typeValClass) )
            {
               totalSize += sizeof(void *); // to hold the pointer to the temporary copy
            }
            else
              totalSize += map[arg].size;
        }

        // Increment the argument index and proceed to the next argument in the signature
        arg++;
        args = jitInfo->getArgNext(args);
    }

        // Hidden type parameter is passed last
    if (jitSigInfo->hasTypeArg()) {
        map[arg].size = sizeof(void*);
        map[arg].type = OpType(typeI);
        if(curGPReg < MAX_GP_ARG_REGISTER) {
            map[arg].isReg = true;
            map[arg].regNum = curGPReg++;
            enregSize += sizeof(void *);
        }
        else {
            map[arg].isReg = false;
            totalSize += map[arg].size;
        }
    }

    return(totalSize);
}

/* compute the offset of the start of the local relative to the frame pointer */
int FJit::localOffset(unsigned base, unsigned size) {
    /* we need to bias by the size of the element since the stack grows down */
    return - (int) (base + size) + prolog_bias;
}

/* Attempts to merge two stacks according to the ECMA spec Part. III. Returns
   true if successful false otherwise. The merged stack is superimposed onto stack1.
*/
int FJit::mergeStacks( unsigned int size1, OpType* stack1, unsigned int size2, OpType* stack2, bool StoredStack1 )
{
    // Verify that the stack sizes are the same
    if ( size1 != size2 )
        return false;

    int ret_val = MERGE_STATE_SUCCESS;

    for ( unsigned int i = 0; i < size1; i++ )
    {
        // If two types are exactly the same do nothing
        if (stack1[i] == stack2[i])
            continue;

        // Get the current stored type
        OpType currentStored = StoredStack1 ? stack1[i] : stack2[i];

        // if you are trying to assign a readonly byref to a non-readonly byref
        // you have to rejit for proper verification
        if(!currentStored.isReadOnly() && (stack1[i].isReadOnly() || stack2[i].isReadOnly()))
            ret_val = MERGE_STATE_REJIT;

        stack1[i].setReadOnly(stack1[i].isReadOnly() || stack2[i].isReadOnly());

        // If the only delta between the types is the readonly bit, you may continue now...
        if (stack1[i] == stack2[i])
            continue;

        if (stack1[i].isRef() && !stack1[i].isNull())
        {
            if (stack2[i].isNull())                  // NULL can be any reference type
                continue;
            if (!stack2[i].isRef())
                return false;

            // Ask the EE to find the common parent,  This always succeeds since System.Object always works
            stack1[i].setHandle( jitInfo->mergeClasses( stack2[i].cls(), stack1[i].cls() ) );
            // Check if the merged type has is not equivalent to the type with which the block was jitted
            // in this case we must rejit the function
            if ( !canAssign( jitInfo, methodInfo->ftn, stack1[i], currentStored ) || !canAssign( jitInfo, methodInfo->ftn, currentStored, stack1[i] ) )
                ret_val = MERGE_STATE_REJIT;
            continue;
        }
        else if (stack1[i].isNull())
        {
            if (stack2[i].isRef())                   // NULL can be any reference type
            {
                stack1[i].setHandle( stack2[i].cls() );
                continue;
            }
            return false;
        }
#if defined(_DEBUG)
        printf( "Tricky Merge: enum [%d -> %d] handle[ %d -> %d ]\n", stack1[i].type_enum, stack2[i].type_enum,
                stack1[i].type_handle, stack2[i].type_handle );
#endif
        return false;
    }

    return ret_val;
}

/* grow an unsigned char[] array by allocating a new one and copying the old values into it, return the size of the new array */
unsigned FJit::growBuffer(__deref_ecount(chars_len) unsigned char** chars, unsigned chars_len, unsigned new_chars_size) {
    unsigned char* temp = *chars;
    unsigned allocated_size = new_chars_size*3/2 + 16;  //*3/2 +16 to cut down on growing
    New(*chars, unsigned char[allocated_size]);
    if (chars_len) memcpy(*chars, temp, chars_len);
    if (temp) delete [] temp;
#ifdef _DEBUG
    memset(&((*chars)[chars_len]), 0xEE, (allocated_size-chars_len));
#endif
    return allocated_size;
}

#ifdef _DEBUG
void FJit::displayGCMapInfo()
{
    char* typeName[] = {
    "typeError",
    "typeByRef",
    "typeRef",
    "typeU1",
    "typeU2",
    "typeI1",
    "typeI2",
    "typeI4",
    "typeI8",
    "typeR4",
    "typeR8"
    "typeRefAny",
    };

    LOGMSG((jitInfo, LL_INFO100, "********* GC map info *******\n"));
    LOGMSG((jitInfo, LL_INFO100, "Locals: (Length = %#x, Frame size = %#x)\n",methodInfo->locals.numArgs,localsFrameSize));
    unsigned int i;
    for (i=0; i< methodInfo->locals.numArgs; i++) {
        if (!localsMap[i].type.isPrimitive())
            LOGMSG((jitInfo, LL_INFO100, "    local %d: offset: -%#x type: %#x\n", i, -localsMap[i].offset, localsMap[i].type.cls()));
        else
            LOGMSG((jitInfo, LL_INFO100, "    local %d: offset: -%#x type: %s\n",i, -localsMap[i].offset, typeName[localsMap[i].type.enum_()]));
    }
    LOGMSG((jitInfo, LL_INFO100, "Bitvectors printed low bit (low local), first\n"));
    LOGMSG((jitInfo, LL_INFO100, "LocalsGCRef bit vector len=%d bits: ",localsGCRef_len));
    unsigned numbytes = (localsGCRef_len+7)/8;
    unsigned byteNumber = 1;
    while (true)
    {
        char* buf = (char*) &(localsGCRef[byteNumber-1]);
        unsigned char bits = *buf;
        for (i=1; i <= 8; i++) {
            if ((byteNumber-1)*8+i > localsGCRef_len)
                break;
            LOGMSG((jitInfo, LL_INFO100, "%1d ", (int) (bits & 1)));
            bits = bits >> 1;
        }
        if ((byteNumber++ * 8) > localsGCRef_len)
            break;

    } // while (true)
    LOGMSG((jitInfo, LL_INFO100, "\n"));

    LOGMSG((jitInfo, LL_INFO100, "interiorGC bitvector len=%d bits: ",interiorGC_len));
    numbytes = (interiorGC_len+7)/8;
    byteNumber = 1;
    while (true)
    {
        char* buf = (char*) &(interiorGC[byteNumber-1]);
        unsigned char bits = *buf;
        for (i=1; i <= 8; i++) {
            if ((byteNumber-1)*8+i > interiorGC_len)
                break;
            LOGMSG((jitInfo, LL_INFO100, "%1d ", (int) (bits & 1)));
            bits = bits >> 1;
        }
        if ((byteNumber++ * 8) > interiorGC_len)
            break;

    } // while (true)
    LOGMSG((jitInfo, LL_INFO100, "\n"));

    LOGMSG((jitInfo, LL_INFO100, "Pinned LocalsGCRef bit vector: len=%d bits: ",pinnedGC_len));
    numbytes = (pinnedGC_len+7)/8;
    byteNumber = 1;
    while (true)
    {
        char* buf = (char*) &(pinnedGC[byteNumber-1]);
        unsigned char bits = *buf;
        for (i=1; i <= 8; i++) {
            if ((byteNumber-1)*8+i > pinnedGC_len)
                break;
            LOGMSG((jitInfo, LL_INFO100, "%1d ", (int) (bits & 1)));
            bits = bits >> 1;
        }
        if ((byteNumber++ * 8) > pinnedGC_len)
            break;

    } // while (true)
    LOGMSG((jitInfo, LL_INFO100, "\n"));

    LOGMSG((jitInfo, LL_INFO100, "Pinned interiorGC bit vector len =%d bits: ",pinnedInteriorGC_len));
    numbytes = (pinnedInteriorGC_len+7)/8;
    byteNumber = 1;
    while (true)
    {
        char* buf = (char*) &(pinnedInteriorGC[byteNumber-1]);
        unsigned char bits = *buf;
        for (i=1; i <= 8; i++) {
            if ((byteNumber-1)*8+i > pinnedInteriorGC_len)
                break;
            LOGMSG((jitInfo, LL_INFO100, "%1d ", (int) (bits & 1)));
            bits = bits >> 1;
        }
        if ((byteNumber++ * 8) > pinnedInteriorGC_len)
            break;

    } // while (true)
    LOGMSG((jitInfo, LL_INFO100, "\n"));


    LOGMSG((jitInfo, LL_INFO100, "Args: (Length = %#x, Frame size = %#x)\n",args_len,argsFrameSize));
    for (i=0; i< args_len; i++)
    {
        if (argsMap[i].type.isPrimitive())
        {
            LOGMSG((jitInfo, LL_INFO100, "    offset: -%#x type: %s\n",-argsMap[i].offset, typeName[argsMap[i].type.enum_()]));
        }
        else
        {
            LOGMSG((jitInfo, LL_INFO100, "    offset: -%#x type: valueclass\n",-argsMap[i].offset));
        }
    }

    stacks.PrintStacks(mapping);
}
#endif // _DEBUG

/************************************************************************************/
/* emits code that will take a stack (..., dest, src) and copy a value class
   at src to dest, pops 'src' and 'dest' and set the stack to (...).  returns
   the number of bytes that 'valClass' takes on the stack */

unsigned  FJit::emit_valClassCopy(CORINFO_CLASS_HANDLE valClass, bool unaligned)
{
    unsigned int numBytes = typeSizeInBytes(jitInfo, valClass);
    unsigned int numWords = (numBytes + sizeof(void*)-1) / sizeof(void*);

    if (numBytes < sizeof(void*)) {
        switch(numBytes) {
            case 1:
                emit_LDIND_I1(false);
                emit_STIND_I1(false);
                return SIZE_STACK_SLOT;
            case 2:
                emit_LDIND_I2(unaligned);
                emit_STIND_I2(unaligned);
                return SIZE_STACK_SLOT;
            case 3:
                break;
            case 4:
                emit_LDIND_I4(unaligned);
                emit_STIND_I4(unaligned);
                return SIZE_STACK_SLOT;
            default:
                _ASSERTE(!"Invalid numBytes");
                RaiseException(SEH_JIT_REFUSED,EXCEPTION_NONCONTINUABLE,0,NULL);
        }
    }

    localsGCRef_len = numWords;
    if (localsGCRef_size < numWords)
        localsGCRef_size = FJit::growBuffer(&localsGCRef, localsGCRef_len, numWords);
    int compressedSize;
    if (valClass == RefAnyClassHandle) {
        compressedSize = 1;
        *localsGCRef = 0;     // No write barrier needed (since BYREFS always on stack)
    }
    else {
        jitInfo->getClassGClayout(valClass, localsGCRef);
        compressedSize = FJit_Encode::compressBooleans(localsGCRef,localsGCRef_len);
    }

    emit_copy_bytes(numBytes,compressedSize,localsGCRef, unaligned);


    return(BYTE_ALIGNED((numWords*sizeof(void*))));
}

/************************************************************************************/
/* emits code that given a stack (..., valClassValue, destPtr), copies 'valClassValue'
   to 'destPtr'.  Leaves the stack (...),  */

void FJit::emit_valClassStore(CORINFO_CLASS_HANDLE valClass, bool unaligned)
{
    // Calculate the precise offset of the valuetype within a word for small valuetypes on big endian machines
    unsigned int EndCorr = bigEndianOffset( typeSizeInBytes(jitInfo, valClass) );
    emit_getSP(SIZE_STACK_SLOT + STACK_BUFFER + EndCorr);              // push SP+4, which points at valClassValue
    unsigned argBytes = emit_valClassCopy(valClass, unaligned);        // copy dest from SP+4;
    // emit_valClassCopy pops off destPtr, now we need to pop the valClass
    emit_drop(argBytes);
}

/************************************************************************************/
/* emits code that takes a stack (... srcPtr) and replaces it with a copy of the
   value class at 'src' (... valClassVal) */

void FJit::emit_valClassLoad(CORINFO_CLASS_HANDLE valClass, bool unaligned)
{
    // Pop the src pointer of the stack and store in in ARG_1
    enregisterTOS;
    mov_register( ARG_1, TOS_REG_1 );
    inRegTOS = false;
    unsigned int StructSize =  WORD_ALIGNED(typeSizeInSlots(jitInfo, valClass))*sizeof(void*);
    // Allocate a the space
    emit_grow( StructSize  );
    // Push the dst pointer onto the stack
    unsigned int EndCorr = bigEndianOffset( typeSizeInBytes(jitInfo, valClass) );
    emit_getSP( STACK_BUFFER + EndCorr );   // push SP+x, which points at the allocated space
    deregisterTOS;
    mov_register( TOS_REG_1, ARG_1 );
    inRegTOS = true;
    deregisterTOS;
    emit_valClassCopy(valClass, unaligned);         // copy into the allicated space from 'src'
}

/************************************************************************************/
/* emits code that takes a stack (..., ptr, valclass).  and produces (..., ptr, valclass, ptr), */

void FJit::emit_copyPtrAroundValClass(CORINFO_CLASS_HANDLE valClass)
{
    emit_getSP((WORD_ALIGNED(typeSizeInSlots(jitInfo, valClass))*sizeof(void*) + STACK_BUFFER));
    emit_LDIND_PTR(false);
}

/************************************************************************************/
/* emits code that takes a stack (..., objPtr, arg0, arg1, ..., argN-1) and
   produces (..., objPtr, arg0, arg1, ..., argN-1, objPtr) */
void FJit::emit_copyPtrAroundArgs(CORINFO_SIG_INFO & sigInfo)
{
    unsigned offset = STACK_BUFFER + getTotalArgSize(sigInfo);
    emit_getSP(offset);
    emit_LDIND_PTR(false);
}

/************************************************************************************/
/* emits code that takes a stack (..., ptr1, arg0, arg1, ..., argN-1, ptr2) and
   produces (..., ptr2, arg0, arg1, ..., argN-1) */
void FJit::emit_copyPtrDownAroundArgs(CORINFO_SIG_INFO & sigInfo)
{
    unsigned offset = STACK_BUFFER + getTotalArgSize(sigInfo) + BYTE_ALIGNED(sizeof(void*));
    deregisterTOS;
    emit_mov_arg_stack(offset,STACK_BUFFER,sizeof(void*));
    emit_POP_PTR();
}

void FJit::copyPtrUpAroundArgs(CORINFO_SIG_INFO & targetSigInfo)
{
    if(targetSigInfo.numArgs > 0)
    {
        // need to copy around args
        // {... , ptr, args} -> {..., ptr, args, ptr }
        emit_copyPtrAroundArgs(targetSigInfo);
        pushOp(topOp(targetSigInfo.numArgs));
    }
}

void FJit::copyPtrDownAroundArgs(CORINFO_SIG_INFO & targetSigInfo)
{
    if(targetSigInfo.numArgs > 0)
    {
        // need to copy back down
        // {..., old, args, ptr } -> {... , ptr, args}
       emit_copyPtrDownAroundArgs(targetSigInfo);

       // have to change type information downwards and pop off top arg
       topOp(targetSigInfo.numArgs + 1) = topOp(0);
       popOp();
    }
}

unsigned FJit::getTotalArgSize(CORINFO_SIG_INFO & sigInfo)
{
    if(sigInfo.numArgs == 0) return 0;

    unsigned argsSize = 0, enregisteredSize = 0;
    unsigned thisOffset = sigInfo.hasThis() ? 1 : 0;
    unsigned numArgs = sigInfo.numArgs + thisOffset;

    argInfo * argsInfo;
    New(argsInfo, argInfo[numArgs]);
    computeArgInfo(&sigInfo, argsInfo, 0, enregisteredSize);

    for(unsigned iArg = thisOffset; iArg < numArgs; ++iArg)
    {
        argsSize += BYTE_ALIGNED(argsInfo[iArg].size);
    }
    delete [] argsInfo;
    return argsSize;
}


/**************************************************************************************************
 * This function figures saves the arguments that were passed in registers onto the stack.
 * The exact destination on the stack is platform dependent.
 **************************************************************************************************/
void FJit::storeEnregisteredArguments()
{
  unsigned size_total_arg = 0;
  unsigned float_reg_num  = 0;
  unsigned gp_reg_num     = 0;
  unsigned arg_size       = sizeof(void *);

  if ( false ON_SPARC_ONLY(|| methodInfo->args.isVarArg()) )
    for ( gp_reg_num = 0; gp_reg_num < MAX_GP_ARG_REGISTER; arg_size = sizeof(void *) )
      {  store_gp_arg( arg_size, gp_reg_num, size_total_arg ); }

  // The arguments for vararg methods are passed on the stack on PPC and have already been saved on Sparc
  if ( methodInfo->args.isVarArg() )
    return;

  if ( methodInfo->args.hasRetBuffArg() && (!methodInfo->args.hasThis() || ReturnBufferFirst) && !PushEnregArgs &&
       EnregReturnBuffer )
     store_gp_arg( arg_size, gp_reg_num, size_total_arg );


  for (unsigned i = 0; i < args_len; i++)
    {
      arg_size = typeSizeFromJitType((unsigned)argsMap[i].type.enum_(), false);
      if ( arg_size == 0 )
        arg_size = !PASS_VALUETYPE_BYREF ? typeSizeInSlots(jitInfo, argsMap[i].type.cls())*sizeof(void *): sizeof(void *);

      if ( argsMap[i].isReg )
      {
    //INDEBUG(printf("Argument %d is enregistered, Type %d Size %d GR %d FR %d\n", i, argsMap[i].type.enum_(), arg_size, gp_reg_num, float_reg_num);)
        if ( floatEnregisterMap[argsMap[i].type.enum_()] )
          store_float_arg( arg_size, float_reg_num, gp_reg_num, size_total_arg )
        else
          store_gp_arg( arg_size, gp_reg_num, size_total_arg );

          arg_size = sizeof(void *);
          if ( methodInfo->args.hasRetBuffArg() && methodInfo->args.hasThis() && size_total_arg == sizeof(void *) &&
               !ReturnBufferFirst && EnregReturnBuffer )
            store_gp_arg( arg_size, gp_reg_num, size_total_arg );
      }
      else
        size_total_arg += arg_size;
    }
}

/**************************************************************************************************
 * This function figures restores the arguments from the stack into the registers.
 * The exact destination on the stack is platform dependent.
 **************************************************************************************************/
void FJit::restoreEnregisteredArguments()
{
  unsigned size_total_arg = 0;
  unsigned float_reg_num  = 0;
  unsigned gp_reg_num     = 0;
  unsigned arg_size       = sizeof(void *);

  if ( false ON_SPARC_ONLY(|| methodInfo->args.isVarArg()) )
    for ( gp_reg_num = 0; gp_reg_num < MAX_GP_ARG_REGISTER; arg_size = sizeof(void *) )
      {  restore_gp_arg( arg_size, gp_reg_num, size_total_arg ); }

  // The arguments for vararg methods are passed on the stack on PPC and have already been saved on Sparc
  if ( PARAMETER_SPACE && methodInfo->args.isVarArg() )
    return;

  if ( methodInfo->args.hasRetBuffArg() && (!methodInfo->args.hasThis() || ReturnBufferFirst) && !PushEnregArgs &&
       EnregReturnBuffer )
     restore_gp_arg( arg_size, gp_reg_num, size_total_arg );


  for (unsigned i = 0; i < args_len; i++)
    {
      arg_size = typeSizeFromJitType((unsigned)argsMap[i].type.enum_(), false);
      if ( arg_size == 0 )
        arg_size = !PASS_VALUETYPE_BYREF ? typeSizeInSlots(jitInfo, argsMap[i].type.cls())*sizeof(void *): sizeof(void *);

      if ( argsMap[i].isReg )
      {
    //INDEBUG(printf("Argument %d is enregistered, Type %d Size %d GR %d FR %d\n", i, argsMap[i].type.enum_(), arg_size, gp_reg_num, float_reg_num);)
        if ( floatEnregisterMap[argsMap[i].type.enum_()] )
          restore_float_arg( arg_size, float_reg_num, gp_reg_num, size_total_arg )
        else
          restore_gp_arg( arg_size, gp_reg_num, size_total_arg );

          arg_size = sizeof(void *);
          if ( methodInfo->args.hasRetBuffArg() && methodInfo->args.hasThis() && size_total_arg == sizeof(void *) &&
               !ReturnBufferFirst && EnregReturnBuffer )
            restore_gp_arg( arg_size, gp_reg_num, size_total_arg );
      }
      else
        size_total_arg += arg_size;
    }
}

void FJit::alignArguments()
{
    int alignedDst = -((int)(localsFrameSize+sizeof(prolog_data)));
    for (unsigned i = 0; i < args_len; i++)
    {
      if (ALIGN_ARGS && (argsMap[i].type.enum_() == typeR8 || argsMap[i].type.enum_() == typeI8) &&
                        (argsMap[i].offset % SIZE_STACK_SLOT))
      {
    //INDEBUG(printf("Argument %d is unaligned, Type %d Size %d Src %d Dst %d\n", i, argsMap[i].type.enum_(), arg_size,argsMap[i].offset, alignedDst);)
      move_unaligned_arg( argsMap[i].offset, alignedDst );
          argsMap[i].offset = alignedDst;
          alignedDst += SIZE_STACK_SLOT;
      }
    }
}


/**************************************************************************************************
 * This function figures out the size of the current stack frame and the size of the stack frame of
 * the target function. It also generates the flags instructing the EE about which general purpose
 * registers are saved in the function prolog so that the saved values do not get overwritten during
 * the tail call. Returns bool indicating explicitly whether tailcall is allowed
 * or not.
 **************************************************************************************************/
bool FJit::setupForTailcall(CORINFO_SIG_INFO & CallerSigInfo, CORINFO_SIG_INFO & TargetSigInfo,
                            int & stackSizeCaller, int & stackSizeTarget, int & flags )
{

    unsigned targetArgs     = TargetSigInfo.totalILArgs();
    unsigned callerArgs     = methodInfo->args.totalILArgs();

    argInfo * targetArgInfo,  *callerArgInfo;

    New(targetArgInfo, argInfo[targetArgs]);
    New(callerArgInfo, argInfo[callerArgs]);

    // Figure out the size of the arguments of the current function
    unsigned int CallerStackArgSize, EnregisteredCaller = 0;
    CallerStackArgSize = computeArgInfo(&(CallerSigInfo), callerArgInfo, 0, EnregisteredCaller);
    if ( !PushEnregArgs && !PARAMETER_SPACE )
       EnregisteredCaller = 0;

    stackSizeCaller = (CallerStackArgSize + EnregisteredCaller)/sizeof(void*);

    // Figure out the size of the arguments of the target function
    unsigned int targetCallStackSize, EnregisteredTarget = 0;
    targetCallStackSize = computeArgInfo(&(TargetSigInfo), targetArgInfo, 0, EnregisteredTarget);
    if ( !PushEnregArgs && !PARAMETER_SPACE )
       EnregisteredTarget = 0;

    stackSizeTarget = (targetCallStackSize + EnregisteredTarget)/sizeof(void*);

    //Set flags indicating which registers have been saved in the prolog of the current function
    flags = 0;

    bool retval = true;

    if(PASS_VALUETYPE_BYREF)
    {
        bool targetHasValType = false;
        bool sameStack = callerArgs == targetArgs;

        // if target has value type, caller and the target must have
        // compatible signatures.

        for(unsigned iArg = 0;
            iArg < targetArgs && iArg < callerArgs && (sameStack || !targetHasValType);
            ++iArg)
        {
            if(targetArgInfo[iArg].type.isValClass())
            {
                targetHasValType = true;
                if( !callerArgInfo[iArg].type.isValClass() ||
                    callerArgInfo[iArg].size != targetArgInfo[iArg].size)
                {
                    sameStack = false;
                }
            }
            else if(targetArgInfo[iArg].size != callerArgInfo[iArg].size)
            {
                sameStack = false;
            }
        }
        retval = targetHasValType ? sameStack : true;
    }

    delete [] targetArgInfo;
    delete [] callerArgInfo;
    return(retval);
}

void FJit::resetState( bool clearStacks )
{
  // Reset the IL stack
  resetOpStack();
  // Reset the offset split stack
  SplitOffsets.reset();
  // reset the state array
  resetStateArray();

  resetContextState();
  // local variables are not cleared
  // mapping and fixup table are not reset
  // the IL stacks for merges are not reset
}

//
// reportDebuggingData is called to pass IL to native mappings and IL
// variable to native variable mappings to the Runtime. This
// information is then passed to the debugger and used to debug the
// jitted code.
//
// NOTE: we currently assume the following:
//
// 1) the FJIT maintains a full mapping of every IL offset to the
// native code associated with each IL instruction. Thus, its not
// necessary to query the Debugger for specific IL offsets to track.
//
// 2) the FJIT keeps all arguments and variables in a single home over
// the life of a method. This means that we don't have to query the
// Debugger for specific variable lifetimes to track.
//
void FJit::reportDebuggingData(CORINFO_SIG_INFO* sigInfo)
{
    // Figure out the start and end offsets of the body of the method
    // (exclude prolog and epilog.)
    unsigned int bodyStart = mapInfo.prologSize;
    unsigned int bodyEnd = (unsigned int)(mapInfo.methodSize - mapInfo.epilogSize);

    bool DumpMap = false;
#if defined(_DEBUG)
    // Display the map for the generated code
    static ConfigMethodSet fJitDumpMap;
    fJitDumpMap.ensureInit(L"JitDumpMap");
    if (fJitDumpMap.contains(szDebugMethodName, szDebugClassName, PCCOR_SIGNATURE(methodInfo->args.sig)))
    {
        printf("Map dump of '%s::%s' \n", szDebugClassName, szDebugMethodName);
        DumpMap = true;
    }
#endif

    // Report the IL->native offset mapping first.
    mapping->reportDebuggingData(jitInfo,
                                 methodInfo->ftn,
                                 bodyStart,
                                 bodyEnd,
                                 &ver_stacks,
                                 DumpMap );


    // Next report any arguments and locals.
    unsigned int varCount = args_len + methodInfo->locals.numArgs;

    if (sigInfo->isVarArg())
    {
        varCount++;
    }

    if (varCount > 0)
    {
      // Use the allocate method provided by the debugging interface...
      ICorDebugInfo::NativeVarInfo *varMapping =
            (ICorDebugInfo::NativeVarInfo*) jitInfo->allocateArray(
                                            varCount * sizeof(varMapping[0]));
      if ( varMapping != NULL )
      {
        unsigned int varIndex = 0;
        unsigned int varNumber = 0;
        unsigned argIndex = 0;

        // Args always come first, slots 0..n
        if (sigInfo->isVarArg())
        {
            //  this ptr is first
            if (sigInfo->hasThis())
            {
                varMapping[varIndex].loc.vlType = ICorDebugInfo::VLT_STK;
                varMapping[varIndex].loc.vlStk.vlsBaseReg = ICorDebugInfo::REGNUM_FP;
                varMapping[varIndex].loc.vlStk.vlsOffset = argsMap[argIndex].offset;
                varMapping[varIndex].startOffset = bodyStart;
                varMapping[varIndex].endOffset = bodyEnd;
                varMapping[varIndex].varNumber = varNumber;
                varIndex++;
                varNumber++;
                argIndex++;
            }
            // next report varArg cookie
            varMapping[varIndex].loc.vlType = ICorDebugInfo::VLT_STK;
            varMapping[varIndex].loc.vlStk.vlsBaseReg = ICorDebugInfo::REGNUM_FP;
            varMapping[varIndex].loc.vlStk.vlsOffset = offsetVarArgToken;
            varMapping[varIndex].startOffset = bodyStart;
            varMapping[varIndex].endOffset = bodyEnd;
            varMapping[varIndex].varNumber = (unsigned)ICorDebugInfo::VARARGS_HND_ILNUM;
            varIndex++;
            // varNumber NOT incremented
        }

        if (sigInfo->isVarArg() && !PARAMETER_SPACE )
        {
            // next report all fixed varargs with offsets relative to base of fixed args
            for ( ; argIndex < args_len ; varIndex++, argIndex++,varNumber++)
            {
                varMapping[varIndex].startOffset = bodyStart;
                varMapping[varIndex].endOffset = bodyEnd;
                varMapping[varIndex].varNumber = varNumber;

                varMapping[varIndex].loc.vlType = ICorDebugInfo::VLT_FIXED_VA;
                varMapping[varIndex].loc.vlFixedVarArg.vlfvOffset =
                                        - argsMap[argIndex].offset;
            }
    }
        else
        {
            for (;  argIndex < args_len; argIndex++, varIndex++, varNumber++)
            {
                // We track arguments across the entire method, including
                // the prolog and epilog.
                varMapping[varIndex].startOffset = bodyStart;
                varMapping[varIndex].endOffset = bodyEnd;
                varMapping[varIndex].varNumber = varNumber;

                varMapping[varIndex].loc.vlType = ICorDebugInfo::VLT_STK;
                varMapping[varIndex].loc.vlStk.vlsBaseReg = ICorDebugInfo::REGNUM_FP;
                varMapping[varIndex].loc.vlStk.vlsOffset = argsMap[argIndex].offset;
            }
        }
        // Locals next, slots n+1..m
        for (unsigned i = 0; i < methodInfo->locals.numArgs; i++, varIndex++, varNumber++)
        {
            // Only track locals over the body of the method (i.e., no
            // prolog or epilog.)
            varMapping[varIndex].startOffset = bodyStart;
            varMapping[varIndex].endOffset = bodyEnd;
            varMapping[varIndex].varNumber = varNumber;

            // Locals are all EBP relative.
            varMapping[varIndex].loc.vlType = ICorDebugInfo::VLT_STK;
            varMapping[varIndex].loc.vlStk.vlsBaseReg = ICorDebugInfo::REGNUM_FP;
            varMapping[varIndex].loc.vlStk.vlsOffset = localsMap[i].offset;
        }

        _ASSERTE(varCount == varIndex);

        // Pass the array to the debugger...
        jitInfo->setVars(methodInfo->ftn, varIndex, varMapping);
      }
    }
}

/************************************************************************************/
inline void getSequencePoints( ICorJitInfo* jitInfo,
                               CORINFO_METHOD_HANDLE methodHandle,
                               ULONG32 *cILOffsets,
                               DWORD **pILOffsets,
                               ICorDebugInfo::BoundaryTypes *implicitBoundaries)
{
    jitInfo->getBoundaries(methodHandle, cILOffsets, pILOffsets, implicitBoundaries);
}

inline void cleanupSequencePoints(ICorJitInfo* jitInfo, DWORD * pILOffsets)
{
    jitInfo->freeArray(pILOffsets);
}

inline bool searchSequencePoints( unsigned ilOffset, unsigned count, DWORD * seqPointArray )
{
     // Binary search the array of sequence points
     int low, mid, high;
     low = 0;
     high = count - 1;
     while (low <= high) {
           mid = (low+high)/2;
           if ( seqPointArray[mid] == ilOffset) {
             return true;
           }
           if ( seqPointArray[mid] < ilOffset ) {
            low = mid+1;
           }
           else {
            if ( mid == 0)
              return false;
            high = mid-1;
           }
     }

     if ( seqPointArray[low] == ilOffset)
            return true;

     return false;
}

void FJit::emitSequencePointPre( unsigned ilOffset, unsigned & current_count )
{
     // Check if we are currently at a sequence point
     if ( (offsetsImplicit & ICorDebugInfo::STACK_EMPTY_BOUNDARIES) && (opStack_len == 0) )
     {
            emit_sequence_point_marker();
     }
     else if ( cSequencePoints &&   /* will be 0 if no debugger is attached */
               (current_count < cSequencePoints) &&
           searchSequencePoints((unsigned)(ilOffset), cSequencePoints, sequencePointOffsets ) )
     {
            if (opStack_len == 0)         // only recognize sequence points that are at zero stack
                emit_sequence_point_marker();
            current_count++;
     }
}

void FJit::emitSequencePointPost( unsigned ilOffset, OPCODE opcode )
{
     // Check if we are currently at a sequence point
     if ( (offsetsImplicit & ICorDebugInfo::NOP_BOUNDARIES) && (opcode == CEE_NOP) )
     {
            emit_sequence_point_marker();
     }
     else if ( (offsetsImplicit & ICorDebugInfo::CALL_SITE_BOUNDARIES) &&
               (opcode == CEE_CALL || opcode == CEE_CALLI || opcode == CEE_CALLVIRT || opcode == CEE_JMP || opcode == CEE_NEWOBJ) )
     {
            emit_sequence_point_marker();
     }
}

/********************************************************************************
 * Determine the EH nesting level at ilOffset. Just walk the EH table
 * and find out how many handlers enclose it. The lowest nesting level = 1.
********************************************************************************/
unsigned int FJit::Compute_EH_NestingLevel(unsigned ilOffset)
{
    DWORD nestingLevel = 1;
    CORINFO_EH_CLAUSE clause;
    unsigned exceptionCount = methodInfo->EHcount;
    _ASSERTE(exceptionCount > 0);
    // A short cut for functions with only one exception clause
    if (exceptionCount == 1)
    {
#ifdef _DEBUG
        jitInfo->getEHinfo(methodInfo->ftn, 0, &clause);
        _ASSERTE((clause.Flags & CORINFO_EH_CLAUSE_FILTER) ?
                    ilOffset == clause.FilterOffset || ilOffset == clause.HandlerOffset :
                    ilOffset == clause.HandlerOffset);

#endif
        return nestingLevel;
    }
    for (unsigned except = 0; except < exceptionCount; except++)
    {
        jitInfo->getEHinfo(methodInfo->ftn, except, &clause);
        if (ilOffset > clause.HandlerOffset && ilOffset < clause.HandlerOffset+clause.HandlerLength)
            nestingLevel++;
    }

    return nestingLevel;
}

/*******************************************************************
 * This function looks for enclosing exception handling clause given the
 * IL offset of the next instruction. This function depends on the fact
 * that shared handlers are not allowed
 *******************************************************************/
void FJit::getEnclosingClause( unsigned nextIP, CORINFO_EH_CLAUSE * retClause, int NotTry, unsigned & Start, unsigned & End )
{
   _ASSERTE( retClause != NULL );
   // Clear the current clause
   makeClauseEmpty(retClause);
   // Initalize some local variable for ease of access
   CORINFO_EH_CLAUSE       clause;
   // Reset the loop variable and the return value
   Start = 0, End = methodInfo->ILCodeSize;
   for (unsigned except = 0; except < methodInfo->EHcount; except++)
   {
     jitInfo->getEHinfo(methodInfo->ftn, except, &clause);

     // Check if the IL offset is inside the try
     if (clause.TryOffset < nextIP &&  nextIP <= clause.TryOffset+clause.TryLength && !NotTry)
       // Check if this try block is enclosed in the last block we found
       if ( ( clause.TryOffset >  Start && End >= clause.TryOffset+clause.TryLength ) ||
            ( clause.TryOffset >= Start && End >  clause.TryOffset+clause.TryLength ) )
       {
         Start = clause.TryOffset; End = clause.TryOffset+clause.TryLength;
         (*retClause) = clause;
       }
     // Check if the IL offset is inside the filter
     if (( clause.Flags & CORINFO_EH_CLAUSE_FILTER ) && clause.FilterOffset < nextIP &&  nextIP <= clause.HandlerOffset )
       // Check if this try block is enclosed in the last block we found
       if ( ( clause.FilterOffset >  Start && End >= clause.HandlerOffset ) ||
            ( clause.FilterOffset >= Start && End >  clause.HandlerOffset ) )
       {
         Start = clause.FilterOffset; End = clause.HandlerOffset;
         (*retClause) = clause;
       }
     // Check if the IL offset is inside the handler
     if (clause.HandlerOffset < nextIP &&  nextIP <= clause.HandlerOffset+clause.HandlerLength )
       // Check if this try block is enclosed in the last block we found
       if ( ( clause.HandlerOffset >  Start && End >= clause.HandlerOffset+clause.HandlerLength ) ||
            ( clause.HandlerOffset >= Start && End >  clause.HandlerOffset+clause.HandlerLength ) )
       {
         Start = clause.HandlerOffset; End = clause.HandlerOffset+clause.HandlerLength;
         (*retClause) = clause;
       }
   }
}

/*******************************************************************
 * This function pushes the starting offset of handlers associated with
 * the given ilOffset onto the split stack. The highest address is pushed
 * first.
 *******************************************************************/
void FJit::pushHandlerOffsets(unsigned ilOffset)
{
    CORINFO_EH_CLAUSE clause;
    unsigned exceptionCount = methodInfo->EHcount;
    // If there is a try block there must be at least one exception clause
    _ASSERTE(exceptionCount > 0);

    unsigned *              Handlers   = new unsigned[ exceptionCount ];

    // Find all handlers associated with this try block
    for (unsigned except = 0; except < exceptionCount; except++)
      {
         jitInfo->getEHinfo(methodInfo->ftn, except, &clause);
         if ( ilOffset == clause.TryOffset )
         {
           if ( clause.Flags & CORINFO_EH_CLAUSE_FILTER )
             Handlers[ except ] = clause.FilterOffset;
           else
             Handlers[ except ] = clause.HandlerOffset;
         }
         else
           Handlers[ except ] = (unsigned)-1;
      }

    for (unsigned i = 0; i < exceptionCount; i++)
    {
      unsigned int last_handler  = (unsigned)-1;
      int handler_index          = 0;
      // Find the last handler (il offset wise) associated with the try block
      for (unsigned j = 0; j < exceptionCount; j++)
        if ( last_handler > Handlers[j] )
          { last_handler = Handlers[j]; handler_index = j; }

      // Exit if there no more handlers left
      if ( last_handler == (unsigned)-1 )
        break;
      else
      {
        SplitOffsets.pushOffset( last_handler );
        Handlers[handler_index] = (unsigned)-1;
      }
    }

    // Deallocate the array of handlers
    delete[] Handlers;
}

int FJit::findOffsetOfThisPtr(CORINFO_SIG_INFO & CalleeSigInfo)
{
    unsigned tempMapSize, i;

    tempMapSize = CalleeSigInfo.numArgs  + (CalleeSigInfo.hasThis() ? 1 : 0) +
                                           (CalleeSigInfo.hasTypeArg() ? 1 : 0);
    argInfo * tempMap = (argInfo*) _alloca(sizeof(argInfo) * (tempMapSize));

    // Figure out the size of the arguments of the current function
    unsigned int CalleeStackArgSize, EnregisteredCallee = 0;
    CalleeStackArgSize = computeArgInfo(&(CalleeSigInfo), tempMap, 0, EnregisteredCallee);

    // Compute the size of the arguments on the IL stack
    unsigned ilStackSize = CalleeStackArgSize + EnregisteredCallee;

    // The vararg cookie is not present on the IL stack
    if (PARAMETER_SPACE && CalleeSigInfo.isVarArg() )
        ilStackSize -= sizeof(void *);

    // add in a slot for R4 since R4's are stored as R8's on the IL stack and
    // add alignment pad if necessary
    for (i=0; i < tempMapSize; i++)
    {
        if ( (tempMap[i].type.isPrimitive() && tempMap[i].type.enum_() == typeR4) )
            ilStackSize += sizeof( void *);
        else if ( SIZE_STACK_SLOT != sizeof(void *) && (tempMap[i].size % SIZE_STACK_SLOT) )
            ilStackSize += sizeof( void *);
    }

    return ilStackSize;
}
/*******************************************************************
 * This function reports the verification failure in DEBUG mode, does
 * some clean up and resets the state of the JIT to indicate a verification
 * failure
 *******************************************************************/
FJitResult FJit::verificationFailure(INDEBUG( char * ErrorMessage ) )
{
    INDEBUG(printf("Verification failed: %s::%s at %x\n",szDebugClassName, szDebugMethodName, InstStart );)
    INDEBUG(printf("Ver. Cond: %s \n\n\n", ErrorMessage);)

    IsVerifiableCode = false;
    
    // Clean up the sequence points
    if(cSequencePoints > 0)
      cleanupSequencePoints(jitInfo, sequencePointOffsets);
    // Set up the FJit to indicate the verification failure
    ver_failure = 1;
    ver_failure_offset = InstStart;
    resetContextState(false);

    if (flags & CORJIT_FLG_IMPORT_ONLY)
        return FJIT_VERIFICATIONFAILED;
    else
        return jitCompileVerificationThrow();
}

/*******************************************************************
 * This function returns true if the object contains pointers that
 * may point at local stack - local variable, args, etc.
 *******************************************************************/
int FJit::verIsByRefLike( OpType Obj )
{
   // Managed pointers and typed reference
   if ( Obj.enum_() == typeByRef || Obj.enum_() == typeRefAny )
        return TRUE;
   // For classes check if any internal field contains stack pointers
   if ( Obj.enum_() == typeRef && !Obj.isNull() || Obj.enum_() == typeValClass )
    return jitInfo->getClassAttribs( Obj.cls(), methodInfo->ftn) & CORINFO_FLG_CONTAINS_STACK_PTR;

   // Integers, floats, etc - NULL objects included to match V1 .NET Framework
   return false;
}

/****************************************************************************************************
 * This function verifies the current stack against the stack stored in ver_stacks structure for offset
 * nextIP. The store_result parameter determines where and if the result of the merge is stored.
 * If store_result == 0 the result is stored in the ver_stack structure, store_result == 1 the result is
 * stored both in the current stack and in the ver_stack structure. Finally for store_result == 2 the result
 * is only stored in the current stack
 ****************************************************************************************************/
int FJit::verifyStacks( unsigned int nextIP, int store_result )
{
  //INDEBUG(printf("Verifing for %x: ", nextIP );)

  int success = 1;
  unsigned int label = ver_stacks.findLabel(nextIP);
  if (label == LABEL_NOT_FOUND) {
    // INDEBUG(if (JitVerify) printf("Added new stack length %d \n", opStack_len );)
     ver_stacks.add(nextIP, opStack, opStack_len );
  }
  else if (JitVerify) {

    if (tempOpStack_size < opStack_size) {
        if (tempOpStack) delete [] tempOpStack;
        tempOpStack_size = opStack_size;
        New(tempOpStack,OpType[tempOpStack_size]);
    }

    OpType * stack2 = tempOpStack;
    unsigned int size2 = ver_stacks.setStackFromLabel(label, stack2, opStack_size);

    //INDEBUG(if (JitVerify) printf("Verifing stack lengths %d %d StoreRes %d\n",opStack_len, size2, store_result);)
    switch ( store_result )
    {
    case 0:
          // Do a merge and store the merged stack in the JitVerify stacks table
          success = mergeStacks( size2, stack2, opStack_len, opStack, true );
          ver_stacks.add(nextIP, stack2, size2, true );
          break;
    case 1:
          // Do a merge and store the merged stack in the JitVerify stacks table and current stack
          success = mergeStacks(  opStack_len, opStack,  size2, stack2, false );
          ver_stacks.add(nextIP, opStack, opStack_len, true );
          break;
    default:
          RaiseException(SEH_JIT_REFUSED,EXCEPTION_NONCONTINUABLE,0,NULL);
    }
  }

  return success;
}

/*******************************************************************
 * This function verifies an access to the array and return the type
 * of the array element. The stack is assumed to contain an index
 * and an array pointer
 *******************************************************************/
int FJit::verifyArrayAccess( OpType & ResultType )
{
  // Check that there is an array and an index on the stack
  if ( opStack_len < 2) return FAILED_VALIDATION;
  OpType index = topOp();    // index into the array (typeInt)
  OpType array = topOp(1);   // array object (typeRef)
  // Verify that index is a valid integer type
  if ( index.enum_() != typeI ) return FAILED_VALIDATION;
  // Verify that the array object is a valid object
  if ( array.enum_() != typeRef ) return FAILED_VALIDATION;
  // Verify that we have type information for the array
  if (array.isNull() || array.cls() == NULL ) return FAILED_VERIFICATION;
  // Verify that the array object is actually an array
  if (!(jitInfo->isSDArray( array.cls()))) return FAILED_VERIFICATION;
  // Verify that the type of the array element matches the supplied type
  CORINFO_CLASS_HANDLE childClassHandle = NULL;
  CorInfoType ArrayElemType = jitInfo->getChildType( array.cls(), &childClassHandle);
  ResultType.init(ArrayElemType, childClassHandle );

  // Report success
  return SUCCESS_VERIFICATION;
}

/*******************************************************************
 * This function verifies a load of an element from the array. The
 * LoadType indicates the type of the load instruction used. The stack
 * is assumed to contain an index and an array pointer
 *******************************************************************/
int FJit::verifyArrayLoad( OpTypeEnum LoadType, OpType & ResultType )
{
  // Convert the unsigned to signed ( this done to match behavior of V1 .NET Framework )
  switch ( LoadType ) {
    case typeU1: LoadType = typeI1; break;
    case typeU2: LoadType = typeI2; break;
    default: break;
  }
  // Initialize the result type for cases when running without verification
  ResultType.init( LoadType, NULL );
  int resultAccess = verifyArrayAccess( ResultType );

  // Convert the unsigned to signed ( this done to match behavior of V1 .NET Framework )
  ResultType.toSigned();

  if ( resultAccess != SUCCESS_VERIFICATION )
  {
     // Check for null array ( we know that the array is on the stack because validation passed )
     if ( resultAccess == FAILED_VERIFICATION && topOp(1).isNull() && LoadType == typeRef )
     {
       ResultType.setTarget( typeRef, NULL );
       return SUCCESS_VERIFICATION;
     }
     return resultAccess;
  }
  else if ( ResultType.enum_() != LoadType )
    return FAILED_VERIFICATION;
  else
    return SUCCESS_VERIFICATION;
}

/*******************************************************************
 * This function verifies a store of an element into the array. The
 * StoreType indicates the type of the store instruction used. The stack
 * is assumed to contain a value to be stored, an index and an array pointer.
 *******************************************************************/
int FJit::verifyArrayStore( OpTypeEnum StoreType, OpType & ElemType )
{
  // Check that there is an array, an index and a value on the stack
  if ( opStack_len < 3) return FAILED_VALIDATION;
  OpType value = topOp();    // value to be store in the array
  POP_STACK(1);                    // remove the value off the stack
  // Initialize the result type for cases when running without verification
  ElemType.init( StoreType, NULL );
  int resultAccess = verifyArrayAccess( ElemType );
  pushOp( value );
  // If array wasn't verified/validated return error code
  if ( resultAccess != SUCCESS_VERIFICATION || !JitVerify )
     return resultAccess;
  // Convert the unsigned to signed
  ElemType.toSigned();
  switch ( StoreType ) {
    case typeU1: StoreType = typeI1; break;
    case typeU2: StoreType = typeI2; break;
    default: break;
  }
  // Check if the instruction matches the array element type
  if ( ElemType.enum_() != StoreType )
    return FAILED_VERIFICATION;
  // Check if the value on the stack matches the array element type
  ElemType.toFPNormalizedType();
  if ( !canAssign( jitInfo, methodInfo->ftn, value, ElemType ) )
    return FAILED_VERIFICATION;
  // Everything looks correct
  return SUCCESS_VERIFICATION;
}

/*******************************************************************
 * This function obtains the type of an argument from the signature of a
 * function
 *******************************************************************/
OpType FJit::getTypeFromSig( CORINFO_SIG_INFO  & sig, CORINFO_ARG_LIST_HANDLE args )
{
  CORINFO_CLASS_HANDLE cls;
  CorInfoTypeWithMod corArgType = jitInfo->getArgType( &sig, args,&cls);
  CorInfoType argType = strip(corArgType);
  // Class handle is valid only for value types
  OpType ArgType = ArgType = createOpType( argType, cls );

  // Obtain the class handle for objects and managed pointers
  if ( ArgType.enum_() == typeByRef )
  {
     cls = jitInfo->getArgClass(&sig, args);
     _ASSERTE(cls != NULL);
     CORINFO_CLASS_HANDLE childClassHandle;
     CorInfoType childType = jitInfo->getChildType(cls, &childClassHandle);
     ArgType.setTarget(childType,childClassHandle);
  } else if ( ArgType.enum_() == typeRef ) {
     cls = jitInfo->getArgClass(&sig, args);
     ArgType.setHandle(cls);
  }

  return ArgType;
}

/*******************************************************************
 * This function verifies that the arguments on the stack match the
 * signature of the function. The verification of the instance pointer
 * has be done separately because it doesn't appear in the function
 * signature
 *******************************************************************/
int FJit::verifyArguments( CORINFO_SIG_INFO & sig, int popCount, bool tailCall )
{
  // Check the number of object on the stack - has to be at least as large as the number of arguments
  unsigned int argCount = sig.numArgs;
  if ( opStack_len < (argCount + popCount)) return FAILED_VALIDATION;
  // Match the argument types to the objects on the stack
  CORINFO_ARG_LIST_HANDLE args = sig.args;

  while (argCount--)
  {
        OpType ActualType   = topOp(popCount+argCount);
        OpType DeclaredType = getTypeFromSig( sig, args );
        DeclaredType.toFPNormalizedType();
        //INDEBUG(printf( "Arg %d: [%d, %d] \n", argCount, ActualType.enum_(), ActualType.cls() );)
        if ( !canAssign( jitInfo, methodInfo->ftn, ActualType, DeclaredType ) )
        {
            return FAILED_VERIFICATION;
        }
        // check that the argument is not a byref for tailcalls
        if (tailCall && verIsByRefLike(ActualType))
            return FAILED_VERIFICATION;

        args = jitInfo->getArgNext(args);
  }

  return SUCCESS_VERIFICATION;
}

/*******************************************************************
 * This function verifies that the type this pointer matches the type of
 * an instance on the stack
 *******************************************************************/
int FJit::verifyThisPtr
( CORINFO_CLASS_HANDLE & instanceClassHnd,  CORINFO_CLASS_HANDLE targetClass, int popCount,bool tailCall)
{
   OpType thisPtr =  topOp(popCount);
   // If this pointer is an object set the instanceHandle to the class handle of the object
   if ( thisPtr.isRef() )
   {
     instanceClassHnd = !thisPtr.isNull() ? thisPtr.cls() : instanceClassHnd;
       // We can not simply check for equality because we need to check if actual object can be cast to the declared
       if (!canAssign(jitInfo, methodInfo->ftn, thisPtr, OpType(typeRef,targetClass )  )) return FAILED_VERIFICATION;
   }
   else
   {
       // Check type compatability of the this argument
       CorInfoType eeThisPtr = jitInfo->asCorInfoType(targetClass);
       OpType DeclaredThis = createOpType(eeThisPtr, targetClass);
       // If this is a managed pointer we need to obtain the target type
       if ( DeclaredThis.isByRef() )
       {
          CORINFO_CLASS_HANDLE targetHandle;
          CorInfoType childType = jitInfo->getChildType(targetClass, &targetHandle);
          DeclaredThis.setTarget(childType,targetHandle);
       }
       if  ( !canAssign(jitInfo, methodInfo->ftn, thisPtr.getTarget(), DeclaredThis ) ) return FAILED_VERIFICATION;
   }
   // For tail call check that the object contains no pointers to local stack
   if  ( tailCall && verIsByRefLike(thisPtr) ) return FAILED_VERIFICATION;

   return SUCCESS_VERIFICATION;
}

/*******************************************************************
 * This function verifies creation of a delegate
 *******************************************************************/
int FJit::verifyDelegate
( unsigned int constructorMemberRef,
  CORINFO_SIG_INFO  & sig, CORINFO_METHOD_HANDLE methodHnd, __in unsigned char* codePtr,
  unsigned DelStartDelta, int popCount )
{
  // Check if there are enough arguments on the stack
  if ( opStack_len < (sig.numArgs + popCount)) return FAILED_VALIDATION;
  // Delegate ctor has two arguments object and ftn pointer
  if ( sig.numArgs != 2 ) return FAILED_VALIDATION;

  // If verification is turned off return
  if ( !JitVerify ) return SUCCESS_VERIFICATION;

  // Obtain the arguments from the stack
  OpType ActObject = topOp(1+popCount);    // object (typeRef)
  OpType ActMethod = topOp(popCount);      // ftn pointer (typeMethod)
  // Verify the stack arguments
  if (!( ActObject.isRef() && ActMethod.isMethod() ))
      return FAILED_VERIFICATION;
  // Obtain the arguments from the function signature
  OpType DeclObject = getTypeFromSig(sig, sig.args);
  DeclObject.toFPNormalizedType();
  OpType DeclMethod = getTypeFromSig(sig, jitInfo->getArgNext(sig.args));
  DeclMethod.toFPNormalizedType();
  // Verify the signature arguments
  if (!( DeclObject.isRef() && DeclMethod.enum_() == typeI ))
      return FAILED_VERIFICATION;
  // Match the object from the stack to the object from the signature
  if (!( canAssign( jitInfo, methodInfo->ftn, ActObject, DeclObject ))) return FAILED_VERIFICATION;

  // There only two legal IL sequences for creation of a delegate
  // 1) ldfnt <token>; newobj delegate;           - length 6 bytes
  // 2) dup; ldvirtfnt <token>; newobj delegate;  - length 7 bytes
  // Check that the delta to the start of the delegate creating sequence is valid

  if ( DelStartDelta != 6 && DelStartDelta != 7 ) return FAILED_VERIFICATION;
  codePtr = &codePtr[-((int)DelStartDelta)]; // back up to the start of the sequence
  switch( DelStartDelta ) {
  case 6: if ( codePtr[0] != CEE_PREFIX1 || codePtr[1] != (CEE_LDFTN & 0xFF) ) return FAILED_VERIFICATION;
          break;
  case 7: if (codePtr[0] != CEE_DUP  || codePtr[1] != CEE_PREFIX1 || codePtr[2] != (CEE_LDVIRTFTN & 0xFF))
            return FAILED_VERIFICATION;
          break;
  }

  // Since we've verified the instruction sequence, we now know that DelegateMethodRef is the valid token <token>

  // Match the signature of the delegate to the signature of the method
  // get the method's exact parent from the cached token
  unsigned int methodParent = jitInfo->getMemberParent(methodInfo->scope, DelegateMethodRef);
  CORINFO_CLASS_HANDLE methodParentCls =
      jitInfo->findClass(methodInfo->scope,
                         methodParent,
                         methodInfo->ftn);
  // get the exact parent of the delegate constructor
  unsigned int constructorParent = jitInfo->getMemberParent(methodInfo->scope, constructorMemberRef);
  CORINFO_CLASS_HANDLE constructorParentCls =
      jitInfo->findClass(methodInfo->scope,
                         constructorParent,
                         methodInfo->ftn);

  // Match the signature of the delegate to the signature of the method
  if (!( jitInfo->isCompatibleDelegate(
                  (ActObject.isNull() ? NULL : ActObject.cls()),
                   methodParentCls,
                   ActMethod.getMethod(),
                   constructorParentCls,
                   methodInfo->scope,
                   DelegateMethodRef,   // this is the cached token of the target method
                   constructorMemberRef
                   ))) return FAILED_VERIFICATION;

  // in the case of protected methods, it is a requirement that the 'this'
  // pointer be a subclass of the current context.  Perform this check
  BOOL targetIsStatic = jitInfo->getMethodAttribs( ActMethod.getMethod(), methodInfo->ftn ) & CORINFO_FLG_STATIC;
  CORINFO_CLASS_HANDLE instanceClassHnd = jitInfo->getMethodClass(methodInfo->ftn);;
  if (!(ActObject.isNull() || targetIsStatic))
          instanceClassHnd = ActObject.cls();
  _ASSERTE( instanceClassHnd != NULL );

  // check the constraints on the target method
  if (!( jitInfo->satisfiesClassConstraints(methodParentCls)))
          return FAILED_VERIFICATION;
  if (!( jitInfo->satisfiesMethodConstraints(methodParentCls, ActMethod.getMethod())))
          return FAILED_VERIFICATION;

  if (!( jitInfo->canAccessMethod(methodInfo->ftn, methodParentCls, ActMethod.getMethod(), instanceClassHnd ) ))
          return FAILED_VERIFICATION;

  return SUCCESS_VERIFICATION;
}

/*****************************************************************************
 * The following code checks the rules for the EH table. The precise rules
 * checked are described in the body of the function. The validation should
 * be done before initializing internal EH structures to prevent security holes
 */
int FJit::verifyHandlers()
{
    CORINFO_EH_CLAUSE clause;
    unsigned exceptionCount = methodInfo->EHcount;

    /*******************************************************
     * Verify that:
     * 1) No try/filter/handler extends past beginning of the code
     * 2) No try/filter/handler has zero size
     * 3) No handler/filter is contained inside the associated try
     * 4) The exception class token is valid for typed catch clauses
     * While most of these conditions are verified again during jitting, we need to
     * do this verification here in order to be able to do more difficult checks
     *
     * This is O(n) sequence of checks
     *******************************************************/
    for (unsigned int except = 0; except < exceptionCount; except++)
    {
       jitInfo->getEHinfo(methodInfo->ftn, except, &clause);

       unsigned TryEndOffset = clause.TryOffset + clause.TryLength;
       if ( clause.TryOffset >= TryEndOffset || TryEndOffset > methodInfo->ILCodeSize )
          return FAILED_VALIDATION;
       unsigned HandlerEndOffset = clause.HandlerOffset + clause.HandlerLength;
       if ( clause.HandlerOffset >= HandlerEndOffset  || HandlerEndOffset  > methodInfo->ILCodeSize )
           return FAILED_VALIDATION;
       if ( clause.HandlerOffset >= clause.TryOffset && clause.HandlerOffset < TryEndOffset )
          return FAILED_VALIDATION;
       if ( HandlerEndOffset > clause.TryOffset && HandlerEndOffset <= TryEndOffset )
          return FAILED_VALIDATION;
       if (clause.Flags & CORINFO_EH_CLAUSE_FILTER )
       {
         if ( clause.FilterOffset > methodInfo->ILCodeSize )
            return FAILED_VALIDATION;
         if ( clause.HandlerOffset - clause.FilterOffset < 2 )
            return FAILED_VALIDATION;
         if ( clause.FilterOffset >= clause.TryOffset && clause.FilterOffset < TryEndOffset )
            return FAILED_VALIDATION;
         if ( methodInfo->ILCode[clause.HandlerOffset - 2] != CEE_PREFIX1 ||
              methodInfo->ILCode[clause.HandlerOffset - 1] != (CEE_ENDFILTER & 0xFF) )
            return FAILED_VALIDATION;
       }
       else if ( ! (clause.Flags & ( CORINFO_EH_CLAUSE_FINALLY | CORINFO_EH_CLAUSE_FAULT ) ) )
       {
         if (!jitInfo->isValidToken(methodInfo->scope, clause.ClassToken ))
            return FAILED_VALIDATION;
       }
    }

    /*****************************************************************************
    * The following code checks the following rules for the EH table:
    * 1. Overlapping of try blocks not allowed.
    * 2. Handler blocks cannot be shared between different try blocks
    * 3. Try blocks with Finally or Fault blocks cannot have other handlers.
    * 4. If block A contains block B, A should also contain B's try/filter/handler.
    * 5. Nested block must appear before containing block
    * 6. No block may appear inside or overlap a filter block
    *
    * Assumptions made:
    *   TryStartOffset < TryEndOffset; HandlerStart < HandlerEndOffset; FilterStart < HandlerStart
    *
    * This the O(n^2) sequence of checks -- can be made O(nlog(n)) by using sorting if necessary
    */

    unsigned int except1, except2;
    CORINFO_EH_CLAUSE clause1, clause2;
    for ( except1 = 0; except1 < exceptionCount; except1++)
    {
       jitInfo->getEHinfo(methodInfo->ftn, except1, &clause1);
       unsigned Try1StartOffset   = clause1.TryOffset;
       unsigned Try1EndOffset     = clause1.TryOffset + clause1.TryLength;
       unsigned Handler1Start     = clause1.HandlerOffset;
       unsigned Handler1EndOffset = clause1.HandlerOffset + clause1.HandlerLength;
       unsigned Filter1Start      = clause1.FilterOffset;
       unsigned FinallyOrFault    = clause1.Flags & ( CORINFO_EH_CLAUSE_FINALLY | CORINFO_EH_CLAUSE_FAULT );

       for ( except2 = 0; except2 < methodInfo->EHcount; except2++) if (except2 != except1 )
       {
          jitInfo->getEHinfo(methodInfo->ftn, except2, &clause2);
          unsigned Try2StartOffset   = clause2.TryOffset;
          unsigned Try2EndOffset     = clause2.TryOffset + clause2.TryLength;
          unsigned Handler2Start     = clause2.HandlerOffset;
          unsigned Handler2EndOffset = clause2.HandlerOffset + clause2.HandlerLength;
          unsigned Filter2Start      = clause2.FilterOffset;
          // Check for overlapping of try blocks
          if ( Try1StartOffset < Try2StartOffset && Try1EndOffset > Try2StartOffset && Try1EndOffset < Try2EndOffset )
             return FAILED_VALIDATION;
          // Check for overlapping of handler/try/filter with a filter
          if ( (clause2.Flags & CORINFO_EH_CLAUSE_FILTER) &&
               ( (Try1StartOffset < Filter2Start &&  Try1StartOffset > Handler2Start) ||
                 (Try1EndOffset   < Filter2Start &&  Try1EndOffset   > Handler2Start) ||
                 (Handler1Start   < Filter2Start &&  Handler1Start   > Handler2Start) ||
                 (Handler1EndOffset < Filter2Start &&  Handler1EndOffset > Handler2Start) ||
                 ((clause1.Flags & CORINFO_EH_CLAUSE_FILTER)&&Filter1Start < Filter2Start && Handler1Start> Handler2Start)))
             return FAILED_VALIDATION;
          // Check for overlapping of the handlers
          if (  Handler1Start < Handler2Start && Handler1EndOffset > Handler2Start && Handler1EndOffset < Handler2EndOffset )
             return FAILED_VALIDATION;
          // Check for extra handlers for trys that have a fault or a finally handler
          if ( FinallyOrFault && Try1StartOffset == Try2StartOffset && Try1EndOffset == Try2EndOffset )
             return FAILED_VALIDATION;
          // Check for shared handlers
          if ( Handler1Start == Handler2Start && Handler1EndOffset == Handler2EndOffset )
             return FAILED_VALIDATION;
          // Check if one try is wholly contained in the other try
          if ( (Try1StartOffset >= Try2StartOffset && Try1EndOffset <  Try2EndOffset) ||
               (Try1StartOffset >  Try2StartOffset && Try1EndOffset <= Try2EndOffset) )
          {
            // Check if enclosing try block preceeds the nested try block
            if ( except2 < except1 )
               return FAILED_VALIDATION;
            // In this case check that filter/handler are inside as well
            if ( Handler1Start < Try2StartOffset || Handler1EndOffset >  Try2EndOffset ||
                 ((clause1.Flags & CORINFO_EH_CLAUSE_FILTER) && clause1.FilterOffset < Try2StartOffset ) )
               return FAILED_VALIDATION;
          }
          // Check if one try is wholly contained in the other's handler
          // We can use a simpler check here since we already checked for shared handlers
          else if ( Try1StartOffset >= Handler2Start  && Try1EndOffset <=  Handler2EndOffset )
          {
            // Check if enclosing try block preceeds the nested try block
            if ( except2 < except1 )
              return FAILED_VALIDATION;
            // In this case check that filter/handler are inside as well
            if ( Handler1Start < Handler2Start || Handler1EndOffset > Handler2EndOffset  ||
                 ((clause1.Flags & CORINFO_EH_CLAUSE_FILTER) && clause1.FilterOffset < Handler2Start ) )
              return FAILED_VALIDATION;
          }
          // Check if the handler is fully enclosed in either try or handler block
          else if ( ( Handler1Start >= Handler2Start  && Handler1EndOffset <  Handler2EndOffset ) ||
                    ( Handler1Start >  Handler2Start  && Handler1EndOffset <= Handler2EndOffset ) ||
                    ( Handler1Start >= Try2StartOffset && Handler1EndOffset <  Try2EndOffset) ||
                    ( Handler1Start >  Try2StartOffset && Handler1EndOffset <= Try2EndOffset) )
              return FAILED_VALIDATION;
       }
    }

    return SUCCESS_VERIFICATION;
}

/*****************************************************************************
 * The following code jits a function that throws a verification exception. This
 * function is jitted instead of a function that failed verification.
 *****************************************************************************/
FJitResult FJit::jitCompileVerificationThrow()
{
    outBuff = codeBuffer;
    outPtr  = outBuff;
    *(entryAddress) = outPtr;
    inRegTOS = false;

    // Emit prolog
    unsigned int localWords = (localsFrameSize+sizeof(void*)-1)/ sizeof(void*);
    emit_prolog(localWords);

    mapInfo.prologSize = outPtr-outBuff;

    // Beginning of function code
    mapping->add(0,(unsigned)(outPtr - outBuff));

    // Jit a verification throw
    emit_verification_throw(ver_failure_offset);

    // End of the function code
    mapping->add(1, (unsigned)(outPtr-outBuff));

    // Generate the epilog
    if (!CALLER_CLEANS_STACK)
    // Callee pops args for varargs functions
      { emit_return(methodInfo->args.isVarArg() ? 0 : argsFrameSize, mapInfo.hasRetBuff ); }
    else                                         // If __cdecl calling convention is used the caller is responsible
       emit_return(0, mapInfo.hasRetBuff);       // for clearing the arguments from the stack


    // Fill in the intermediate IL offsets (in the body of the opcode)
    mapping->fillIn();

    mapInfo.methodSize = outPtr-outBuff;
    mapInfo.epilogSize = (outPtr - outBuff) - mapping->pcFromIL(1);

    //Set total size of the function
    *(codeSize) = outPtr - outBuff;

    return FJIT_VERIFICATIONFAILED;
}


/* rearrange the stack & regs to match the calling convention for the chip,
   return the amount of stack space that the NATIVE call takes up.   (That is
   the amount the ESP needs to be moved after the call is made.  For the default
   convention this number is not needed as it is the callee's responsiblity to
   make this adjustment, but for varargs, the caller needs to do it).  */

unsigned FJit::buildCall(CORINFO_SIG_INFO* sigInfo,
                         BuildCallFlags flags,
                         unsigned int & stackPadorRetBase,
                         bool isTailCall)
{
    // Verify that the calling convention is one which is known to the JIT
    _ASSERTE((sigInfo->callConv & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_DEFAULT ||
             (sigInfo->callConv & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_STDCALL ||
             (sigInfo->callConv & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_C ||
             (sigInfo->callConv & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_THISCALL ||
             (sigInfo->callConv & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_FASTCALL ||
             (sigInfo->callConv & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_VARARG);

    // Calculate the number of arguments
    unsigned int argCount = sigInfo->numArgs;
    unsigned int tailArgs = 0;
    if (sigInfo->hasThis()) argCount++;
    if (sigInfo->hasTypeArg()) argCount++;
    //INDEBUG(printf(" Total Args %d, HasThis %d, HasTypeArg %d, HasRetBuff %d \n",  argCount,  sigInfo->hasThis(),
    //      sigInfo->hasTypeArg(), sigInfo->hasRetBuffArg() );)

    unsigned retValBuffWords          = 0;
    unsigned retValBuffWordsUnaligned = 0;
    unsigned retValBuffOnStack        = 0;
    unsigned enregisteredSize         = 0;
    unsigned enregisteredPad          = 0;
    unsigned ilStackSize              = 0;
    unsigned ilStackPad               = 0;
    unsigned nativeStackSize          = 0;
    unsigned varArgCookieOffset       = 0;
    stackPadorRetBase = 0;

    // calculate the size of the return value buffer if necessary
    if (sigInfo->hasRetBuffArg())
    {
         retValBuffWordsUnaligned = typeSizeInSlots(jitInfo, sigInfo->retTypeClass);
         retValBuffWords          = WORD_ALIGNED(retValBuffWordsUnaligned);
         retValBuffOnStack        = retValBuffWords*sizeof(void*);
         // INDEBUG(printf(" RetBuffWords  %d, RetBuffStack %d \n", retValBuffWords, retValBuffOnStack );)
    }
    // Pop off the arguments
    popOp(sigInfo->totalILArgs());

    // For vararg function on platfroms for variable number of registers is used depending on the signature
    // compute the offset of the vararg cookie in the parameter space
    if (PARAMETER_SPACE && sigInfo->isVarArg() )
    {
         varArgCookieOffset  = ((retValBuffWords&&EnregReturnBuffer? 1 : 0) + (sigInfo->hasThis()? 1 : 0))*sizeof(void *);
         varArgCookieOffset += sizeof(prolog_frame);
    }

    /*  we now have the correct number of arguments
        Note:when we finish, we must have forced TOS out of the inRegTOS register, i.e.
        we either moved it to an arg reg or we did a deregisterTOS
        */
    if (argCount != 0 || retValBuffWords != 0)
    {
       // Generate a map which describes where each IL argument should end up
       argInfo* argsInfo = (argInfo*) _alloca(sizeof(argInfo) * (argCount+1)); // +1 for possible thisLast swap
       nativeStackSize = computeArgInfo(sigInfo, argsInfo, 0, enregisteredSize);
       enregisteredPad = FIXED_ENREG_BUFFER ? (FIXED_ENREG_BUFFER - enregisteredSize) : 0;

       // Run some debug checks to verify the generated map
       _ASSERTE( !FIXED_ENREG_BUFFER || (FIXED_ENREG_BUFFER == (sizeof(void *)*MAX_GP_ARG_REGISTER) ) );
       _ASSERTE( enregisteredSize <= (sizeof(void *)*MAX_GP_ARG_REGISTER) );
       _ASSERTE( !retValBuffWords || enregisteredSize > 0 || !EnregReturnBuffer );
       _ASSERTE( !STACK_BUFFER || !PushEnregArgs );

       // INDEBUG(printf(" NativeStackSize %d enregisteredSize  %d \n", nativeStackSize, enregisteredSize );)

       if (flags & CALL_THIS_LAST)
       {
          _ASSERTE(argCount > 0 && sigInfo->hasThis());
          //this has been push last, rather than first as the argMap assumed,
          //So, lets fix up argMap to match the actual call site
          //This only works because <this> is always enregistered so the stack offsets in argMap are unaffected
          argsInfo[argCount] =   argsInfo[0];
          argsInfo++;
       }

        /*
          We are going to assume that for any chip, the space taken by an enregistered arg
          is sizeof(void*).
          Note:
                        nativeStackSize describes the size of the eventual call stack.
                        The order of the args in argsInfo (note <this> is now treated like any other arg
                                arg 0
                                arg 1
                                ...
                                arg n
                        the order on the stack is:
                           tos: arg n
                                arg n-1
                                ...
                                arg 0
        */

         //see if we can just pop some stuff off TOS quickly
         //See if stuff at TOS is going to regs.
         // This also insure that the 'thisLast' argument is gone for the loop below
         while (argCount > 0 && argsInfo[argCount-1].isReg && (!PARAMETER_SPACE || flags & CALL_THIS_LAST) )
         {
            --argCount; tailArgs++;
            emit_mov_TOS_arg(argsInfo[argCount].regNum);
            //INDEBUG(printf("Moved arg  %d from TOS to reg %d \n",argCount, argsInfo[argCount].regNum );)
            if (PARAMETER_SPACE) break; // We only want to pop of 'this pointer'
         }

         // If there are more args than would go in registers or we have a return
         // buff, we need to rearrange the stack
         if (( argCount != 0 || PARAMETER_SPACE && tailArgs ) || retValBuffWords != 0)
         {
             deregisterTOS;
             // Compute the size of the arguments on the IL stack
             ilStackSize = retValBuffWords > 0 && EnregReturnBuffer ?
                                nativeStackSize + enregisteredSize - (tailArgs+1)*sizeof(void *):
                                nativeStackSize + enregisteredSize - (tailArgs)*sizeof(void *);
             unsigned i;

             // The vararg cookie is not present on the IL stack
             if (PARAMETER_SPACE && sigInfo->isVarArg() )
               ilStackSize -= sizeof(void *);

             // add in a slot for R4 since R4's are stored as R8's on the IL stack and
             // add alignment pad if necessary
             for (i=0; i < argCount; i++)
             {
               if ( (argsInfo[i].type.isPrimitive() && argsInfo[i].type.enum_() == typeR4) )
                     ilStackSize += sizeof( void *);
               else if ( PASS_VALUETYPE_BYREF &&
                         (argsInfo[i].type.enum_() == typeRefAny || argsInfo[i].type.enum_() == typeValClass) )
             ilStackSize += (BYTE_ALIGNED(argsInfo[i].size) - sizeof(void *));
               else if ( SIZE_STACK_SLOT != sizeof(void *) && (argsInfo[i].size % SIZE_STACK_SLOT) )
                     ilStackSize += sizeof( void *);
             }

             // If the buffer for enregistered arguments has fixed size, use it instead of the actual size
             enregisteredSize = FIXED_ENREG_BUFFER ? FIXED_ENREG_BUFFER : enregisteredSize;

             // if we have a hidden return buff param, allocate space and load the reg
             // in this case the stack is growing, so we have to perform the argument
             // shuffle in the opposite order
             if (retValBuffWords > 0)
             {
                // From a stack tracking perspective, this return value buffer comes
                // into existance before the call is made, we do that tracking here.
                pushOp(OpType(sigInfo->retTypeClass));

                if (!PARAMETER_SPACE)
                {
                    nativeStackSize += retValBuffWords*sizeof(void*);     // allocate the return buffer
                    if ( nativeStackSize < ilStackSize )
                    {
                       stackPadorRetBase = ilStackSize - nativeStackSize; // disallow stack shrinking
                       nativeStackSize = ilStackSize;            // because it may lead to arguments being overwritten
                    }
                }
             }


             // INDEBUG(printf("IlStack Size %d StackPad %d \n",ilStackSize, stackPadorRetBase  );)
             if (PARAMETER_SPACE)
             {
            if (PASS_VALUETYPE_BYREF)
        {
                  ilStackPad      = retValBuffOnStack > ilStackSize ? BYTE_ALIGNED(retValBuffOnStack - ilStackSize) : 0;
                  nativeStackSize = nativeStackSize + enregisteredSize + ilStackSize + ilStackPad + sizeof(prolog_frame) +
                                    retValBuffOnStack;
                }
                else if ( retValBuffOnStack >= ilStackSize )
                  nativeStackSize = nativeStackSize + enregisteredSize + retValBuffOnStack + sizeof(prolog_frame);
                else
                {
                  nativeStackSize = nativeStackSize + enregisteredSize + ilStackSize + sizeof(prolog_frame);
                  stackPadorRetBase = ilStackSize - retValBuffOnStack;
                }
             }

             // Check if we need to pad the call frame so that it is aligned on an evalution stack slot size
             if ( SIZE_STACK_SLOT != sizeof(void *) && (nativeStackSize % SIZE_STACK_SLOT) )
             {
               // Reuse the enregisteredPad for the alignment pad
               enregisteredPad += (nativeStackSize % SIZE_STACK_SLOT);
               nativeStackSize += (nativeStackSize % SIZE_STACK_SLOT);
             }

             //INDEBUG(printf("IlStack Size %d StackPad %d nativeStackSize %d \n",ilStackSize, stackPadorRetBase, nativeStackSize  );)
             // If the space used by the IL arguments can't be recycled for native arguments we need to allocated
             // more room.
             if (nativeStackSize >= ilStackSize || PARAMETER_SPACE )
             {
                  if (nativeStackSize - ilStackSize-STACK_BUFFER)  // get the extra space
                       emit_call_frame(nativeStackSize-ilStackSize-STACK_BUFFER);

                  // figure out the offsets from the moved stack pointer.
                  unsigned ilOffset = nativeStackSize-ilStackSize;  // start at the last IL arg
                  unsigned nativeOffset = !PARAMETER_SPACE ? 0 :
            (PASS_VALUETYPE_BYREF ? nativeStackSize-ilStackSize-enregisteredPad - ilStackPad - retValBuffOnStack :
                            (retValBuffOnStack < ilStackSize ? nativeStackSize-ilStackSize-enregisteredPad
                                                       : nativeStackSize-retValBuffOnStack-enregisteredPad));
                  i = argCount;
                  //INDEBUG(printf("Arg %d ilOffset %d nativeOffset %d \n", i, ilOffset, nativeOffset  );)
                  while(i > 0)
                  {
                     --i;
                     if (argsInfo[i].isReg ON_PPC_ONLY(&& !sigInfo->isVarArg()) )
                     {
                        // Check if enregistering in floating point or general purpose
                        _ASSERTE( argsInfo[i].type.enum_() < typeCount );
                        if (PARAMETER_SPACE) nativeOffset -= argsInfo[i].size;

                        // Skip the vararg cookie on non x86 platforms
                        if (varArgCookieOffset == nativeOffset && PARAMETER_SPACE && sigInfo->isVarArg() )
                          { i++; continue; }

                        if ( floatEnregisterMap[argsInfo[i].type.enum_()] )
                          {
                            emit_mov_arg_floatreg(ilOffset, argsInfo[i].regNum, argsInfo[i].type.enum_() );
                            ilOffset += argsInfo[i].size;
                          }
                        else
                          {
                            unsigned currReg = argsInfo[i].regNum;

                            if ( PASS_VALUETYPE_BYREF &&
                                 (argsInfo[i].type.enum_() == typeRefAny || argsInfo[i].type.enum_() == typeValClass) )
                            {
                                if(isTailCall)
                                {
                                    //
                                    // get old struct pointer to the TOS
                                    //
                                    stackItems * varInfo = &(argsMap[i]);
                                    emit_LDVARA(varInfo->offset, sizeof(void*));
                                    mov_register_indirect_to(TOS_REG_1, TOS_REG_1);
                                    emit_copybytes_nooverlap(ilOffset,argsInfo[i].size);
                                }
                                emit_mov_arg_stack_pointer( ilOffset, currReg, 0, argsInfo[i].type.cls(), isTailCall );
                                nativeOffset += ( argsInfo[i].size - sizeof(void *) );
                                ilOffset     += argsInfo[i].size;
                            }
                            else
                             for (unsigned arg_size = argsInfo[i].size; arg_size > 0 && currReg <= MAX_GP_ARG_REGISTER;
                                                                                            arg_size -= sizeof(void *))
                              if ( currReg < MAX_GP_ARG_REGISTER )
                              {
                                if ((argsInfo[i].type.isPrimitive() && argsInfo[i].type.enum_() == typeR4))
                                  { emit_narrow_R8toR4(ilOffset, ilOffset); }
                                emit_mov_arg_reg(ilOffset, currReg ); currReg++; ilOffset += sizeof(void *);
                              }
                              else
                              {
                                emit_mov_arg_stack(nativeOffset + (argsInfo[i].size-arg_size), ilOffset, arg_size);
                                ilOffset += arg_size;
                                break;
                              }
                          }
                        // R4s are stored as R8s on the eval stack and add alignment pad if necessary
                        if ( argsInfo[i].type.isPrimitive() && argsInfo[i].type.enum_() == typeR4 ||
                             SIZE_STACK_SLOT != sizeof(void *) && (argsInfo[i].size % SIZE_STACK_SLOT) )
                             ilOffset += sizeof(void *);

                        //INDEBUG(printf("Moved arg %d to R%d ilOffset %d nativeOffset %d \n", i, argsInfo[i].regNum, ilOffset,nativeOffset  );)
                     }
                     else
                     {
                        _ASSERTE(nativeOffset <= ilOffset);
                        if (PARAMETER_SPACE) nativeOffset -= argsInfo[i].size;

                        // Skip the vararg cookie on non x86 platforms
                        if (varArgCookieOffset == nativeOffset && PARAMETER_SPACE && sigInfo->isVarArg() )
                          { i++; continue; }

                        if (!(argsInfo[i].type.isPrimitive() && argsInfo[i].type.enum_() == typeR4))
                        {
                            if ( PASS_VALUETYPE_BYREF &&
                                 (argsInfo[i].type.enum_() == typeRefAny || argsInfo[i].type.enum_() == typeValClass)
                               )
                            {
                                nativeOffset += ( argsInfo[i].size - sizeof(void *) );
                                if(isTailCall)
                                {

                                    //
                                    // get old struct pointer to the TOS
                                    //
                                    stackItems * varInfo = &(argsMap[i]);
                                    emit_LDVARA(varInfo->offset, sizeof(void*));
                                    mov_register_indirect_to(TOS_REG_1, TOS_REG_1);
                                    emit_copybytes_nooverlap(ilOffset,argsInfo[i].size);
                                }
                                emit_mov_arg_stack_pointer(ilOffset, 0 /* MAX_GP_ARG_REGISTER */, nativeOffset,argsInfo[i].type.cls(),isTailCall);
                                ilOffset     += argsInfo[i].size;
                            }
                            else
                            {
                             if (ilOffset != nativeOffset)
                                 emit_mov_arg_stack(nativeOffset, ilOffset, argsInfo[i].size)
                             ilOffset += argsInfo[i].size;
                          }
                          // Add alignment pad if necessary
                          if ( SIZE_STACK_SLOT != sizeof(void *) && (argsInfo[i].size % SIZE_STACK_SLOT) )
                            ilOffset += sizeof(void *);
                        }
                        else // convert from R8 to R4
                        {
                           emit_narrow_R8toR4(nativeOffset, ilOffset);
                           ilOffset += sizeof(double);
                        }
                        if (!PARAMETER_SPACE) nativeOffset += argsInfo[i].size;
                        //INDEBUG(printf("Moved arg %d to stack ilOffset %d size %d nativeOffset %d \n", i, ilOffset, argsInfo[i].size,nativeOffset  );)
                     }
                   }
                   INDEBUG(int varArgOff = ( varArgCookieOffset == (sizeof(prolog_frame)) ? 1: 0 );)
                   _ASSERTE(!PARAMETER_SPACE && nativeOffset == nativeStackSize - retValBuffWords*sizeof(void*)
                                                                                - stackPadorRetBase  ||
                             PARAMETER_SPACE && nativeOffset == (unsigned)(retValBuffWords && EnregReturnBuffer ?
                              sizeof(prolog_frame)+(tailArgs+1+varArgOff)*sizeof(void *):
                              sizeof(prolog_frame)+(tailArgs+varArgOff)*sizeof(void *)) );
                   _ASSERTE( ilOffset == nativeStackSize );
                }
                else
                {
                   // This is the normal case (for x86 only), the stack will shink because the register
                   // arguments don't take up space
                   unsigned ilOffset = 0;                 // This points just above the first arg.
                   unsigned nativeOffset = retValBuffWords*sizeof(void*); // we want the native args to overwrite the il args
                   i = argCount;
                   //INDEBUG(printf("Arg %d ilOffset %d nativeOffset %d \n", i, ilOffset, nativeOffset  );)
                   while(i > 0)
                   {
                     --i;
                     //INDEBUG(printf("ARG: %d ilOffset %d nativeOffset %d \n",  i, ilOffset, nativeOffset  );)
                     if (argsInfo[i].isReg)
                     {
                       emit_mov_arg_reg(ilOffset, argsInfo[i].regNum);
                       ilOffset += sizeof(void*);
                       nativeOffset += sizeof(void*);
                       //INDEBUG(printf("Moved arg %d to R%d ilOffset %d nativeOffset %d \n", i, argsInfo[i].regNum, ilOffset, nativeOffset  );)
                     }
                     else
                     {
                       if (!(argsInfo[i].type.isPrimitive() && argsInfo[i].type.enum_() == typeR4))
                       {
                         //_ASSERTE(nativeOffset >= ilOffset); // il args always take up more space
                         if ( nativeOffset != 0)
                         {
                            emit_mov_arg_stack((ilOffset-nativeOffset), ilOffset, argsInfo[i].size);
                         }
                         ilOffset += argsInfo[i].size;
                       }
                       else // convert from R8 to R4
                       {
                         //_ASSERTE(nativeOffset >= ilOffset); // il args always take up more space
                         emit_narrow_R8toR4((ilOffset-nativeOffset),ilOffset);
                         ilOffset += sizeof(double);
                         nativeOffset += sizeof(float);
                       }
                     }
                   }
                   //INDEBUG(printf(" ilOffset %d nativeOffset %d \n",  ilOffset, nativeOffset  );)
                   _ASSERTE(ilOffset == ilStackSize);
                   stackPadorRetBase += nativeOffset;    // Pop off the unused part of the stack
                   if ( sigInfo->isVarArg() )
             nativeStackSize += stackPadorRetBase;
                }

                if (retValBuffWords > 0)
                {
                   // Get the GC info for return buffer, an zero out any GC pointers
                   unsigned char* gcInfo;
                   if (sigInfo->retType == CORINFO_TYPE_REFANY)
                   {
                     _ASSERTE(retValBuffWords == 2);
                     static unsigned char refAnyGCInfo[] = { true, false };
                     gcInfo = refAnyGCInfo;
                   }
                   else
                   {
                     gcInfo = (unsigned char*)_alloca(retValBuffWordsUnaligned);
                     jitInfo->getClassGClayout(sigInfo->retTypeClass, gcInfo);
                   }
                   unsigned retValBase = nativeStackSize-retValBuffWords*sizeof(void*) +
                                         bigEndianOffset(typeSizeInBytes(jitInfo, sigInfo->retTypeClass));

                   if (PASS_VALUETYPE_BYREF)
           {
                     retValBase -= (ilStackSize+ilStackPad);
             stackPadorRetBase = retValBase;
           }

                   for (unsigned k = 0; k < retValBuffWordsUnaligned; k++)
                   {
                     if (gcInfo[k])
                       emit_set_zero(retValBase + k*sizeof(void*));
                   }
                   // set the return value buffer argument to the allocate buffer
                   unsigned retBufReg = sigInfo->hasThis(); // return buff param is either the first or second reg param
                   emit_set_return_buffer(retBufReg, retValBase,isTailCall);
                }
          }

          // If this is a varargs function, push the hidden signature variable
          // or if it is calli to an unmanaged target push the sig
          if (sigInfo->isVarArg() || (flags & CALLI_UNMGD))
          {

            CORINFO_VARARGS_HANDLE vasig = jitInfo->getVarArgsHandle(sigInfo);
            // We need to store the vararg cookie at varArgCookieOffset or push it on the top of the stack, and store the
            // this pointer and return buffer on the stack if it is currently in a register
            #if defined(_PPC_)
            if ( (flags & CALL_THIS_LAST) )
                emit_set_arg_pointer( retValBuffWords && ReturnBufferFirst,
                                 (sizeof(prolog_frame)+ (retValBuffWords && ReturnBufferFirst? 1: 0)*sizeof(void *)), true);
            if ( retValBuffWords )
                emit_set_arg_pointer( !(retValBuffWords && ReturnBufferFirst),
                                 (sizeof(prolog_frame)+ (retValBuffWords && ReturnBufferFirst? 0: 1)*sizeof(void *)), true);
            #endif
            emit_set_vararg_cookie(vasig, varArgCookieOffset, false ON_SPARC_ONLY( || true) );
            nativeStackSize = PARAMETER_SPACE ? nativeStackSize: nativeStackSize + sizeof(void*);
          }

          if (PushEnregArgs)
          {
            ON_X86_ONLY(_ASSERTE( enregisteredSize >= 0 && enregisteredSize < 9);)
            if ( enregisteredSize > 4 ) push_register(ARG_2, false);
            if ( enregisteredSize > 0 ) push_register(ARG_1, false);
          }
    }
    else
    {
          deregisterTOS;

          if (PARAMETER_SPACE && !STACK_BUFFER)
          {
            nativeStackSize += sizeof(prolog_frame);
            emit_call_frame( nativeStackSize + (sigInfo->isVarArg() || (flags & CALLI_UNMGD) ? sizeof(void *) : 0));
          }
          else if (STACK_BUFFER)
            nativeStackSize = STACK_BUFFER;

          // If this is a varargs function, push the hidden signature variable
          // or if it is calli to an unmanaged target push the sig
          if (sigInfo->isVarArg() || (flags & CALLI_UNMGD))
          {
            CORINFO_VARARGS_HANDLE vasig = jitInfo->getVarArgsHandle(sigInfo);
            emit_set_vararg_cookie(vasig, varArgCookieOffset, false ON_SPARC_ONLY( || true) );
            nativeStackSize = STACK_BUFFER ? nativeStackSize : nativeStackSize + sizeof(void*);
          }

    }

    // If anything is left on the stack, we need to log it for GC tracking puposes.
    if ( !PASS_VALUETYPE_BYREF || retValBuffWords == 0 )
      { LABELSTACK(outPtr-codeBuffer, 0); }
    else
      // We do not use LABELSTACK(outPtr-codeBuffer, 0) because the return buffer may be above TOS
      stacks.append((unsigned)(outPtr-codeBuffer), opStack, opStack_len, ((ilStackSize+ilStackPad)/sizeof(void *)) );

    if (!PushEnregArgs)
       return(nativeStackSize - retValBuffWords*sizeof(void*) -(int)STACK_BUFFER  );
    else
       return(nativeStackSize - retValBuffWords*sizeof(void*)  + enregisteredSize );
}

/************************************************************************************/
/* jit the method. if successful, return number of bytes jitted, else return 0 */
FJitResult FJit::jitCompile(
                BYTE ** ReturnAddress,
                unsigned * ReturncodeSize
                )
{
/*****************************************************************************
 * The following macro reads a value from the IL stream. It checks that the size
 * of the object doesn't exceed the length of the stream. It also checks that
 * the data has not been previously read and marks it as read, unless the "reread"
 * variable is set to true.
 *****************************************************************************/
#define GET(val, type, reread)                                                                    \
        {                                                                                         \
            unsigned int            size_operand;                                                 \
            VALIDITY_CHECK( inPtr + sizeof(type) <= inBuffEnd );                                  \
            for ( size_operand = 0; size_operand < sizeof(type) && !reread; size_operand++ )      \
               VALIDITY_CHECK(!state[inPtr- inBuff+size_operand].isJitted)                        \
            switch(sizeof(type)) {                                                                \
                case 1: val = (type)*inPtr; break;                                                \
                case 2: val = (type)GET_UNALIGNED_VAL16(inPtr); break;                            \
                case 4: val = (type)GET_UNALIGNED_VAL32(inPtr); break;                            \
                case 8: val = (type)GET_UNALIGNED_VAL64(inPtr); break;                            \
                default: val = (type)0; _ASSERTE(!"Invalid size"); break;                         \
            }                                                                                     \
            inPtr += sizeof(type);                                                                \
            for ( size_operand = 1; size_operand <= sizeof(type) && !reread; size_operand++ )     \
               state[inPtr-inBuff-size_operand].isJitted = true;                                  \
        }

#define LEAVE_CRIT                                                       \
            if (methodInfo->args.hasThis()) {                            \
                emit_WIN32(emit_LDVAR_I4(offsetOfRegister(0)))           \
                emit_WIN64(emit_LDVAR_I8(offsetOfRegister(0)));          \
                emit_EXIT_CRIT();                                        \
           }                                                             \
            else {                                                       \
                void* syncHandle;                                        \
                syncHandle = jitInfo->getMethodSync(methodInfo->ftn);    \
                emit_EXIT_CRIT_STATIC(syncHandle);                       \
            }
#define ENTER_CRIT                                                       \
            if (methodInfo->args.hasThis()) {                            \
                emit_WIN32(emit_LDVAR_I4(offsetOfRegister(0)))           \
                emit_WIN64(emit_LDVAR_I8(offsetOfRegister(0)));          \
                emit_ENTER_CRIT();                                       \
            }                                                            \
            else {                                                       \
                void* syncHandle;                                        \
                syncHandle = jitInfo->getMethodSync(methodInfo->ftn);    \
                emit_ENTER_CRIT_STATIC(syncHandle);                      \
            }

#define CURRENT_INDEX (inPtr - inBuff)

    TailCallForbidden = !!((methodInfo->args.callConv  & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_VARARG);
                              // if set, no tailcalls allowed. Initialized to FALSE. When a security test
                              // changes it to TRUE, it remains TRUE for the duration of the jitting of the function
    outBuff = codeBuffer;
    CORINFO_METHOD_HANDLE   methodHandle= methodInfo->ftn;
    unsigned int    len = methodInfo->ILCodeSize;            // IL size

    inBuff = methodInfo->ILCode;             // IL bytes
    inBuffEnd = &inBuff[len];          // end of IL
    entryAddress = ReturnAddress;
    codeSize = ReturncodeSize;

    // Information about arguments and locals
    offsetVarArgToken = sizeof(prolog_frame);

    // Local variables declared for convenience and flags
    unsigned        offset;
    unsigned        address;
    signed int      i4;
    int             merge_state;
    FJitResult      JitResult = FJIT_OK;
    unsigned char   opcode_val;

    InstStart = 0;
    DelegateStart = 0;
    DelegateMethodRef = 0;
    UnalignedOffset   = (unsigned)-1;

JitAgain:

    MadeTailCall = false;      // if a tailcall has been made and subsequently TailCallForbidden is set to TRUE,
                               // we will rejit the code, disallowing tailcalls.
    inRegTOS = false;          // flag indicating if the top of the stack is in a register
    controlContinue = true;    // does control we fall thru to next il instr

    inPtr = inBuff;            // Set the current IL offset to the start of the IL buffer
    outPtr = outBuff;          // Set the current output buffer position to the start of the buffer

    codeGenState = FJIT_OK;    // Reset the global error flag
    JitResult    = FJIT_OK;    // Reset the result flag for simple operations that don't set it

    UnalignedAccess = false;   // Reset the unaligned access flag

#ifdef _DEBUG
    didLocalAlloc = false;
#endif
    // Can not jit a native method
    VALIDITY_CHECK(!(methodAttributes & (CORINFO_FLG_NATIVE)));
    // Zero sized methods are not allowed
    VALIDITY_CHECK(methodInfo->ILCodeSize > 0);
    // Can not jit methods with shared bodies
    VALIDITY_CHECK(!(methodAttributes & CORINFO_FLG_SHAREDINST) );

    *(entryAddress) = outPtr;

#if defined(_DEBUG)
    static ConfigMethodSet fJitHalt;
    fJitHalt.ensureInit(L"JitHalt");
    if (fJitHalt.contains(szDebugMethodName, szDebugClassName, PCCOR_SIGNATURE(methodInfo->args.sig))) {
        emit_break();
    }
#endif

    //Skip verification if possible
    JitVerify = !(flags & CORJIT_FLG_SKIP_VERIFICATION);
    IsVerifiableCode = true; // assume the code is verifiable unless proven otherwise

    // load any constraints for verification, detecting and rejecting cycles
    if (JitVerify)
    {
        BOOL hasCircularClassConstraints = FALSE;
        BOOL hasCircularMethodConstraints = FALSE;
        jitInfo->initConstraintsForVerification(methodHandle,&hasCircularClassConstraints,&hasCircularMethodConstraints);
        VERIFICATION_CHECK(!hasCircularClassConstraints);
        VERIFICATION_CHECK(!hasCircularMethodConstraints);
    }

#if defined(_SPARC_) || defined(_PPC_)
    // Check if the offset of the vararg token has been computed correctly
    offsetVarArgToken += ( methodInfo->args.hasThis() ? sizeof( void * ) : 0 ) +
                         ( methodInfo->args.hasRetBuffArg() && EnregReturnBuffer ? sizeof( void * ) : 0 );
#endif
    // it may be worth optimizing the following to only initialize locals so as to cover all refs.
    unsigned int localWords = (localsFrameSize+sizeof(void*)-1)/ sizeof(void*);

    emit_prolog(localWords);

    if (flags & CORJIT_FLG_PROF_ENTERLEAVE)
    {
        BOOL bHookFunction;
        void *eeHandle;
        void *profilerHandle;
        BOOL bIndirected;
        jitInfo->GetProfilingHandle(methodHandle,
                                    &bHookFunction,
                                    &eeHandle,
                                    &profilerHandle,
                                    &bIndirected);

        if (bHookFunction)
        {
            _ASSERTE(!bIndirected);  // FJIT does not handle NGEN case
            _ASSERTE(!inRegTOS);
            ULONG func = (ULONG) jitInfo->getHelperFtn(CORINFO_HELP_PROF_FCN_ENTER);
            _ASSERTE(func != NULL);
            emit_callhelper_prof4(func,
                                  (CorJitFlag) CORINFO_HELP_PROF_FCN_ENTER,
                                  eeHandle,
                                  profilerHandle,
                                  NULL,  // FRAME_INFO (see definition of FunctionEnter2 in corprof.idl)
                                  NULL); // ARG_INFO (see definition of FunctionEnter2 in corprof.idl)
        }
    }

    // Do we need to insert a "JustMyCode" callback?
    if (flags & CORJIT_FLG_DEBUG_CODE)
    {
       CORINFO_JUST_MY_CODE_HANDLE *pDbgHandle;
       CORINFO_JUST_MY_CODE_HANDLE dbgHandle = jitInfo->getJustMyCodeHandle(methodHandle, &pDbgHandle);
       _ASSERTE(!dbgHandle || !pDbgHandle);

       if (dbgHandle || pDbgHandle)
         emit_justmycode_callback( dbgHandle, pDbgHandle );
    }

#ifdef LOGGING
    if (codeLog) {
        emit_log_entry(szDebugClassName, szDebugMethodName);
    }
#endif

    // Get sequence points
    unsigned                     nextSequencePoint = 0;
    if (flags & CORJIT_FLG_DEBUG_INFO) {
        getSequencePoints(jitInfo,methodHandle,&cSequencePoints,&sequencePointOffsets,&offsetsImplicit);
    }
    else {
        cSequencePoints = 0;
        offsetsImplicit = ICorDebugInfo::NO_BOUNDARIES;
    }

    mapInfo.prologSize = outPtr-outBuff;

    // note: entering of the critical section is not part of the prolog
    mapping->add(CURRENT_INDEX,(unsigned)(outPtr - outBuff));

    if (methodAttributes & CORINFO_FLG_SYNCH) {
        ENTER_CRIT;
    }

    // Verify the exception handlers' table
    int ver_exceptions = verifyHandlers();
    VALIDITY_CHECK( ver_exceptions != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_exceptions != FAILED_VERIFICATION );
    // Initialize the state map with the exception handling information
    initializeExceptionHandling();

    bool First        = true;
    popSplitStack     = false; // Start jitting at the next offset on the split stack
    UncondBranch      = false; // Executing an unconditional branch
    LeavingTryBlock   = false; // Executing a "leave" from a try block
    LeavingCatchBlock = false; // Executing a "leave" from a catch block
    FinishedJitting   = false; // Finished jitting the IL stream

    makeClauseEmpty(&currentClause);

    _ASSERTE(!inRegTOS);

    while (!FinishedJitting)
    {
      //INDEBUG( printf("IL offset: %x PopStack: %d StackEmpty: %d\n", CURRENT_INDEX, popSplitStack, SplitOffsets.isEmpty() );)
START_LOOP:
        // If we jitted the last statement or an uncondtional branch with jitted target
        // we need to restart at the next split offset
        if ( inPtr >= inBuffEnd || popSplitStack )
        {
          // Remove the IL offsets that's already been jitted
          while ( !SplitOffsets.isEmpty() && state[SplitOffsets.top()].isJitted )
             (void)SplitOffsets.popOffset();

          //INDEBUG(SplitOffsets.dumpStack();)

          // We reached the end of the IL opcode stream, but not all code has been jitted
          // Pop the offset from the split offsets stack
          if (!SplitOffsets.isEmpty())
          {
             inPtr = (unsigned char *)&inBuff[SplitOffsets.popOffset()];
             //INDEBUG(printf("Starting jitting at %d \n", inPtr-inBuff );)
             // Treat a split as a forward jump
             controlContinue = false;
             // Reset flag
             popSplitStack = false;

          }
          else
          {
            // Check for a fall through at the end of the function
            VALIDITY_CHECK( popSplitStack || inBuff[InstStart] == CEE_THROW );
            goto END_JIT_LOOP;
          }
        }

        // Check if max stack value has been exceded
        VERIFICATION_CHECK( methodInfo->maxStack >= opStack_len );

        //INDEBUG(if (JitVerify) printf("IL offset is %x\n", CURRENT_INDEX );)

        // Guard against a fall through into/from a catch/finally/filter
        VALIDITY_CHECK(!(state[CURRENT_INDEX].isHandler) && !(state[CURRENT_INDEX].isFilter) &&
                       !(state[CURRENT_INDEX].isEndBlock) || !controlContinue || UncondBranch );
        UncondBranch = false; // This flag is only used to check for fall through

        if (controlContinue) {
            if (state[CURRENT_INDEX].isJmpTarget && inRegTOS != state[CURRENT_INDEX].isTOSInReg) {
                if (inRegTOS) {
                        deregisterTOS;
                }
                else {
                        enregisterTOS;
                }
            }
        }
        else {  // controlContinue == false
            unsigned int label = ver_stacks.findLabel(CURRENT_INDEX);
            if (label == LABEL_NOT_FOUND) {
                CHECK_POP_STACK(opStack_len);
                inRegTOS = false;
            }
            else {
                opStack_len  = ver_stacks.setStackFromLabel(label, opStack, opStack_size);
                inRegTOS = state[CURRENT_INDEX].isTOSInReg;
            }
            controlContinue = true;
        }

        //Check if this IL offset has already been jitted. Note, that to see if
        //an offset has been jitted we need to check that it is not in skipped code
        //intervals and that an offset equal to or above it has been jitted
        if ( state[inPtr-inBuff].isJitted )
        {
          //INDEBUG( printf("Detected jitted code: IL offset is %x\n",CURRENT_INDEX  );)
          // The skipped code interval must just have ended
          // If verification is enabled we need to compare the current state of the stack with the saved one
          merge_state = verifyStacks(CURRENT_INDEX, 0);
          VERIFICATION_CHECK( merge_state );
          if ( JitVerify && merge_state == MERGE_STATE_REJIT )
            { resetState(false); goto JitAgain; }
          // Emit a jump to the jitted code
          ilrel = CURRENT_INDEX;
          if (state[inPtr-inBuff].isTOSInReg)
            { enregisterTOS; }
          else
            { deregisterTOS; }

          address = mapping->pcFromIL(inPtr-inBuff);
          VALIDITY_CHECK(address > 0 );
          emit_jmp_abs_address(CEE_CondAlways, address + (unsigned)outBuff, true);

          // INDEBUG(printf("Emitted a jump to %d\n", outPtr+address-outBuff);)
          // Remove the IL offsets that's already been jitted
          while ( !SplitOffsets.isEmpty() && state[SplitOffsets.top()].isJitted )
              (void)SplitOffsets.popOffset();

          // Pop the offset from the split offsets stack
          if (!SplitOffsets.isEmpty())
            {
              inPtr = (unsigned char *)&inBuff[SplitOffsets.popOffset()];
              //INDEBUG(printf("Starting jitting at %d \n", inPtr-inBuff );)
              // Treat a split as a forward jump
              controlContinue = false;
              //INDEBUG(SplitOffsets.dumpStack();)
              goto START_LOOP;
            }
          else
            goto END_JIT_LOOP;
        }

        // If the current offset is a beginning of a try block, it is necessary to push the addresses of
        // associated handlers onto the split offsets stack in the correct order
        if (state[CURRENT_INDEX].isTry)
          {
            //INDEBUG(printf("Pushed Handlers at %x\n", CURRENT_INDEX );)
            // The stack has to be empty on an entry to a try block
            VALIDITY_CHECK(isOpStackEmpty());
            // Push the starting offset of the try block onto the split offsets stack
            SplitOffsets.pushOffset(CURRENT_INDEX);
            // Push the starting addresses of all the handlers onto the split offsets stack
            pushHandlerOffsets(CURRENT_INDEX);
            // Emit a jump to the start of the try block
            fixupTable->insert((void**) outPtr);
            emit_jmp_abs_address(CEE_CondAlways, CURRENT_INDEX, false);
            //INDEBUG(SplitOffsets.dumpStack();)
            state[CURRENT_INDEX].isTry = 0; // Reset the flag once the handlers have been pushed onto the stack
            // Start jitting the first handler
            popSplitStack = true;
            controlContinue = false;
            First = false;
            continue;
          }

        // This IL opcode will be jitted
        if (!First)
           mapping->add(CURRENT_INDEX,(unsigned)(outPtr - outBuff));
        First = false;

        if (state[CURRENT_INDEX].isHandler) {
        if ( (offsetsImplicit & ICorDebugInfo::CALL_SITE_BOUNDARIES) != 0 )
               emit_sequence_point_marker();
            unsigned int nestingLevel = Compute_EH_NestingLevel(inPtr-inBuff);
            emit_storeTOS_in_JitGenerated_local(nestingLevel,state[CURRENT_INDEX].isFilter);
        }


        state[CURRENT_INDEX].isTOSInReg = inRegTOS;

        // Check if we are currently at a sequence point
        emitSequencePointPre( CURRENT_INDEX, nextSequencePoint );

        // If verification is enabled we need to store the current state of the stack
        merge_state = verifyStacks(CURRENT_INDEX, 1);
        VERIFICATION_CHECK( merge_state );
        if ( JitVerify && merge_state == MERGE_STATE_REJIT )
          { resetState(false); goto JitAgain; }

        InstStart = CURRENT_INDEX;

        if ( InstStart == UnalignedOffset ) UnalignedAccess = true;
#ifdef LOGGING
        ilrel = inPtr - inBuff;
#endif
        GET(opcode_val, unsigned char, false );
        OPCODE  opcode = OPCODE(opcode_val);
DECODE_OPCODE:

#ifdef LOGGING
    if (codeLog && opcode != CEE_PREFIXREF && (opcode < CEE_PREFIX7 || opcode > CEE_PREFIX1)) {
        bool oldstate = inRegTOS;
        emit_log_opcode(ilrel, opcode, oldstate);
        inRegTOS = oldstate;
    }
#endif
        switch (opcode)
        {

        case CEE_PREFIX1:
            GET(opcode_val, unsigned char, false);
            opcode = OPCODE(opcode_val + 256);
            goto DECODE_OPCODE;

        case CEE_LDARG_0:
        case CEE_LDARG_1:
        case CEE_LDARG_2:
        case CEE_LDARG_3:
            offset = (opcode - CEE_LDARG_0);
            // Make sure that the offset is legal (with respect to the IL encoding)
            VERIFICATION_CHECK(offset < 4);
            JitResult = compileDO_LDARG( opcode, offset);
            break;

        case CEE_LDARG_S:
            GET(offset, unsigned char, false);
            JitResult = compileDO_LDARG(opcode, offset);
            break;

        case CEE_LDARG:
            GET(offset, unsigned short, false);
            JitResult = compileDO_LDARG(opcode, offset);
            break;

        case CEE_LDLOC_0:
        case CEE_LDLOC_1:
        case CEE_LDLOC_2:
        case CEE_LDLOC_3:
            offset = (opcode - CEE_LDLOC_0);
            // Make sure that the offset is legal (with respect to the IL encoding)
            VERIFICATION_CHECK(offset < 4);
            JitResult = compileDO_LDLOC(opcode, offset);
            break;

        case CEE_LDLOC_S:
            GET(offset, unsigned char, false);
            JitResult = compileDO_LDLOC(opcode, offset);
            break;

        case CEE_LDLOC:
            GET(offset, unsigned short, false);
            JitResult = compileDO_LDLOC(opcode, offset);
            break;

        case CEE_STARG_S:
            GET(offset, unsigned char, false);
            JitResult = compileDO_STARG(offset);
            break;

        case CEE_STARG:
            GET(offset, unsigned short, false);
            JitResult = compileDO_STARG(offset);
            break;

        case CEE_STLOC_0:
        case CEE_STLOC_1:
        case CEE_STLOC_2:
        case CEE_STLOC_3:
            offset = (opcode - CEE_STLOC_0);
            // Make sure that the offset is legal (with respect to the IL encoding)
            VALIDITY_CHECK( offset < 4);
            JitResult = compileDO_STLOC(offset);
            break;

        case CEE_STLOC_S:
            GET(offset, unsigned char, false);
            JitResult = compileDO_STLOC(offset);
            break;

        case CEE_STLOC:
            GET(offset, unsigned short, false);
            JitResult = compileDO_STLOC(offset);
            break;

        case CEE_ADD:
            JitResult = compileCEE_ADD();
            break;

        case CEE_ADD_OVF:
            JitResult = compileCEE_ADD_OVF();
            break;

        case CEE_ADD_OVF_UN:
            JitResult = compileCEE_ADD_OVF_UN();
            break;

        case CEE_SUB:
            JitResult = compileCEE_SUB();
            break;

        case CEE_SUB_OVF:
            JitResult = compileCEE_SUB_OVF();
            break;

        case CEE_SUB_OVF_UN:
            JitResult = compileCEE_SUB_OVF_UN();
            break;

        case CEE_MUL:
            JitResult = compileCEE_MUL();
            break;

        case CEE_MUL_OVF:
            JitResult = compileCEE_MUL_OVF();
            break;

        case CEE_MUL_OVF_UN:
            JitResult = compileCEE_MUL_OVF_UN();
            break;

        case CEE_DIV:
            JitResult = compileCEE_DIV();
            break;

        case CEE_DIV_UN:
            JitResult = compileCEE_DIV_UN();
            break;

        case CEE_REM:
            JitResult = compileCEE_REM();
            break;

        case CEE_REM_UN:
            JitResult = compileCEE_REM_UN();
            break;

        case CEE_LOCALLOC:
            JitResult = compileCEE_LOCALLOC();
            break;

        case CEE_NEG:
            JitResult = compileCEE_NEG();
            break;

        case CEE_LDIND_U1:
            JitResult = compileCEE_LDIND_U1();
            break;
        case CEE_LDIND_U2:
            JitResult = compileCEE_LDIND_U2();
            break;
        case CEE_LDIND_U4:
            JitResult = compileCEE_LDIND_U4();
            break;
        case CEE_LDIND_I1:
            JitResult = compileCEE_LDIND_I1();
            break;
        case CEE_LDIND_I2:
            JitResult = compileCEE_LDIND_I2();
            break;
        case CEE_LDIND_I4:
            JitResult = compileCEE_LDIND_I4();
            break;
        case CEE_LDIND_I8:
            JitResult = compileCEE_LDIND_I8();
            break;
        case CEE_LDIND_R4:
            JitResult = compileCEE_LDIND_R4();
            break;
        case CEE_LDIND_R8:
            JitResult = compileCEE_LDIND_R8();
            break;
        case CEE_LDIND_I:
            JitResult = compileCEE_LDIND_I();
            break;

        case CEE_LDIND_REF:
            JitResult = compileCEE_LDIND_REF();
            break;

        case CEE_STIND_I1:
            JitResult = compileCEE_STIND_I1();
            break;

        case CEE_STIND_I2:
            JitResult = compileCEE_STIND_I2();
            break;

        case CEE_STIND_I4:
            JitResult = compileCEE_STIND_I4();
            break;

        case CEE_STIND_I8:
            JitResult = compileCEE_STIND_I8();
            break;

        case CEE_STIND_I:
            JitResult = compileCEE_STIND_I();
            break;

        case CEE_STIND_R4:
            JitResult = compileCEE_STIND_R4();
            break;

        case CEE_STIND_R8:
            JitResult = compileCEE_STIND_R8();
            break;

        case CEE_STIND_REF:
            JitResult = compileCEE_STIND_REF();
            break;

        case CEE_LDC_I4_M1 :
        case CEE_LDC_I4_0 :
        case CEE_LDC_I4_1 :
        case CEE_LDC_I4_2 :
        case CEE_LDC_I4_3 :
        case CEE_LDC_I4_4 :
        case CEE_LDC_I4_5 :
        case CEE_LDC_I4_6 :
        case CEE_LDC_I4_7 :
        case CEE_LDC_I4_8 :
            i4 = (opcode - CEE_LDC_I4_0);
            // Make sure that the offset is legal (with respect to the IL encoding)
            VALIDITY_CHECK(-1 <= i4 && i4 <= 8);
            goto DO_CEE_LDC_I4;

        case CEE_LDC_I4_S:
            GET(i4, signed char, false);
            goto DO_CEE_LDC_I4;

        case CEE_LDC_I4:
            GET(i4, signed int, false);
            goto DO_CEE_LDC_I4;

DO_CEE_LDC_I4:
            emit_LDC_I4(i4);
            pushOp(OpType(typeI4));
            break;

        case CEE_LDC_I8:
            JitResult = compileCEE_LDC_I8();
            break;

        case CEE_LDC_R4:
            JitResult = compileCEE_LDC_R4();
            break;
        case CEE_LDC_R8:
            JitResult = compileCEE_LDC_R8();
            break;

        case CEE_LDNULL:
            JitResult = compileCEE_LDNULL();
            break;

        case CEE_LDLOCA_S:
            GET(offset, unsigned char, false);
            JitResult = compileDO_LDLOCA(opcode, offset);
            break;

        case CEE_LDLOCA:
            GET(offset, unsigned short, false);
            JitResult = compileDO_LDLOCA(opcode, offset);
            break;

        case CEE_LDSTR:
            JitResult = compileCEE_LDSTR();
            break;

        case CEE_CPBLK:
            JitResult = compileCEE_CPBLK();
            break;

        case CEE_INITBLK:
            JitResult = compileCEE_INITBLK();
            break;

         case CEE_INITOBJ:
            JitResult = compileCEE_INITOBJ();
            break;

        case CEE_CPOBJ:
            JitResult = compileCEE_CPOBJ();
            break;

        case CEE_LDOBJ:
            JitResult = compileCEE_LDOBJ();
            break;

        case CEE_STOBJ:
            JitResult = compileCEE_STOBJ();
            break;

        case CEE_MKREFANY:
            JitResult = compileCEE_MKREFANY();
            break;

        case CEE_SIZEOF:
            JitResult = compileCEE_SIZEOF();
            break;

        case CEE_LEAVE_S:
            GET(ilrel, signed char, false);
            JitResult = compileDO_LEAVE();
            break;

        case CEE_LEAVE:
            GET(ilrel, int, false);
            JitResult = compileDO_LEAVE();
            break;

        case CEE_BR:
            GET(ilrel, int, false);
            JitResult = compileDO_BR();
            break;

        case CEE_BR_S:
            GET(ilrel, signed char, false);
            JitResult = compileDO_BR();
            break;

        case CEE_BRTRUE:
            GET(ilrel, int, false);
            JitResult = compileDO_BR_boolean(CEE_CondNotEq);
            break;

        case CEE_BRTRUE_S:
            GET(ilrel, signed char, false);
            JitResult = compileDO_BR_boolean(CEE_CondNotEq);
            break;

        case CEE_BRFALSE:
            GET(ilrel, int, false);
            JitResult = compileDO_BR_boolean(CEE_CondEq);
            break;

        case CEE_BRFALSE_S:
            GET(ilrel, signed char, false);
            JitResult = compileDO_BR_boolean(CEE_CondEq);
            break;

        case CEE_CEQ:
            JitResult = compileCEE_CEQ();
            break;

        case CEE_CGT:
            JitResult = compileCEE_CGT();
            break;

        case CEE_CGT_UN:
            JitResult = compileCEE_CGT_UN();
            break;

        case CEE_CLT:
            JitResult = compileCEE_CLT();
            break;

        case CEE_CLT_UN:
            JitResult = compileCEE_CLT_UN();
            break;

        case CEE_BEQ_S:
            GET(ilrel, char, false);
            JitResult = compileDO_CEE_BEQ();
            break;

        case CEE_BEQ:
            GET(ilrel, int, false);
            JitResult = compileDO_CEE_BEQ();
            break;

        case CEE_BNE_UN_S:
            GET(ilrel, char, false);
            JitResult = compileDO_CEE_BNE();
            break;

        case CEE_BNE_UN:
            GET(ilrel, int, false);
            JitResult = compileDO_CEE_BNE();
            break;

        case CEE_BGT_S:
            GET(ilrel, char, false);
            JitResult = compileDO_CEE_BGT();
            break;

        case CEE_BGT:
            GET(ilrel, int, false);
            JitResult = compileDO_CEE_BGT();
            break;

        case CEE_BGT_UN_S:
            GET(ilrel, char, false);
            JitResult = compileDO_CEE_BGT_UN();
            break;

        case CEE_BGT_UN:
            GET(ilrel, int, false);
            JitResult = compileDO_CEE_BGT_UN();
            break;

        case CEE_BGE_S:
            GET(ilrel, char, false);
            JitResult = compileDO_CEE_BGE();
            break;

        case CEE_BGE:
            GET(ilrel, int, false);
            JitResult = compileDO_CEE_BGE();
            break;

        case CEE_BGE_UN_S:
            GET(ilrel, char, false);
            JitResult = compileDO_CEE_BGE_UN();
            break;

        case CEE_BGE_UN:
            GET(ilrel, int, false);
            JitResult = compileDO_CEE_BGE_UN();
            break;

        case CEE_BLT_S:
            GET(ilrel, char, false);
            JitResult = compileDO_CEE_BLT();
            break;

        case CEE_BLT:
            GET(ilrel, int, false);
            JitResult = compileDO_CEE_BLT();
            break;

        case CEE_BLT_UN_S:
            GET(ilrel, char, false);
            JitResult = compileDO_CEE_BLT_UN();
            break;

        case CEE_BLT_UN:
            GET(ilrel, int, false);
            JitResult = compileDO_CEE_BLT_UN();
            break;

        case CEE_BLE_S:
            GET(ilrel, char, false);
            JitResult = compileDO_CEE_BLE();
            break;

        case CEE_BLE:
            GET(ilrel, int, false);
            JitResult = compileDO_CEE_BLE();
            break;

        case CEE_BLE_UN_S:
            GET(ilrel, char, false);
            JitResult = compileDO_CEE_BLE_UN();
            break;

        case CEE_BLE_UN:
            GET(ilrel, int, false);
            JitResult = compileDO_CEE_BLE_UN();
            break;

        case CEE_BREAK:
            emit_break();
            break;

        case CEE_AND:
            JitResult = compileCEE_AND();
            break;

        case CEE_OR:
            JitResult = compileCEE_OR();
            break;

        case CEE_XOR:
            JitResult = compileCEE_XOR();
            break;

        case CEE_NOT:
            JitResult = compileCEE_NOT();
            break;

        case CEE_SHR:
            JitResult = compileCEE_SHR();
            break;

        case CEE_SHR_UN:
            JitResult = compileCEE_SHR_UN();
            break;

        case CEE_SHL:
            JitResult = compileCEE_SHL();
            break;

        case CEE_DUP:
            JitResult = compileCEE_DUP();
            break;

        case CEE_POP:
            JitResult = compileCEE_POP();
            break;

        case CEE_NOP:
            emit_il_nop();
            break;

        case CEE_LDARGA_S:
            GET(offset, signed char, false);
            JitResult = compileDO_LDARGA(opcode, offset);
            break;

        case CEE_LDARGA:
            GET(offset, unsigned short, false);
            JitResult = compileDO_LDARGA(opcode, offset);
            break;

        case CEE_REFANYVAL:
            JitResult = compileCEE_REFANYVAL();
            break;

        case CEE_REFANYTYPE:
            JitResult = compileCEE_REFANYTYPE();
            break;

        case CEE_ARGLIST:
            JitResult = compileCEE_ARGLIST();
            break;

        case CEE_ILLEGAL:
            VALIDITY_CHECK(false);
            break;

        case CEE_CALLI:
            JitResult = compileCEE_CALLI();
            break;

        case CEE_CALL:
            JitResult = compileCEE_CALL();
            break;

        case CEE_CALLVIRT:
            JitResult = compileCEE_CALLVIRT();
            break;

        case CEE_CASTCLASS:
            JitResult = compileCEE_CASTCLASS();
            break;

        case CEE_CONV_I1:
            JitResult = compileCEE_CONV_I1();
            break;

        case CEE_CONV_I2:
            JitResult = compileCEE_CONV_I2();
            break;

        emit_WIN32(case CEE_CONV_I:)
        case CEE_CONV_I4:
            JitResult = compileCEE_CONV_I4();
            break;

        case CEE_CONV_U1:
            JitResult = compileCEE_CONV_U1();
            break;

        case CEE_CONV_U2:
            JitResult = compileCEE_CONV_U2();
            break;

        emit_WIN32(case CEE_CONV_U:)
        case CEE_CONV_U4:
            JitResult = compileCEE_CONV_U4();
            break;

        emit_WIN64(case CEE_CONV_I:)
        case CEE_CONV_I8:
            JitResult = compileCEE_CONV_I8();
            break;

        emit_WIN64(case CEE_CONV_U:)
        case CEE_CONV_U8:
            JitResult = compileCEE_CONV_U8();
            break;

        case CEE_CONV_R4:
            JitResult = compileCEE_CONV_R4();
            break;

        case CEE_CONV_R8:
            JitResult = compileCEE_CONV_R8();
            break;

        case CEE_CONV_R_UN:
            JitResult = compileCEE_CONV_R_UN();
            break;

        case CEE_CONV_OVF_I1:
            JitResult = compileCEE_CONV_OVF_I1();
            break;

        case CEE_CONV_OVF_U1:
            JitResult = compileCEE_CONV_OVF_U1();
            break;

        case CEE_CONV_OVF_I2:
            JitResult = compileCEE_CONV_OVF_I2();
            break;

        case CEE_CONV_OVF_U2:
            JitResult = compileCEE_CONV_OVF_U2();
            break;

        emit_WIN32(case CEE_CONV_OVF_I:)
        case CEE_CONV_OVF_I4:
            JitResult = compileCEE_CONV_OVF_I4();
            break;

        emit_WIN32(case CEE_CONV_OVF_U:)
        case CEE_CONV_OVF_U4:
            JitResult = compileCEE_CONV_OVF_U4();
            break;

        emit_WIN64(case CEE_CONV_OVF_I:)
        case CEE_CONV_OVF_I8:
            JitResult = compileCEE_CONV_OVF_I8();
            break;

        emit_WIN64(case CEE_CONV_OVF_U:)
        case CEE_CONV_OVF_U8:
            JitResult = compileCEE_CONV_OVF_U8();
            break;

        case CEE_CONV_OVF_I1_UN:
            JitResult = compileCEE_CONV_OVF_I1_UN();
            break;

        case CEE_CONV_OVF_U1_UN:
            JitResult = compileCEE_CONV_OVF_U1_UN();
            break;

        case CEE_CONV_OVF_I2_UN:
            JitResult = compileCEE_CONV_OVF_I2_UN();
            break;

        case CEE_CONV_OVF_U2_UN:
            JitResult = compileCEE_CONV_OVF_U2_UN();
            break;

        emit_WIN32(case CEE_CONV_OVF_I_UN:)
        case CEE_CONV_OVF_I4_UN:
            JitResult = compileCEE_CONV_OVF_I4_UN();
            break;

        emit_WIN32(case CEE_CONV_OVF_U_UN:)
        case CEE_CONV_OVF_U4_UN:
            JitResult = compileCEE_CONV_OVF_U4_UN();
            break;

        emit_WIN64(case CEE_CONV_OVF_I_UN:)
        case CEE_CONV_OVF_I8_UN:
            JitResult = compileCEE_CONV_OVF_I8_UN();
            break;

        emit_WIN64(case CEE_CONV_OVF_U_UN:)
        case CEE_CONV_OVF_U8_UN:
            JitResult = compileCEE_CONV_OVF_U8_UN();
            break;

        case CEE_LDTOKEN:
            JitResult = compileCEE_LDTOKEN();
            break;

        case CEE_BOX:
            JitResult = compileCEE_BOX();
            break;

        case CEE_UNBOX:
            JitResult = compileCEE_UNBOX();
            break;

        case CEE_UNBOX_ANY:
            JitResult = compileCEE_UNBOX_ANY();
            break;

        case CEE_CONSTRAINED:
            JitResult = compileCEE_CONSTRAINED();
            break;

        case CEE_READONLY:
            JitResult = compileCEE_READONLY();
            break;

        case CEE_ISINST:
            JitResult = compileCEE_ISINST();
            break;

        case CEE_JMP:
            JitResult = compileCEE_JMP();
            break;

        case CEE_LDELEM_U1:
            JitResult = compileCEE_LDELEM_U1();
            break;

        case CEE_LDELEM_U2:
            JitResult = compileCEE_LDELEM_U2();
            break;

        case CEE_LDELEM_U4:
            JitResult = compileCEE_LDELEM_U4();
            break;

        case CEE_LDELEM_I:
            JitResult = compileCEE_LDELEM_I();
            break;

        case CEE_LDELEM_I1:
            JitResult = compileCEE_LDELEM_I1();
            break;

        case CEE_LDELEM_I2:
            JitResult = compileCEE_LDELEM_I2();
            break;

        case CEE_LDELEM_I4:
            JitResult = compileCEE_LDELEM_I4();
            break;

        case CEE_LDELEM_I8:
            JitResult = compileCEE_LDELEM_I8();
            break;

        case CEE_LDELEM_R4:
            JitResult = compileCEE_LDELEM_R4();
            break;

        case CEE_LDELEM_R8:
            JitResult = compileCEE_LDELEM_R8();
            break;

        case CEE_LDELEM_REF:
            JitResult = compileCEE_LDELEM_REF();
            break;

        case CEE_LDELEM:
            JitResult = compileCEE_LDELEM();
            break;

        case CEE_LDELEMA:
            JitResult = compileCEE_LDELEMA();
            break;

        case CEE_LDSFLD:
        case CEE_LDFLD:
            JitResult = compileCEE_LDFLD(opcode);
            break;

        case CEE_LDFLDA:
        case CEE_LDSFLDA:
            JitResult = compileCEE_LDFLDA(opcode);
            break;

        case CEE_STSFLD:
        case CEE_STFLD:
            JitResult = compileCEE_STFLD(opcode);
            break;

        case CEE_LDFTN:
            JitResult = compileCEE_LDFTN();
            break;

        case CEE_LDLEN:
            JitResult = compileCEE_LDLEN();
            break;

        case CEE_LDVIRTFTN:
            JitResult = compileCEE_LDVIRTFTN();
            break;

        case CEE_NEWARR:
            JitResult = compileCEE_NEWARR();
            break;

        case CEE_NEWOBJ:
            JitResult = compileCEE_NEWOBJ();
            break;

        case CEE_ENDFILTER:
            JitResult = compileCEE_ENDFILTER();
            break;

        case CEE_ENDFINALLY:
            JitResult = compileCEE_ENDFINALLY();
            break;

        case CEE_RET:
            JitResult = compileCEE_RET();
            break;

        case CEE_STELEM_I1:
            JitResult = compileCEE_STELEM_I1();
            break;

        case CEE_STELEM_I2:
            JitResult = compileCEE_STELEM_I2();
            break;

        case CEE_STELEM_I4:
            JitResult = compileCEE_STELEM_I4();
            break;

        case CEE_STELEM_I8:
            JitResult = compileCEE_STELEM_I8();
            break;

        case CEE_STELEM_I:
            JitResult = compileCEE_STELEM_I();
            break;

        case CEE_STELEM_R4:
            JitResult = compileCEE_STELEM_R4();
            break;

        case CEE_STELEM_R8:
            JitResult = compileCEE_STELEM_R8();
            break;

        case CEE_STELEM_REF:
            JitResult = compileCEE_STELEM_REF();
            break;

        case CEE_STELEM:
            JitResult = compileCEE_STELEM();
            break;

        case CEE_CKFINITE:
            VERIFICATION_CHECK(topOp().enum_() == typeR8);
            emit_CKFINITE_R();
            break;

        case CEE_SWITCH:
            JitResult = compileCEE_SWITCH();
            break;

        case CEE_THROW:
            JitResult = compileCEE_THROW();
            break;

        case CEE_RETHROW:
            JitResult = compileCEE_RETHROW();
            break;

        case CEE_TAILCALL:
            JitResult = compileCEE_TAILCALL();
            break;

        case CEE_UNALIGNED:
            JitResult = compileCEE_UNALIGNED();
            break;

        case CEE_VOLATILE:
            break;      // since we neither cache reads or suppress writes this is a nop

        default:
            VALIDITY_CHECK(false INDEBUG(&& "Invalid opcode detected\n"));

        } // switch statement

        // Reset unaligned access flag
        UnalignedAccess = false;

        // Check if we are currently at a post instruction sequence point
        emitSequencePointPost( InstStart, OPCODE(InstStart) );

        // Check to see if anything failed
        if (JitResult != FJIT_OK)
        {
            if (JitResult == FJIT_JITAGAIN)
                goto JitAgain;
            else
                return JitResult;
        }
    }

END_JIT_LOOP:
    // INDEBUG(printf("Done jitting function %s::%s \n",szDebugClassName, szDebugMethodName );)
    /*Note: from here to the end, we must not do anything that effects what may have been
    loaded via an emit_loadresult_()<type> previously.  We are just going to emit the epilog. */


    if (JitVerify)
        jitInfo->setMethodAttribs(methodInfo->ftn, IsVerifiableCode ? CORINFO_FLG_VERIFIABLE
                                                                    : CORINFO_FLG_UNVERIFIABLE);

    mapping->add(len, (outPtr-outBuff));

    if (flags & CORJIT_FLG_PROF_ENTERLEAVE)
    {
        BOOL bHookFunction;
        void *eeHandle;
        void *profilerHandle;
        BOOL bIndirected;
        jitInfo->GetProfilingHandle(methodHandle,
                                    &bHookFunction,
                                    &eeHandle,
                                    &profilerHandle,
                                    &bIndirected);

        if (bHookFunction)
        {
            _ASSERTE(!bIndirected); // FJIT does not handle indirected case

            inRegTOS = true;        // lie so that eax is always saved
            emit_save_TOS();        // squirel away the return value, this is safe since GC cannot happen
                                    // until we finish the epilog
            emit_POP_PTR();         // and remove from stack
            ULONG func = (ULONG) jitInfo->getHelperFtn(CORINFO_HELP_PROF_FCN_LEAVE);
            _ASSERTE(func != NULL);
            emit_callhelper_prof4(func,
                                  CORINFO_HELP_PROF_FCN_LEAVE,
                                  eeHandle,
                                  profilerHandle,
                                  NULL,  // FRAME_INFO (see definition of FunctionLeave2 in corprof.idl)
                                  NULL); // RET_VAL (see definition of FunctionLeave2 in corprof.idl)
            emit_restore_TOS();
        }
    }

    // Add a pcoffset for the epilog of the function to the map
    mapping->add(len+1,(unsigned)(outPtr - outBuff));

    // the single epilog that all returns jump to ( by convention caller always removes arguments for vararg calls )
    compileEpilog(methodInfo->args.isVarArg() ? 0 : argsFrameSize );

    // Verify that none of the jump target point in the middle of an opcode
    VALIDITY_CHECK( fixupTable->verifyTargets(mapping) );
    fixupTable->setSavedIP(storedStartIP);

    // Fill in the intermediate IL offsets (in the body of the opcode)
    mapping->fillIn();

    // Set the method size and epilog size information
    mapInfo.methodSize = outPtr-outBuff;
    unsigned EndPCOffset = mapping->pcFromIL( len + 1 );
    VALIDITY_CHECK( EndPCOffset > 0 );
    mapInfo.epilogSize = (outPtr - outBuff) - EndPCOffset;

    *codeSize = outPtr - outBuff;

    if(cSequencePoints > 0)
        cleanupSequencePoints(jitInfo,sequencePointOffsets);
    return codeGenState;
}

//
// Various defines used by helper functions
//

        // operations that can take any type including value classes
#define TYPE_SWITCH(type, emit, args)                                    \
            switch (type.enum_()) {                                      \
                emit_WIN32(case typeByRef:)                              \
                emit_WIN32(case typeRef:)                                \
                emit_WIN32(case typeMethod:)                             \
                case typeI4:                                             \
                    emit##_I4 args;                                      \
                    break;                                               \
                emit_WIN64(case typeByRef:)                              \
                emit_WIN64(case typeRef:)                                \
                emit_WIN64(case typeMethod:)                             \
                case typeI8:                                             \
                    emit##_I8 args;                                      \
                    break;                                               \
                case typeR4:                                             \
                    emit##_R4 args;                                      \
                    break;                                               \
                case typeR8:                                             \
                    emit##_R8 args;                                      \
                    break;                                               \
                case typeRefAny:                                         \
                    emit##_VC (args, (CORINFO_CLASS_HANDLE)type.enum_() ); \
                    break;                                               \
                default:                                                 \
                    INDEBUG(if (!type.isValClass()) printf("FAILED: Type is %d Cls %d \n", type.enum_(), type.cls() );) \
                    VALIDITY_CHECK(type.isValClass());                   \
                    emit##_VC (args, type.cls())                         \
                }

#define TYPE_SWITCH_Bcc(CItest,CRtest,BjmpCond,CjmpCond,AllowPtr)       \
        {                                                               \
            FJitResult    JitResult;                                  \
            int             op;                                         \
    /* not need to check stack since there is a following pop that checks it */ \
            if (ilrel < 0) {                                      \
                emit_trap_gc();                                         \
            }                                                           \
            switch (topOp().enum_()) {                            \
                emit_WIN32(case typeByRef:)                             \
                emit_WIN32(case typeRef:)                               \
                emit_WIN32(case typeMethod:)                            \
                case typeI4:                                            \
                    emit_BR_I4(CItest##_I4,CjmpCond,BjmpCond,op);       \
                    POP_STACK(2);                                       \
                    JitResult = compileDO_JMP(op); \
                    if (JitResult != FJIT_OK)                         \
                        return JitResult;                               \
                    break;                                              \
                emit_WIN64(case typeByRef:)                             \
                emit_WIN64(case typeRef:)                               \
                emit_WIN64(case typeMethod:)                            \
                emit_WIN64(_ASSERTE(AllowPtr || topOp(1).enum_() == typeI8);) \
                case typeI8:                                            \
                    emit_BR_I8(CItest##_I8,CjmpCond,BjmpCond,op);       \
                    POP_STACK(2);                                       \
                    JitResult = compileDO_JMP(op); \
                    if (JitResult != FJIT_OK)                         \
                        return JitResult;                               \
                    break;                                              \
                case typeR8:                                            \
                    emit_BR_R8(CRtest##_R8,CjmpCond,BjmpCond,op);       \
                    POP_STACK(2);                                       \
                    JitResult = compileDO_JMP(op); \
                    if (JitResult != FJIT_OK)                   \
                        return JitResult;                               \
                    break;                                              \
                default:                                                \
                    VALIDITY_CHECK(false)                               \
                    break;                                              \
            }                                                           \
        }                                                               \

        // operations that can take any type including value classes and small types
#define TYPE_SWITCH_PRECISE(type, emit, args)                            \
            switch (type.enum_()) {                                      \
                case typeU1:                                             \
                    emit##_U1 args;                                      \
                    break;                                               \
                case typeU2:                                             \
                    emit##_U2 args;                                      \
                    break;                                               \
                case typeI1:                                             \
                    emit##_I1 args;                                      \
                    break;                                               \
                case typeI2:                                             \
                    emit##_I2 args;                                      \
                    break;                                               \
                emit_WIN32(case typeByRef:)                              \
                emit_WIN32(case typeRef:)                                \
                emit_WIN32(case typeMethod:)                             \
                case typeI4:                                             \
                    emit##_I4 args;                                      \
                    break;                                               \
                emit_WIN64(case typeByRef:)                              \
                emit_WIN64(case typeRef:)                                \
                emit_WIN64(case typeMethod:)                             \
                case typeI8:                                             \
                    emit##_I8 args;                                      \
                    break;                                               \
                case typeR4:                                             \
                    emit##_R4 args;                                      \
                    break;                                               \
                case typeR8:                                             \
                    emit##_R8 args;                                      \
                    break;                                               \
                case typeRefAny:                                         \
                    emit##_VC (args, (CORINFO_CLASS_HANDLE)type.enum_() ); \
                    break;                                               \
                default:                                                 \
                    INDEBUG(if (!type.isValClass()) printf("FAILED: Type is %d \n", type.enum_() );) \
                    VALIDITY_CHECK(type.isValClass());                         \
                    emit##_VC (args,  type.cls() );                      \
                }

        // operations that can take number I or R
#define TYPE_SWITCH_ARITH(type, emit, args)                              \
            /* no need to check stack here because the following pop will check it */ \
            switch (type.enum_()) {                                      \
                case typeI4:                                             \
                        emit##_I4 args;                                  \
                        break;                                           \
                case typeI8:                                             \
                        emit##_I8 args;                                  \
                        break;                                           \
                case typeR8:                                             \
                        emit##_R8 args;                                  \
                        break;                                           \
                case typeRef:                                            \
                case typeByRef:                                          \
                case typeMethod:                                         \
                        emit_WIN32(emit##_I4 args;) emit_WIN64(emit##_I8 args;) \
                        break;                                           \
                default:                                                 \
                    FJIT_FAIL(FJIT_INTERNALERROR);                       \
                }

        // operations that work on pointers and numbers(eg add sub)
#define TYPE_SWITCH_PTR(type, emit, args)                                \
            /* no need to check stack here because the following pop will check it */ \
            switch (type.enum_()) {                                      \
                emit_WIN32(case typeByRef:)                              \
                emit_WIN32(case typeRef:)                                \
                emit_WIN32(case typeMethod:)                             \
                case typeI4:                                             \
                    emit##_I4 args;                                      \
                    break;                                               \
                emit_WIN64(case typeByRef:)                              \
                emit_WIN64(case typeRef:)                                \
                emit_WIN64(case typeMethod:)                             \
                case typeI8:                                             \
                    emit##_I8 args;                                      \
                    break;                                               \
                case typeR8:                                             \
                    emit##_R8 args;                                      \
                    break;                                               \
                default:                                                 \
                    FJIT_FAIL(FJIT_INTERNALERROR);                       \
                }

#define TYPE_SWITCH_INT(type, emit, args)                                \
            /* no need to check stack here because the following pop will check it */ \
            switch (type.enum_()) {                                      \
                case typeI4:                                             \
                    emit##_I4 args;                                      \
                    break;                                               \
                case typeI8:                                             \
                    emit##_I8 args;                                      \
                    break;                                               \
                default:                                                 \
                    FJIT_FAIL(FJIT_INTERNALERROR);                       \
                }

    // operations that can take just integers I
#define TYPE_SWITCH_LOGIC(type, emit, args)                              \
            /* no need to check stack here because the following pop will check it */ \
            switch (type.enum_()) {                                      \
                case typeI4:                                             \
                    emit##_U4 args;                                      \
                    break;                                               \
                case typeI8:                                             \
                    emit##_U8 args;                                      \
                    break;                                               \
                default:                                                 \
                    FJIT_FAIL(FJIT_INTERNALERROR);                       \
                }


FJitResult FJit::compileCEE_MUL()
{
    OpType result_mul;
    BINARY_NUMERIC_RESULT(topOp(),topOp(1), CEE_MUL, result_mul);
    TYPE_SWITCH_ARITH(topOp(), emit_MUL, ());
    POP_STACK(2);
    pushOp(result_mul);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_MUL_OVF()
{
    OpType result_mul;
    BINARY_OVERFLOW_RESULT(topOp(),topOp(1), CEE_MUL_OVF, result_mul);
    OpType type = topOp();
    POP_STACK(2);
    TYPE_SWITCH_INT(type, emit_MUL_OVF, ());
    pushOp(result_mul);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_MUL_OVF_UN()
{
    OpType result_mul;
    BINARY_OVERFLOW_RESULT(topOp(),topOp(1), CEE_MUL_OVF_UN, result_mul);
    TYPE_SWITCH_LOGIC(topOp(), emit_MUL_OVF, ());
    POP_STACK(2);
    pushOp(result_mul);
    return FJIT_OK;
}


FJitResult FJit::compileCEE_DIV()
{
    OpType result_div;
    BINARY_NUMERIC_RESULT(topOp(),topOp(1), CEE_DIV, result_div);
    TYPE_SWITCH_ARITH(topOp(), emit_DIV, ());
    POP_STACK(2);
    pushOp(result_div);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_DIV_UN()
{
    INTEGER_OPERATIONS(topOp(), topOp(1), CEE_DIV_UN)
    TYPE_SWITCH_LOGIC(topOp(), emit_DIV_UN, ());
    POP_STACK(1); // The resulting type is always the same as the types of the operands
    return FJIT_OK;
}

FJitResult FJit::compileCEE_ADD()
{
    OpType result_add;
    BINARY_NUMERIC_RESULT(topOp(),topOp(1), CEE_ADD, result_add);
    TYPE_SWITCH_PTR(topOp(), emit_ADD, ());
    POP_STACK(2);
    pushOp(result_add);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_ADD_OVF()
{
    OpType result_add;
    BINARY_OVERFLOW_RESULT(topOp(),topOp(1), CEE_ADD_OVF, result_add);
    TYPE_SWITCH_INT(topOp(), emit_ADD_OVF, ());
    POP_STACK(2);
    pushOp(result_add);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_ADD_OVF_UN()
{
    OpType result_add;
    BINARY_OVERFLOW_RESULT(topOp(),topOp(1), CEE_ADD_OVF_UN, result_add);
    TYPE_SWITCH_LOGIC(topOp(), emit_ADD_OVF, ());
    POP_STACK(2);
    pushOp(result_add);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_SUB()
{
    OpType result_sub;
    BINARY_NUMERIC_RESULT(topOp(),topOp(1), CEE_SUB, result_sub);
    TYPE_SWITCH_PTR(topOp(), emit_SUB, ());
    POP_STACK(2);
    pushOp(result_sub);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_REM()
{
    OpType result_rem;
    BINARY_NUMERIC_RESULT(topOp(),topOp(1), CEE_DIV, result_rem);
    TYPE_SWITCH_ARITH(topOp(), emit_REM, ());
    POP_STACK(2);
    pushOp(result_rem);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_REM_UN()
{
    INTEGER_OPERATIONS(topOp(), topOp(1), CEE_REM_UN)
    TYPE_SWITCH_LOGIC(topOp(), emit_REM_UN, ());
    POP_STACK(1); // The resulting type is always the same as the types of the operands
    return FJIT_OK;
}

FJitResult FJit::compileCEE_SUB_OVF()
{
    OpType result_sub;
    BINARY_OVERFLOW_RESULT(topOp(),topOp(1), CEE_SUB_OVF, result_sub);
    TYPE_SWITCH_INT(topOp(), emit_SUB_OVF, ());
    POP_STACK(2);
    pushOp(result_sub);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_SUB_OVF_UN()
{
    OpType result_sub;
    BINARY_OVERFLOW_RESULT(topOp(),topOp(1), CEE_SUB_OVF_UN, result_sub);
    TYPE_SWITCH_LOGIC(topOp(), emit_SUB_OVF, ());
    POP_STACK(2);
    pushOp(result_sub);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDIND_U1()
{
    emit_LDIND_U1(false);
    CHECK_STACK(1);
    /*VALIDITY_CHECK( topOpE() == typeByRef || topOpE() == typeI ); - Matching V1 .NET Framework */
    VERIFICATION_CHECK(topOpE() == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, topOp().getTarget(), OpType(typeU1)));
    POP_STACK(1);
    OpType LoadedType(typeU1, NULL);
    LoadedType.toFPNormalizedType();
    pushOp(LoadedType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDIND_U2()
{
    emit_LDIND_U2(UnalignedAccess);
    CHECK_STACK(1);
    /*VALIDITY_CHECK( topOpE() == typeByRef || topOpE() == typeI ); - Matching V1 .NET Framework */
    VERIFICATION_CHECK(topOpE() == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, topOp().getTarget(), OpType(typeU2)));
    POP_STACK(1);
    OpType LoadedType(typeU2, NULL);
    LoadedType.toFPNormalizedType();
    pushOp(LoadedType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDIND_I1()
{
    emit_LDIND_I1(false);
    CHECK_STACK(1);
    /*VALIDITY_CHECK( topOpE() == typeByRef || topOpE() == typeI ); - Matching V1 .NET Framework */
    VERIFICATION_CHECK(topOpE() == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, topOp().getTarget(), OpType(typeI1)));
    POP_STACK(1);
    OpType LoadedType(typeI1, NULL);
    LoadedType.toFPNormalizedType();
    pushOp(LoadedType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDIND_I2()
{
    emit_LDIND_I2(UnalignedAccess);
    CHECK_STACK(1);
    /*VALIDITY_CHECK( topOpE() == typeByRef || topOpE() == typeI ); - Matching V1 .NET Framework */
    VERIFICATION_CHECK(topOpE() == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, topOp().getTarget(), OpType(typeI2)));
    POP_STACK(1);
    OpType LoadedType(typeI2, NULL);
    LoadedType.toFPNormalizedType();
    pushOp(LoadedType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDIND_I4()
{
    emit_LDIND_I4(UnalignedAccess);
    CHECK_STACK(1);
    /*VALIDITY_CHECK( topOpE() == typeByRef || topOpE() == typeI ); - Matching V1 .NET Framework */
    VERIFICATION_CHECK(topOpE() == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, topOp().getTarget(), OpType(typeI4)));
    POP_STACK(1);
    OpType LoadedType(typeI4, NULL);
    LoadedType.toFPNormalizedType();
    pushOp(LoadedType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDIND_U4()
{
    emit_LDIND_U4(UnalignedAccess);
    CHECK_STACK(1);
    /*VALIDITY_CHECK( topOpE() == typeByRef || topOpE() == typeI ); - Matching V1 .NET Framework */
    VERIFICATION_CHECK(topOpE() == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, topOp().getTarget(), OpType(typeI4)));
    POP_STACK(1);
    OpType LoadedType(typeI4, NULL);
    LoadedType.toFPNormalizedType();
    pushOp(LoadedType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDIND_I8()
{
    emit_LDIND_I8(UnalignedAccess);
    CHECK_STACK(1);
    /*VALIDITY_CHECK( topOpE() == typeByRef || topOpE() == typeI ); - Matching V1 .NET Framework */
    VERIFICATION_CHECK(topOpE() == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, topOp().getTarget(), OpType(typeI8)));
    POP_STACK(1);
    OpType LoadedType(typeI8, NULL);
    LoadedType.toFPNormalizedType();
    pushOp(LoadedType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDIND_R4()
{
    emit_LDIND_R4(UnalignedAccess);
    CHECK_STACK(1);
    /*VALIDITY_CHECK( topOpE() == typeByRef || topOpE() == typeI ); - Matching V1 .NET Framework */
    VERIFICATION_CHECK(topOpE() == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, topOp().getTarget(), OpType(typeR4)));
    POP_STACK(1);
    OpType LoadedType(typeR4, NULL);
    LoadedType.toFPNormalizedType();
    pushOp(LoadedType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDIND_R8()
{
    emit_LDIND_R8(UnalignedAccess);
    CHECK_STACK(1);
    /*VALIDITY_CHECK( topOpE() == typeByRef || topOpE() == typeI ); - Matching V1 .NET Framework */
    VERIFICATION_CHECK(topOpE() == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, topOp().getTarget(), OpType(typeR8)));
    POP_STACK(1);
    OpType LoadedType(typeR8, NULL);
    LoadedType.toFPNormalizedType();
    pushOp(LoadedType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDIND_I()
{
    emit_LDIND_I(UnalignedAccess);
    CHECK_STACK(1);
    /*VALIDITY_CHECK( topOpE() == typeByRef || topOpE() == typeI ); - Matching V1 .NET Framework */
    VERIFICATION_CHECK(topOpE() == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, topOp().getTarget(), OpType(typeI)));
    POP_STACK(1);
    OpType LoadedType(typeI, NULL);
    LoadedType.toFPNormalizedType();
    pushOp(LoadedType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDIND_REF()
{
    emit_LDIND_PTR(UnalignedAccess);
    // There must be an object on the stack
    CHECK_STACK(1);
    // The pointer can be either a managed pointer or a natural int
    VALIDITY_CHECK( topOpE() == typeByRef || topOpE() == typeI );
    // Natural int can't be used as a pointer in verifiable code
    VERIFICATION_CHECK( topOpE() == typeByRef && topOp().cls() != NULL );
    CORINFO_CLASS_HANDLE cls = topOp().cls();
    POP_STACK(1);
    pushOp(OpType(typeRef, cls));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDFLD( OPCODE opcode)
{

    unsigned                address = 0;
    unsigned int            token, parentToken;
    DWORD                   fieldAttributes;
    CorInfoType             jitType;
    CORINFO_CLASS_HANDLE    targetClass = NULL, parentClass = NULL;
    bool                    fieldIsStatic;

    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_METHOD_HANDLE   tokenContext = methodInfo->ftn;
    CORINFO_FIELD_HANDLE    targetField;

    // Get MemberRef token for object field
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    targetField = jitInfo->findField (tokenScope, token,tokenContext);
    VALIDITY_CHECK(targetField);
    fieldAttributes = jitInfo->getFieldAttribs(targetField,methodInfo->ftn);

    fieldIsStatic =  (fieldAttributes & CORINFO_FLG_STATIC) ? true : false;
    targetClass = jitInfo->findClass(tokenScope, jitInfo->getMemberParent(tokenScope, token), tokenContext);
    VALIDITY_CHECK(targetClass);
    // targetClass is the enclosing class

    CORINFO_CLASS_HANDLE valClass;
    jitType = jitInfo->getFieldType(targetField, &valClass, targetClass);

    if (fieldIsStatic)
    {
        emit_initclass(targetClass);
    }

    OpType fieldType = createOpType(jitType, valClass );

    OpType type;
#if !defined(FJIT_NO_VALIDATION)


    // Initialize the type correctly getting additional information for managed pointers and objects
    if ( fieldType.enum_() == typeByRef )
    {
        _ASSERTE(valClass != NULL);
        CORINFO_CLASS_HANDLE childClassHandle;
        CorInfoType childType = jitInfo->getChildType(valClass, &childClassHandle);
        fieldType.setTarget(OpType(childType).enum_(),childClassHandle);
    }
    else if ( fieldType.enum_() == typeRef )
        VALIDITY_CHECK( valClass != NULL );
    // Verify that the correct type of the instruction is used
    VALIDITY_CHECK( fieldIsStatic || (opcode == CEE_LDFLD) );
    CORINFO_CLASS_HANDLE instanceClassHnd = jitInfo->getMethodClass(methodInfo->ftn);

    //INDEBUG(printf( "Field Type [%d, %d] %d \n",fieldType.enum_(),fieldType.cls(),valClass );)
#endif
    if (opcode == CEE_LDFLD)
    {
        // There must be an object on the stack
        CHECK_STACK(1);
        type = topOp();
        
        if (type.type_enum == typeR4 || type.type_enum == typeR8) {
            return FJIT_OK;    
        }
 
        // The object on the stack can be managed pointer, object, native int, instance of object
        VALIDITY_CHECK( type.isPtr() || type.enum_() == typeValClass );
        // Verification doesn't allow native int to be used
        VERIFICATION_CHECK( type.enum_() != typeI || (type.cls() && isPrimitiveValueType(type.cls())) );
        // Store the object reference for the access check
        instanceClassHnd = type.cls();
        OpType targetType = createOpType(type.enum_(), targetClass );

        // Check that the object on the stack encloses the field
        VERIFICATION_CHECK( canAssign( jitInfo, methodInfo->ftn, type, targetType));
        // Remove the instance object of the IL stack
        POP_STACK(1);
        if (fieldIsStatic) {
            // we don't need this pointer
            if (type.isValClass())
            {
                unsigned sizeValClass = typeSizeInSlots(jitInfo, type.cls()) * sizeof(void*);
                emit_drop(BYTE_ALIGNED(sizeValClass));
            }
            else
            {
                emit_POP_PTR();
            }
        }
        else
        {
            //INDEBUG(printf( "Object Type [%d, %d] \n",type.enum_(),type.cls() );)
        if (type.isValClass() || (type.enum_() == typeI && type.cls() && isPrimitiveValueType(type.cls())) )
            {                               // the object itself is a value class
                pushOp(type);               // we are going to leave it on the stack
                emit_getSP(STACK_BUFFER);   // push pointer to object
            }
        }
    }

    // get the exact parent of the field
    parentToken = jitInfo->getMemberParent(tokenScope, token);
    parentClass = jitInfo->findClass(tokenScope,
                                     parentToken,
                                     methodInfo->ftn);

    VERIFICATION_CHECK( jitInfo->satisfiesClassConstraints(parentClass));
    VERIFICATION_CHECK( jitInfo->canAccessField(methodInfo->ftn, parentClass, targetField, instanceClassHnd) );
    if(fieldAttributes & (CORINFO_FLG_HELPER | CORINFO_FLG_SHARED_HELPER | CORINFO_FLG_GENERICS_STATIC))
    {
        LABELSTACK((outPtr-outBuff),0); // Note this can be removed if these become fcalls
        if (fieldIsStatic)                  // static fields go through pointer
        {
                // Load up the address of the static
            CorInfoHelpFunc helperNum = jitInfo->getFieldHelper(targetField, CORINFO_ADDRESS);
            void* helperFunc = jitInfo->getHelperFtn(helperNum,NULL);
            emit_FIELDADDRHelper(helperFunc, targetField);

                // do the indirection
            return compileDO_LDIND_BYTYPE(fieldType, UnalignedAccess);
        }
        else {
            // get the helper
            CorInfoHelpFunc helperNum = jitInfo->getFieldHelper(targetField, CORINFO_GET);
            void* helperFunc = jitInfo->getHelperFtn(helperNum,NULL);
            _ASSERTE(helperFunc);

            // Force correct code gen for valuetypes which are treated
            // as primitives by the type system.
            if ( helperNum == CORINFO_HELP_GETFIELDSTRUCT && isPrimitiveValueType(valClass) )
          jitType = CORINFO_TYPE_VALUECLASS;

            switch (jitType)
            {
                case CORINFO_TYPE_BYTE:
                case CORINFO_TYPE_BOOL:
                case CORINFO_TYPE_CHAR:
                case CORINFO_TYPE_SHORT:
                case CORINFO_TYPE_INT:
                emit_WIN32(case CORINFO_TYPE_PTR:)
                emit_WIN32(case CORINFO_TYPE_NATIVEINT:)
                emit_WIN32(case CORINFO_TYPE_NATIVEUINT:)
                case CORINFO_TYPE_UBYTE:
                case CORINFO_TYPE_USHORT:
                case CORINFO_TYPE_UINT:
                    emit_LDFLD_helper(helperFunc,targetField);
                    emit_pushresult_I4();
                    break;
                case CORINFO_TYPE_FLOAT:
                    emit_LDFLD_helper(helperFunc,targetField);
                    emit_pushresult_R4();
                    break;
                emit_WIN64(case CORINFO_TYPE_PTR:)
                emit_WIN64(case CORINFO_TYPE_NATIVEINT:)
                emit_WIN64(case CORINFO_TYPE_NATIVEUINT:)
                case CORINFO_TYPE_LONG:
                case CORINFO_TYPE_ULONG:
                    emit_LDFLD_helper(helperFunc,targetField);
                    emit_pushresult_I8();
                    break;
                case CORINFO_TYPE_DOUBLE:
                    emit_LDFLD_helper(helperFunc,targetField);
                    emit_pushresult_R8();
                    break;
                case CORINFO_TYPE_CLASS:
                    emit_LDFLD_helper(helperFunc,targetField);
                    emit_pushresult_I4();
                    break;
                case CORINFO_TYPE_VALUECLASS: {
                    // allocate return buff, zeroing to make valid GC pointers
                    int slots = typeSizeInSlots(jitInfo, valClass);
                    slots = WORD_ALIGNED(slots);
                    pushOp(fieldType);
                    emit_GetFieldStruct(slots, targetField, helperFunc, valClass);

                    CHECK_POP_STACK(1);             // pop  return value
                    } break;
                default:
                    FJIT_FAIL(FJIT_INTERNALERROR);
                    break;
            }
        }
    }
    // else no helper for this field
    else {
        bool isEnCField = (fieldAttributes & CORINFO_FLG_EnC) ? true : false;
        if (fieldIsStatic)
        {
            {
                if (!(address = (unsigned) jitInfo->getFieldAddress(targetField)))
                    FJIT_FAIL(FJIT_INTERNALERROR);
                emit_pushconstant_Ptr(address);
            }
        }
        else // field is not static
        {
            if (opcode == CEE_LDSFLD)
                FJIT_FAIL(FJIT_INTERNALERROR);
            {
                address = jitInfo->getFieldOffset(targetField);
                emit_pushconstant_Ptr(address);
            }
            _ASSERTE(opcode == CEE_LDFLD); //if (opcode == CEE_LDFLD)
        }

        if (fieldAttributes & CORINFO_FLG_STATIC_IN_HEAP)
        {
            emit_LDFLD_REF(true, false);
            emit_LDC_I4(sizeof(void*));
            emit_WIN32(emit_ADD_I4()) emit_WIN64(emit_ADD_I8());
        }

        switch (jitType) {
        case CORINFO_TYPE_BYTE:
        case CORINFO_TYPE_BOOL:
            emit_LDFLD_I1((fieldIsStatic || isEnCField), false);
            break;
        case CORINFO_TYPE_SHORT:
            emit_LDFLD_I2((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        case CORINFO_TYPE_INT:
            emit_LDFLD_I4((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        case CORINFO_TYPE_FLOAT:
            emit_LDFLD_R4((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        case CORINFO_TYPE_UBYTE:
            emit_LDFLD_U1((fieldIsStatic || isEnCField), false);
            break;
        case CORINFO_TYPE_CHAR:
        case CORINFO_TYPE_USHORT:
            emit_LDFLD_U2((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        emit_WIN32(case CORINFO_TYPE_PTR:)
        emit_WIN32(case CORINFO_TYPE_NATIVEINT:)
        emit_WIN32(case CORINFO_TYPE_NATIVEUINT:)
        case CORINFO_TYPE_UINT:
            emit_LDFLD_U4((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        emit_WIN64(case CORINFO_TYPE_PTR:)
        emit_WIN64(case CORINFO_TYPE_NATIVEINT:)
        emit_WIN64(case CORINFO_TYPE_NATIVEUINT:)
        case CORINFO_TYPE_ULONG:
        case CORINFO_TYPE_LONG:
            emit_LDFLD_I8((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        case CORINFO_TYPE_DOUBLE:
            emit_LDFLD_R8((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        case CORINFO_TYPE_CLASS:
            emit_LDFLD_REF((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        case CORINFO_TYPE_VALUECLASS:
            if (!fieldIsStatic && !isEnCField) {
                emit_WIN32(emit_ADD_I4()) emit_WIN64(emit_ADD_I8(0));
            }
            emit_valClassLoad(valClass, UnalignedAccess);
            break;
        default:
            FJIT_FAIL(FJIT_INTERNALERROR);
            break;
        }

    }
    if (!fieldIsStatic &&
        ( type.isValClass() || (type.enum_() == typeI && type.cls() && isPrimitiveValueType(type.cls())) ) )
    {
        // at this point things are not quite right, the problem
        // is that we did not pop the original value class.  Thus
        // the stack is (..., obj, field), and we just want (..., field)
        // This code does the fixup.
        CHECK_POP_STACK(1);
        unsigned fieldSize;
        if (jitType == CORINFO_TYPE_VALUECLASS)
            fieldSize = typeSizeInSlots(jitInfo, valClass) * sizeof(void*);
        else
            fieldSize = computeArgSize(jitType, 0, 0);
        if (jitType == CORINFO_TYPE_FLOAT)
            fieldSize += sizeof(double) - sizeof(float);    // adjust for the fact that the float is promoted to double on the IL stack
        unsigned objSize = typeSizeInSlots(jitInfo, type.cls())*sizeof(void*);
        objSize = BYTE_ALIGNED(objSize);

        if (fieldSize <= sizeof(void*) && inRegTOS) {
            emit_drop(objSize);     // just get rid of the obj
            _ASSERTE(inRegTOS);     // make certain emit_drop does not deregister
        }
        else {
            deregisterTOS;
            emit_mov_arg_stack(objSize, 0, fieldSize);
            emit_drop(objSize);
        }
    }
    fieldType.toFPNormalizedType();
    pushOp(fieldType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDFLDA( OPCODE opcode)
{
    unsigned                address;
    bool                    fieldIsStatic;
    DWORD                   fieldAttributes;
    CORINFO_FIELD_HANDLE    targetField;
    CORINFO_CLASS_HANDLE    targetClass = NULL, parentClass = NULL;
    CorInfoType             jitType;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    unsigned int            token, parentToken;

    // Get MemberRef token for object field
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    targetField = jitInfo->findField (tokenScope, token,methodInfo->ftn);
    VALIDITY_CHECK(targetField);
    fieldAttributes = jitInfo->getFieldAttribs(targetField,methodInfo->ftn);
    targetClass = jitInfo->findClass(tokenScope, jitInfo->getMemberParent(tokenScope, token), methodInfo->ftn);
    VALIDITY_CHECK(targetClass);

    // get the exact parent of the field
    parentToken = jitInfo->getMemberParent(tokenScope, token);
    parentClass = jitInfo->findClass(tokenScope,
                                     parentToken,
                                     methodInfo->ftn);

    DWORD classAttribs = jitInfo->getClassAttribs(targetClass,methodInfo->ftn);
    fieldIsStatic = fieldAttributes & CORINFO_FLG_STATIC ? true : false;

#if !defined(FJIT_NO_VALIDATION)
    // Verify that the correct type of the instruction is used
    VERIFICATION_CHECK( fieldIsStatic || (opcode == CEE_LDFLDA) );
    // Verify that a not trying to use an RVA
    VERIFICATION_CHECK( !(CORINFO_FLG_UNMANAGED & fieldAttributes ) );

    // Verify that if final field can be accessed correctly
    if (( fieldAttributes & CORINFO_FLG_FINAL) )
    {
        VERIFICATION_CHECK( (methodAttributes & CORINFO_FLG_CONSTRUCTOR) &&
                            (((methodAttributes & CORINFO_FLG_STATIC) != 0) == fieldIsStatic) &&
                                targetClass == jitInfo->getMethodClass(methodInfo->ftn)
                            INDEBUG( || !"bad use of initonly field (address taken)") );
    }
    if (opcode == CEE_LDFLDA)
    {
        // There must be an instace object on the stack
        CHECK_STACK(1);
        // The object on the stack can be managed pointer, object, native int, instance of object
        VALIDITY_CHECK( topOp().isPtr() );
        // Verification doesn't allow native int to be used
        VERIFICATION_CHECK( topOpE() == typeRef || topOpE() == typeByRef );
        OpType TargetType = createOpType( topOpE(), targetClass );
        // Check that the object on the stack encloses the field
        VERIFICATION_CHECK( canAssign( jitInfo, methodInfo->ftn, topOp(), TargetType) );
        // Verify the field parent's constraints
        VERIFICATION_CHECK( jitInfo->satisfiesClassConstraints(parentClass));
        // Verify that the field is accessible
        VERIFICATION_CHECK( jitInfo->canAccessField(methodInfo->ftn, parentClass, targetField, topOp().cls()) );
        // Remove the instance object from the stack
        POP_STACK(1);
    }
    else
    {
        // Verify the field parent's constraints
        VERIFICATION_CHECK( jitInfo->satisfiesClassConstraints(parentClass));
        // Verify that the field is accessible
        VERIFICATION_CHECK( jitInfo->canAccessField(methodInfo->ftn, parentClass, targetField,
                                                        jitInfo->getMethodClass(methodInfo->ftn)) );
    }
    // Determine the type of the field and verify that it is not a pointer
    CORINFO_CLASS_HANDLE fieldClass;
    jitType = jitInfo->getFieldType(targetField, &fieldClass, targetClass);
    OpType fieldType = createOpType(jitType, fieldClass);

    // Verify that we are not trying to obtain an address of a managed pointer
    VERIFICATION_CHECK( fieldType.enum_() != typeByRef );
    // If this is an object the handle must be initialized
    VALIDITY_CHECK( fieldType.enum_() != typeRef || fieldClass != NULL );
    // Construct the return type
    OpType RetType( typeByRef );
    RetType.setTarget( fieldType.enum_(), fieldClass );
#else
    if (opcode == CEE_LDFLDA)
        CHECK_POP_STACK(1);
    OpType RetType( typeI );
#endif

    if (fieldIsStatic)
    {
        if (opcode == CEE_LDFLDA)
        {
            emit_POP_PTR();
        }
        emit_initclass(targetClass);

        if (fieldAttributes & (CORINFO_FLG_HELPER | CORINFO_FLG_SHARED_HELPER | CORINFO_FLG_GENERICS_STATIC))
        {
            _ASSERTE((fieldAttributes & CORINFO_FLG_EnC) == 0);
            // get the helper
            CorInfoHelpFunc helperNum = jitInfo->getFieldHelper(targetField,CORINFO_ADDRESS);
            void* helperFunc = jitInfo->getHelperFtn(helperNum,NULL);
            _ASSERTE(helperFunc);
            emit_FIELDADDRHelper(helperFunc, targetField);
        }
        else
        {
            address = (unsigned) jitInfo->getFieldAddress(targetField);
            VALIDITY_CHECK(address);
            emit_pushconstant_Ptr(address);

            if (fieldAttributes & CORINFO_FLG_STATIC_IN_HEAP)
            {
                emit_LDFLD_REF(true, false);
                emit_LDC_I4(sizeof(void*));
                emit_WIN32(emit_ADD_I4()) emit_WIN64(emit_ADD_I8());
            }
        }
    }
    else
    {
        VALIDITY_CHECK( opcode != CEE_LDSFLDA );
        if (fieldAttributes & (CORINFO_FLG_HELPER | CORINFO_FLG_SHARED_HELPER | CORINFO_FLG_GENERICS_STATIC))
        {
            // The GC may occur inside the helper so save the stack
            LABELSTACK((outPtr-outBuff),0);
            // Get the helper
            CorInfoHelpFunc helperNum = jitInfo->getFieldHelper(targetField,CORINFO_ADDRESS);
            void* helperFunc = jitInfo->getHelperFtn(helperNum,NULL);
            _ASSERTE(helperFunc);
            emit_LDFLD_helper(helperFunc,targetField);
            emit_pushresult_I4();
        }
        else
        {
            address = jitInfo->getFieldOffset(targetField); // Get the offset of the field
            emit_check_null_reference(true);            // Check that the object pointer is not null
            emit_pushconstant_Ptr(address);                 // Push the field offset onto the stack
            emit_WIN32(emit_ADD_I4()) emit_WIN64(emit_ADD_I8()); // Add the offset to the object pointer
        }
    }
    if ((classAttribs & CORINFO_FLG_UNMANAGED) || (fieldAttributes & CORINFO_FLG_UNMANAGED))
    {
        VERIFICATION_CHECK(false INDEBUG( && "Unmanaged ptr"));
        pushOp( OpType(typeI) );
    }
    else
        pushOp( RetType );

    return FJIT_OK;
}

FJitResult FJit::compileDO_LDVAR( OPCODE opcode, stackItems* varInfo)
{
    OpType          trackedType;

    TYPE_SWITCH_PRECISE(varInfo->type, emit_LDVAR, (varInfo->offset));
    trackedType = varInfo->type;
    trackedType.toFPNormalizedType();
    pushOp(trackedType);
    return FJIT_OK;
}

FJitResult FJit::compileDO_LDVARA( OPCODE opcode, stackItems* varInfo)
{
    unsigned size = typeSizeFromJitType(varInfo->type.enum_(), true);
    if ( size == 0 )
        size = !PASS_VALUETYPE_BYREF||varInfo->offset < 0 ? typeSizeInBytes( jitInfo, varInfo->type.cls() ) : sizeof(void *);
    unsigned varOffset = varInfo->offset;
    emit_LDVARA(varOffset, size);
    // If by value composite structures are passed as pointers to a temporary copy - dereference to get the true address
    if ( PASS_VALUETYPE_BYREF && (varInfo->type.enum_() == typeRefAny || varInfo->type.enum_() == typeValClass)
         && varInfo->offset > 0 )
         { mov_register_indirect_to(TOS_REG_1,TOS_REG_1); }
    VERIFICATION_CHECK( varInfo->type.enum_() != typeByRef );
    OpType PtrType(typeByRef);
    PtrType.setTarget( varInfo->type.enum_(), varInfo->type.cls() );
    pushOp(PtrType);
    return FJIT_OK;
}

FJitResult FJit::compileDO_LDFTN(
    unsigned int token,
    CORINFO_METHOD_HANDLE targetMethod)
{
    CORINFO_LOOKUP addrInfo;
    CORINFO_SIG_INFO        targetSigInfo;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_METHOD_HANDLE   tokenContext = methodInfo->ftn;

    jitInfo->getFunctionFixedEntryPointInfo(tokenScope, token, tokenContext, &addrInfo);
    VALIDITY_CHECK(!addrInfo.lookupKind.needsRuntimeLookup);
    VALIDITY_CHECK(addrInfo.constLookup.accessType == IAT_VALUE);
    VALIDITY_CHECK (addrInfo.constLookup.addr != 0);

    VALIDITY_CHECK((jitInfo->getMethodSig(targetMethod, &targetSigInfo), !targetSigInfo.hasTypeArg()));
    emit_WIN32(emit_LDC_I4(addrInfo.constLookup.addr)) emit_WIN64(emit_LDC_I8(addrInfo.addr));
    pushOp(OpType(targetMethod));
    return FJIT_OK;
}

FJitResult FJit::compileDO_LDLOC( OPCODE opcode, unsigned offset)
{
    // Make sure that the offset is legal (with respect to the number of locals )
    VERIFICATION_CHECK(offset < methodInfo->locals.numArgs);
    VERIFICATION_CHECK((methodInfo->options & CORINFO_OPT_INIT_LOCALS) != 0);

    return compileDO_LDVAR(opcode, &(localsMap[offset]));
}

FJitResult FJit::compileDO_LDLOCA( OPCODE opcode, unsigned offset)
{
    // Make sure that the offset is legal (with respect to the IL encoding)
    VALIDITY_CHECK(offset < methodInfo->locals.numArgs);
    VERIFICATION_CHECK((methodInfo->options & CORINFO_OPT_INIT_LOCALS) != 0);

    return compileDO_LDVARA(opcode, &(localsMap[offset]));
}

FJitResult FJit::compileDO_LDARG( OPCODE opcode, unsigned offset)
{
    stackItems* varInfo;

    // Make sure that the offset is legal (with respect to the number of arguments)
    VERIFICATION_CHECK(offset < args_len);
    varInfo = &(argsMap[offset]);
    if (methodInfo->args.isVarArg() && !varInfo->isReg && !PARAMETER_SPACE) {
        emit_VARARG_LDARGA(varInfo->offset, offsetVarArgToken);
        return compileDO_LDIND_BYTYPE(varInfo->type, false);
    }
    else
    {
        return compileDO_LDVAR(opcode, varInfo);
    }
}

FJitResult FJit::compileDO_LDARGA( OPCODE opcode, unsigned offset)
{
    stackItems* varInfo;

    VALIDITY_CHECK(offset < args_len)
    varInfo = &(argsMap[offset]);
    // Make sure that the argument is not a managed pointer
    VERIFICATION_CHECK( varInfo->type.enum_() != typeByRef );

    if (methodInfo->args.isVarArg() && !varInfo->isReg && !PARAMETER_SPACE) {
        emit_VARARG_LDARGA(varInfo->offset, offsetVarArgToken);
        // Construct the pointer type for the IL stack
        OpType PtrType(typeByRef);
        PtrType.setTarget( varInfo->type.enum_(), varInfo->type.cls() );
        pushOp( PtrType );
        return FJIT_OK;
    }
    else
    {
        return compileDO_LDVARA(opcode, varInfo);
    }
}

FJitResult FJit::compileDO_LDIND_BYTYPE( OpType trackedType, bool unaligned)
{
    TYPE_SWITCH_PRECISE(trackedType, emit_LDIND, (unaligned));
    trackedType.toFPNormalizedType();
    pushOp(trackedType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LOCALLOC()
{

#ifdef _DEBUG
    didLocalAlloc = true;
#endif

#if !defined(FJIT_NO_VALIDATION)
    // Instruction is never verifiable
    VERIFICATION_CHECK(false INDEBUG( && "Localloc is unverifiable"));
    // Make sure that the stack is empty except for one value representing the size
    VALIDITY_CHECK(opStack_len == 1);
    // If validation is on check that localloc is not attempted from inside a filter, fault, finally, catch handler
    CORINFO_EH_CLAUSE retClause; unsigned Start = 0, End = 0;
    getEnclosingClause( inPtr-inBuff, &retClause, 1, Start, End );
    VALIDITY_CHECK(isClauseEmpty(&retClause));
#endif
    emit_LOCALLOC(true,methodInfo->EHcount);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_NEG()
{
    OpType result_neg;
    UNARY_NUMERIC_RESULT(topOp(), CEE_NEG, result_neg)
    TYPE_SWITCH_ARITH(topOp(), emit_NEG, ());
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDC_I8()
{
    signed __int64  i8;
    GET(i8, signed __int64, false);
    emit_LDC_I8(i8);
    pushOp(OpType(typeI8));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDC_R4()
{
    signed int      i4;
    GET(i4, signed int /* float */, false);
    emit_LDC_R4(i4);
    pushOp(OpType(typeR8));   // R4 is immediately promoted to R8 on the IL stack
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDC_R8()
{
    signed __int64  i8;
    GET(i8, signed __int64 /*double*/, false);
    emit_LDC_R8(i8);
    pushOp(OpType(typeR8));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDNULL()
{
    emit_LDNULL();
    pushOp(OpType(typeRef,typeRef));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDSTR()
{
    unsigned int            token;
    InfoAccessType          iat;

    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    GET(token, unsigned int, false); VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    void* literalHnd = NULL;

    iat = jitInfo->constructStringLiteral(tokenScope,token, &literalHnd);

    // the code only ever supported the equivalent of IAT_PVALUE, this is now asserted
    VALIDITY_CHECK(iat == IAT_PVALUE);

    // Check if the string was constructed successfully
    VALIDITY_CHECK(literalHnd != 0);
    emit_WIN32(emit_LDC_I4(literalHnd)) emit_WIN64(emit_LDC_I8(literalHnd)) ;
    emit_LDIND_PTR(false);
    // Get the type handle for strings
    CORINFO_CLASS_HANDLE s_StringClass = jitInfo->getBuiltinClass(CLASSID_STRING);
        VALIDITY_CHECK( s_StringClass != NULL );
    pushOp(OpType(typeRef, s_StringClass ));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CPBLK()
{
    VERIFICATION_CHECK(false);
    emit_CPBLK();
    CHECK_POP_STACK(3);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_INITBLK()
{
    VERIFICATION_CHECK(false);
    emit_INITBLK();
    CHECK_POP_STACK(3);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_INITOBJ()
{
    unsigned int    SizeOfClass;
    unsigned int    token;
    CORINFO_CLASS_HANDLE    targetClass = NULL;

    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    // Verify that the token corresponds to a valid typeRef or typeDef
    targetClass = jitInfo->findClass(tokenScope,token,methodInfo->ftn);
    VALIDITY_CHECK(targetClass);
    // Verify that there is a value on the stack
    CHECK_STACK(1);

    // Make sure it is not readonly
    VERIFICATION_CHECK(!topOp().isReadOnly());

    // Verify that the value is a valid managed pointer or natural int
    // Removed to match behavior of V1 .NET Framework
    // VALIDITY_CHECK(topOpE() == typeByRef || topOpE() == typeI );
    // Natural int can't be used as pointer in verifiable code
    VERIFICATION_CHECK(topOpE() == typeByRef );
    //  Verify that the address matches the token
    CorInfoType eeType = jitInfo->asCorInfoType(targetClass);
    OpType objType = createOpType(eeType, targetClass);
    VERIFICATION_CHECK(  topOp().cls() == targetClass || topOp().targetAsEnum() == objType.enum_());

    //GENERICS: argument can be a reference
    if (jitInfo->getClassAttribs(targetClass, methodInfo->ftn) & CORINFO_FLG_VALUECLASS) {
        SizeOfClass = typeSizeInSlots(jitInfo, targetClass);
        emit_init_bytes((SizeOfClass*sizeof(void *)));
        POP_STACK(1);
    }
    else {
        emit_LDNULL();
        pushOp(OpType(typeRef,typeRef));
        emit_STIND_REF(false);
        POP_STACK(2);
    }
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CPOBJ()
{
    unsigned int    token;
    CORINFO_CLASS_HANDLE    targetClass = NULL;

    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    // Verify that the token corresponds to a valid typeRef or typeDef
    targetClass = jitInfo->findClass(tokenScope,token,methodInfo->ftn);
    VALIDITY_CHECK(targetClass);
    // Verify that there are two values on the stack
    CHECK_STACK(2);

    // Make sure it is not readonly
    // only the destination object though
    VERIFICATION_CHECK(!topOp(1).isReadOnly());

    // Verify that the values are either valid managed pointers or natural ints
    // Removed to match behavior of V1 .NET Framework
    //VALIDITY_CHECK(topOpE() == typeByRef || topOpE() == typeI );
    //VALIDITY_CHECK(topOpE(1) == typeByRef || topOpE(1) == typeI );
    // Natural int can't be used as pointer in verifiable code
    VERIFICATION_CHECK(topOpE() == typeByRef && topOpE(1) == typeByRef);
    // Two objects should be the same as the target
    CorInfoType eeType = jitInfo->asCorInfoType(targetClass);
    OpType objType = createOpType(eeType, targetClass);

    VERIFICATION_CHECK( topOp().cls() == targetClass || topOp().targetAsEnum() == objType.enum_() );
    VERIFICATION_CHECK( topOp(1).cls() == targetClass || topOp(1).targetAsEnum() == objType.enum_() );

    //GENERICS: argument can be a reference
    if (jitInfo->getClassAttribs(targetClass, methodInfo->ftn) & CORINFO_FLG_VALUECLASS) {
        emit_valClassCopy(targetClass,false);
    }
    else {
        emit_LDIND_PTR(false);
        emit_STIND_REF(false);
    }
    POP_STACK(2);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDOBJ()
{
    unsigned int    token;

    GET(token, unsigned int, false);
    return compileHelperCEE_LDOBJ(token);
}

FJitResult FJit::compileHelperCEE_LDOBJ(unsigned int token)
{
    CORINFO_CLASS_HANDLE    targetClass;

    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;

    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    // Verify that the token corresponds to a valid typeRef or typeDef
    targetClass = jitInfo->findClass(tokenScope,token,tokenContext);
    VALIDITY_CHECK(targetClass);
    // Verify that there is a value on the stack
    CHECK_STACK(1);
    // Verify that the value is a valid managed pointer or natural int
    // Removed to match behavior of V1 .NET Framework
    // VALIDITY_CHECK(topOpE() == typeByRef || topOpE() == typeI );
    // Natural int can't be used as pointer in verifiable code
    VERIFICATION_CHECK(topOpE() == typeByRef );
    // Construct the stack type for the object
    CorInfoType eeType = jitInfo->asCorInfoType(targetClass);
    OpType retType = createOpType(eeType, targetClass);
    //  Verify that the address matches the token
    VERIFICATION_CHECK( topOp().cls() == targetClass || topOp().targetAsEnum() == retType.enum_());

    TYPE_SWITCH_PRECISE(retType, emit_LDIND, (UnalignedAccess));
    POP_STACK(1);
    retType.toFPNormalizedType();
    pushOp(retType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_STOBJ()
{
    unsigned int    token;
    CORINFO_CLASS_HANDLE    targetClass = NULL;

    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;

    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    // Verify that the token corresponds to a valid typeRef or typeDef
    targetClass = jitInfo->findClass(tokenScope,token,tokenContext);
    VALIDITY_CHECK(targetClass);
    // Verify that there are two values on the stack
    CHECK_STACK(2);

    // Make sure it is not readonly
    VERIFICATION_CHECK(!topOp(1).isReadOnly());

    // Verify that the address is either valid managed pointer or natural int
    // Removed to match behavior of V1 .NET Framework
    // VALIDITY_CHECK(topOpE(1) == typeByRef || topOpE(1) == typeI );
    // Verifiable code doesn't allow native ints as pointers
    VERIFICATION_CHECK(topOpE(1) == typeByRef );

    // Obtain a type from a type handle
    CorInfoType eeType = jitInfo->asCorInfoType(targetClass);
    OpType TypeOnStack = createOpType(eeType, targetClass);

    // Verify that the pointer is addressing an object of this type
    VERIFICATION_CHECK( topOp(1).matchTarget( TypeOnStack ) ||
                        targetClass == topOp(1).cls() );
    TypeOnStack.toFPNormalizedType();
    // Verify that type handle matches the type of the object on the stack
    VERIFICATION_CHECK( canAssign(jitInfo, methodInfo->ftn, topOp(), topOp(1).getTarget()) );

    // Since floats are promoted to F, have to treat them specially
    if (eeType == CORINFO_TYPE_FLOAT)
        return compileCEE_STIND_R4();
    else if (eeType == CORINFO_TYPE_DOUBLE)
        return compileCEE_STIND_R8();

    //GENERICS: argument can be a reference
    if (jitInfo->getClassAttribs(targetClass, methodInfo->ftn) & CORINFO_FLG_VALUECLASS) {
        emit_copyPtrAroundValClass(targetClass);
        emit_valClassStore(targetClass, UnalignedAccess);
        emit_POP_PTR();     // also pop off original ptr
    }
    else {
        emit_STIND_REF(UnalignedAccess);
    }
    POP_STACK(2);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_STIND_I1()
{
    CHECK_STACK(2);

    // Make sure it is not readonly
    VERIFICATION_CHECK(!topOp(1).isReadOnly());

    VALIDITY_CHECK( topOpE(1) == typeByRef || topOpE(1) == typeI );
    OpType TypeOnStack( typeI1 );
    TypeOnStack.toSigned(); /* to match V1 .NET Framework */
    OpType TargetType =  topOp(1).getTarget();
    TargetType.toSigned(); /* to match V1 .NET Framework */
    VERIFICATION_CHECK(topOpE(1) == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, TargetType, TypeOnStack));
    TypeOnStack.toFPNormalizedType();
    VERIFICATION_CHECK(canAssign(jitInfo, methodInfo->ftn, topOp(), TypeOnStack ));
    emit_STIND_I1(false);
    POP_STACK(2);

    return FJIT_OK;
}

FJitResult FJit::compileCEE_STIND_I2()
{
    CHECK_STACK(2);

    // Make sure it is not readonly
    VERIFICATION_CHECK(!topOp(1).isReadOnly());

    VALIDITY_CHECK( topOpE(1) == typeByRef || topOpE(1) == typeI );
    OpType TypeOnStack( typeI2 );
    TypeOnStack.toSigned(); /* to match V1 .NET Framework */
    OpType TargetType =  topOp(1).getTarget();
    TargetType.toSigned(); /* to match V1 .NET Framework */
    VERIFICATION_CHECK(topOpE(1) == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, TargetType, TypeOnStack));
    TypeOnStack.toFPNormalizedType();
    VERIFICATION_CHECK(canAssign(jitInfo, methodInfo->ftn, topOp(), TypeOnStack ));
    emit_STIND_I2(UnalignedAccess);
    POP_STACK(2);

    return FJIT_OK;
}

FJitResult FJit::compileCEE_STIND_I4()
{
    CHECK_STACK(2);

    // Make sure it is not readonly
    VERIFICATION_CHECK(!topOp(1).isReadOnly());

    VALIDITY_CHECK( topOpE(1) == typeByRef || topOpE(1) == typeI );
    OpType TypeOnStack( typeI4 );
    TypeOnStack.toSigned(); /* to match V1 .NET Framework */
    OpType TargetType =  topOp(1).getTarget();
    TargetType.toSigned(); /* to match V1 .NET Framework */
    VERIFICATION_CHECK(topOpE(1) == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, TargetType, TypeOnStack));
    TypeOnStack.toFPNormalizedType();
    VERIFICATION_CHECK(canAssign(jitInfo, methodInfo->ftn, topOp(), TypeOnStack ));
    emit_STIND_I4(UnalignedAccess);
    POP_STACK(2);

    return FJIT_OK;
}

FJitResult FJit::compileCEE_STIND_I8()
{
    CHECK_STACK(2);

    // Make sure it is not readonly
    VERIFICATION_CHECK(!topOp(1).isReadOnly());

    VALIDITY_CHECK( topOpE(1) == typeByRef || topOpE(1) == typeI );
    OpType TypeOnStack( typeI8 );
    TypeOnStack.toSigned(); /* to match V1 .NET Framework */
    OpType TargetType =  topOp(1).getTarget();
    TargetType.toSigned(); /* to match V1 .NET Framework */
    VERIFICATION_CHECK(topOpE(1) == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, TargetType, TypeOnStack));
    TypeOnStack.toFPNormalizedType();
    VERIFICATION_CHECK(canAssign(jitInfo, methodInfo->ftn, topOp(), TypeOnStack ));
    emit_STIND_I8(UnalignedAccess);
    POP_STACK(2);

    return FJIT_OK;
}

FJitResult FJit::compileCEE_STIND_I()
{
    CHECK_STACK(2);

    // Make sure it is not readonly
    VERIFICATION_CHECK(!topOp(1).isReadOnly());

    VALIDITY_CHECK( topOpE(1) == typeByRef || topOpE(1) == typeI );
    OpType TypeOnStack( typeI );
    TypeOnStack.toSigned(); /* to match V1 .NET Framework */
    OpType TargetType =  topOp(1).getTarget();
    TargetType.toSigned(); /* to match V1 .NET Framework */
    VERIFICATION_CHECK(topOpE(1) == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, TargetType, TypeOnStack));
    TypeOnStack.toFPNormalizedType();
    VERIFICATION_CHECK(canAssign(jitInfo, methodInfo->ftn, topOp(), TypeOnStack ));
    emit_STIND_I(UnalignedAccess);
    POP_STACK(2);

    return FJIT_OK;
}

FJitResult FJit::compileCEE_STIND_R4()
{
    CHECK_STACK(2);

    // Make sure it is not readonly
    VERIFICATION_CHECK(!topOp(1).isReadOnly());

    VALIDITY_CHECK( topOpE(1) == typeByRef || topOpE(1) == typeI );
    OpType TypeOnStack( typeR4 );
    TypeOnStack.toSigned(); /* to match V1 .NET Framework */
    OpType TargetType =  topOp(1).getTarget();
    TargetType.toSigned(); /* to match V1 .NET Framework */
    VERIFICATION_CHECK(topOpE(1) == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, TargetType, TypeOnStack));
    TypeOnStack.toFPNormalizedType();
    VERIFICATION_CHECK(canAssign(jitInfo, methodInfo->ftn, topOp(), TypeOnStack ));
    emit_STIND_R4(UnalignedAccess);
    POP_STACK(2);

    return FJIT_OK;
}

FJitResult FJit::compileCEE_STIND_R8()
{
    CHECK_STACK(2);

    // Make sure it is not readonly
    VERIFICATION_CHECK(!topOp(1).isReadOnly());

    VALIDITY_CHECK( topOpE(1) == typeByRef || topOpE(1) == typeI );
    OpType TypeOnStack( typeR8 );
    TypeOnStack.toSigned(); /* to match V1 .NET Framework */
    OpType TargetType =  topOp(1).getTarget();
    TargetType.toSigned(); /* to match V1 .NET Framework */
    VERIFICATION_CHECK(topOpE(1) == typeByRef &&
    canAssign(jitInfo, methodInfo->ftn, TargetType, TypeOnStack));
    TypeOnStack.toFPNormalizedType();
    VERIFICATION_CHECK(canAssign(jitInfo, methodInfo->ftn, topOp(), TypeOnStack ));
    emit_STIND_R8(UnalignedAccess);
    POP_STACK(2);

    return FJIT_OK;
}

FJitResult FJit::compileCEE_STIND_REF()
{
    CHECK_STACK(2);

    // Make sure it is not readonly
    VERIFICATION_CHECK(!topOp(1).isReadOnly());

    VALIDITY_CHECK( topOpE(1) == typeByRef || topOpE(1) == typeI );
    VERIFICATION_CHECK(topOpE() == typeRef );
    VERIFICATION_CHECK(topOpE(1) == typeByRef && canAssign(jitInfo, methodInfo->ftn, topOp(), topOp(1).getTarget() ));
    emit_STIND_REF(UnalignedAccess);
    POP_STACK(2);

    return FJIT_OK;
}

FJitResult FJit::compileCEE_MKREFANY()
{
    unsigned int    token;
    CORINFO_CLASS_HANDLE    targetClass = NULL;

    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;

    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    // Verify that the token corresponds to a valid typeRef or typeDef
    targetClass = jitInfo->findClass(tokenScope,token,tokenContext);
    VALIDITY_CHECK(targetClass);
    // Verify that there is a value on the stack
    CHECK_STACK(1);

    // Make sure it is not readonly
    VERIFICATION_CHECK(!topOp().isReadOnly());

    // Verify that the value is a valid managed pointer or natural int
    // Removed to match behavior of V1 .NET Framework which doesn't make that check
    // VALIDITY_CHECK(topOpE() == typeByRef || topOpE() == typeI );
    // Verifiable code doesn't allow native ints as pointers
    VERIFICATION_CHECK(topOpE() == typeByRef );
    // Verify that the pointer addresses an object of targetClass
    CorInfoType ObjCorType = jitInfo->asCorInfoType(targetClass);
    OpType ObjType = createOpType(ObjCorType, targetClass);
    VERIFICATION_CHECK(topOp().matchTarget( ObjType ));
    emit_MKREFANY(targetClass);
    POP_STACK(1);
    pushOp(OpType(typeRefAny));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_SIZEOF()
{
    unsigned int    SizeOfClass;
    unsigned int    token;
    CORINFO_CLASS_HANDLE    targetClass = NULL;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;

    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    // Verify that the token corresponds to a valid typeRef or typeDef
    targetClass = jitInfo->findClass(tokenScope,token,tokenContext);
    VALIDITY_CHECK(targetClass);
    // Verify that the class is a value type
    DWORD classAttributes = jitInfo->getClassAttribs(targetClass, methodInfo->ftn);

    SizeOfClass = (classAttributes & CORINFO_FLG_VALUECLASS) ? jitInfo->getClassSize(targetClass) : sizeof(void*);
    emit_LDC_I4(SizeOfClass);
    pushOp(typeI);
    return FJIT_OK;
}

FJitResult FJit::compileDO_LEAVE()
{

    unsigned exceptionCount = methodInfo->EHcount;
    CORINFO_EH_CLAUSE clause;
    unsigned nextIP = inPtr - inBuff;
    unsigned target = nextIP + ilrel;

    // Clear the current clause
    makeClauseEmpty(&currentClause);
    // Reset the flag
    LeavingTryBlock = false;

    // LEAVE clears the stack
    while (!isOpStackEmpty()) {
        TYPE_SWITCH(topOp(), emit_POP, ());
        popOp(1);
    }

    // the following code relies on the ordering of the Exception Info. Table to call the
    // endcatches and finallys in the right order (see Exception Spec. doc.)
    for (unsigned except = 0; except < exceptionCount; except++)
    {
        jitInfo->getEHinfo(methodInfo->ftn, except, &clause);

        // Make sure that the current offset is not inside a fault block
        if (clause.Flags & CORINFO_EH_CLAUSE_FAULT)
            if (clause.HandlerOffset < nextIP && nextIP <= clause.HandlerOffset+clause.HandlerLength &&
            !(clause.HandlerOffset <= target && target < clause.HandlerOffset+clause.HandlerLength))
                VALIDITY_CHECK(false INDEBUG(&& "Cannot leave from a fault block!"));

        if (clause.Flags & CORINFO_EH_CLAUSE_FINALLY)
        {

            if (clause.HandlerOffset < nextIP && nextIP <= clause.HandlerOffset+clause.HandlerLength &&
                !(clause.HandlerOffset <= target && target < clause.HandlerOffset+clause.HandlerLength))
                VALIDITY_CHECK(false INDEBUG(&& "Cannot leave from a finally!"));

            // we can't leave a finally; check if we are leaving the associated try
            if (clause.TryOffset < nextIP && nextIP <= clause.TryOffset+clause.TryLength
                && !(clause.TryOffset <= target && target < clause.TryOffset+clause.TryLength))
            {
                // call the finally
                fixupTable->insert((void**) outPtr);
                emit_call_abs_address(clause.HandlerOffset, false);
                // Set the clause pointer
                currentClause = clause;
                LeavingTryBlock = true;
            }
        }

        // Check if we are leaving a handler, in that case emit a call to Jit_EndCatch
        if (clause.HandlerOffset < nextIP && nextIP <= clause.HandlerOffset+clause.HandlerLength &&
            !(clause.HandlerOffset <= target && target < clause.HandlerOffset+clause.HandlerLength) &&
            !(clause.Flags & CORINFO_EH_CLAUSE_FINALLY))
        {
            emit_reset_storedTOS_in_JitGenerated_local(false);
            emit_ENDCATCH();
            controlContinue = false;
            // Set the clause pointer
            currentClause = clause;
            LeavingCatchBlock = true;
        }
        // Check if we are inside a try block
        else if ( clause.TryOffset < nextIP && nextIP <= clause.TryOffset+clause.TryLength )
        {
        // Check if this try is contained inside the one we already saved
        if ( isClauseEmpty(&currentClause) || ( clause.TryOffset >= currentClause.TryOffset &&
                (clause.TryOffset+clause.TryLength) < (currentClause.TryOffset+currentClause.TryLength)) )
            {
            // Set the clause pointer
            currentClause = clause;
            // Check if we are leaving the try block
            if ( !(clause.TryOffset <= target && target < clause.TryOffset+clause.TryLength) )
                LeavingTryBlock = true;
            }
        }
    }
    // The leave is inside a try block and the target is outside
    if ( !isClauseEmpty(&currentClause) && LeavingTryBlock || LeavingCatchBlock )
    {
    //INDEBUG(printf( "Leaving CLAUSE: [%d, %d]\n", currentClause.TryOffset, currentClause.TryOffset+currentClause.TryLength );)
        // Put the target in front of the first offset outside the EH block containing it or
        // at the bottom of the split stack

        CORINFO_EH_CLAUSE retClause; unsigned Start = 0, End = 0; int i;
        getEnclosingClause( inPtr-inBuff, &retClause, 0, Start, End );

        for (i = SplitOffsets.getNumberItems()-1; i >= 0; i-- )
            if ( SplitOffsets.getItem(i) >= End || SplitOffsets.getItem(i) < Start )
            break;
        if ( i < 0 )
            SplitOffsets.putAt(target, 0);
        else
            SplitOffsets.putInFront(target, i);

        //INDEBUG(SplitOffsets.dumpStack();)

        // The branch is always generated at a "leave" instruction. It is not always necessary in a case when
        // "leave" is used where "br" instruction could have been used. Since the ECMA spec discourages such
        // use and warns that "leave" is less effecient than "br", this should not cause major problems
    }

    return compileDO_BR();
}


FJitResult FJit::compileCEE_STELEM_I1()
{
    OpType ElemType;
    int ver_res = verifyArrayStore( typeI1, ElemType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_STELEM_I1();
    POP_STACK(3);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_STELEM_I2()
{
    OpType ElemType;
    int ver_res = verifyArrayStore( typeI2, ElemType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_STELEM_I2();
    POP_STACK(3);
    return FJIT_OK;
}
FJitResult FJit::compileCEE_STELEM_I4()
{
    OpType ElemType;
    int ver_res = verifyArrayStore( typeI4, ElemType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_STELEM_I4();
    POP_STACK(3);
    return FJIT_OK;
}
FJitResult FJit::compileCEE_STELEM_I8()
{
    OpType ElemType;
    int ver_res = verifyArrayStore( typeI8, ElemType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_STELEM_I8();
    POP_STACK(3);
    return FJIT_OK;
}
FJitResult FJit::compileCEE_STELEM_I()
{
    OpType ElemType;
    int ver_res = verifyArrayStore( typeI, ElemType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_STELEM_I();
    POP_STACK(3);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_STELEM_R4()
{
    OpType ElemType;
    int ver_res = verifyArrayStore( typeR4, ElemType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_STELEM_R4();
    POP_STACK(3);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_STELEM_R8()
{
    OpType ElemType;
    int ver_res = verifyArrayStore( typeR8, ElemType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_STELEM_R8();
    POP_STACK(3);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_STELEM_REF()
{
    OpType ElemType;
    int ver_res = verifyArrayStore( typeRef, ElemType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_STELEM_REF();
    POP_STACK(3);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_STELEM()
{
    unsigned int    token;
    CORINFO_CLASS_HANDLE    targetClass = NULL;

    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;

    // Get token for class/interface
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    VALIDITY_CHECK(targetClass = jitInfo->findClass(tokenScope, token,tokenContext));
    // Obtain a type from a type handle
    CorInfoType eeType = jitInfo->asCorInfoType(targetClass);
    OpType TokenType = createOpType(eeType, targetClass);
    // Validate the store
    OpType ElemType;
    int resultAccess = verifyArrayStore( TokenType.enum_(), ElemType );
    VALIDITY_CHECK( resultAccess  != FAILED_VALIDATION );
    VERIFICATION_CHECK( resultAccess != FAILED_VERIFICATION );

     // Since floats are promoted to F, have to treat them specially
    if (eeType == CORINFO_TYPE_FLOAT) {
        emit_STELEM_R4();
    }
    else if (eeType == CORINFO_TYPE_DOUBLE) {
        emit_STELEM_R8();
    }
    else if (jitInfo->getClassAttribs(targetClass,methodInfo->ftn) & CORINFO_FLG_VALUECLASS) {
        // begin by emitting code that takes a stack (..., ptr, int, valclass)
        // and produces (..., ptr, int, valclass, ptr, int).
        unsigned delta = BYTE_ALIGNED(typeSizeInBytes(jitInfo, targetClass)) + SIZE_STACK_SLOT;
        emit_getSP((delta + STACK_BUFFER));
        emit_LDIND_PTR(false);
        emit_getSP((delta + STACK_BUFFER));
        emit_LDIND_PTR(false);

        unsigned size = jitInfo->getClassSize(targetClass);
        emit_LDELEMA_0(size,false);    // zero means type field before array elements
        emit_valClassStore(targetClass, false);

        // now remove the original ptr and index.
        emit_POP_PTR();
        emit_POP_PTR();
    }
    else {
        emit_STELEM_REF();
    }
    POP_STACK(3);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEM_U1()
{
    OpType ResultType;
    int ver_res = verifyArrayLoad( typeU1, ResultType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_LDELEM_U1();
    POP_STACK(2);
    ResultType.toFPNormalizedType();
    pushOp(ResultType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEM_U2()
{
    OpType ResultType;
    int ver_res = verifyArrayLoad( typeU2, ResultType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_LDELEM_U2();
    POP_STACK(2);
    ResultType.toFPNormalizedType();
    pushOp(ResultType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEM_U4()
{
    OpType ResultType;
    int ver_res = verifyArrayLoad( typeI4, ResultType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_LDELEM_U4();
    POP_STACK(2);
    ResultType.toFPNormalizedType();
    pushOp(ResultType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEM_I1()
{
    OpType ResultType;
    int ver_res = verifyArrayLoad( typeI1, ResultType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_LDELEM_I1();
    POP_STACK(2);
    ResultType.toFPNormalizedType();
    pushOp(ResultType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEM_I2()
{
    OpType ResultType;
    int ver_res = verifyArrayLoad( typeI2, ResultType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_LDELEM_I2();
    POP_STACK(2);
    ResultType.toFPNormalizedType();
    pushOp(ResultType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEM_I4()
{
    OpType ResultType;
    int ver_res = verifyArrayLoad( typeI4, ResultType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_LDELEM_I4();
    POP_STACK(2);
    ResultType.toFPNormalizedType();
    pushOp(ResultType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEM_I8()
{
    OpType ResultType;
    int ver_res = verifyArrayLoad( typeI8, ResultType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_LDELEM_I8();
    POP_STACK(2);
    ResultType.toFPNormalizedType();
    pushOp(ResultType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEM_I()
{
    OpType ResultType;
    int ver_res = verifyArrayLoad( typeI, ResultType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_LDELEM_I();
    POP_STACK(2);
    ResultType.toFPNormalizedType();
    pushOp(ResultType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEM_R4()
{
    OpType ResultType;
    int ver_res = verifyArrayLoad( typeR4, ResultType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_LDELEM_R4();
    POP_STACK(2);
    ResultType.toFPNormalizedType();
    pushOp(ResultType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEM_R8()
{
    OpType ResultType;
    int ver_res = verifyArrayLoad( typeR8, ResultType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_LDELEM_R8();
    POP_STACK(2);
    ResultType.toFPNormalizedType();
    pushOp(ResultType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEM_REF()
{
    OpType ResultType;
    int ver_res = verifyArrayLoad( typeRef, ResultType );
    VALIDITY_CHECK( ver_res != FAILED_VALIDATION );
    VERIFICATION_CHECK( ver_res != FAILED_VERIFICATION );
    emit_LDELEM_REF();
    POP_STACK(2);
    ResultType.toFPNormalizedType();
    pushOp(ResultType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDELEMA()
{
    return this->compileHelperCEE_LDELEMA(false);
}

FJitResult FJit::compileHelperCEE_LDELEMA(bool isReadOnly)
{
    unsigned int    token;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_CLASS_HANDLE    targetClass = NULL;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    // Get token for class/interface
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    targetClass = jitInfo->findClass(tokenScope, token, tokenContext);
    VALIDITY_CHECK(targetClass);

    // Obtain a type from a type handle
    CorInfoType eeType = jitInfo->asCorInfoType(targetClass);
    OpType TypeInIL = createOpType(eeType, targetClass);
    TypeInIL.toSigned();

    // Validate the access
    OpType ElemType;
    int resultAccess = verifyArrayAccess( ElemType );
    VALIDITY_CHECK( resultAccess  != FAILED_VALIDATION );
    VERIFICATION_CHECK( resultAccess != FAILED_VERIFICATION );

    // Make sure that the IL type matches the element type precisely
    OpType signedElemType = ElemType; /* To match behavior of V1 we explicitly allow signed/unsigned mismatches */
    signedElemType.toSigned();
    VERIFICATION_CHECK( canAssign( jitInfo, methodInfo->ftn, TypeInIL, signedElemType, true) );

    // assume it is an array of pointers
    unsigned size = sizeof(void*);
    if (jitInfo->getClassAttribs(targetClass,methodInfo->ftn) & CORINFO_FLG_VALUECLASS) {
        size = jitInfo->getClassSize(targetClass);

        // zero means type field before array elements
        emit_LDELEMA_0(size,isReadOnly);
    }
    else {
        emit_LDELEMA(size,targetClass,isReadOnly);
    }
    POP_STACK(2);

    // Construct the managed pointer type for the stack
    OpType ResultType(typeByRef, NULL, false, isReadOnly);
    if ( resultAccess != FAILED_VERIFICATION ) {
        VERIFICATION_CHECK( !ElemType.isByRef() && (!ElemType.isRef() || ElemType.cls() != NULL ));
        ResultType.setTarget( ElemType.enum_(), ElemType.cls() );
    }
    pushOp(ResultType);
    return FJIT_OK;
}


FJitResult FJit::compileCEE_LDELEM()
{
    unsigned int    token;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_CLASS_HANDLE    targetClass = NULL;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    // Get token for class/interface
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    VALIDITY_CHECK(targetClass = jitInfo->findClass(tokenScope, token,tokenContext));

    // Obtain a type from a type handle
    CorInfoType eeType = jitInfo->asCorInfoType(targetClass);
    OpType TokenType = createOpType(eeType, targetClass);

    // Validate the access
    OpType ElemType;
    int resultAccess = verifyArrayLoad( TokenType.enum_(), ElemType );
    VALIDITY_CHECK( resultAccess  != FAILED_VALIDATION );
    VERIFICATION_CHECK( resultAccess != FAILED_VERIFICATION );

    if (jitInfo->getClassAttribs(targetClass,methodInfo->ftn) & CORINFO_FLG_VALUECLASS) {
        unsigned size = jitInfo->getClassSize(targetClass);
        emit_LDELEMA_0(size,false);    // zero means type field before array elements
        TYPE_SWITCH_PRECISE(ElemType, emit_LDIND, (false));
    }
    else {
        emit_LDELEM_REF();
    }
    POP_STACK(2);
    ElemType.toFPNormalizedType();
    pushOp(ElemType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_STFLD( OPCODE opcode)
{

    unsigned int            token, parentToken;
    DWORD                   fieldAttributes;
    CorInfoType             jitType;
    bool                    fieldIsStatic, misMatchedOpcode = false;
    unsigned                address = 0;

    CORINFO_FIELD_HANDLE    targetField;
    CORINFO_CLASS_HANDLE    targetClass = NULL, parentClass = NULL;

    OpType                  trackedType;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;

    // Get MemberRef token for object field
    GET(token, unsigned int, false);   VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    targetField = jitInfo->findField (tokenScope, token, tokenContext);
    VALIDITY_CHECK(targetField);

    fieldAttributes = jitInfo->getFieldAttribs(targetField,methodInfo->ftn);

    CORINFO_CLASS_HANDLE valClass;
    jitType = jitInfo->getFieldType(targetField, &valClass);
    fieldIsStatic = fieldAttributes & CORINFO_FLG_STATIC ? true : false;
    misMatchedOpcode = fieldIsStatic && (opcode == CEE_STFLD);

#if !defined(FJIT_NO_VALIDATION)
    // get the exact parent of the field
    parentToken = jitInfo->getMemberParent(tokenScope, token);
    parentClass = jitInfo->findClass(tokenScope,
                                     parentToken,
                                     methodInfo->ftn);

    // Verify that the correct type of the instruction is used
    VALIDITY_CHECK( fieldIsStatic || (opcode == CEE_STFLD) );
    // Verify that a not trying to use an RVA
    VERIFICATION_CHECK( !(CORINFO_FLG_UNMANAGED & fieldAttributes ) );

    // Verify that if final field can be accessed correctly
    if ((fieldAttributes & CORINFO_FLG_FINAL) )
    {
        VERIFICATION_CHECK( (methodAttributes & CORINFO_FLG_CONSTRUCTOR) &&
                        (((methodAttributes & CORINFO_FLG_STATIC) != 0) == fieldIsStatic ) &&
                            parentClass == jitInfo->getMethodClass(methodInfo->ftn)
                        INDEBUG( || !"bad use of initonly field (set)") );
    }


    // Make sure that there is the right number of arguments on the stack
    if (opcode == CEE_STFLD) {
        CHECK_STACK(2)
        // Verify that the instance object is either a managed pointer or a managed object
        VERIFICATION_CHECK( topOp(1).isRef() || topOp(1).isByRef() )
        // Check that the object on the stack encloses the field
        VERIFICATION_CHECK( canAssign( jitInfo, methodInfo->ftn, topOp(1), OpType( topOpE(1), parentClass )) );
        // Verify the field parent's constraints
        VERIFICATION_CHECK( jitInfo->satisfiesClassConstraints(parentClass));
        // Verify that can access field
        VERIFICATION_CHECK( jitInfo->canAccessField(methodInfo->ftn, parentClass, targetField, topOp(1).cls() ) );
    } else {
        CHECK_STACK(1);
        // Verify the field parent's constraints
        VERIFICATION_CHECK( jitInfo->satisfiesClassConstraints(parentClass));
        VERIFICATION_CHECK( jitInfo->canAccessField(methodInfo->ftn, parentClass, targetField,
                                                        jitInfo->getMethodClass(methodInfo->ftn)) );
    }
#endif
    // Verify that the type of the value matches the type of the field
    OpType fieldStackType = createOpType(jitType, valClass );

    fieldStackType.toFPNormalizedType();
    VERIFICATION_CHECK( canAssign( jitInfo, methodInfo->ftn, topOp(), fieldStackType ));

    if (fieldIsStatic)
    {
        targetClass = jitInfo->getFieldClass(targetField);
        VALIDITY_CHECK(targetClass);
        emit_initclass(targetClass);
    }

    if (fieldAttributes & (CORINFO_FLG_HELPER | CORINFO_FLG_SHARED_HELPER | CORINFO_FLG_GENERICS_STATIC))
    {
        if (fieldIsStatic)                  // static fields go through pointer
        {
            CorInfoHelpFunc helperNum = jitInfo->getFieldHelper(targetField, CORINFO_ADDRESS);
            void* helperFunc = jitInfo->getHelperFtn(helperNum,NULL);
            emit_FIELDADDRHelper(helperFunc, targetField);
            trackedType = createOpType(jitType, valClass);

            if (trackedType.enum_() == typeRef)
            {
                emit_STIND_REV_Ref(UnalignedAccess);
            }
            else
            {
                TYPE_SWITCH_PRECISE(trackedType, emit_STIND_REV, (UnalignedAccess));
            }
            trackedType.toNormalizedType();
            CHECK_POP_STACK(1);             // pop value
        }
        else
        {
            // get the helper
            CorInfoHelpFunc helperNum = jitInfo->getFieldHelper(targetField,CORINFO_SET);
            void* helperFunc = jitInfo->getHelperFtn(helperNum,NULL);
            _ASSERTE(helperFunc && opcode == CEE_STFLD && misMatchedOpcode == false);
            // Force correct code gen for valuetypes which are treated
            // as primitives by the type system.
            if ( helperNum == CORINFO_HELP_SETFIELDSTRUCT && isPrimitiveValueType(valClass) )
                jitType = CORINFO_TYPE_VALUECLASS;

            unsigned fieldSize;
            switch (jitType)
            {
                case CORINFO_TYPE_FLOAT:            // since on the IL stack we always promote floats to doubles
                    emit_conv_RtoR4();
                    // Fall through
                case CORINFO_TYPE_BYTE:
                case CORINFO_TYPE_BOOL:
                case CORINFO_TYPE_CHAR:
                case CORINFO_TYPE_SHORT:
                case CORINFO_TYPE_INT:
                case CORINFO_TYPE_UBYTE:
                case CORINFO_TYPE_USHORT:
                case CORINFO_TYPE_UINT:
                emit_WIN32(case CORINFO_TYPE_PTR:)
                emit_WIN32(case CORINFO_TYPE_NATIVEINT:)
                emit_WIN32(case CORINFO_TYPE_NATIVEUINT:)
                    fieldSize = sizeof(INT32);
                    goto DO_PRIMITIVE_HELPERCALL;
                case CORINFO_TYPE_DOUBLE:
                case CORINFO_TYPE_LONG:
                case CORINFO_TYPE_ULONG:
                emit_WIN64(case CORINFO_TYPE_PTR:)
                emit_WIN64(case CORINFO_TYPE_NATIVEINT:)
                emit_WIN64(case CORINFO_TYPE_NATIVEUINT:)
                    fieldSize = sizeof(INT64);
                    goto DO_PRIMITIVE_HELPERCALL;
                case CORINFO_TYPE_CLASS:
                    fieldSize = sizeof(INT32);

                DO_PRIMITIVE_HELPERCALL:
                    CHECK_POP_STACK(2);             // pop value and pop object pointer
                    LABELSTACK((outPtr-outBuff),0);
                    emit_STFLD_NonStatic_field_helper(targetField,fieldSize,helperFunc);
                    break;
                case CORINFO_TYPE_VALUECLASS: {
                    emit_copyPtrAroundValClass(valClass);
                    int slots = typeSizeInSlots(jitInfo, valClass); // Figure out size of the value class
                    emit_SetFieldStruct(WORD_ALIGNED(slots), targetField, helperFunc, valClass);
                    CHECK_POP_STACK(  2 );         // pop value class and pop object pointer
                    emit_drop( SIZE_STACK_SLOT );  // remove the object pointer from the actual stack
                    } break;
                default:
                    FJIT_FAIL(FJIT_INTERNALERROR);
                    break;
            }
        }
    }
    else /* not a special field */
    {
        bool isEnCField = (fieldAttributes & CORINFO_FLG_EnC) ? true : false;
        if (fieldIsStatic)
        {
            address = (unsigned) jitInfo->getFieldAddress(targetField);
            VALIDITY_CHECK(address);
        }
        else
        {
            address = jitInfo->getFieldOffset(targetField);
        }

        if (jitType == CORINFO_TYPE_FLOAT)
        {
            emit_conv_RtoR4();
        }
        else if (jitType == CORINFO_TYPE_DOUBLE)
        {
            emit_conv_RtoR8();
        }
        {
            {
                emit_pushconstant_Ptr(address);
            }
        }
        CHECK_POP_STACK(1);             // pop value
        if (opcode == CEE_STFLD && !misMatchedOpcode)
            CHECK_POP_STACK(1);         // pop object pointer

        if (fieldAttributes & CORINFO_FLG_STATIC_IN_HEAP)
        {
            emit_LDFLD_REF(true, false);
            emit_LDC_I4(sizeof(void*));
            emit_WIN32(emit_ADD_I4()) emit_WIN64(emit_ADD_I8());
        }

        switch (jitType) {
        case CORINFO_TYPE_UBYTE:
        case CORINFO_TYPE_BYTE:
        case CORINFO_TYPE_BOOL:
            emit_STFLD_I1((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        case CORINFO_TYPE_SHORT:
        case CORINFO_TYPE_USHORT:
        case CORINFO_TYPE_CHAR:
            emit_STFLD_I2((fieldIsStatic || isEnCField), UnalignedAccess);
            break;

        emit_WIN32(case CORINFO_TYPE_PTR:)
        emit_WIN32(case CORINFO_TYPE_NATIVEINT:)
        emit_WIN32(case CORINFO_TYPE_NATIVEUINT:)
        case CORINFO_TYPE_UINT:
        case CORINFO_TYPE_INT:
            emit_STFLD_I4((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        case CORINFO_TYPE_FLOAT:
            emit_STFLD_R4((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        emit_WIN64(case CORINFO_TYPE_PTR:)
        emit_WIN64(case CORINFO_TYPE_NATIVEINT:)
        emit_WIN64(case CORINFO_TYPE_NATIVEUINT:)
        case CORINFO_TYPE_ULONG:
        case CORINFO_TYPE_LONG:
            emit_STFLD_I8((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        case CORINFO_TYPE_DOUBLE:
            emit_STFLD_R8((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        case CORINFO_TYPE_CLASS:
            emit_STFLD_REF((fieldIsStatic || isEnCField), UnalignedAccess);
            break;
        case CORINFO_TYPE_VALUECLASS:
            if (!fieldIsStatic && !isEnCField)
            {
                _ASSERTE(inRegTOS); // we need to undo the pushConstant_ptr since it needs to be after the emit_copyPtrAroundValClass
                inRegTOS = false;
                emit_copyPtrAroundValClass(valClass);
                emit_pushconstant_Ptr(address);
                emit_WIN32(emit_ADD_I4()) emit_WIN64(emit_ADD_I8());
                emit_valClassStore(valClass, UnalignedAccess);
                emit_POP_PTR();         // also pop off original ptr
            }
            else // static or EnC field
            {
                _ASSERTE(inRegTOS); // address of valclass field
                emit_valClassStore(valClass, UnalignedAccess);
            }
            break;
        default:
            FJIT_FAIL(FJIT_INTERNALERROR);
            break;
        }

        if (isEnCField && !fieldIsStatic)   {               // also for EnC fields, we use a helper to get the address, so the THIS pointer is unused
            emit_POP_PTR();
        }
    }   /* else, not a special field */

    if (misMatchedOpcode) {     // using STFLD on a static, we have a unused THIS pointer
        CHECK_POP_STACK(1);     // pop the unused this value
        emit_drop( SIZE_STACK_SLOT );
    }

    return FJIT_OK;
}

FJitResult FJit::compileCEE_CEQ()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_CEQ)
    TYPE_SWITCH_PTR(topOp(), emit_CEQ, ());
    POP_STACK(2);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CGT()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_CGT)
    TYPE_SWITCH_PTR(topOp(), emit_CGT, ());
    POP_STACK(2);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CGT_UN()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_CGT_UN)
    TYPE_SWITCH_PTR(topOp(), emit_CGT_UN, ());
    POP_STACK(2);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CLT()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_CLT)
    TYPE_SWITCH_PTR(topOp(), emit_CLT, ());
    POP_STACK(2);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CLT_UN()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_CLT_UN)
    TYPE_SWITCH_PTR(topOp(), emit_CLT_UN, ());
    POP_STACK(2);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_OR()
{
    INTEGER_OPERATIONS(topOp(), topOp(1), CEE_OR)
    TYPE_SWITCH_LOGIC(topOp(), emit_OR, ());
    POP_STACK(1); // The resulting type is always the same as the types of the operands
    return FJIT_OK;
}

FJitResult FJit::compileCEE_AND()
{
    INTEGER_OPERATIONS(topOp(), topOp(1), CEE_AND)
    TYPE_SWITCH_LOGIC(topOp(), emit_AND, ());
    POP_STACK(1); // The resulting type is always the same as the types of the operands
    return FJIT_OK;
}

FJitResult FJit::compileCEE_XOR()
{
    INTEGER_OPERATIONS(topOp(), topOp(1), CEE_XOR)
    TYPE_SWITCH_LOGIC(topOp(), emit_XOR, ());
    POP_STACK(1); // The resulting type is always the same as the types of the operands
    return FJIT_OK;
}

FJitResult FJit::compileCEE_NOT()
{
    INTEGER_OPERATIONS(topOp(), topOp(), CEE_NOT)
    TYPE_SWITCH_LOGIC(topOp(), emit_NOT, ());
    // It is unnessary to modify the stack because NOT doesn't affect the type of the operand
    return FJIT_OK;
}

FJitResult FJit::compileCEE_SHR()
{
    SHIFT_OPERATIONS(topOp(0), topOp(1), CEE_SHL);
    TYPE_SWITCH_LOGIC(topOp(1), emit_SHR_S, ());
    POP_STACK(1);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_SHR_UN()
{
    SHIFT_OPERATIONS(topOp(0), topOp(1), CEE_SHL);
    TYPE_SWITCH_LOGIC(topOp(1), emit_SHR, ());
    POP_STACK(1);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_SHL()
{
    SHIFT_OPERATIONS(topOp(0), topOp(1), CEE_SHL);
    TYPE_SWITCH_LOGIC(topOp(1), emit_SHL, ());
    POP_STACK(1);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_DUP()
{
    CHECK_STACK(1)
    TYPE_SWITCH(topOp(), emit_DUP, ());
    pushOp(topOp());
    DelegateStart = InstStart;      // Possible start point for dup; ldvirtftn <token>; sequence
    return FJIT_OK;
}

FJitResult FJit::compileCEE_POP()
{
    CHECK_STACK(1);
    TYPE_SWITCH(topOp(), emit_POP, ());
    POP_STACK(1);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CASTCLASS()
{
    void*                   helper_ftn;
    unsigned int            token;
    CORINFO_CLASS_HANDLE    targetClass = NULL;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    targetClass = jitInfo->findClass(tokenScope, token, tokenContext, CORINFO_TOKENKIND_Casting);
    VALIDITY_CHECK(targetClass);
    // There must be an object on the stack
    CHECK_STACK(1);
    // That object must be an object ref
    VERIFICATION_CHECK( topOp().isRef() );
    // Get a helper function for the cast
    helper_ftn = jitInfo->getHelperFtn(jitInfo->getChkCastHelper(tokenScope, token, tokenContext));
    _ASSERTE(helper_ftn);
    POP_STACK(1);           // Note that this pop /push can not be optimized because there is a
                            // call to an EE helper, and the stack tracking has to be accurate
                            // at that point
    emit_CASTCLASS(targetClass, helper_ftn);
    // The check are made in EE and exception is thrown from there during jitting.
    pushOp(OpType(typeRef,targetClass));

    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_I1()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_BYTE );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_TOI1, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_I2()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_SHORT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_TOI2, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_I4()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_INT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_TOI4, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_U1()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_UBYTE );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_TOU1, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_U2()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_USHORT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_TOU2, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_U4()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_UINT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_TOU4, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_I8()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_LONG );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_TOI8, ());
    POP_STACK(1);
    pushOp(typeI8);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_U8()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_ULONG );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_TOU8, ());
    POP_STACK(1);
    pushOp(typeI8);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_R4()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_FLOAT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_TOR4, ());
    POP_STACK(1);
    pushOp(typeR8);   // R4 is immediately promoted to R8
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_R8()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_DOUBLE );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_TOR8, ());
    POP_STACK(1);
    pushOp(typeR8);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_R_UN()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_DOUBLE );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_UN_TOR, ());
    POP_STACK(1);
    pushOp(typeR8);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_I1()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_BYTE );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_TOI1, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_U1()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_UBYTE );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_TOU1, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_I2()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_SHORT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_TOI2, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_U2()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_USHORT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_TOU2, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_I4()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_INT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_TOI4, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_U4()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_UINT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_TOU4, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_I8()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_LONG );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_TOI8, ());
    POP_STACK(1);
    pushOp(typeI8);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_U8()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_ULONG );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_TOU8, ());
    POP_STACK(1);
    pushOp(typeI8);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_I1_UN()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_BYTE );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_UN_TOI1, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_U1_UN()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_UBYTE );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_UN_TOU1, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_I2_UN()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_SHORT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_UN_TOI2, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_U2_UN()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_USHORT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_UN_TOU2, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_I4_UN()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_INT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_UN_TOI4, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_U4_UN()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_UINT );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_UN_TOU4, ());
    POP_STACK(1);
    pushOp(typeI4);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_I8_UN()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_LONG );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_UN_TOI8, ());
    POP_STACK(1);
    pushOp(typeI8);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONV_OVF_U8_UN()
{
    CONVERSION_OPERATIONS( topOp(), CORINFO_TYPE_ULONG );
    TYPE_SWITCH_ARITH(topOp(), emit_CONV_OVF_UN_TOU8, ());
    POP_STACK(1);
    pushOp(typeI8);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDTOKEN()
{
    unsigned int            token;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    // Get token for class/interface
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    CORINFO_CLASS_HANDLE tokenType;
    TokenToHandle(token, tokenType, CORINFO_TOKENKIND_Ldtoken);
    pushOp(OpType(tokenType));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_BOX()
{
    unsigned int            token;

    // Get token for class/interface
    GET(token, unsigned int, false);
    return compileHelperCEE_BOX(token);
}

FJitResult FJit::compileHelperCEE_BOX(unsigned int token)
{
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_CLASS_HANDLE    targetClass;
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    // Verify that the token is valid
    targetClass = jitInfo->findClass(tokenScope, token, methodInfo->ftn);
    VALIDITY_CHECK(targetClass);
    // Get the stack type of the class
    CorInfoType eeType = jitInfo->asCorInfoType(targetClass);
    OpType targetType = createOpType( eeType, targetClass );
    targetType.toFPNormalizedType();
    // Verify that we are not attempting to box pointers to local stack
    VERIFICATION_CHECK(!verIsByRefLike(targetType ) );

    // Verify the class constraints of the boxed type in case we are boxing an uninitialized value
    VERIFICATION_CHECK(jitInfo->satisfiesClassConstraints(targetClass));

    // Check that there is an object on the stack
    CHECK_STACK(1);

    // Call this because CLR uses embedGenericHandle to activate
    //   the target assembly (if needed). So if we would not call it,
    //   later in the game the compiled code could try to call into
    //   the assembly which was not activated yet.
    // On the other hand we don't actually emit any code inside TokenToHandle().
    CORINFO_CLASS_HANDLE tokenType;
    TokenToHandle(token, tokenType, CORINFO_TOKENKIND_Ldtoken, false);

    // Check whether the token refers to a value type
    DWORD attribs = jitInfo->getClassAttribs(targetClass, methodInfo->ftn);
    if (attribs & CORINFO_FLG_VALUECLASS) {
        // Verify that the token matches the of the item on the stack
        VERIFICATION_CHECK( targetType.enum_() == topOpE() && topOpE() != typeValClass ||
                            topOpE() == typeValClass && topOp().cls() == targetClass);
        // Floats were promoted, put them back before continuing.
        if (eeType == CORINFO_TYPE_FLOAT) {
            emit_conv_RtoR4();
        }
        else if (eeType == CORINFO_TYPE_DOUBLE) {
            emit_conv_RtoR8();
        }

        unsigned vcSize = typeSizeInBytes(jitInfo, targetClass);
        void* helper_ftn = jitInfo->getHelperFtn(jitInfo->getBoxHelper(targetClass));
        _ASSERTE(helper_ftn);
        emit_BOXVAL(targetClass, vcSize, helper_ftn);
    }
    else {
        // BOX can be used on things that are not value classes, in which
        // case we get a NOP.  However the verifier's view of the type on the
        // stack changes (in generic code a 'T' becomes a 'boxed T')
        VERIFICATION_CHECK( topOp().isRef() && topOp().cls() == targetClass);
        emit_il_nop();
    }
    // Remove the value from the stack
    POP_STACK(1);
    // Create the object type for the stack
    pushOp(OpType(typeRef, jitInfo->getTypeForBox(targetClass)));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_UNBOX()
{
    unsigned int            token;
    CORINFO_CLASS_HANDLE    targetClass = NULL;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    // Get token for class/interface
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    // Verify that the token is valid
    targetClass = jitInfo->findClass(tokenScope, token, tokenContext);
    VALIDITY_CHECK(targetClass);
    // Verify that the token refers to a value type
    VALIDITY_CHECK(jitInfo->getClassAttribs(targetClass, methodInfo->ftn) & CORINFO_FLG_VALUECLASS);
    // Check that there is an object on the stack
    CHECK_STACK(1);
    // Verify that the token on the stack is an object
    CorInfoType eeType = jitInfo->asCorInfoType(targetClass);
    VERIFICATION_CHECK( topOp().isRef());
    // The check for targetClass == topOp().cls() is not made at verification time
    // because Jit_Unbox check for validity of the cast and throws InvalidCastException if
    // unbox is unsuccessful. This is necessary in order to verify a sequence of IL opcodes such as
    // ldloc.1 isinst Int32; brfalse.s L; ldloc.1 unbox Int32; where local.1 is declared as an
    // object.

    void* helper_ftn = jitInfo->getHelperFtn(jitInfo->getUnBoxHelper(targetClass, NULL));
    _ASSERTE(helper_ftn);
    POP_STACK(1);
    emit_UNBOX(targetClass, helper_ftn);
    OpType PtrType(typeByRef);
    PtrType.setTarget( eeType, targetClass );
    pushOp(PtrType);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_UNBOX_ANY()
{
    unsigned int            token;
    CORINFO_CLASS_HANDLE    targetClass = NULL;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    // Get token for class/interface
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    // Verify that the token is valid
    VALIDITY_CHECK(targetClass = jitInfo->findClass(tokenScope, token, tokenContext));

    // Check that there is an object on the stack
    CHECK_STACK(1);
    // Verify that the value on the stack is an object ref or a null
    CorInfoType eeType = jitInfo->asCorInfoType(targetClass);
    VERIFICATION_CHECK( topOp().isRef() );
    // The check for targetClass == topOp().cls() is not made at verification time
    // because Jit_Unbox check for validity of the cast and throws InvalidCastException
    // if unbox is unsuccessful. This is necessary in order to verify a sequence of IL
    // opcodes such as ldloc.1 isinst Int32; brfalse.s L; ldloc.1 unbox Int32;
    // where local.1 is declared as an object.

    if (jitInfo->getClassAttribs(targetClass, methodInfo->ftn) & CORINFO_FLG_VALUECLASS) {
        OpType retType = createOpType(eeType, targetClass);
        void* helper_ftn = jitInfo->getHelperFtn(jitInfo->getUnBoxHelper(targetClass, NULL));
        _ASSERTE(helper_ftn);
        POP_STACK(1);
        emit_UNBOX(targetClass, helper_ftn);
        TYPE_SWITCH_PRECISE(retType, emit_LDIND, (false));
        retType.toFPNormalizedType();
        pushOp(retType);
    }
    else {
        // Get a helper function for the cast
        void *helper_ftn = jitInfo->getHelperFtn(jitInfo->getChkCastHelper(tokenScope, token, tokenContext));
        _ASSERTE(helper_ftn);
        POP_STACK(1);
        // Note that this pop /push can not be optimized because there is a
        // call to an EE helper, and the stack tracking has to be accurate
        // at that point
        emit_CASTCLASS(targetClass, helper_ftn);
        pushOp(OpType(typeRef,targetClass));
    }
    return FJIT_OK;
}

FJitResult FJit::compileCEE_CONSTRAINED()
{
    _ASSERTE(methodInfo != NULL);
    _ASSERTE(jitInfo != NULL);

    unsigned int            constraintToken;
    unsigned char           callvirtOp;
    FJitResult              retval;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_METHOD_HANDLE   tokenContext = methodInfo->ftn;

    // Get token for Constraint
    GET(constraintToken, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, constraintToken));

    // Call this because CLR uses embedGenericHandle to activate
    //   the target assembly (if needed). So if we would not call it,
    //   later in the game the compiled code could try to call into
    //   the assembly which was not activated yet.
    // On the other hand we don't actually emit any code inside TokenToHandle().
    CORINFO_CLASS_HANDLE tokenType;
    TokenToHandle(constraintToken, tokenType, CORINFO_TOKENKIND_Ldtoken, false);

    // at least objPtr should be on stack
    CHECK_STACK(1);

    // get next opcode, must be callvirt
    GET(callvirtOp, unsigned char, false);
    VALIDITY_CHECK(callvirtOp == CEE_CALLVIRT);

    unsigned int            funcToken;
    GET(funcToken, unsigned int, false);

    CORINFO_CALL_INFO callInfo;
    jitInfo->getCallInfo(methodInfo->ftn,
                tokenScope,
                funcToken,
                constraintToken,
                tokenContext,
                CORINFO_CALLINFO_CALLVIRT,
                &callInfo);

    switch (callInfo.thisTransform)
    {
    case CORINFO_NO_THIS_TRANSFORM:
        {
            return this->compileHelperCEE_CALL(funcToken, callInfo.targetMethodHandle,false /*readonly*/);
        }

    case CORINFO_BOX_THIS:
        {
            CORINFO_SIG_INFO targetSigInfo;
            jitInfo->getMethodSig(callInfo.targetMethodHandle, &targetSigInfo);
            // this is slightly ineffecient, especially when dealing with large
            // valuetypes but effeciency is not paramount in fjit

            // {... , objPtr, args} -> {..., objPtr, args, objPtr }
            copyPtrUpAroundArgs(targetSigInfo);

            // {..., objPtr, args, objPtr } -> {..., objPtr, args, *objPtr }
            if( (retval = this->compileHelperCEE_LDOBJ(constraintToken)) != FJIT_OK)
                return retval;

            // {..., objPtr, args, *objPtr } -> {..., objPtr, args, boxedPtr }
            if( (retval = this->compileHelperCEE_BOX(constraintToken)) != FJIT_OK)
                return retval;

            // {..., objPtr, args, boxedPtr } -> {... , boxedPtr, args}
            copyPtrDownAroundArgs(targetSigInfo);

            return this->compileHelperCEE_CALLVIRT(funcToken);
        }
    case CORINFO_DEREF_THIS:
        {
            CORINFO_SIG_INFO targetSigInfo;
            jitInfo->getMethodSig(callInfo.targetMethodHandle, &targetSigInfo);

            // {... , &this, args} -> {..., &this, args, &this }
            copyPtrUpAroundArgs(targetSigInfo);

            // it was a reference type
            if( (retval = this->compileCEE_LDIND_REF()) != FJIT_OK)
                return retval;

            // {... , &this, args, this} -> {..., this, args}
            copyPtrDownAroundArgs(targetSigInfo);

            return this->compileHelperCEE_CALLVIRT(funcToken);
        }
    default:
        _ASSERTE(!"Unreachable");
        return FJIT_INTERNALERROR;
    }

}

FJitResult FJit::compileCEE_READONLY()
{
    _ASSERTE(methodInfo != NULL);
    _ASSERTE(jitInfo != NULL);
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    unsigned char           readonlyOp;

    // get next opcode, must be either ldelema or a call to Array::Address
    GET(readonlyOp, unsigned char, false);
    VALIDITY_CHECK(readonlyOp == CEE_LDELEMA || readonlyOp == CEE_CALL || readonlyOp == CEE_CALLVIRT);

    if(readonlyOp == CEE_CALL)
    {
        unsigned int            funcToken, parentToken;
        GET(funcToken, unsigned int, false);

        CORINFO_METHOD_HANDLE   targetMethodHandle;
        targetMethodHandle = jitInfo->findMethod(tokenScope, funcToken, methodInfo->ftn);
        VALIDITY_CHECK(targetMethodHandle);

        CORINFO_SIG_INFO targetSigInfo;
        jitInfo->getMethodSig(targetMethodHandle, &targetSigInfo);

        parentToken = jitInfo->getMemberParent(tokenScope, funcToken);

        CORINFO_CLASS_HANDLE targetClass;
        targetClass = jitInfo->findClass(tokenScope, parentToken, methodInfo->ftn);
        VALIDITY_CHECK(targetClass);

        unsigned int targetClassAttributes;
        targetClassAttributes = jitInfo->getClassAttribs(targetClass, methodInfo->ftn);

        //
        // this is way the mainline JIT determines that Array::Address is being called
        // (it is the only method in class Array that returns a byref)
        // so emulate the behavior here
        //
        bool isArrayAddressMethod = targetSigInfo.retType & CORINFO_TYPE_BYREF &&
                                    targetClassAttributes & CORINFO_FLG_ARRAY;
        VALIDITY_CHECK(isArrayAddressMethod);

        FJitResult retval;
        if((retval = this->compileHelperCEE_CALL(funcToken,
                                                targetMethodHandle,
                                                true /*readonly*/)) != FJIT_OK )
        {
            return retval;
        }

        _ASSERTE(topOp().isByRef()); // sanity check
        topOp().setReadOnly(true);
        return retval;
    }
    else if(readonlyOp == CEE_CALLVIRT)
    {
        unsigned int funcToken;
        GET(funcToken, unsigned int, false);
        VERIFICATION_CHECK(jitInfo->isValidToken(methodInfo->scope, funcToken));

        FJitResult retval;
        if((retval = this->compileHelperCEE_CALLVIRT(funcToken,
                                                     true /*readonly*/)) != FJIT_OK )
        {
            return retval;
        }
        _ASSERTE(FALSE);
        topOp().setReadOnly(true);
        return retval;
    }
    else if(readonlyOp == CEE_LDELEMA)
    {
        return this->compileHelperCEE_LDELEMA(true);
    }

    _ASSERTE(!"invalid opcode after .readonly prefix");
    return FJIT_INTERNALERROR;
}

FJitResult FJit::compileCEE_ISINST()
{
    unsigned int            token;
    CORINFO_CLASS_HANDLE    targetClass;
    void*                   helper_ftn;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    // Get token for class/interface
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    targetClass = jitInfo->findClass(tokenScope, token, tokenContext, CORINFO_TOKENKIND_Casting);
    VALIDITY_CHECK(targetClass);
    // There must be an object on the top of the stack
    CHECK_STACK(1);
    // Check if the object is null - do nothing in this case
    VERIFICATION_CHECK( topOp().isNull() || topOp().isRef() );
    if (topOp().isNull())
    {
        emit_il_nop();
    }
    else
    {
        // Otherwise get to a helper function to check if the object is an instance of targetClass
        helper_ftn = jitInfo->getHelperFtn(jitInfo->getIsInstanceOfHelper(tokenScope, token, tokenContext));
        _ASSERTE(helper_ftn);
        POP_STACK(1);
        emit_ISINST(targetClass, helper_ftn);
        pushOp(OpType(typeRef,targetClass));
    }
    return FJIT_OK;
}

FJitResult FJit::compileCEE_JMP()
{
    unsigned int            token;
    CORINFO_METHOD_HANDLE   targetMethod;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    // The jmp instruction is unverifiable
    VERIFICATION_CHECK(false INDEBUG( && "JMP is not verifiable") );
    GET(token, unsigned int, false );
    // Validate the token
    targetMethod = jitInfo->findMethod(tokenScope, token, tokenContext);
    VALIDITY_CHECK(targetMethod);

    // insert a GC check
    emit_trap_gc();

    CORINFO_CONST_LOOKUP addrInfo;
    jitInfo->getFunctionEntryPoint(targetMethod, IAT_PVALUE, &addrInfo);
    VALIDITY_CHECK(addrInfo.addr);
    VALIDITY_CHECK(addrInfo.accessType == IAT_PVALUE && isOpStackEmpty());

#ifdef _DEBUG
#endif

    // Notify the profiler of a tailcall/jmpcall
    if (flags & CORJIT_FLG_PROF_ENTERLEAVE)
    {
        BOOL bHookFunction;
        void *eeHandle;
        void *profilerHandle;
        BOOL bIndirected;
        jitInfo->GetProfilingHandle(methodInfo->ftn,
                                    &bHookFunction,
                                    &eeHandle,
                                    &profilerHandle,
                                    &bIndirected);

        if (bHookFunction)
        {
            _ASSERTE(!bIndirected); // FJIT does not handle indirected case.
            _ASSERTE(!inRegTOS);
            ULONG func = (ULONG) jitInfo->getHelperFtn(CORINFO_HELP_PROF_FCN_TAILCALL);
            _ASSERTE(func != NULL);
            emit_callhelper_prof3(func,
                                  CORINFO_HELP_PROF_FCN_TAILCALL,
                                  eeHandle,
                                  profilerHandle,
                                  NULL); // FRAME_INFO (see definition of FunctionTailcall2 in corprof.idl)
        }
    }

    // Restore the argument registers
    restoreEnregisteredArguments();
    // Remove the frame and restore fp, sp and callee saved registers, while preserving the return address
    restore_frame(false, true);
    // Branch to the start of the function
    emit_jmp_absolute(* (unsigned *)addrInfo.addr);

    // Control flow does not continue to the next instruction
    controlContinue = false;
    popSplitStack = true;

    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDLEN()
{
    // That there is an object on the stack
    CHECK_STACK(1);
    // Verify that the array object is a valid object
    VALIDITY_CHECK( topOpE() == typeRef );
    // Verify that the array object is actually an array or is null
    if (!topOp().isNull())
        { VERIFICATION_CHECK(jitInfo->isSDArray( topOp().cls())); }
    emit_LDLEN();
    POP_STACK(1);
    pushOp(OpType(typeI, NULL));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDVIRTFTN()
{
    unsigned int            token;
    CORINFO_METHOD_HANDLE   targetMethod;
    CORINFO_CLASS_HANDLE    targetClass;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    // token for function
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    targetMethod = jitInfo->findMethod(tokenScope, token, tokenContext); VALIDITY_CHECK(targetMethod);
    targetClass  = jitInfo->getMethodClass (targetMethod);                      VALIDITY_CHECK(targetClass);

    CORINFO_SIG_INFO        targetSigInfo;
    jitInfo->getMethodSig(targetMethod, &targetSigInfo);
    _ASSERTE(!targetSigInfo.hasTypeArg());
    DWORD methodAttribs;
    methodAttribs = jitInfo->getMethodAttribs(targetMethod,methodInfo->ftn);
    DWORD classAttribs;
    classAttribs = jitInfo->getClassAttribs(targetClass,methodInfo->ftn);

    // Make sure that the method is non-static and not abstract
    VERIFICATION_CHECK( !( methodAttribs & CORINFO_FLG_STATIC ) );
    // Make sure that there is an item on the stack
    CHECK_STACK(1);
    // Verify that the item is an object
    VERIFICATION_CHECK( topOp().isRef() );
    DelegateMethodRef = token; // For delegate verification

    CORINFO_CALL_INFO virtCallInfo;
    // Call this because the CLR "misuses" this method to activate
    //   the target assembly (if needed). So if we would not call it
    //   later in the game the compiled code could try to call into
    //   the assembly which was not activated yet.
    // On the other hand we don't actually need any information
    //   provided by this call.
    jitInfo->getCallInfo(methodInfo->ftn,
                         tokenScope,
                         token,
                         0, // constraintToken -                                                             
                         tokenContext,
                         CORINFO_CALLINFO_CALLVIRT,
                         &virtCallInfo);

    if ((methodAttribs & CORINFO_FLG_FINAL) || !(methodAttribs & CORINFO_FLG_VIRTUAL))
    {
        emit_POP_I4();      // Don't need this pointer
        CHECK_POP_STACK(1);
        return compileDO_LDFTN(token, targetMethod);
    }

    if (methodAttribs & CORINFO_FLG_EnC && !(classAttribs & CORINFO_FLG_INTERFACE))
    {
        _ASSERTE(!"LDVIRTFTN for EnC NYI");
    }
    else {
         emit_ldvirtftn_helper(token, jitInfo->getMemberParent(methodInfo->scope, token));
    }

    CHECK_POP_STACK(1);
    pushOp(OpType(targetMethod));

    return FJIT_OK;
}

FJitResult FJit::compileCEE_ENDFILTER()
{
    CHECK_STACK(1);
    VERIFICATION_CHECK( topOpE() == typeI4 );
    // Make sure that we are inside a filter and that endfilter is the last instruction
#if !defined(FJIT_NO_VALIDATION)
    {
    // Here we depend on the IL code sequence the filter is always followed by the handler
    // So the instruction after endfilter must be the first instruction of the handler
    CORINFO_EH_CLAUSE retClause; unsigned Start = 0, End = 0;
    getEnclosingClause( inPtr-inBuff, &retClause, 0, Start, End );
    VALIDITY_CHECK(     !isClauseEmpty(&retClause) &&
                        (retClause.Flags & CORINFO_EH_CLAUSE_FILTER ) &&
                        retClause.FilterOffset < InstStart && (DWORD) (inPtr-inBuff) == retClause.HandlerOffset  )
    }
#endif
    emit_loadresult_I4();   // put top of stack in the return register
    POP_STACK(1);
    VALIDITY_CHECK( isOpStackEmpty() );
    // We do not set popSplitStack = true because we depend on the assumption that
    // filter is in front of the handler so we will jit the handler next. However
    // by setting controlContinue to false we reset the stack to the correct
    // state for the beginning of the handler
    controlContinue = false;
    emit_reset_storedTOS_in_JitGenerated_local(true);
    emit_relocate_retval_filter();
    emit_ret(0, false);

    return FJIT_OK;
}

FJitResult FJit::compileCEE_LDFTN()
{
    unsigned int            token;
    CORINFO_METHOD_HANDLE   targetMethod;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    // token for function
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    targetMethod = jitInfo->findMethod(tokenScope, token, tokenContext);
    VALIDITY_CHECK(targetMethod);
    DelegateStart = InstStart;      // Possible start point for dup; ldvirtftn <token>; sequence
    DelegateMethodRef = token;      // We cache the member ref of a potential delegate target method, for verification
    return compileDO_LDFTN(token, targetMethod);
}

FJitResult FJit::compileCEE_NEWARR()
{
    unsigned int            token;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_CLASS_HANDLE    targetClass;

    // token for element type
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    targetClass = jitInfo->findClass(tokenScope, token, tokenContext);
    VALIDITY_CHECK(targetClass);
    CORINFO_CLASS_HANDLE elementClass = targetClass;
    // convert to the array class for this element type
    targetClass = jitInfo->getSDArrayForClass(targetClass);
    // Check if a single dimensional array of target is valid
    VALIDITY_CHECK( targetClass );
    // Verify that the element is not byref like object
    VERIFICATION_CHECK(
        !(jitInfo->getClassAttribs(elementClass, methodInfo->ftn) & CORINFO_FLG_CONTAINS_STACK_PTR));
    // There has to be a number of elements on the stack
    CHECK_STACK(1);
    // Number of elements has to be of type natural int
    VERIFICATION_CHECK( topOpE() == typeI );
    // Remove the number of elements from the stack
    POP_STACK(1);
    emit_NEWOARR(targetClass);
    pushOp(OpType(typeRef, targetClass));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_NEWOBJ()
{

    unsigned int            token, parentToken;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    unsigned int            argBytes;
    CORINFO_CLASS_HANDLE    targetClass;
    CORINFO_METHOD_HANDLE   targetMethod;
    void*                   helper_ftn;
    CORINFO_SIG_INFO        targetSigInfo;

    unsigned int targetMethodAttributes;
    unsigned int targetClassAttributes;
    unsigned int targetCallStackSize;
    unsigned int stackPadorRetBase = 0;

    // MemberRef token for constructor
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    parentToken = jitInfo->getMemberParent(tokenScope, token);
    targetMethod = jitInfo->findMethod(tokenScope, token, tokenContext);
    VALIDITY_CHECK(targetMethod);
    targetClass = jitInfo->findClass(tokenScope, parentToken, tokenContext);
    VALIDITY_CHECK(targetClass);
    targetClassAttributes = jitInfo->getClassAttribs(targetClass,methodInfo->ftn);

    jitInfo->getMethodSig(targetMethod, &targetSigInfo);
   // VALIDITY_CHECK(!targetSigInfo.hasTypeArg());
    targetMethodAttributes = jitInfo->getMethodAttribs(targetMethod,methodInfo->ftn);
    VERIFICATION_CHECK((targetMethodAttributes & CORINFO_FLG_CONSTRUCTOR ));
    VERIFICATION_CHECK((targetMethodAttributes & (CORINFO_FLG_STATIC|CORINFO_FLG_ABSTRACT)) == 0 );
    if (targetClassAttributes & CORINFO_FLG_ARRAY) {
#if !defined(FJIT_NO_VALIDATION)
        CORINFO_CLASS_HANDLE elemTypeHnd;
        // Get the element type of the array
        CorInfoType corType = jitInfo->getChildType(targetClass, &elemTypeHnd);
        // Verify that it is valid
        VALIDITY_CHECK(!(elemTypeHnd == 0 && corType == CORINFO_TYPE_VALUECLASS));
        // Verify that the array created is not made of byref like objects
        VERIFICATION_CHECK(elemTypeHnd == 0 ||
                !(jitInfo->getClassAttribs(elemTypeHnd,methodInfo->ftn) & CORINFO_FLG_CONTAINS_STACK_PTR));
        VALIDITY_CHECK(targetClassAttributes & CORINFO_FLG_VAROBJSIZE);
        // Verify that the arguments on the stack match the method signature
        int result_arg_ver = (JitVerify ? verifyArguments( targetSigInfo, 0,false) : SUCCESS_VERIFICATION);
        VALIDITY_CHECK( result_arg_ver != FAILED_VALIDATION );
        VERIFICATION_CHECK( result_arg_ver != FAILED_VERIFICATION INDEBUG(|| !"Array ctor"));
#endif
        // allocate md array
        targetSigInfo.callConv = CORINFO_CALLCONV_VARARG;
        argInfo* tempMap;
        New(tempMap,argInfo[targetSigInfo.numArgs]);
        unsigned enregSize = 0;
        targetCallStackSize = computeArgInfo(&targetSigInfo, tempMap, 0, enregSize);
        // get the total size of the argument, undo the vararg cookie
        targetCallStackSize += enregSize - (PARAMETER_SPACE?sizeof(void*):0);
        delete [] tempMap;
        POP_STACK(targetSigInfo.numArgs);
        emit_NEWOBJ_array(targetClass, targetMethod, targetCallStackSize);
        pushOp(OpType(typeRef,targetClass));

    }
    else if (targetClassAttributes & CORINFO_FLG_VAROBJSIZE) {
        // variable size objects that are not arrays, e.g. string
        // call the constructor with a null `this' pointer
        emit_WIN32(emit_LDC_I4(0)) emit_WIN64(emit_LDC_I8(0));
        pushOp(typeI4);
        CORINFO_CONST_LOOKUP addrInfo;
        jitInfo->getFunctionEntryPoint(targetMethod, IAT_VALUE, &addrInfo);
        VALIDITY_CHECK(addrInfo.addr);
        VALIDITY_CHECK(addrInfo.accessType == IAT_VALUE || addrInfo.accessType == IAT_PVALUE);

        targetSigInfo.retType      = CORINFO_TYPE_CLASS;
        targetSigInfo.retTypeClass = targetClass;
        // Verify that the arguments on the stack match the method signature
        int result_arg_ver = (JitVerify ? verifyArguments(targetSigInfo, 1, false) : SUCCESS_VERIFICATION);
        VALIDITY_CHECK( result_arg_ver != FAILED_VALIDATION );
        VERIFICATION_CHECK( result_arg_ver != FAILED_VERIFICATION INDEBUG(|| !"VarObj ctor" ));
        argBytes = buildCall(&targetSigInfo, CALL_THIS_LAST, stackPadorRetBase,false);
        emit_callnonvirt((unsigned) addrInfo.addr, 0, addrInfo.accessType == IAT_PVALUE);
        return compileDO_PUSH_CALL_RESULT(argBytes, stackPadorRetBase, token, targetSigInfo, NULL);
    }
    else if (targetClassAttributes & CORINFO_FLG_VALUECLASS) {
        // This acts just like a static method that returns a value class
        targetSigInfo.retTypeClass = targetClass;
        targetSigInfo.retType = CORINFO_TYPE_VALUECLASS;
        if ( EnregReturnBuffer )
          targetSigInfo.callConv = CorInfoCallConv(targetSigInfo.callConv & ~CORINFO_CALLCONV_HASTHIS);
        else // Fake entry for the this pointer
          { pushOp(OpType(typeRef,targetClass)); deregisterTOS; emit_grow( SIZE_STACK_SLOT ); }

        // Verify that the arguments on the stack match the method signature
        int result_arg_ver = (JitVerify ? verifyArguments(targetSigInfo, (EnregReturnBuffer ?0 : 1), false) : SUCCESS_VERIFICATION);
        VALIDITY_CHECK( result_arg_ver != FAILED_VALIDATION );
        VERIFICATION_CHECK( result_arg_ver != FAILED_VERIFICATION INDEBUG(|| !"Valueclass ctor") );
        argBytes = buildCall(&targetSigInfo, (EnregReturnBuffer? CALL_NONE : CALL_THIS_LAST), stackPadorRetBase,false);

        CORINFO_CONST_LOOKUP addrInfo;
        jitInfo->getFunctionEntryPoint(targetMethod, IAT_VALUE, &addrInfo);
        VALIDITY_CHECK(addrInfo.addr);
        VALIDITY_CHECK(addrInfo.accessType == IAT_VALUE || addrInfo.accessType == IAT_PVALUE);

        // If the ret. buffer pointer was emitted to the stack we need to move it into a register to act as a this pointer
        if ( !EnregReturnBuffer )
          emit_set_arg_pointer(false, RETURN_BUFF_OFFSET, false );

        emit_callnonvirt((unsigned)addrInfo.addr, 0, addrInfo.accessType == IAT_PVALUE);

        if ( PASS_VALUETYPE_BYREF && targetSigInfo.hasRetBuffArg() )
        {
           _ASSERTE( CALLER_CLEANS_STACK );
           emit_copy_VC( (STACK_BUFFER + argBytes), stackPadorRetBase, targetSigInfo.retTypeClass);
        }

        if (targetSigInfo.isVarArg())
        { emit_drop(argBytes); }
        else
        {
          if (CALLER_CLEANS_STACK)     // If __cdecl convention it is necessary for
            {emit_drop((argBytes+ (!PARAMETER_SPACE ? stackPadorRetBase : 0 )));}        // the caller to clear the arguments of the stack
          else
            emit_drop(stackPadorRetBase)
    }
    }
    else {
        //allocate normal object
        helper_ftn = jitInfo->getHelperFtn(jitInfo->getNewHelper(targetClass, methodInfo->ftn,parentToken,tokenScope));
        _ASSERTE(helper_ftn);
        //pushOp(typeRef); we don't do this and compensate for it in the popOp down below
        emit_NEWOBJ(targetClass, helper_ftn);
        pushOp(OpType(typeRef,targetClass));

        emit_save_TOS();        //squirrel the newly created object away; will be reported in FJit_EETwain
        //note: the newobj is still on TOS

        if (targetClassAttributes & CORINFO_FLG_DELEGATE)
        {
            // Rules for verifying delegates are unique
            int result_del_ver =  verifyDelegate( token,
                                                  targetSigInfo, targetMethod,
                                                  &inBuff[InstStart], InstStart-DelegateStart, 1);
            VALIDITY_CHECK( result_del_ver != FAILED_VALIDATION );
            VERIFICATION_CHECK( result_del_ver != FAILED_VERIFICATION );
        }
        else
        {
            // Verify that the arguments on the stack match the method signature
            int result_arg_ver =(JitVerify ? verifyArguments(targetSigInfo,1,false) : SUCCESS_VERIFICATION);
            VALIDITY_CHECK( result_arg_ver != FAILED_VALIDATION );
            VERIFICATION_CHECK( result_arg_ver != FAILED_VERIFICATION INDEBUG(|| !"Normal ctor") );
            // We don't need to verify the this pointer because we have created it
        }
        if (targetSigInfo.hasTypeArg())
        {
            CORINFO_CLASS_HANDLE tokenType;
            // Instantiated generic method
            if (TypeFromToken(token) == mdtMethodSpec)
                TokenToHandle(token, tokenType);
            // otherwise must be an instance method in a generic struct, a static method in a generic type, or a runtime-generated array method
            else
                TokenToHandle(parentToken, tokenType);
        }

        argBytes = buildCall(&targetSigInfo, CALL_THIS_LAST, stackPadorRetBase,false);

        CORINFO_CONST_LOOKUP addrInfo;
        jitInfo->getFunctionEntryPoint(targetMethod, IAT_VALUE, &addrInfo);
        VALIDITY_CHECK(addrInfo.addr);
        VALIDITY_CHECK(addrInfo.accessType == IAT_VALUE || addrInfo.accessType == IAT_PVALUE);

        emit_callnonvirt((unsigned)addrInfo.addr, 0, addrInfo.accessType == IAT_PVALUE);

        if (targetSigInfo.isVarArg())
        {emit_drop(argBytes);}
        else
        {
          if (CALLER_CLEANS_STACK)     // If __cdecl convention it is necessary for
            {emit_drop((argBytes+ (!PARAMETER_SPACE ? stackPadorRetBase : 0 )));}        // the caller to clear the arguments of the stack
          else
            emit_drop(stackPadorRetBase)
    }

        emit_restore_TOS(); //push the new obj back on TOS
        pushOp(OpType(typeRef,targetClass));
    }
    return FJIT_OK;
}

FJitResult FJit::compileDO_PUSH_CALL_RESULT(
    unsigned int argBytes,
    unsigned int stackPadorRetBase,
    unsigned int token,
    CORINFO_SIG_INFO targetSigInfo,
    CORINFO_CLASS_HANDLE targetClass)
{
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    _ASSERTE(argBytes != 0xBADF00D);    // need to set this before getting here

    if ( PASS_VALUETYPE_BYREF && targetSigInfo.hasRetBuffArg() )
    {
      _ASSERTE( CALLER_CLEANS_STACK );
      emit_copy_VC( (STACK_BUFFER + argBytes), stackPadorRetBase, targetSigInfo.retTypeClass);
    }

    if (targetSigInfo.isVarArg())
      { emit_drop(argBytes);}
    else
    {
      if (CALLER_CLEANS_STACK)          // If __cdecl convention is used, it is necessary for
         {emit_drop((argBytes+(!PARAMETER_SPACE ? stackPadorRetBase : 0 )));}         // the caller to clear the arguments of the stack
      else
         emit_drop(stackPadorRetBase)            // Need to remove pad about which callee doesn't know
    }

    if (targetSigInfo.retType != CORINFO_TYPE_VOID) {
        if (targetClass != 0 && (jitInfo->getClassAttribs(targetClass, methodInfo->ftn) & CORINFO_FLG_ARRAY ))
        {
            jitInfo->findCallSiteSig(tokenScope,token,tokenContext,&targetSigInfo);
        }
        OpType type = createOpType(targetSigInfo.retType, targetSigInfo.retTypeClass);
        TYPE_SWITCH_PRECISE(type,emit_pushresult,());
        if (!targetSigInfo.hasRetBuffArg()) // return buff logged in buildCall
        {
            if ( type.isByRef() )
            {
                CORINFO_CLASS_HANDLE childClassHandle;
                CorInfoType childType =  jitInfo->getChildType(type.cls(), &childClassHandle);
                type.setTarget(childType,childClassHandle);
            }
            type.toFPNormalizedType();
            pushOp(type);
        }
    }

    return FJIT_OK;
}

FJitResult FJit::compileDO_BR_boolean(int op)
{
    CHECK_STACK(1);
    VERIFICATION_CHECK( topOpE() == typeI4 ||  topOpE() == typeI8 ||  topOp().isRef() ||
                        topOp().isByRef() );
    if (ilrel < 0) {
        emit_trap_gc();
    }
    if (topOp() == typeI8)
    {
        emit_testTOS_I8();
    }
    else
    {
        emit_testTOS();
    }
    POP_STACK(1);
    return compileDO_JMP(op);
}

FJitResult FJit::compileDO_CEE_BEQ()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_BEQ)
    TYPE_SWITCH_Bcc(emit_CEQ,   // for I
                    emit_CEQ,   // for R
                    CEE_CondEq, // condition used for direct jumps
                    CEE_CondNotEq, // condition used when calling C<cond> helpers
                    true);      // allow Ref and ByRef
    return FJIT_OK;
}

FJitResult FJit::compileDO_CEE_BNE()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_BNE_UN)
    TYPE_SWITCH_Bcc(emit_CEQ,   // for I
                    emit_CEQ,   // for R
                    CEE_CondNotEq, // condition used for direct jumps
                    CEE_CondEq, // condition used when calling C<cond> helpers
                    true);      // allow Ref and ByRef
    return FJIT_OK;
}

FJitResult FJit::compileDO_CEE_BGT()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_BGT)
    TYPE_SWITCH_Bcc(emit_CGT,   // for I
                    emit_CGT,   // for R
                    CEE_CondGt, // condition used for direct jumps
                    CEE_CondNotEq, // condition used when calling C<cond> helpers
                    false);     // do not allow Ref and ByRef
    return FJIT_OK;
}
FJitResult FJit::compileDO_CEE_BGT_UN()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_BGE_UN)
    TYPE_SWITCH_Bcc(emit_CGT_UN,   // for I
                    emit_CGT_UN,   // for R
                    CEE_CondAbove, // condition used for direct jumps
                    CEE_CondNotEq, // condition used when calling C<cond> helpers
                    fals);      // do not allow Ref and ByRef
    return FJIT_OK;
}

FJitResult FJit::compileDO_CEE_BGE()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_BGE)
    TYPE_SWITCH_Bcc(emit_CLT,   // for I
                    emit_CLT_UN,   // for R
                    CEE_CondGtEq, // condition used for direct jumps
                    CEE_CondEq, // condition used when calling C<cond> helpers
                    false);     // do not allow Ref and ByRef
    return FJIT_OK;
}

FJitResult FJit::compileDO_CEE_BGE_UN()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_BGE_UN)
    TYPE_SWITCH_Bcc(emit_CLT_UN,   // for I
                    emit_CLT,   // for R
                    CEE_CondAboveEq, // condition used for direct jumps
                    CEE_CondEq, // condition used when calling C<cond> helpers
                    false);     // do not allow Ref and ByRef
    return FJIT_OK;
}

FJitResult FJit::compileDO_CEE_BLT()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_BLT)
    TYPE_SWITCH_Bcc(emit_CLT,   // for I
                    emit_CLT,   // for R
                    CEE_CondLt, // condition used for direct jumps
                    CEE_CondNotEq, // condition used when calling C<cond> helpers
                    false);     // do not allow Ref and ByRef
    return FJIT_OK;
}

FJitResult FJit::compileDO_CEE_BLT_UN()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_BLT_UN)
    TYPE_SWITCH_Bcc(emit_CLT_UN,   // for I
                    emit_CLT_UN,   // for R
                    CEE_CondBelow, // condition used for direct jumps
                    CEE_CondNotEq, // condition used when calling C<cond> helpers
                    false);        // do not allow Ref and ByRef
    return FJIT_OK;
}

FJitResult FJit::compileDO_CEE_BLE()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_BLE)
    TYPE_SWITCH_Bcc(emit_CGT,   // for I
                    emit_CGT_UN,   // for R
                    CEE_CondLtEq, // condition used for direct jumps
                    CEE_CondEq, // condition used when calling C<cond> helpers
                    false);     // do not allow Ref and ByRef
    return FJIT_OK;
}

FJitResult FJit::compileDO_CEE_BLE_UN()
{
    BINARY_COMP_BRANCH(topOp(), topOp(1), CEE_BLE_UN)
    TYPE_SWITCH_Bcc(emit_CGT_UN,   // for I
                    emit_CGT,   // for R
                    CEE_CondBelowEq, // condition used for direct jumps
                    CEE_CondEq, // condition used when calling C<cond> helpers
                    false);     // do not allow Ref and ByRef
    return FJIT_OK;
}

FJitResult FJit::compileDO_BR()
{
    // Emit a call to GC on backward jumps and leaves
    if (ilrel < 0) {
        emit_trap_gc();
    }
    return compileDO_JMP(CEE_CondAlways);
}

FJitResult FJit::compileDO_JMP( int op)
{

    // Define variables for ease of access
    unsigned target = &inPtr[ilrel]-inBuff;
    unsigned nextIP = inPtr-inBuff;
    unsigned int    len = methodInfo->ILCodeSize;            // IL size
    unsigned        address;
    int             merge_state;

#if !defined(FJIT_NO_VALIDATION)
    // If validation is on check for branches into/out of try/catch/finally/filter blocks
    // For "leave" instructions the validation is done with respect to target only i.e.
    // it is ok for the current instruction to be inside a block and target to be outside of it, but
    // not reverse

    CORINFO_EH_CLAUSE clause;
    for (unsigned except = 0; (except < methodInfo->EHcount); except++) {
            jitInfo->getEHinfo(methodInfo->ftn, except, &clause);
            if ( isClauseEmpty(&currentClause) ) {
                // Jumps from/into a try block are not allowed
                bool nextIP_InTry = clause.TryOffset < nextIP && nextIP <= clause.TryOffset+clause.TryLength;
                bool target_InTry = clause.TryOffset <= target && target < clause.TryOffset+clause.TryLength;
                if ( (nextIP_InTry ^ target_InTry) && ( nextIP_InTry || clause.TryOffset != target ) )
                { INDEBUG(printf( "Clause [%x, %x] CC [%x,%x] Target %x NextIP %x \n", clause.TryOffset,
                                clause.TryOffset+ clause.TryLength, currentClause.TryOffset,
                                currentClause.TryOffset+ currentClause.TryLength,target, nextIP );)
                VALIDITY_CHECK( false INDEBUG(&& "Attempting to branch from/into a try block")); }
                // Jumps from/into finally, catch clauses are not allowed
                if ( (clause.HandlerOffset < nextIP && nextIP <= clause.HandlerOffset+clause.HandlerLength) ^
                    (clause.HandlerOffset <= target && target < clause.HandlerOffset+clause.HandlerLength) )
                { INDEBUG(printf( "Clause [%x, %x] CC[%x, %x]Target %x NextIP %x \n", clause.HandlerOffset,
                                clause.HandlerOffset+ clause.HandlerLength,currentClause.HandlerOffset,
                                currentClause.HandlerOffset+ currentClause.HandlerLength,target, nextIP );)
                    VALIDITY_CHECK(false INDEBUG(&& "Attempting to branch from/into a handler")); }
                // Jump from/into filter clauses are not allowed
                if ((clause.Flags & CORINFO_EH_CLAUSE_FILTER) &&
                    (clause.FilterOffset < nextIP && nextIP <= clause.HandlerOffset) ^
                    (clause.FilterOffset <= target && target < clause.HandlerOffset) )
                    VALIDITY_CHECK(false INDEBUG(&& "Attempting to branch from/into a filter"));
            }
            else
            {
                // Leaves into a try block are not allowed unless we are leaving the associated try block
                if ( !(clause.TryOffset < nextIP && nextIP <= clause.TryOffset+clause.TryLength) &&
                    (clause.TryOffset < target && target < clause.TryOffset+clause.TryLength) )
                if (!(currentClause.HandlerOffset < nextIP && nextIP <= currentClause.HandlerOffset+currentClause.HandlerLength) || clause.TryOffset != currentClause.TryOffset || clause.TryLength != currentClause.TryLength )
                { INDEBUG(printf( "Clause [%x, %x] H [%x,%x] Target %x NextIP %x \n", clause.TryOffset,
                                clause.TryOffset+ clause.TryLength, clause.HandlerOffset,
                                clause.HandlerOffset+ clause.HandlerLength,target, nextIP );)
                VALIDITY_CHECK(false INDEBUG(&& "Attempting to leave into a try block"));
                }
                // Leaves into a catch/finally are not allowed
                if ( !(clause.HandlerOffset < nextIP && nextIP <= clause.HandlerOffset+clause.HandlerLength) &&
                    (clause.HandlerOffset <= target && target < clause.HandlerOffset+clause.HandlerLength) )
                { INDEBUG(printf( "Clause [%x, %x] CC[%x, %x]Target %x NextIP %x \n", clause.HandlerOffset,
                                clause.HandlerOffset+ clause.HandlerLength,currentClause.HandlerOffset,
                                currentClause.HandlerOffset+ currentClause.HandlerLength,target, nextIP );)
                    VALIDITY_CHECK(false INDEBUG(&& "Attempting to leave into a handler")); }
                if ((clause.Flags & CORINFO_EH_CLAUSE_FILTER) &&
                    (clause.FilterOffset < nextIP && nextIP <= clause.HandlerOffset) ^
                    (clause.FilterOffset <= target && target < clause.HandlerOffset) )
                    VALIDITY_CHECK(false INDEBUG(&& "Attempting to leave from/into a filter"));
            }
    }

    // Clear the current clause
    makeClauseEmpty(&currentClause);
#endif
    if ((ilrel == 0) && (op == CEE_CondAlways) && !LeavingCatchBlock && !LeavingTryBlock) {
        deregisterTOS;
        UncondBranch = true;
        return FJIT_OK;
    }

    //INDEBUG( printf("DO_JUMP op %d Target %x From %x To %x\n", op, ilrel, nextIP, target );)
    //INDEBUG(SplitOffsets.dumpStack();})

    // Make sure the target is not past beginning or end of the function
    VALIDITY_CHECK(&inPtr[ilrel]<(inBuff+len) && (target) >= 0);

    // If the target has already been jitted
    if ( state[target].isJitted ) {
        //Mark as a jump target
        state[target].isJmpTarget = true;
        // Emit a jump to the known PCoffset
        if (state[target].isTOSInReg)
            { enregisterTOS; }
        else
            { deregisterTOS; }
        address = mapping->pcFromIL(target);
        VALIDITY_CHECK(address > 0 );
        // Check that target is not in a middle of instruction
        VALIDITY_CHECK(address > 0 );
        emit_jmp_abs_address(op, address + (unsigned)outBuff, true);
        //  Do the stack merge of current with target
        merge_state = verifyStacks( target, 0 );
        VERIFICATION_CHECK( merge_state );
        if ( JitVerify && merge_state == MERGE_STATE_REJIT )
        {
            resetState(false);
            return FJIT_JITAGAIN;
        }
        // Do not jit the next IL opcode (Only for unconditional branches)
        if ( op == CEE_CondAlways )
        {
            popSplitStack = true;
            controlContinue = false;
        }
    }
    // Target has not been jitted
    else {
        // Emit an inverted jump to the next instruction(Only for conditional branches)
        if ( op != CEE_CondAlways )
        {
            //Invert the branch condition
            invertBranch( op );
            //Treat this as a forward jump to the next IL instruction so deregister the top of the stack
            state[ nextIP ].isJmpTarget = true;     //we mark fwd jmps as true
            deregisterTOS;
            //  Do the stack merge of current with target if necessary
            merge_state = verifyStacks( nextIP, 0 );
            VERIFICATION_CHECK( merge_state );
            if ( JitVerify && merge_state == MERGE_STATE_REJIT )
            {
                resetState(false);
                return FJIT_JITAGAIN;
            }
            state[ nextIP ].isTOSInReg = false;     //we always deregister on forward jmps
            // Add to the fix up table and emit a jump to the IL offset
            fixupTable->insert((void**) outPtr);
            emit_jmp_abs_address(op,nextIP, false);
            // Push the next instruction onto the split stack
            SplitOffsets.pushOffset(nextIP);
        }

        // Treat this as a forward branch
        state[target].isTOSInReg = false;
        deregisterTOS;

        // If we are leaving a try block - always emit a jump
        if ( LeavingTryBlock || LeavingCatchBlock )
        {
            state[target].isJmpTarget = true;
            //  Do the stack merge of current with target if necessary
            merge_state = verifyStacks( target, 0 );
            VERIFICATION_CHECK( merge_state );
            if ( JitVerify && merge_state == MERGE_STATE_REJIT )
            {
                resetState(false);
                return FJIT_JITAGAIN;
            }
            // Add to the fix up table and emit a jump to the IL offset
            fixupTable->insert((void**) outPtr);
            emit_jmp_abs_address(op, target, false);
            controlContinue = false;
            popSplitStack = true;
        }
        else {
            UncondBranch = true;
            // Start jitting at the jump target
            inPtr = &inPtr[ilrel];
        }
    }

    // Reset the flag indicating that a leave from a try block was jitted
    LeavingTryBlock   = false;
    LeavingCatchBlock = false;
    return FJIT_OK;
}

FJitResult FJit::compileEpilog(unsigned argsTotalSize)
{

    inRegTOS = false;

#ifdef _DEBUG
    if (!didLocalAlloc)
    {
        // it may be worth optimizing the following to only initialize locals so as to cover all refs.
        unsigned int localWords = (localsFrameSize+sizeof(void*)-1)/ sizeof(void*);
        unsigned retSlots;
        if (methodInfo->args.retType == CORINFO_TYPE_VALUECLASS)
            retSlots = typeSizeInSlots(jitInfo, methodInfo->args.retTypeClass);
        else
        {
            retSlots = computeArgSize(methodInfo->args.retType, 0, 0) / sizeof(void*);
            // adjust for the fact that the float is promoted to double on the IL stack
            if (methodInfo->args.retType == CORINFO_TYPE_FLOAT)
                retSlots += (sizeof(double) - sizeof(float))/sizeof(void*);
        }
        emit_stack_check(localWords + WORD_ALIGNED(retSlots));
    }
#endif // _DEBUG

#ifdef LOGGING
    if (codeLog) {
        emit_log_exit(szDebugClassName, szDebugMethodName);
    }
#endif
    if (methodAttributes & CORINFO_FLG_SYNCH) {
        LEAVE_CRIT;
    }

    // If this method returns a value load it into the machine dependent return mechanism
    if (methodInfo->args.retType != CORINFO_TYPE_VOID)
    {
        OpType type = createOpType(methodInfo->args.retType, methodInfo->args.retTypeClass);
        TYPE_SWITCH_PRECISE(type, emit_loadresult, ());
    }


    if (CALLER_CLEANS_STACK)
    {
        // If __cdecl calling convention is used the caller is responsible
        // for clearing the arguments from the stack
        argsTotalSize = 0;
    }

    emit_return(argsTotalSize, mapInfo.hasRetBuff);

    return FJIT_OK;
}

FJitResult FJit::compileCEE_RET()
{

    unsigned int    len = methodInfo->ILCodeSize;            // IL size

#if !defined(FJIT_NO_VALIDATION)
    // Verify that the 'ret' instruction is not inside an exception handling clause
    CORINFO_EH_CLAUSE clause;
    unsigned nextIP = inPtr - inBuff;

    for (unsigned except = 0; except < methodInfo->EHcount; except++) {
        jitInfo->getEHinfo(methodInfo->ftn, except, &clause);
        // Returns from a try block are not allowed
        if (clause.TryOffset < nextIP && nextIP <= clause.TryOffset+clause.TryLength)
            VALIDITY_CHECK(false INDEBUG( && "Attempting to return from a try block" ));
        // Returns from finally, filter, catch clauses are not allowed
        if (clause.HandlerOffset < nextIP && nextIP <= clause.HandlerOffset+clause.HandlerLength)
            VALIDITY_CHECK(false INDEBUG( && "Attempting to return from a handler") );
    }
#endif
    // INDEBUG(printf( "Jitting Return at %d\n", inPtr-inBuff );)

    // If this method returns a value make sure it is on the stack and has the appropriate type
    if (methodInfo->args.retType != CORINFO_TYPE_VOID) {
        // Check the type of the return value with the function signature
        CHECK_STACK_SIZE(1);
        OpType type = createOpType(methodInfo->args.retType, methodInfo->args.retTypeClass);
        type.toFPNormalizedType();
        VERIFICATION_CHECK( canAssign(jitInfo, methodInfo->ftn, topOp(), type ) );
        // Make sure that the return value doesn't contain any pointers to local stack
        VERIFICATION_CHECK( !verIsByRefLike(topOp() ) );
        POP_STACK(1);
    }
    else
        // The stack must be empty
        CHECK_STACK_SIZE(0);

    deregisterTOS;

    // Check if it is possible to emit the epilog right away, otherwise emit a forward branch to the epilog
    if (inPtr != &inBuff[len] || !SplitOffsets.isEmpty()) {
        //INDEBUG(printf( "Detected more code. Branching to the epilog \n");)

        fixupTable->insert((void**) outPtr);
        emit_jmp_abs_address(CEE_CondAlways, len, false);
        controlContinue = false;
        popSplitStack = true;
    }
    else
        FinishedJitting = true;

    return FJIT_OK;
}

FJitResult FJit::compileCEE_ENDFINALLY()
{
#if !defined(FJIT_NO_VALIDATION)
    // Verify that the current IL offset is inside a finally block
    CORINFO_EH_CLAUSE retClause; unsigned Start = 0, End = 0;
    getEnclosingClause( inPtr-inBuff, &retClause, 0, Start, End );
    VALIDITY_CHECK( !isClauseEmpty(&retClause) &&
                    (retClause.Flags & (CORINFO_EH_CLAUSE_FINALLY | CORINFO_EH_CLAUSE_FAULT) ) &&
                     retClause.HandlerOffset <= InstStart &&
                     InstStart < retClause.HandlerOffset + retClause.HandlerLength  )
#endif
    controlContinue = false;
    popSplitStack = true;
    emit_reset_storedTOS_in_JitGenerated_local(false);
    emit_ret(0, false);

    return FJIT_OK;
}

FJitResult FJit::compileDO_STARG( unsigned offset)
{
    OpType                  trackedType;
    stackItems            * varInfo;

    // Make sure that the offset is legal (with respect to the number of arguments)
    VALIDITY_CHECK(offset < args_len);
    // Obtain the type of the argument from the signature
    varInfo = &argsMap[offset];
    trackedType = varInfo->type;
    trackedType.toFPNormalizedType();
    // Verify that there is a value on the stack
    CHECK_STACK(1);
    // Verify that the type of the value on the stack matches the type of the argument
    VERIFICATION_CHECK( canAssign(jitInfo, methodInfo->ftn,  topOp(), trackedType ) || !"DO_STARG");
    trackedType = varInfo->type;

    if (methodInfo->args.isVarArg() && !varInfo->isReg && !PARAMETER_SPACE) {
        trackedType.toNormalizedType();
        emit_VARARG_LDARGA(varInfo->offset, offsetVarArgToken);
        TYPE_SWITCH(trackedType, emit_STIND_REV, (false));
        CHECK_POP_STACK(1);
    }
    else
    {
        TYPE_SWITCH_PRECISE(trackedType,emit_STVAR, (varInfo->offset));
        POP_STACK(1);
    }

    return FJIT_OK;
}

FJitResult FJit::compileDO_STLOC( unsigned offset)
{
    OpType                  trackedType;
    stackItems            * varInfo;

    // Make sure that the offset is legal (with respect to the number of locals )
    VALIDITY_CHECK(offset < methodInfo->locals.numArgs);
    // Obtain the type of the argument from the signature
    varInfo = &localsMap[offset];
    trackedType = varInfo->type;
    trackedType.toFPNormalizedType();
    // Verify that there is a value on the stack
    CHECK_STACK(1);
    // Verify that the type of the value on the stack matches the type of the argument
    VERIFICATION_CHECK( canAssign(jitInfo, methodInfo->ftn, topOp(), trackedType ) || !"DO_STLOC" );
    trackedType = varInfo->type;
    //trackedType.toNormalizedType();
    TYPE_SWITCH_PRECISE(trackedType,emit_STVAR, (varInfo->offset));
    POP_STACK(1);
    return FJIT_OK;
}


FJitResult FJit::compileCEE_CALLI()
{
    unsigned int            argBytes, stackPadorRetBase = 0;
    unsigned int            token;
    unsigned                sizeRetBuff;
    CORINFO_SIG_INFO        targetSigInfo;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    VERIFICATION_CHECK(false INDEBUG( || !"CALLI is not verifiable") );
    GET(token, unsigned int, false);   // token for sig of function
    jitInfo->findSig(tokenScope, token, methodInfo->ftn, &targetSigInfo);
    emit_save_TOS();        // squirel away the target ftn address
    emit_POP_PTR();         // and remove from stack
    CHECK_POP_STACK(1);

    VALIDITY_CHECK(!targetSigInfo.hasTypeArg());
    sizeRetBuff = targetSigInfo.hasRetBuffArg() ? typeSizeInBytes(jitInfo, targetSigInfo.retTypeClass) : 0;

    if ((targetSigInfo.callConv & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_STDCALL ||
        (targetSigInfo.callConv & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_C ||
        (targetSigInfo.callConv & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_THISCALL ||
        (targetSigInfo.callConv & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_FASTCALL)
    {
        // unmanaged calli is not supported
        return FJIT_INTERNALERROR;
    }
    else
    {
        argBytes = buildCall(&targetSigInfo, CALL_NONE, stackPadorRetBase, false);
        emit_restore_TOS(); //push the saved target ftn address
        emit_calli(sizeRetBuff);
    }

    return compileDO_PUSH_CALL_RESULT(argBytes, stackPadorRetBase, token, targetSigInfo, NULL);
}

FJitResult FJit::compileCEE_CALL()
{
    unsigned int            token;
    CORINFO_METHOD_HANDLE   targetMethod;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));

    CORINFO_CALL_INFO callInfo;

    // Call this because the CLR "misuses" this method to activate
    //   the target assembly (if needed). So if we would not call it
    //   later in the game the compiled code could try to call into
    //   the assembly which was not activated yet.
    // On the other hand we don't actually need any information
    //   provided by this call.
    jitInfo->getCallInfo(methodInfo->ftn,
                         tokenScope,
                         token,
                         0, // constraintToken -                                                             
                         methodInfo->ftn,
                         CORINFO_CALLINFO_KINDONLY,
                         &callInfo);

    targetMethod = jitInfo->findMethod(tokenScope, token, methodInfo->ftn);
    VALIDITY_CHECK(targetMethod);

    return this->compileHelperCEE_CALL(token, targetMethod, false /*readonly*/);
}

FJitResult FJit::compileHelperCEE_CALL(unsigned int token,
                              CORINFO_METHOD_HANDLE targetMethod,
                              bool isReadOnly /* = false */)
{
    unsigned int            argBytes, stackPadorRetBase = 0;
    unsigned int            parentToken;
    CORINFO_CLASS_HANDLE    targetClass, parentClass = NULL;
    CORINFO_SIG_INFO        targetSigInfo;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    // Get attributes for the method being called
    DWORD methodAttribs;
    methodAttribs = jitInfo->getMethodAttribs(targetMethod,methodInfo->ftn);
    // Get the class of the method being called
    targetClass = jitInfo->getMethodClass (targetMethod);

    // get the exact parent of the method
    parentToken = jitInfo->getMemberParent(tokenScope, token);
    parentClass = jitInfo->findClass(tokenScope,
                                     parentToken,
                                     methodInfo->ftn);

    // Get the attributes of the class of the method being called
    DWORD classAttribs;
    classAttribs = jitInfo->getClassAttribs(targetClass, methodInfo->ftn);
    // Verify that the method has an implementation i.e. it is not abstract
    VERIFICATION_CHECK(!(methodAttribs & CORINFO_FLG_ABSTRACT ));
    if (methodAttribs & CORINFO_FLG_SECURITYCHECK)
    {
        TailCallForbidden = TRUE;
        if (MadeTailCall)
        { // we have already made a tailcall, so cleanup and jit this method again
            if(cSequencePoints > 0)
                cleanupSequencePoints(jitInfo,sequencePointOffsets);
            resetContextState();
            return FJIT_JITAGAIN;
        }
    }

    jitInfo->getMethodSig(targetMethod, &targetSigInfo);
    if (targetSigInfo.isVarArg())
        jitInfo->findCallSiteSig(tokenScope,token,tokenContext,&targetSigInfo);

    // Verify that the arguments on the stack match the method signature
    int result_arg_ver = ( JitVerify ? verifyArguments( targetSigInfo, 0, false) : SUCCESS_VERIFICATION );
    VALIDITY_CHECK( result_arg_ver != FAILED_VALIDATION );
    VERIFICATION_CHECK( result_arg_ver != FAILED_VERIFICATION );
    // Verify the this argument for non-static methods( it is not part of the method signature )
    CORINFO_CLASS_HANDLE instanceClassHnd = jitInfo->getMethodClass(methodInfo->ftn);
    if (!( methodAttribs& CORINFO_FLG_STATIC) )
    {
        // For arrays we don't have the correct class handle
        if ( classAttribs & CORINFO_FLG_ARRAY)
            targetClass = jitInfo->findMethodClass( tokenScope, token, tokenContext );
        int result_this_ver = ( JitVerify
                                    ? verifyThisPtr(instanceClassHnd, targetClass, targetSigInfo.numArgs, false )
                                    : SUCCESS_VERIFICATION );
        VERIFICATION_CHECK( result_this_ver != FAILED_VERIFICATION );
    }

    // Verify the constraints on the target method (including its parent)
    VERIFICATION_CHECK( jitInfo->satisfiesClassConstraints(parentClass));
    VERIFICATION_CHECK( jitInfo->satisfiesMethodConstraints(parentClass, targetMethod));

    // Verify that the method is accessible from the call site
    VERIFICATION_CHECK(jitInfo->canAccessMethod(methodInfo->ftn, parentClass, targetMethod, instanceClassHnd ));

    if (targetSigInfo.hasTypeArg())
    {
        CORINFO_CLASS_HANDLE tokenType;
        // Instantiated generic method
        if(isReadOnly)
        {
            // when the call is readonly the Array Stub expects the type arg to
            // be zero
            emit_LDC_I(0);
        }
        else
        {
            TokenToHandle(parentToken, tokenType);
        }
    }

    argBytes = buildCall(&targetSigInfo, CALL_NONE, stackPadorRetBase, false );

    CORINFO_CONST_LOOKUP addrInfo;
    jitInfo->getFunctionEntryPoint(targetMethod, IAT_VALUE, &addrInfo);
    VALIDITY_CHECK(addrInfo.addr);
    VALIDITY_CHECK(addrInfo.accessType == IAT_VALUE || addrInfo.accessType == IAT_PVALUE);

    emit_callnonvirt((unsigned)addrInfo.addr, 
                     (targetSigInfo.hasRetBuffArg() ? typeSizeInBytes(jitInfo, targetSigInfo.retTypeClass) : 0),
                     addrInfo.accessType == IAT_PVALUE);

    return compileDO_PUSH_CALL_RESULT(argBytes, stackPadorRetBase, token, targetSigInfo, targetClass);
}


FJitResult FJit::compileCEE_CALLVIRT()
{
    unsigned int token;

    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(methodInfo->scope, token));

    return this->compileHelperCEE_CALLVIRT(token);
}

FJitResult FJit::compileHelperCEE_CALLVIRT(unsigned int token,
                              bool isReadOnly /* = false */)
{
    unsigned                offset;
    unsigned int            argBytes, stackPadorRetBase = 0;
    unsigned int            parentToken;
    unsigned                sizeRetBuff;
    CORINFO_METHOD_HANDLE   targetMethod;
    CORINFO_CLASS_HANDLE    targetClass, parentClass = NULL;
    CORINFO_SIG_INFO        targetSigInfo;
    CORINFO_METHOD_HANDLE   tokenContext= methodInfo->ftn;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;

    targetMethod = jitInfo->findMethod(tokenScope, token, tokenContext);
    VALIDITY_CHECK(targetMethod);

    DWORD methodAttribs;
    methodAttribs = jitInfo->getMethodAttribs(targetMethod, methodInfo->ftn);
    if (methodAttribs & CORINFO_FLG_SECURITYCHECK)
    {
        TailCallForbidden = true;
        if (MadeTailCall)
        { // we have already made a tailcall, so cleanup and jit this method again
            if(cSequencePoints > 0)
                cleanupSequencePoints(jitInfo,sequencePointOffsets);
            resetContextState();
            return FJIT_JITAGAIN;
        }
    }

    targetClass = jitInfo->getMethodClass(targetMethod);

    // get the exact parent of the method
    parentToken = jitInfo->getMemberParent(tokenScope, token);
    parentClass = jitInfo->findClass(tokenScope,
                                     parentToken,
                                     methodInfo->ftn);

    VALIDITY_CHECK(targetClass);

    DWORD classAttribs;
    classAttribs = jitInfo->getClassAttribs(targetClass,methodInfo->ftn);

    jitInfo->getMethodSig(targetMethod, &targetSigInfo);
    if (targetSigInfo.isVarArg())
        jitInfo->findCallSiteSig(tokenScope,token,tokenContext,&targetSigInfo);

    // Can't use callvirt on a value class
    VERIFICATION_CHECK(!(classAttribs & CORINFO_FLG_VALUECLASS));
    // Can't use callvirt on a static method
    VERIFICATION_CHECK(targetSigInfo.hasThis());

    // Verify that the arguments on the stack match the method signature
    int result_arg_ver = ( JitVerify ? verifyArguments( targetSigInfo, 0, false) : SUCCESS_VERIFICATION );
    VALIDITY_CHECK( result_arg_ver != FAILED_VALIDATION );
    VERIFICATION_CHECK( result_arg_ver != FAILED_VERIFICATION INDEBUG(|| !"CEE_CALLVIRT"));
    CORINFO_CLASS_HANDLE instanceClassHnd = jitInfo->getMethodClass(methodInfo->ftn);
    // Verify the this argument for non-static methods( it is not part of the method signature )
    if (!( methodAttribs& CORINFO_FLG_STATIC) )
    {
        // For arrays we don't have the correct class handle
        if ( classAttribs & CORINFO_FLG_ARRAY)
            targetClass = jitInfo->findMethodClass( tokenScope, token, tokenContext );
        int result_this_ver = ( JitVerify
                                    ? verifyThisPtr(instanceClassHnd, targetClass, targetSigInfo.numArgs, false)
                                    : SUCCESS_VERIFICATION );
        VERIFICATION_CHECK( result_this_ver != FAILED_VERIFICATION );
    }

    // Verify the constraints on the target method (including its parent)
    VERIFICATION_CHECK( jitInfo->satisfiesClassConstraints(parentClass));
    VERIFICATION_CHECK( jitInfo->satisfiesMethodConstraints(parentClass, targetMethod));

    // Verify that the method is accessible from the call site
    VERIFICATION_CHECK(jitInfo->canAccessMethod(methodInfo->ftn, parentClass, targetMethod, instanceClassHnd ));

    if (targetSigInfo.hasTypeArg())
    {
        CORINFO_CLASS_HANDLE tokenType;
        // Instantiated generic method
        if(isReadOnly)
        {
            // when the call is readonly the Array Stub expects the type arg to
            // be zero
            emit_LDC_I(0);
        }
        else
        {
            TokenToHandle(parentToken, tokenType);
        }

    }

    CORINFO_CALL_INFO virtCallInfo;

    jitInfo->getCallInfo(methodInfo->ftn,
                         tokenScope,
                         token,
                         0, // constraintToken -                                                             
                         tokenContext,
                         CORINFO_CALLINFO_CALLVIRT,
                         &virtCallInfo);

    if (virtCallInfo.kind == CORINFO_VIRTUALCALL_LDVIRTFTN)
    {
       int this_ptr = findOffsetOfThisPtr(targetSigInfo);
       emit_getSP((STACK_BUFFER + this_ptr - SIZE_STACK_SLOT));
       emit_LDIND_I4(false);
       emit_ldvirtftn_helper(token, jitInfo->getMemberParent(methodInfo->scope, token));
       emit_save_TOS();        // squirel away the target ftn address
       emit_POP_PTR();         // and remove from stack
    }

    argBytes = buildCall(&targetSigInfo, CALL_NONE, stackPadorRetBase, false);

    sizeRetBuff = targetSigInfo.hasRetBuffArg() ? typeSizeInBytes(jitInfo, targetSigInfo.retTypeClass) : 0;

    _ASSERTE (virtCallInfo.kind != CORINFO_CALL_CODE_POINTER);

    if (virtCallInfo.kind == CORINFO_VIRTUALCALL_LDVIRTFTN)
    {
        emit_restore_TOS(); //push the saved target ftn address

        // Now we can use the sequence for CALLI.
        emit_calli(targetSigInfo.hasRetBuffArg() ? typeSizeInBytes(jitInfo, targetSigInfo.retTypeClass) : 0);
    }
    else if (virtCallInfo.kind == CORINFO_VIRTUALCALL_STUB)
    {
        _ASSERTE (!virtCallInfo.stubLookup.lookupKind.needsRuntimeLookup);
        _ASSERTE (virtCallInfo.stubLookup.constLookup.addr != NULL);
        _ASSERTE(virtCallInfo.stubLookup.constLookup.accessType == IAT_PVALUE);
        emit_call_stub((unsigned int) virtCallInfo.stubLookup.constLookup.addr);
    }
    else if (virtCallInfo.kind == CORINFO_CALL)
    {
        if (virtCallInfo.nullInstanceCheck)
        {
            emit_check_null_reference(false);
        }

        CORINFO_CONST_LOOKUP addrInfo;
        jitInfo->getFunctionEntryPoint(targetMethod, IAT_VALUE, &addrInfo);
        VALIDITY_CHECK(addrInfo.addr);
        VALIDITY_CHECK(addrInfo.accessType == IAT_VALUE || addrInfo.accessType == IAT_PVALUE);

        emit_callnonvirt((unsigned)addrInfo.addr, sizeRetBuff, addrInfo.accessType == IAT_PVALUE);
    }
    else if (virtCallInfo.kind == CORINFO_VIRTUALCALL_VTABLE)
    {
        if (jitInfo->getClassAttribs(targetClass,methodInfo->ftn) & CORINFO_FLG_INTERFACE)
        {
            offset = jitInfo->getMethodVTableOffset(targetMethod);
            _ASSERTE(!(methodAttribs & CORINFO_FLG_EnC));
            unsigned InterfaceTableOffset;
            InterfaceTableOffset = jitInfo->getInterfaceTableOffset(targetClass);
            emit_callinterface_new(InterfaceTableOffset*4,
                offset, sizeRetBuff );
        }
        else
        {

            offset = jitInfo->getMethodVTableOffset(targetMethod);
            _ASSERTE(!(methodAttribs & CORINFO_FLG_DELEGATE_INVOKE));
            emit_callvirt(offset, sizeRetBuff);
        }
    }
    return compileDO_PUSH_CALL_RESULT(argBytes, stackPadorRetBase, token, targetSigInfo, targetClass);
}


FJitResult FJit::compileCEE_TAILCALL()
{
    unsigned int            token, parentToken = 0;
    unsigned                sizeRetBuff;
    OPCODE                  opcode;
    unsigned char           opcode_val;
    unsigned int            argBytes, stackPadorRetBase = 0;
    unsigned                offset;
    CORINFO_METHOD_HANDLE   targetMethod = NULL;
    CORINFO_CLASS_HANDLE    targetClass = NULL;
    CORINFO_SIG_INFO        targetSigInfo;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_METHOD_HANDLE   tokenContext = methodInfo->ftn;
    DWORD                   methodAttribs = 0, classAttribs = 0;

    unsigned char* savedInPtr;
    savedInPtr = inPtr;
    // Obtain the instruction following the .tail prefix
    GET(opcode_val, unsigned char, false);
    opcode = OPCODE(opcode_val);
    // Verify that the legal sequence is followed .tail {call|calli|callvirt} ret
    VERIFICATION_CHECK( opcode == CEE_CALL || opcode == CEE_CALLI || opcode == CEE_CALLVIRT );
    VALIDITY_CHECK( (inBuffEnd - inPtr) > 4 && inPtr[4] == CEE_RET );
    // Callvirt instruction is currently unverifiable
    VERIFICATION_CHECK( opcode != CEE_CALLI INDEBUG( && "CALLI is unverifiable"));
#ifdef LOGGING
    if (codeLog && opcode != CEE_PREFIXREF && (opcode > CEE_PREFIX1 || opcode < CEE_PREFIX7)) {
        bool oldstate = inRegTOS;
        emit_log_opcode(ilrel, opcode, oldstate);
        inRegTOS = oldstate;
    }
#endif
    // Get the token for the function
    GET(token, unsigned int, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));

    // Obtain the function signature
    if (opcode == CEE_CALLI)
    {
        jitInfo->findSig(tokenScope, token, tokenContext, &targetSigInfo);
    }
    else
    {
        targetMethod = jitInfo->findMethod(tokenScope, token, tokenContext);
        VALIDITY_CHECK(targetMethod);
        parentToken = jitInfo->getMemberParent(tokenScope, token);
        targetClass = jitInfo->findClass(tokenScope, parentToken, tokenContext);
        VALIDITY_CHECK(targetClass);
        jitInfo->getMethodSig(targetMethod, &targetSigInfo);
    }

    // Tail calls are invalid from inside try/filter/fault/finally/catch blocks
    CORINFO_EH_CLAUSE retClause; unsigned Start = 0, End = 0;
    getEnclosingClause( inPtr-inBuff, &retClause, 0, Start, End );
    VALIDITY_CHECK( isClauseEmpty(&retClause) );

    // Need to check that there is nothing on the stack except for the arguments to function being called
    CHECK_STACK_SIZE( targetSigInfo.numArgs + (targetSigInfo.hasThis() ? 1 : 0 ) + (opcode == CEE_CALLI ? 1 : 0));

    // Need to check that the return value of the current function is compatiable with the return
    // value of the function being called
    if (methodInfo->args.retType != CORINFO_TYPE_VOID)
    {
        // Obtain the stack type of the return value of the caller
        OpType retCaller = createOpType(methodInfo->args.retType, methodInfo->args.retTypeClass);
        retCaller.toFPNormalizedType();
        // Make sure the target function is not void
        VALIDITY_CHECK( targetSigInfo.retType != CORINFO_TYPE_VOID );
        // Obtain the stack type of the return value of the target
        OpType retTarget = createOpType( targetSigInfo.retType, targetSigInfo.retTypeClass);
        retTarget.toFPNormalizedType();
        VERIFICATION_CHECK( canAssign(jitInfo, methodInfo->ftn, retTarget, retCaller ) );
    }
    else
        VALIDITY_CHECK( targetSigInfo.retType == CORINFO_TYPE_VOID );

    if ( opcode != CEE_CALLI )
    {
        methodAttribs = jitInfo->getMethodAttribs(targetMethod, methodInfo->ftn);
        classAttribs = jitInfo->getClassAttribs(targetClass,methodInfo->ftn);

        // Check that there are no managed pointers being passed as a parameters and that the
        // arguments on the stack match the signature of the target function
        int result_arg_ver=(JitVerify ? verifyArguments(targetSigInfo, 0, true) : SUCCESS_VERIFICATION );
        VERIFICATION_CHECK( result_arg_ver != FAILED_VERIFICATION );
        // Check the this pointer if present to match the signature and not to contain any pointers
        // to local stack
        CORINFO_CLASS_HANDLE instanceClassHnd = jitInfo->getMethodClass(methodInfo->ftn);

        if (!( methodAttribs & CORINFO_FLG_STATIC) )
        {
            int result_this_ver = ( JitVerify
                                ? verifyThisPtr(instanceClassHnd, targetClass, targetSigInfo.numArgs, true)
                                : SUCCESS_VERIFICATION );
            VERIFICATION_CHECK( result_this_ver != FAILED_VERIFICATION );
        }
    }

    // No more VERIFICATION_CHECK's after this comment. All verification should be completed.

    // Obtain argument sizes
    int sizeCaller = 0, sizeTarget = 0, Flags = 0;
    bool thisTailCallAllowed = !TailCallForbidden;

    thisTailCallAllowed = thisTailCallAllowed && setupForTailcall(methodInfo->args,targetSigInfo, sizeCaller, sizeTarget, Flags);

    // Determine if tailcall is allowed by checking if the two method are located within
    // the same assembly and if they have compatiable security permissions
    thisTailCallAllowed = thisTailCallAllowed && jitInfo->canTailCall(methodInfo->ftn, ((opcode == CEE_CALL) ? targetMethod : NULL), true);

    // If the tail call is not allowed reset the flags and continue jitting the instruction after
    // the .tail prefix. We don't support tailcalls on vararg function so ignore the prefix and
    // on function which require synchronization. We don't support tail calls to functions
    // that have more arguments that the current function
    if (!thisTailCallAllowed ||
        (targetSigInfo.callConv & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_VARARG ||
        methodAttributes & CORINFO_FLG_SYNCH ||
        (CALLER_CLEANS_STACK && sizeTarget > sizeCaller)
        )
    {
      /* INDEBUG(printf("Denied .tail. Security: %d CallConv: %d Sync: %d ArgSize: %d (%d, %d)\n",
                        !thisTailCallAllowed,
                        (targetSigInfo.callConv  & CORINFO_CALLCONV_MASK) == CORINFO_CALLCONV_VARARG,
                        methodAttributes & CORINFO_FLG_SYNCH,
                        sizeTarget > sizeCaller, sizeTarget, sizeCaller ); )*/

        // reset the state of the flags to not jitted for the opcodes read after the .tail prefix
        for ( unsigned int size_operand = 0; size_operand <= (unsigned)(inPtr-savedInPtr); size_operand++ )
            state[inPtr-inBuff-size_operand].isJitted = false;
        // we don't have to rejit, but we need to ignore the tailcall prefix for this call
        inPtr = savedInPtr;
        return FJIT_OK;
    }

    // We decide what to do next on the basis of the type of the call instruction, but we will now
    // try to make a tailcall
    MadeTailCall = true;
    // Emit a GC check in case this tailcall is part of a closed loop
    emit_trap_gc();
    switch (opcode)
    {
    case CEE_CALLI:
        emit_save_TOS();        //squirrel away the target ftn address
        emit_POP_PTR();         //and remove from stack
        VALIDITY_CHECK(!targetSigInfo.hasTypeArg());
        argBytes = buildCall(&targetSigInfo, CALL_NONE, stackPadorRetBase,true);
        emit_tail_call(sizeCaller, sizeTarget, Flags);
        emit_restore_TOS();     //push the saved target ftn address
        emit_restore_state();   //restore callee saved registers that may have been modified
        emit_callhelper_il(FJit_pHlpTailCall);

        break;
    case CEE_CALL:

        // Need to notify profiler of Tailcall so that it can maintain accurate shadow stack
        if (flags & CORJIT_FLG_PROF_ENTERLEAVE)
        {
            BOOL bHookFunction;
            void *eeHandle;
            void *profilerHandle;
            BOOL bIndirected;

            jitInfo->GetProfilingHandle(methodInfo->ftn,
                                        &bHookFunction,
                                        &eeHandle,
                                        &profilerHandle,
                                        &bIndirected);

            if (bHookFunction)
            {
                _ASSERTE(!bIndirected); // FJIT does not handle indirected case.

                deregisterTOS;
                ULONG func = (ULONG) jitInfo->getHelperFtn(CORINFO_HELP_PROF_FCN_TAILCALL);
                emit_callhelper_prof3(func,
                                      CORINFO_HELP_PROF_FCN_TAILCALL,
                                      eeHandle,
                                      profilerHandle,
                                      NULL); // FRAME_INFO (see definition of FunctionTailcall2 in corprof.idl)
            }
        }

        if (targetSigInfo.hasTypeArg())
        {
            //INDEBUG( printf("Signature for tail call has a typearg\n"); )
            emit_LDC_I(targetClass);
        }
        argBytes = buildCall(&targetSigInfo, CALL_NONE, stackPadorRetBase,true);
        // push count of old arguments
        emit_tail_call(sizeCaller, sizeTarget, Flags);

        {
            CORINFO_CONST_LOOKUP addrInfo;
            jitInfo->getFunctionEntryPoint(targetMethod, IAT_VALUE, &addrInfo);
            VALIDITY_CHECK(addrInfo.addr);
            VALIDITY_CHECK(addrInfo.accessType == IAT_VALUE || addrInfo.accessType == IAT_PVALUE);

            emit_LDC_I((unsigned)addrInfo.addr);
            if (addrInfo.accessType == IAT_PVALUE)
            {
                emit_LDIND_PTR(false);
            }
        }
        emit_restore_state();   //restore callee saved registers that may have been modified
        emit_callhelper_il(FJit_pHlpTailCall);
        break;
    case CEE_CALLVIRT:

        VALIDITY_CHECK(targetClass = jitInfo->getMethodClass (targetMethod));

        // Need to notify profiler of Tailcall so that it can maintain accurate shadow stack
        if (flags & CORJIT_FLG_PROF_ENTERLEAVE)
        {
            BOOL bHookFunction;
            void *eeHandle;
            void *profilerHandle;
            BOOL bIndirected;

            jitInfo->GetProfilingHandle(methodInfo->ftn,
                                        &bHookFunction,
                                        &eeHandle,
                                        &profilerHandle,
                                        &bIndirected);

            if (bHookFunction)
            {
                _ASSERTE(!bIndirected); // FJIT does not handle indirected case.

                deregisterTOS;
                ULONG func = (ULONG) jitInfo->getHelperFtn(CORINFO_HELP_PROF_FCN_TAILCALL);
                emit_callhelper_prof3(func,
                                      CORINFO_HELP_PROF_FCN_TAILCALL,
                                      eeHandle,
                                      profilerHandle,
                                      NULL); // FRAME_INFO (see definition of FunctionTailcall2 in corprof.idl)
            }
        }

        if (targetSigInfo.hasTypeArg())
        {
            CORINFO_CLASS_HANDLE tokenType;
            // Instantiated generic method
            if (TypeFromToken(token) == mdtMethodSpec)
                TokenToHandle(token, tokenType);
            // otherwise must be an instance method in a generic struct, a static method in a generic type, or a runtime-generated array method
            else
                TokenToHandle(parentToken, tokenType);
        }

        CORINFO_CALL_INFO virtCallInfo;
        jitInfo->getCallInfo(methodInfo->ftn,
                             tokenScope,
                             token,
                             0, // constraintToken -                                                             
                             tokenContext,
                             CORINFO_CALLINFO_CALLVIRT,
                             &virtCallInfo);

        if (virtCallInfo.kind == CORINFO_VIRTUALCALL_LDVIRTFTN) {
            int this_ptr = findOffsetOfThisPtr(targetSigInfo);
            emit_getSP((STACK_BUFFER + this_ptr - SIZE_STACK_SLOT));
            emit_LDIND_I4(false);
            emit_ldvirtftn_helper(token, jitInfo->getMemberParent(methodInfo->scope, token));
            emit_save_TOS();        // squirel away the target ftn address
            emit_POP_PTR();         // and remove from stack
        }

        argBytes = buildCall(&targetSigInfo, CALL_NONE, stackPadorRetBase,true);

        sizeRetBuff = targetSigInfo.hasRetBuffArg() ? typeSizeInBytes(jitInfo, targetSigInfo.retTypeClass) : 0;

        _ASSERTE (virtCallInfo.kind != CORINFO_CALL_CODE_POINTER);

        if (virtCallInfo.kind == CORINFO_VIRTUALCALL_LDVIRTFTN)
        {
            emit_tail_call(sizeCaller, sizeTarget, Flags);
            emit_restore_TOS(); //push the saved target ftn address
        }
        else if (virtCallInfo.kind == CORINFO_VIRTUALCALL_STUB)
        {
            _ASSERTE (!virtCallInfo.stubLookup.lookupKind.needsRuntimeLookup);
            _ASSERTE (virtCallInfo.stubLookup.constLookup.addr != NULL);
            _ASSERTE(virtCallInfo.stubLookup.constLookup.accessType == IAT_PVALUE);

            // This value (0x2) should be the same as the one in the following lines
            // in jithelper JIT_TailCall so it understands that it is a Virtual Stub Dispatch tailcall.
            //    test    edx, 2
            //    jnz     VSDTailCall
            emit_tail_call(sizeCaller, sizeTarget, Flags | 0x2);
            emit_LDC_I((unsigned int) virtCallInfo.stubLookup.constLookup.addr);
        }
        else if (virtCallInfo.kind == CORINFO_CALL)
        {
            if (virtCallInfo.nullInstanceCheck)
            {
                emit_check_null_reference(false);
            }
            emit_tail_call(sizeCaller, sizeTarget, Flags);

            CORINFO_CONST_LOOKUP addrInfo;
            jitInfo->getFunctionEntryPoint(targetMethod, IAT_VALUE, &addrInfo);
            VALIDITY_CHECK(addrInfo.addr);
            VALIDITY_CHECK(addrInfo.accessType == IAT_VALUE || addrInfo.accessType == IAT_PVALUE);

            emit_LDC_I((unsigned)addrInfo.addr);
            if (addrInfo.accessType == IAT_PVALUE)
            {
                emit_LDIND_PTR(false);
            }
        }
        else if (virtCallInfo.kind == CORINFO_VIRTUALCALL_VTABLE)
        {
             if ( classAttribs & CORINFO_FLG_INTERFACE) {
                offset = jitInfo->getMethodVTableOffset(targetMethod);
                emit_tail_call(sizeCaller, sizeTarget, Flags);
                unsigned InterfaceTableOffset;
                InterfaceTableOffset = jitInfo->getInterfaceTableOffset(targetClass);
                emit_compute_interface_new(InterfaceTableOffset*4,
                                        offset,
                                        sizeRetBuff);
            }
            else {
                offset = jitInfo->getMethodVTableOffset(targetMethod);
                VALIDITY_CHECK(!(methodAttribs & CORINFO_FLG_DELEGATE_INVOKE));
                emit_tail_call(sizeCaller, sizeTarget, Flags);
                emit_compute_virtaddress(offset, sizeRetBuff);
            }
        }
        emit_restore_state();   //restore callee saved registers that may have been modified
        emit_callhelper_il(FJit_pHlpTailCall);

        break;
    default:
        FJIT_FAIL(FJIT_INTERNALERROR);
        break;
    } // switch (opcode) for tailcall
    controlContinue = false;
    popSplitStack = true;
    return FJIT_OK;
}


FJitResult FJit::compileCEE_THROW()
{
    CHECK_STACK(1);
    VERIFICATION_CHECK( topOp().isRef() );
    emit_THROW();
    controlContinue = false;
    popSplitStack = true;
    return FJIT_OK;
}

FJitResult FJit::compileCEE_RETHROW()
{
#if !defined(FJIT_NO_VALIDATION)
    // If validation is on check that rethrow is attempted from inside a catch handler
    CORINFO_EH_CLAUSE retClause; unsigned Start = 0, End = 0;
    getEnclosingClause( inPtr-inBuff, &retClause, 1, Start, End );
    VERIFICATION_CHECK(!isClauseEmpty(&retClause) &&
                    !(retClause.Flags & (CORINFO_EH_CLAUSE_FAULT | CORINFO_EH_CLAUSE_FINALLY)) &&
                    retClause.HandlerOffset <= InstStart &&
                    InstStart < retClause.HandlerOffset + retClause.HandlerLength )
#endif
    emit_RETHROW();
    controlContinue = false;
    popSplitStack = true;
    return FJIT_OK;
}

FJitResult FJit::compileCEE_SWITCH()
{
    int             merge_state;

    CHECK_STACK(1);
    unsigned int limit;
    unsigned int ilTableOffset;
    unsigned int ilNext, caseIP;
    unsigned char* saveInPtr;
    saveInPtr = inPtr;
    GET(limit, unsigned int, false);
    // Make sure that the limit is non-zero
    VERIFICATION_CHECK( limit > 0 );
    // Make sure that the argument on the stack is an integer
    VERIFICATION_CHECK( topOpE() == typeI4 );
    // insert a GC check if there is a backward branch
    while (limit-- > 0)
    {
        GET(ilrel, signed int, false);
        if (ilrel < 0)
        {
            emit_trap_gc();
            break;
        }
    }
    inPtr = saveInPtr;
    GET(limit, unsigned int, true);
    ilTableOffset = inPtr - inBuff;
    ilNext = ilTableOffset + limit*4;
    // Make sure the switch statement doesn't extend pas the end of the function
    VALIDITY_CHECK(ilNext < methodInfo->ILCodeSize);
    emit_pushconstant_4(limit);
    emit_SWITCH(limit);
    POP_STACK(1);

    //mark the start of the il branch table
    mapping->add(ilTableOffset, (unsigned) (outPtr - outBuff));
    //  Do the stack merge of current with out of bounds label
    merge_state = verifyStacks( ilNext, 0 );
    VERIFICATION_CHECK( merge_state );
    if ( JitVerify && merge_state == MERGE_STATE_REJIT )
    {
        resetState(false);
        return FJIT_JITAGAIN;
    }

    while (limit-- > 0) {
        GET(ilrel, signed int, true);
        caseIP = ilNext+ilrel;
#if !defined(FJIT_NO_VALIDATION)
        // If validation is on check for branches into/out of try/catch/finally/filter blocks
        CORINFO_EH_CLAUSE clause;

        for (unsigned except = 0; except < methodInfo->EHcount; except++) {
                jitInfo->getEHinfo(methodInfo->ftn, except, &clause);
                // Jumps from/into a try block are not allowed
                if ( (clause.TryOffset < ilNext && ilNext <= clause.TryOffset+clause.TryLength) ^
                    (clause.TryOffset < caseIP && caseIP < clause.TryOffset+clause.TryLength) )
                VALIDITY_CHECK(false INDEBUG(&& "Attempting to branch from/into a try block"));
                // Returns from finally, fault, catch clauses are not allowed
                if ( (clause.HandlerOffset < ilNext && ilNext <= clause.HandlerOffset+clause.HandlerLength) ^
                    (clause.HandlerOffset < caseIP && caseIP < clause.HandlerOffset+clause.HandlerLength) )
                VALIDITY_CHECK(false INDEBUG(&& "Attempting to branch from/into a handler"));
                // Jump from/into filter clauses are not allowed
                if ((clause.Flags & CORINFO_EH_CLAUSE_FILTER) &&
                    (clause.FilterOffset < ilNext && ilNext <= clause.HandlerOffset) ^
                    (clause.FilterOffset <= caseIP && caseIP < clause.HandlerOffset) )
                VALIDITY_CHECK(false INDEBUG(&& "Attempting to branch from/into a filter"));
                }
#endif
        //  Do the stack merge of current with stack at the label
        merge_state = verifyStacks( caseIP, 0 );
        VERIFICATION_CHECK( merge_state );
        if ( JitVerify && merge_state == MERGE_STATE_REJIT )
        {
            resetState(false);
            return FJIT_JITAGAIN;
        }
        // Backward jump to a target which has the TOS in a register
        VALIDITY_CHECK(!(ilrel < 0 && state[caseIP].isTOSInReg));
        // Emit unconditional branch to the IP offset
        fixupTable->insert((void**) outPtr);
        emit_jmp_abs_address(CEE_CondAlways, caseIP, false);
        SplitOffsets.pushOffset(caseIP);
    }
    fixupTable->insert((void**) outPtr);
    emit_jmp_abs_address(CEE_CondAlways, ilNext, false);
    SplitOffsets.pushOffset(ilNext);
    popSplitStack   = true;
    controlContinue = false;
    _ASSERTE(inPtr == ilNext+inBuff);
    return FJIT_OK;
}

FJitResult FJit::compileCEE_REFANYVAL()
{
    unsigned int            token;
    CORINFO_MODULE_HANDLE   tokenScope = methodInfo->scope;
    CORINFO_CLASS_HANDLE    targetClass;

    GET(token, unsigned __int32, false);
    VERIFICATION_CHECK(jitInfo->isValidToken(tokenScope, token));
    // Verify that the token corresponds to a class
    targetClass = jitInfo->findClass(tokenScope, token, methodInfo->ftn);
    VALIDITY_CHECK( targetClass );
    // There should be a refany on the stack
    CHECK_STACK(1);
    // This should be a validity check according to the spec, because the spec says
    // that REFANYVAL is always verifiable. However, V1 .NET Framework throws verification exception
    // so to match this behavior this is a verification check as well.
    if ( JitVerify && topOpE() == typeValClass )
    {
        CorInfoType eeType =  jitInfo->asCorInfoType(topOp().cls());
        VERIFICATION_CHECK( OpType(eeType).enum_() == typeRefAny );
    }
    else
        { VERIFICATION_CHECK( topOpE() == typeRefAny ); }
    POP_STACK(1);     // pop off the refany
    // The helper will throw InvalidCast or TypeLoad at runtime if typedref doesn't point to targetClass
    // so for verification purposes refanyval is always successful
    emit_REFANYVAL( targetClass );

    OpType val = OpType( typeByRef );

    // Convert the target type to a jit type
    CorInfoType eeType = jitInfo->asCorInfoType(targetClass);
    OpType targetType = createOpType(eeType, targetClass);
    val.setTarget( targetType.enum_(), targetType.cls() );

    pushOp( val );

    return FJIT_OK;
}

FJitResult FJit::compileCEE_REFANYTYPE()
{

    // There should be a refany on the stack
    CHECK_STACK(1);
    // There has to be a typedref on the stack
    // This should be a validity check according to the spec, because the spec says
    // that REFANYTYPE is always verifiable. However, V1 .NET Framework throws verification exception
    // so to match this behavior this is a verification check as well.
    VERIFICATION_CHECK( topOpE() == typeRefAny );
    // Pop off the Refany
    POP_STACK(1);
    _ASSERTE(offsetof(CORINFO_RefAny, type) == sizeof(void*));      // Type is the second thing
    emit_WIN32(emit_POP_I4()) emit_WIN64(emit_POP_I8());            // Just pop off the data, leaving the type.
    CORINFO_CLASS_HANDLE s_TypeHandleClass = jitInfo->getBuiltinClass(CLASSID_TYPE_HANDLE);
    VALIDITY_CHECK( s_TypeHandleClass != NULL );
    pushOp(OpType(typeValClass, s_TypeHandleClass));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_ARGLIST()
{
    // The following comment refers to the implementation (default calling conv) on x86 platform
    // The varargs token is always the last item pushed, which is
    // argument 'closest' to the frame pointer
    VALIDITY_CHECK(methodInfo->args.isVarArg());
    emit_LDVARA( offsetVarArgToken, sizeof(void *) );
    CORINFO_CLASS_HANDLE s_ArgHandle = jitInfo->getBuiltinClass(CLASSID_ARGUMENT_HANDLE);
    VALIDITY_CHECK( s_ArgHandle != NULL );
    pushOp(OpType(typeValClass, s_ArgHandle));
    return FJIT_OK;
}

FJitResult FJit::compileCEE_UNALIGNED()
{
    unsigned char * OpcodePtr;
    unsigned __int8 alignment;

    _ASSERTE( UnalignedAccess == false );
    // Get the alignment value and verify that it is correct
    GET(alignment, unsigned __int8, false);
    VERIFICATION_CHECK( alignment == 1 || alignment == 2 || alignment == 4 INDEBUG(|| "Invalid alignment detected\n"));

    OpcodePtr = inPtr;

    // Skip volatile prefix if necessary
    if ( OpcodePtr[0] == CEE_PREFIX1 && OpcodePtr[1] == (CEE_VOLATILE & 0xFF) )
       OpcodePtr = OpcodePtr + 2;

    // Verify that the legal sequence is followed .unaligned {ldind|stind|ldfld|stfld|ldobj|stobj|initblk|cpblk} ret
    VERIFICATION_CHECK( (CEE_LDIND_I1 <= OpcodePtr[0] && OpcodePtr[0] <= CEE_STIND_R8) || /* ldind && stind */
                        (CEE_STIND_I == OpcodePtr[0]) ||
                        (OpcodePtr[0] == CEE_PREFIX1 && OpcodePtr[1] == (CEE_INITBLK & 0xFF)) ||
                        (OpcodePtr[0] == CEE_PREFIX1 && OpcodePtr[1] == (CEE_CPBLK & 0xFF)) ||
                         OpcodePtr[0] == CEE_LDOBJ || OpcodePtr[0] == CEE_STOBJ ||
                         OpcodePtr[0] == CEE_LDFLD || OpcodePtr[0] == CEE_STFLD );

    UnalignedOffset = (inPtr-inBuff);
    return FJIT_OK;
}
