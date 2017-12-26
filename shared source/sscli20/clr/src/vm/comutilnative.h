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
/*============================================================
**
** Header:  COMUtilNative
**       
**
** Purpose: A dumping ground for classes which aren't large
** enough to get their own file in the VM.
**
** Date:  April 8, 1998
**
===========================================================*/
#ifndef _COMUTILNATIVE_H_
#define _COMUTILNATIVE_H_

#include "object.h"
#include "util.hpp"
#include "cgensys.h"
#include "fcall.h"

//
//
// COMCHARACTER
//
//
class COMCharacter {
public:
    //These are here for support from native code.  They are never called from our managed classes.
    static BOOL nativeIsWhiteSpace(WCHAR c);
    static BOOL nativeIsDigit(WCHAR c);
};

//
//
// PARSE NUMBERS
//
//

#define MinRadix 2
#define MaxRadix 36

class ParseNumbers {

    enum FmtFlags {
      LeftAlign = 0x1,  //Ensure that these conform to the values specified in the managed files.
      CenterAlign = 0x2,
      RightAlign = 0x4,
      PrefixSpace = 0x8,
      PrintSign = 0x10,
      PrintBase = 0x20,
      TreatAsUnsigned = 0x10,
      PrintAsI1 = 0x40,
      PrintAsI2 = 0x80,
      PrintAsI4 = 0x100,
      PrintRadixBase = 0x200,
      AlternateForm = 0x400};

public:

    static INT32 GrabInts(const INT32 radix, __in_ecount(length) WCHAR *buffer, const int length, int *i, BOOL isUnsigned);
    static INT64 GrabLongs(const INT32 radix, __in_ecount(length) WCHAR *buffer, const int length, int *i, BOOL isUnsigned);    

    static FCDECL5(LPVOID, IntToString, INT32 l, INT32 radix, INT32 width, CLR_CHAR paddingChar, INT32 flags);
    static FCDECL5_VII(LPVOID, LongToString, INT64 l, INT32 radix, INT32 width, CLR_CHAR paddingChar, INT32 flags);
    static FCDECL4(INT32, StringToInt, StringObject * s, INT32 radix, INT32 flags, INT32* currPos);
    static FCDECL4(INT64, StringToLong, StringObject * s, INT32 radix, INT32 flags, INT32* currPos);
};

//
//
// EXCEPTION NATIVE
//
//

void FreeExceptionData(ExceptionData *pedata);

class ExceptionNative
{
private:
    enum ExceptionMessageKind {
        ThreadAbort = 1,
        ThreadInterrupted = 2,
        OutOfMemory = 3
    };

public:
    static FCDECL1(Object*, GetClassName, Object* pThisUNSAFE);
    static FCDECL1(FC_BOOL_RET, IsImmutableAgileException, Object* pExceptionUNSAFE);
    static FCDECL1(FC_BOOL_RET, IsTransient, INT32 hresult);
    static BOOL      IsException(MethodTable* pMT);
    static FCDECL1(StringObject*, GetMessageFromNativeResources, ExceptionMessageKind kind);

    // NOTE: caller cleans up any partially initialized BSTRs in pED
    static void      GetExceptionData(OBJECTREF, ExceptionData *);

    // Note: these are on the PInvoke class to hide these from the user.
    static FCDECL0(EXCEPTION_POINTERS*, GetExceptionPointers);
    static FCDECL0(INT32, GetExceptionCode);
};


//
//
// GUID NATIVE
//
//

class GuidNative
{
public:
    static FCDECL1(void, CompleteGuid, GUID* thisPtr);
};


//
// BitConverter
//
class BitConverter {
public:
    static FCDECL1(Object*, Base64StringToByteArray, StringObject* pvInString);
    static FCDECL3(Object*, Base64CharArrayToByteArray, CHARArray* pInCharArray, INT32 offset, INT32 length);

    static INT32 ConvertBase64ToByteArray(__out_ecount(length) INT32 *value, __in_ecount(length + offset) WCHAR *c,UINT offset,UINT length, UINT* trueLength);
    static INT32 ConvertByteArrayToByteStream(INT32 *value, __out_ecount(length) U1* b,UINT length);

    static const WCHAR base64[];
};


//
// Buffer
//
class Buffer {
public:

    // BlockCopy
    // This method from one primitive array to another based
    //      upon an offset into each an a byte count.
    static FCDECL5(VOID, BlockCopy, ArrayBase *src, int srcOffset, ArrayBase *dst, int dstOffset, int count);
    static FCDECL5(VOID, InternalBlockCopy, ArrayBase *src, int srcOffset, ArrayBase *dst, int dstOffset, int count);

    static FCDECL2(FC_UINT8_RET, GetByte, ArrayBase* arrayUNSAFE, INT32 index);
    static FCDECL3(void, SetByte, ArrayBase* arrayUNSAFE, INT32 index, UINT8 value);
    static FCDECL1(INT32, ByteLength, ArrayBase* arrayUNSAFE);
};

#define MIN_GC_MEMORYPRESSURE_THRESHOLD 100000

class GCInterface {
private:
    static BOOL m_cacheCleanupRequired;
    static MethodDesc *m_pCacheMethod;
    static UINT64   m_ulMemPressure;
    static UINT64   m_ulThreshold;
    static INT32    m_gc_gen; 
    static INT32    m_gc_counts[3];
    static INT32    m_induced_gc_counts[3];

public:
    static CrstStatic m_MemoryPressureLock;

    static BOOL IsCacheCleanupRequired();
    static void CleanupCache();
    static void SetCacheCleanupRequired(BOOL);
    static UINT64 InterlockedAdd(UINT64 addend);
    static UINT64 InterlockedSub(UINT64 subtrahend);
    
    // The following structure is provided to the stack skip function.  It will
    // skip until the frame below the supplied stack crawl mark.
    struct SkipStruct {
        StackCrawlMark *stackMark;
        MethodDesc*     pMeth;
    };
    static StackWalkAction SkipMethods(CrawlFrame*, VOID*);

    static FCDECL1(int,     GetGenerationWR, LPVOID handle);
    static FCDECL1(int,     GetGeneration, Object* objUNSAFE);
    static FCDECL0(INT64,   GetTotalMemory);
    static FCDECL1(void,    CollectGeneration, INT32 generation);
    static FCDECL0(int,     GetMaxGeneration);
    static FCDECL0(void,    RunFinalizers);
    static FCDECL1(void,    KeepAlive, Object *obj);
    static FCDECL1(void,    FCSuppressFinalize, Object *obj);
    static FCDECL1(void,    FCReRegisterForFinalize, Object *obj);
    static FCDECL0(void,    NativeSetCleanupCache);
    static FCDECL1(int,     CollectionCount, INT32 generation);
    static FCDECL1_V(void,    NativeAddMemoryPressure, UINT64 bytesAllocated);
    static FCDECL1_V(void,    NativeRemoveMemoryPressure, UINT64 bytesAllocated);

    static void RemoveMemoryPressure(UINT64 bytesAllocated);
    static void AddMemoryPressure(UINT64 bytesAllocated);

};

class COMInterlocked
{
public:
        static FCDECL1(INT32, Increment32, INT32 *location);
        static FCDECL1(INT32, Decrement32, INT32 *location);
        static FCDECL1(INT64, Increment64, INT64 *location);
        static FCDECL1(INT64, Decrement64, INT64 *location);
        static FCDECL2(INT32, Exchange, INT32 *location, INT32 value);
        static FCDECL2_IV(INT64,   Exchange64, INT64 *location, INT64 value);
        static FCDECL2(LPVOID, ExchangePointer, LPVOID* location, LPVOID value);
        static FCDECL3(INT32, CompareExchange,        INT32* location, INT32 value, INT32 comparand);
        static FCDECL3_IVV(INT64, CompareExchange64,        INT64* location, INT64 value, INT64 comparand);
        static FCDECL3(LPVOID, CompareExchangePointer, LPVOID* location, LPVOID value, LPVOID comparand);
        static FCDECL2_IV(float, ExchangeFloat, float *location, float value);
        static FCDECL2_IV(double, ExchangeDouble, double *location, double value);
        static FCDECL3_IVV(float, CompareExchangeFloat, float *location, float value, float comparand);
        static FCDECL3_IVV(double, CompareExchangeDouble, double *location, double value, double comparand);
        static FCDECL2(LPVOID, ExchangeObject, LPVOID* location, LPVOID value);
        static FCDECL3(LPVOID, CompareExchangeObject, LPVOID* location, LPVOID value, LPVOID comparand);
        static FCDECL2(INT32, ExchangeAdd32, INT32 *location, INT32 value);
        static FCDECL2_IV(INT64, ExchangeAdd64, INT64 *location, INT64 value);
        static FCDECL2_VV(void, ExchangeGeneric, TypedByRef location, TypedByRef value);
        static FCDECL3_VVI(void, CompareExchangeGeneric, TypedByRef location, TypedByRef value, LPVOID comparand);
};

class ManagedLoggingHelper {

public:
    static FCDECL6(INT32, GetRegistryLoggingValues, CLR_BOOL* bLoggingEnabled, CLR_BOOL* bLogToConsole, INT32 *bLogLevel, CLR_BOOL* bPerfWarnings, CLR_BOOL* bCorrectnessWarnings, CLR_BOOL* bSafeHandleStackTraces);
};


class ValueTypeHelper {
public:
    static FCDECL1(FC_BOOL_RET, CanCompareBits, Object* obj);
    static FCDECL2(FC_BOOL_RET, FastEqualsCheck, Object* obj1, Object* obj2);
    static FCDECL1(INT32, GetHashCode, Object* objRef);
};

#endif // _COMUTILNATIVE_H_
