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
#include "nlstable.h"       // class NLSTable
#include "unicodecattable.h"     // Class declaraction.
#include "memoryreport.h"

CharacterInfoTable* CharacterInfoTable::m_pDefaultInstance = NULL;

/*
    The structure of unicat.nlp:
        The first 256 bytes is the level 1 index for the highest 8 bits (the 8 part),
            and this is pointed by m_pByteData.  The content is an index (which
            has a value in byte range) points to an item in the level 2 index.
        Followed by the the level 1 index is the level 2 index for the highest 4 bits of 
            the lowest 8 bits (the 4 part).  The content is an offset (which has 
            a value in word range) points to an item in the level 3 value table.
        Every item in the level 3 value table has 16 bytes.
 */

CharacterInfoTable::CharacterInfoTable(LPBYTE bytePtr) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(bytePtr));
    } CONTRACTL_END;

    // This strucutre should match the same header defintion in the managed class CharUnicodeInfo.
     struct UnicodeDataHeader {
        WCHAR TableName[16];    // WCHAR[16]
        WORD version[4];    // WORD[4]
        DWORD OffsetToUnicodeCategory; // DWORD
        DWORD OffsetToNumbericIndex; // DWORD
        DWORD OffsetToNumbericValue; // DWORD
    };

    UnicodeDataHeader* pFileHeader = (UnicodeDataHeader*)bytePtr;
    
    m_pLevel1ByteIndex = (LPWORD)(bytePtr + pFileHeader->OffsetToUnicodeCategory);

}

BYTE CharacterInfoTable::GetUnicodeCategory(WCHAR wch) {
    LEAF_CONTRACT;
    // Access the 8:4:4 table.  The compiler should be smart enough to remove the redundant locals in the following code.
    // These locals are added so that we can debug this easily from the debug build.
    WORD index = m_pLevel1ByteIndex[GET8(wch)];
    index = m_pLevel1ByteIndex[index + GETHI4(wch)];
    LPBYTE pValueByteArray = (LPBYTE)&(m_pLevel1ByteIndex[index]);
    BYTE result = pValueByteArray[GETLO4(wch)];
    return (result);
}

CharacterInfoTable* CharacterInfoTable::CreateInstance(LPBYTE bytePtr) {
    WRAPPER_CONTRACT;

    if (m_pDefaultInstance != NULL) {
        return (m_pDefaultInstance);
    }

    MEMORY_REPORT_CONTEXT_SCOPE("NLS");

    // The following will throw exception when OOM happens.    
    NewHolder<CharacterInfoTable> pCharacterInfoTable(new CharacterInfoTable(bytePtr));

    // Check if m_pDefaultInstance has been set by another thread before the current thread.
    PVOID result = FastInterlockCompareExchangePointer((LPVOID*)&m_pDefaultInstance, (LPVOID)pCharacterInfoTable, (LPVOID)NULL);
    if (result == NULL) {
        // We are the first here.  Tell holder to keep the pointer.
        pCharacterInfoTable.SuppressRelease();
    } else {
        // Do nothing here.
        // The holder will clean up the object when it goes out the scope.
    }
    return (m_pDefaultInstance);
}

CharacterInfoTable* CharacterInfoTable::GetInstance() {
    LEAF_CONTRACT;
    return (m_pDefaultInstance);
}
