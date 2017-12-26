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
#ifndef __UNICODECAT_TABLE_H
#define __UNICODECAT_TABLE_H

////////////////////////////////////////////////////////////////////////////
//
//  Class:    CharacterInfoTable
//  Purpose:  This is the class to map a view of the Unicode category table and 
//            Unicode numeric value table.  It also provides
//            methods to access the Unicode category information.
//
//  Date:       August 31, 1999
//
////////////////////////////////////////////////////////////////////////////

class CharacterInfoTable {
    public:
        static CharacterInfoTable* CreateInstance(LPBYTE bytePtr);
        static CharacterInfoTable* GetInstance();

        
        CharacterInfoTable(LPBYTE bytePtr);


        BYTE GetUnicodeCategory(WCHAR wch);

    private:
    
        static CharacterInfoTable* m_pDefaultInstance;        
        LPWORD m_pLevel1ByteIndex;
};
#endif  // __UNICODECAT_TABLE_H
