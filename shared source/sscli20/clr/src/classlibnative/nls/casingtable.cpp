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

#include "common.h"
#include "nlstable.h"   // class NLSTable
#include "nativetextinfo.h" // class NativeTextInfo
#include "casingtable.h" // class declaration
#include "memoryreport.h"

CasingTable::CasingTable() :
    NLSTable(SystemDomain::SystemAssembly()) {

    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    m_pTextInfoTable = NULL;
    m_pTextInfoExceptionTable = NULL;
    
    m_pDefaultNativeTextInfo = NULL;

    m_nExceptionCount = 0;
    m_ppExceptionTextInfoArray = NULL;

    m_pUpperCaseTable = NULL;
    m_pLowerCaseTable = NULL;
    m_pTitleCasingTable = NULL;
    m_pPlaneOffset = NULL;
}

CasingTable::~CasingTable() {
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    if (m_pDefaultNativeTextInfo) {
        delete m_pDefaultNativeTextInfo;
        m_pDefaultNativeTextInfo=NULL;
    }

    if (m_ppExceptionTextInfoArray) {
        for (int i=0; i<m_nExceptionCount; i++) {
            if (m_ppExceptionTextInfoArray[i]) {
                delete (m_ppExceptionTextInfoArray[i]);
            }
        }
        delete[] m_ppExceptionTextInfoArray;
    }   
}

/*=================================SetData==========================
**Action: Initialize the uppercase table pointer and lowercase table pointer from the specified data pointer.
**Returns: None.
**Arguments:
**      pCasingData WORD pointer to the casing data.
**Exceptions:
============================================================================*/

void CasingTable::SetData(LPBYTE pCasingData) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pCasingData));
    } CONTRACTL_END;
    struct TableHeader {
        WCHAR signatures[16];
        WORD version[4];
        DWORD offsetToUpperCaseTable;
        DWORD offsetToLowerCaseTable;
        DWORD offsetToTitleCaseTable;
        PlaneOffset planeOffset[16];
        WORD exceptionCount;
    };

    m_pTextInfoTable = pCasingData;

    // The first 0x20 bytes are the signature of the table.
    // The next 8 bytes are the version of the table.
    TableHeader* tableHeader = (TableHeader*)m_pTextInfoTable; 

    m_pUpperCaseTable = m_pTextInfoTable + tableHeader->offsetToUpperCaseTable;
    m_pLowerCaseTable = m_pTextInfoTable + tableHeader->offsetToLowerCaseTable;
    m_pTitleCasingTable = m_pTextInfoTable + tableHeader->offsetToTitleCaseTable;
    m_pPlaneOffset = &(tableHeader->planeOffset[0]);
    m_nExceptionCount = VAL16(tableHeader->exceptionCount);
        
}


/*=============================AllocateDefaultTable=============================
**Action:  Allocates the default casing table, gets the exception header information
**         for all tables, and allocates the cache of individual tables.  This should
**         always be called before calling AllocateIndividualTable.
**Returns: TRUE if success. Otherwise, retrun FALSE.
**Arguments: 
**      bytePtr The byte pointer to the head of the casing table.
**Exceptions: None
**
** NOTE NOTE NOTE NOTE NOTE NOTE
** This method requires synchronization.  Currently, we handle this through the 
** class initializer for System.Globalization.TextInfo.  If you need to call this
** outside of that paradigm, make sure to add your own synchronization.
==============================================================================*/
NativeTextInfo* CasingTable::AllocateDefaultTable(LPBYTE bytePtr) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;
    
    MEMORY_REPORT_CONTEXT_SCOPE("NLS");

    // This method is not thread-safe.  It needs managed code to provide syncronization.
    // The code is in the static ctor of TextInfo.
    if (m_pDefaultNativeTextInfo!=NULL)
        return (m_pDefaultNativeTextInfo);
    
    SetData(bytePtr);

    // An exception will be throw if OOM happens in the following line.
    NewHolder<NativeTextInfo> pTemp(new NativeTextInfo(
        m_pTextInfoTable, 
        m_pUpperCaseTable, 
        m_pLowerCaseTable, 
        m_pTitleCasingTable,
        m_pPlaneOffset
    ));
    
    if (!GetExceptionHeader()) {
        return (NULL);
    }
    PVOID result = FastInterlockCompareExchangePointer((LPVOID*)&m_pDefaultNativeTextInfo, (LPVOID)pTemp, (LPVOID)NULL);
    if (result == NULL) {
        // Returning NULL means that m_pDefuaultTable was NULL before the FastInterlockCompareExchangePointer().
        // So we beat other threads to get here.  Tell holder to keep the pointer.
        pTemp.SuppressRelease();
    } else {
        // Do nothing here.
        // The holder will clean up the object when it goes out the scope.
    }

    _ASSERTE(m_pDefaultNativeTextInfo != NULL);

    return (m_pDefaultNativeTextInfo);
}

NativeTextInfo* CasingTable::GetDefaultNativeTextInfo() {
    LEAF_CONTRACT;
    _ASSERTE(m_pDefaultNativeTextInfo != NULL);
    return (m_pDefaultNativeTextInfo);        
}

/*===========================InitializeNativeTextInfo============================
**Action: Verify that the correct casing table for a given lcid has already been
**        created.  Create it if it hasn't previously been.  
**Returns:    void
**Arguments:  The lcid for the table to be created.
**Exceptions: None.
**
** LOCK CONTRACT: NOT NEEDED.
** 
**  The NativeTextInfo is cached in a table indexed by the exceptionIndex.
**  The method uses InterlockCompareExchange() to prevent multiple threads
**  to store in the same slot.
**
==============================================================================*/
NativeTextInfo* CasingTable::InitializeNativeTextInfo(LPBYTE pExceptionFile, INT32 exceptionIndex) {
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pExceptionFile));
        PRECONDITION(m_nExceptionCount >= 0);
        PRECONDITION(exceptionIndex >= 0 && exceptionIndex < m_nExceptionCount);
    } CONTRACTL_END;
    
    //
    //If this locale has exceptions and we haven't yet allocated the table,
    //go ahead and allocate it now.  Cache the result in m_ppExceptionTextInfoArray.
    //
    if (m_ppExceptionTextInfoArray[exceptionIndex] == NULL) {
        MEMORY_REPORT_CONTEXT_SCOPE("NLS");

        struct TableHeader {
            WCHAR signatures[16];
            WORD version[4];
            INT32 ExceptionCount;
            PlaneOffset exceptionCasingOffset[1];    
        };
        TableHeader* pHeader = (TableHeader*)pExceptionFile;
        PlaneOffset* pCasingOffset = &(pHeader->exceptionCasingOffset[0]);
        LPBYTE pUpperCaseTable = (pExceptionFile + pCasingOffset[exceptionIndex].OffsetToUppercasing);
        LPBYTE pLowerCaseTable = (pExceptionFile + pCasingOffset[exceptionIndex].OffsetToLowercasing);

        NewHolder<NativeTextInfo> pTemp(new NativeTextInfo(
            m_pTextInfoTable, 
            pUpperCaseTable, 
            pLowerCaseTable, 
            m_pTitleCasingTable,
            m_pPlaneOffset
        ));

        PVOID result = FastInterlockCompareExchangePointer((LPVOID*)&m_ppExceptionTextInfoArray[exceptionIndex], (LPVOID)pTemp, (LPVOID)NULL);
        if (result == NULL) {
            // Returning NULL means that m_pDefuaultTable was NULL before the FastInterlockCompareExchangePointer().
            // So we beat other threads to get here.  Tell holder to keep the pointer.
            pTemp.SuppressRelease();
        } else {
            // Do nothing here.
            // The holder will clean up the object when it goes out the scope.
        }

        _ASSERTE(m_ppExceptionTextInfoArray[exceptionIndex] != NULL);
        
            
    }
    return (m_ppExceptionTextInfoArray[exceptionIndex]);
}

// This can not be a static method because MapDataFile() is not a static method anymore after
// adding the Assembly versioning support in NLSTable.  // Is this true any more?
BOOL CasingTable::GetExceptionHeader() {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    //
    // Create m_ppExceptionTextInfoArray, and initialize the pointers to NULL.
    // m_ppExceptionTextInfoArray holds pointers to all of the tables including the default
    // casing table
    //
    m_ppExceptionTextInfoArray = new PNativeTextInfo[m_nExceptionCount];
    if (m_ppExceptionTextInfoArray == NULL) {
        return (FALSE);
    }
    ZeroMemory((LPVOID)m_ppExceptionTextInfoArray, m_nExceptionCount * sizeof (PNativeTextInfo));
    
    return (TRUE);
}




