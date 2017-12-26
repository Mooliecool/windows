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
** Class:  COMStringBuffer
**
**                                        
**
** Purpose: The implementation of the StringBuffer class.
**
** Date:  March 9, 1998
** 
===========================================================*/
#include "common.h"

#include "object.h"
#include "excep.h"
#include "frames.h"
#include "vars.hpp"
#include "comstring.h"
#include "comstringcommon.h"
#include "comstringbuffer.h"

#define DEFAULT_CAPACITY 16
#define DEFAULT_MAX_CAPACITY 0x7FFFFFFF

//
//Static Class Variables
//
MethodTable* COMStringBuffer::s_pStringBufferClass;

/*==============================CalculateCapacity===============================
**Calculates the new capacity of our buffer.  If the size of the buffer is 
**less than a fixed number (10000 in this case), we just double the buffer until
**we have enough space.  Once we get larger than 10000, we use a series of heuristics
**to determine the most appropriate size.
**
**Args:  currentCapacity:  The current capacity of the buffer
**       requestedCapacity: The minimum required capacity of the buffer
**Returns: The new capacity of the buffer.
**Exceptions: None.
==============================================================================*/
INT32 COMStringBuffer::CalculateCapacity (STRINGBUFFERREF thisRef, INT32 currentCapacity, INT32 requestedCapacity) {
    CONTRACTL {
        THROWS;
        MODE_COOPERATIVE;
        GC_NOTRIGGER;
        PRECONDITION(requestedCapacity >= 0);
    } CONTRACTL_END;

    INT32 newCapacity=currentCapacity;
    INT32 maxCapacity=thisRef->GetMaxCapacity();
    
    //This unfortunate situation can occur if they manually set the capacity to 0.
    if (newCapacity<=0) {
        newCapacity=DEFAULT_CAPACITY; 
    }

    if (requestedCapacity>maxCapacity) {
        COMPlusThrowArgumentOutOfRange(L"capacity", L"ArgumentOutOfRange_Capacity");
    }

    //Double until we find something bigger than what we need.
    while (newCapacity<requestedCapacity) {
        newCapacity*=2;
        if (newCapacity <= 0) { // if requestedCapacity is too large, don't try to double it.
            newCapacity = requestedCapacity;
            break;
        }
    }
    
    _ASSERTE(newCapacity > 0);
    //Also handle the unlikely case where we double so much that we're larger
    //than maxInt.
    if (newCapacity<=maxCapacity) {
        return newCapacity;
    }
    
    return maxCapacity;
}


/*==================================CopyString==================================
**Action: Creates a new copy of the string and then clears the dirty bit.
**        The Allocated String has a capacity of exactly newCapacity (we assume that
**        the checks for maxCapacity have been done elsewhere.)  If newCapacity is smaller
**        than the current size of the String, we truncate the String.
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
STRINGREF COMStringBuffer::CopyString(STRINGBUFFERREF *thisRef, STRINGREF CurrString, int newCapacity) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    int CurrLen;
    int copyLength;
    STRINGREF Local;

    _ASSERTE(newCapacity>=0);
    _ASSERTE(newCapacity<=(*thisRef)->GetMaxCapacity());

    //Get the data out of our current String.
    CurrLen = CurrString->GetStringLength();

    //Calculate how many characters to copy.  If we have enough capacity to 
    //accomodate all of our current String, we'll take the entire thing, otherwise
    //we'll just take the most that we can fit.
    if (newCapacity>=CurrLen) {
       copyLength = CurrLen;
    } else {
       _ASSERTE(!"Copying less than the full String.  Was this intentional?");
       copyLength = newCapacity;
    }

    //CurrString needs to be protected because it is used in NewString only *after*
    //we allocate a new string.
    GCPROTECT_BEGIN(CurrString);
    Local = COMString::NewString(&CurrString, 0, copyLength, newCapacity);
    GCPROTECT_END(); //CurrString

    return Local;
}

STRINGREF COMStringBuffer::GetRequiredString(STRINGBUFFERREF *thisRef, STRINGREF thisString, int requiredCapacity) {
    WRAPPER_CONTRACT;

    INT32 currCapacity = thisString->GetArrayLength()-1;
    if ((currCapacity>=requiredCapacity)) {
        return thisString;
    }
    return CopyString(thisRef, thisString, CalculateCapacity((*thisRef), currCapacity, requiredCapacity));
}

FCIMPL3(void, COMStringBuffer::ReplaceBufferInternal, StringBufferObject* thisRefUNSAFE, __in_ecount(newLength) WCHAR* newBuffer, INT32 newLength)
{
    STRINGBUFFERREF thisRef = (STRINGBUFFERREF)ObjectToOBJECTREF(thisRefUNSAFE);
    HELPER_METHOD_FRAME_BEGIN_1(thisRef);

    COMStringBuffer::ReplaceBuffer(&thisRef, newBuffer, newLength);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

/*================================ReplaceBuffer=================================
**This is a helper function designed to be used by N/Direct it replaces the entire
**contents of the String with a new string created by some native method.  This 
**will not be exposed through the StringBuilder class.
==============================================================================*/
void COMStringBuffer::ReplaceBuffer(STRINGBUFFERREF *thisRef, __in_ecount(newLength) WCHAR *newBuffer, INT32 newLength) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(newBuffer));
        PRECONDITION(newLength>=0);
    } CONTRACTL_END;

    STRINGREF thisString = NULL;
    WCHAR *thisChars;

    _ASSERTE(thisRef);
    _ASSERTE(*thisRef);

    void *tid;
    thisString = GetThreadSafeString(*thisRef,&tid);

    //This will ensure that we have enough space and take care of the CopyOnWrite
    //if needed.
    thisString = GetRequiredString(thisRef, thisString, newLength);
    thisChars = thisString->GetBuffer();

    //memcpy should blithely ignore any nulls which it finds in newBuffer.
    memcpyNoGCRefs(thisChars, newBuffer, newLength*sizeof(WCHAR));
    thisChars[newLength]='\0';
    thisString->SetStringLength(newLength);
    INT32 currCapacity = thisString->GetArrayLength()-1;
    
    // If the StringBuilder which has 0 capacity, we will be using String.Empty.
    // The high char state of String.Empty is determined, so we will get an assertion
    // if we don't have the check.
    if (!(newLength == 0 && currCapacity ==0)) {
        thisString->SetHighCharState(STRING_STATE_UNDETERMINED);
    }
    ReplaceStringRef(*thisRef, tid, thisString);
}

FCIMPL3(void, COMStringBuffer::ReplaceBufferAnsiInternal, StringBufferObject* thisRefUNSAFE, __in_ecount(newCapacity) CHAR* newBuffer, INT32 newCapacity)
{
    STRINGBUFFERREF thisRef = (STRINGBUFFERREF)ObjectToOBJECTREF(thisRefUNSAFE);
    HELPER_METHOD_FRAME_BEGIN_1(thisRef);

    COMStringBuffer::ReplaceBufferAnsi(&thisRef, newBuffer, newCapacity);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


/*================================ReplaceBufferAnsi=================================
**This is a helper function designed to be used by N/Direct it replaces the entire
**contents of the String with a new string created by some native method.  This 
**will not be exposed through the StringBuilder class.
**
**This version does Ansi->Unicode conversion along the way. Although
**making it a member of COMStringBuffer exposes more stringbuffer internals
**than necessary, it does avoid requiring a temporary buffer to hold
**the Ansi->Unicode conversion.
==============================================================================*/
void COMStringBuffer::ReplaceBufferAnsi(STRINGBUFFERREF *thisRef, __in_ecount(newCapacity) CHAR *newBuffer, INT32 newCapacity) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(newBuffer));
        PRECONDITION(newCapacity>=0);
    } CONTRACTL_END;

    STRINGREF thisString;
    WCHAR *thisChars;

    void *tid;
    thisString = GetThreadSafeString(*thisRef,&tid);

    //This will ensure that we have enough space and take care of the CopyOnWrite
    //if needed.
    thisString = GetRequiredString(thisRef, thisString, newCapacity);
    thisChars = thisString->GetBuffer();


    // NOTE: This call to MultiByte also writes out the null terminator
    // which is currently part of the String representation.
    INT32 ncWritten = MultiByteToWideChar(CP_ACP,
                                          MB_PRECOMPOSED,
                                          newBuffer,
                                          -1,
                                          thisChars,
                                          newCapacity+1);

    if (ncWritten == 0)
    {
        // Normally, we'd throw an exception if the string couldn't be converted.
        // In this particular case, we paper over it instead. The reason is
        // that most likely reason a P/Invoke-called api returned a
        // poison string is that the api failed for some reason, and hence
        // exercised its right to leave the buffer in a poison state.
        // Because P/Invoke cannot discover if an api failed, it cannot
        // know to ignore the buffer on the out marshaling path.
        // Because normal P/Invoke procedure is for the caller to check error
        // codes manually, we don't want to throw an exception on him.
        // We certainly don't want to randomly throw or not throw based on the
        // nondeterministic contents of a buffer passed to a failing api.
        *thisChars = L'\0';
        ncWritten++;
    }
    thisString->SetStringLength(ncWritten - 1);    
    INT32 currCapacity = thisString->GetArrayLength()-1;
    
    // If the StringBuilder which has 0 capacity, we will be using String.Empty.
    // The high char state of String.Empty is determined, so we will get an assertion
    // if we don't have the check.
    if (!(newCapacity == 0 && currCapacity ==0)) {
        thisString->SetHighCharState(STRING_STATE_UNDETERMINED);
    }

    ReplaceStringRef(*thisRef, tid, thisString);
}


// GetThreadSafeString
STRINGREF COMStringBuffer::GetThreadSafeString(STRINGBUFFERREF thisRef,void** currentThread) {
    WRAPPER_CONTRACT;
    *currentThread = ::GetThread();                    
    // Following two reads (stringref, threadid) needs to happen in order.
    // This is guaranteed by using volatile reads. 
    // See ReplaceString method in StringBuilder.cs for details.           
    STRINGREF thisString = thisRef->GetStringRef();
    if (thisRef->GetCurrentThread() != *currentThread) {
        INT32 currCapacity = thisString->GetArrayLength()-1;
        thisString = CopyString(&thisRef, thisString, currCapacity);
    }
    return thisString;
}


/*==============================LocalIndexOfString==============================
**Finds search within base and returns the index where it was found.  The search
**starts from startPos and we return -1 if search isn't found.  This is a direct 
**copy from COMString::IndexOfString, but doesn't require that we build up
**an instance of indexOfStringArgs before calling it.  
**
**Args:
**base -- the string in which to search
**search -- the string for which to search
**strLength -- the length of base
**patternLength -- the length of search
**startPos -- the place from which to start searching.
**
==============================================================================*/
INT32 COMStringBuffer::LocalIndexOfString(__in_ecount(strLength) WCHAR *base, __in_ecount(patternLength) WCHAR *search, int strLength, int patternLength, int startPos) {
  LEAF_CONTRACT
  _ASSERTE(base != NULL);
  _ASSERTE(search != NULL);

  int iThis, iPattern;
  for (iThis=startPos; iThis < (strLength-patternLength+1); iThis++) {
    for (iPattern=0; iPattern<patternLength && base[iThis+iPattern]==search[iPattern]; iPattern++);
    if (iPattern == patternLength) return iThis;
  }
  return -1;
}

/*================================ReplaceString=================================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
FCIMPL5(Object*, COMStringBuffer::ReplaceString, StringBufferObject* thisRefUNSAFE, StringObject* oldValueUNSAFE, StringObject* newValueUNSAFE, INT32 startIndex, INT32 count)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;		
    }
    CONTRACTL_END;

    struct _gc
    {
        STRINGBUFFERREF thisRef;
        STRINGREF       oldValue;
        STRINGREF       newValue;
    } gc;

    gc.thisRef     = (STRINGBUFFERREF)  thisRefUNSAFE;
    gc.oldValue    = (STRINGREF)        oldValueUNSAFE;
    gc.newValue    = (STRINGREF)        newValueUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    int *replaceIndex;
    int index=0;
    INT64 newBuffLength=0;
    int replaceCount=0;
    int replaceHolder=0;
    int readPos, writePos;
    int indexAdvance=0;
    WCHAR *thisBuffer, *oldBuffer, *newBuffer;
    int thisLength, oldLength, newLength;
    int endIndex;
    CQuickBytes replaceIndices;
    STRINGREF thisString=NULL;

    if (gc.thisRef==NULL) {
        COMPlusThrow(kNullReferenceException, L"NullReference_This");
    }

    //Verify all of the arguments.
    if (!gc.oldValue) {
        COMPlusThrowArgumentNull(L"oldValue", L"ArgumentNull_Generic");
    }

    //If they asked to replace oldValue with a null, replace all occurances
    //with the empty string.
    if (!gc.newValue) {
        gc.newValue = COMString::GetEmptyString();
    }

    void *tid;
    thisString = GetThreadSafeString(gc.thisRef,&tid);
    thisLength = thisString->GetStringLength();
    thisBuffer = thisString->GetBuffer();

    RefInterpretGetStringValuesDangerousForGC(gc.oldValue, &oldBuffer, &oldLength);
    RefInterpretGetStringValuesDangerousForGC(gc.newValue, &newBuffer, &newLength);

    //Range check our String.
    if (startIndex<0 || startIndex>thisLength) {
        COMPlusThrowArgumentOutOfRange(L"startIndex", L"ArgumentOutOfRange_Index");
    }

    if (count<0 || startIndex > thisLength - count) {
        COMPlusThrowArgumentOutOfRange(L"count", L"ArgumentOutOfRange_Index");
    }

    //Record the endIndex so that we don't need to do this calculation all over the place.
    endIndex = startIndex + count;

    //If our old Length is 0, we won't know what to replace
    if (oldLength==0) {
         COMPlusThrowArgumentException(L"oldValue", L"Argument_StringZeroLength");
    }

    // If the StringBuilder is empty, we don't need to do anything.
    // In addition, this can avoid the assertion below. See comments in ReplaceBufferAnsi.
    if( thisLength == 0) {
        goto lExit;
    }

    //replaceIndex is made large enough to hold the maximum number of replacements possible:
    //The case where every character in the current buffer gets replaced.
    replaceIndex = (int *)replaceIndices.AllocThrows((thisLength/oldLength+1)*sizeof(int));

    //Calculate all of the indices where our oldStrings end.  Finding the
    //ending is important because we're about to walk the string backwards.
    //If we're going to be walking the array backwards, we need to record the
    //end of the matched string, hence indexAdvance.
    if (newLength>oldLength) {
        indexAdvance = oldLength - 1;
    }

    index=startIndex;
    while (((index=LocalIndexOfString(thisBuffer,oldBuffer,thisLength,oldLength,index))>-1) && (index<=endIndex-oldLength)) {
        replaceIndex[replaceCount++] = index + indexAdvance;
        index+=oldLength;
    }

    //Calculate the new length of the string and ensure that we have sufficent room.
    newBuffLength = thisLength - ((oldLength - newLength) * (INT64)replaceCount);
    if (newBuffLength > 0x7FFFFFFF)
        COMPlusThrowOM();

    thisString = GetRequiredString(&gc.thisRef, thisString, (INT32)newBuffLength);

    //Get another pointer to the buffer in case it changed during the assure.
    thisBuffer = thisString->GetBuffer();
    newBuffer = gc.newValue->GetBuffer();

    //Handle the case where our new string is longer than the old string.
    //This requires walking the buffer backwards in order to do an in-place
    //replacement.
    if (newLength > oldLength) {
        //Decrement replaceCount so that we can use it as an actual index into our array.
        replaceCount--;

        //Walk the array backwards copying each character as we go.  If we reach an instance
        //of the string being replaced, replace the old string with the new string.
        readPos = thisLength-1;
        writePos = newBuffLength-1; 
        while (readPos>=0) {
          if (replaceCount>=0&&readPos==replaceIndex[replaceCount]) {
            replaceCount--;
            readPos-=(oldLength);
            writePos-=(newLength);
            memcpyNoGCRefs(&thisBuffer[writePos+1], newBuffer, newLength*sizeof(WCHAR));
          } else {
            thisBuffer[writePos--] = thisBuffer[readPos--];
          }
        }
        thisBuffer[newBuffLength]='\0';
        //Set the new String length and return.
        thisString->SetStringLength(newBuffLength);
        _ASSERTE(IS_STRING_STATE_UNDETERMINED(thisString->GetHighCharState()));

        ReplaceStringRef(gc.thisRef, tid, thisString);

        goto lExit;
    }

    //Handle the case where our old string is longer than or the same size as
    //the string with which we're about to replace it. This requires us to walk 
    //the buffer forward, differentiating it from the above case which requires us
    //to walk the array backwards.

    //Set replaceHolder to be the upper limit of our array.
    replaceHolder = replaceCount;
    replaceCount=0;

    //Walk the array forwards copying each character as we go.  If we reach an instance
    //of the string being replaced, replace the old string with the new string.
    readPos = 0;
    writePos = 0;
    while (readPos<thisLength) {
        if (replaceCount<replaceHolder&&readPos==replaceIndex[replaceCount]) {
          replaceCount++;
          readPos+=(oldLength);
          memcpyNoGCRefs(&thisBuffer[writePos], newBuffer, newLength*sizeof(WCHAR));
          writePos+=(newLength);
        } else {
          thisBuffer[writePos++] = thisBuffer[readPos++];
        }
    }
    thisBuffer[newBuffLength]='\0';

    thisString->SetStringLength(newBuffLength);
    _ASSERTE(IS_STRING_STATE_UNDETERMINED(thisString->GetHighCharState()));

    //Set the new String length and return.
    ReplaceStringRef(gc.thisRef, tid, thisString);

lExit: ;
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(gc.thisRef);
}
FCIMPLEND

/*==============================NewStringBuffer=================================
**Makes a new empty string buffer with the given capacity.  For EE use.
==============================================================================*/
// Called by the marshaller
STRINGBUFFERREF COMStringBuffer::NewStringBuffer(INT32 capacity) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(capacity >= 0);
    } CONTRACTL_END;

    STRINGREF Local;
    STRINGBUFFERREF Buffer;

    Local = COMString::NewString(capacity);
    PREFIX_ASSUME(OBJECTREFToObject(Local) != NULL);   
    Local->SetStringLength(0);

    _ASSERTE(s_pStringBufferClass != NULL);

    GCPROTECT_BEGIN(Local);
    Buffer = (STRINGBUFFERREF) AllocateObject(s_pStringBufferClass);
    GCPROTECT_END();//Local

    Buffer->SetStringRef(Local);
    Buffer->SetCurrentThread(::GetThread());
    Buffer->SetMaxCapacity(capacity);

    return Buffer;
}


/*===============================LoadStringBuffer===============================
**Initialize the COMStringBuffer Class.  Stores a reference to the class in  
**a static member of COMStringBuffer.
**
**Returns S_OK if it succeeded.  E_FAIL if it is unable to initialze the class.
**
**Args: None
==============================================================================*/
// Called by the loader
HRESULT __stdcall COMStringBuffer::LoadStringBuffer() {
  WRAPPER_CONTRACT;

  // Load the StringBuffer
  COMStringBuffer::s_pStringBufferClass = g_Mscorlib.GetClass(CLASS__STRING_BUILDER);
  
  return S_OK;
}









