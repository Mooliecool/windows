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
#ifndef __DEFAULT_CASING_TABLE_H
#define __DEFAULT_CASING_TABLE_H

////////////////////////////////////////////////////////////////////////////
//
//  Class:    CasingTable
//  Purpose:  This is the class to map a view of casing tables (from l_intl.nlp & l_except.nlp)
//            and create instances of NativeTextInfo from information of these tables.
//            The managed TextInfo will call methods on NativeTextInfo directly to do uppercasing/
//            lowercasing.
//
//  Date:     August 31, 1999
//
//  Note:     
//            The data used to do the lowercasing/uppercasing is stored in l_intl.nlp & l_except.nlp.
//          
//            l_intl.nlp stores the default linguistic casing table.  Default linguistic casing
//            table is the data used by most cultures to do linguistic-correct casing.
//            
//            However, there are cultures that do casing a little different from the default linguistic casing.
//            We say these cultures have 'exceptions'.  Based on the fact that the casing for most code points are 
//            the same and a few code points are different, we store the 'delta' information to 
//            the default linguistic casing table for these cultures.  The LCIDs for exception cultures 
//            and delta information are stored in l_except.nlp.
//          
//            One important culture which has an exception is the 'invariant culture' in NLS+.  Invariant 
//            culture has a culture ID of zero.  The casing table for the invariant culture is the one
//            used by file system to do casing.  It is not linguistically correct, but we have to provide
//            this for compatibilty with the file system. The invariant culture casing table is made from 
//            l_intl.nlp and fix up code points from l_except.nlp.
//
//
//            In summary, we have three types of casing tables:
//            1. Default linguistic casing table: 
//               This is like calling ::LCMapString() using LCMAP_LINGUISTIC_CASING. The
//               result of the casing is linguistically correct. However, not every culture
//               can use this table. See point 2.
//            2. Default linguistic casing table + Exception:
//               Turkish has a different linguistic casing.  There are two chars in Turkish
//               that have a different result from the default linguistic casing.  
//            3. Invariant culture casing.
//               This is like calling ::LCMapString() WITHOUT using LCMAP_LINGUISTIC_CASING.
//               This exists basically for the file system.
//
//            For those who understand Win32 NLS, we changed the l_intl.nls to make
//                linguitic casing to be the default table in l_intl.nlp. 
//            In Win32, the invariant culture casing is the default table, and stored in l_intl.nls.
//                And the linguistic casing is the exception with culture ID 0x0000.
//            The reason is that we use linguitic casing by default in NLS+ world, so the change
//                here saves us from fixing up linguistic casing.
//
////////////////////////////////////////////////////////////////////////////


class NativeTextInfo;
typedef NativeTextInfo* PNativeTextInfo;

class CasingTable : public NLSTable {
    public:
        CasingTable();
        ~CasingTable();
        
        NativeTextInfo* AllocateDefaultTable(LPBYTE pTextInfoTable);
        NativeTextInfo* InitializeNativeTextInfo(LPBYTE pExceptionFile, INT32 exceptionIndex);

        NativeTextInfo* GetDefaultNativeTextInfo();
                        
    private:
        void SetData(LPBYTE pCasingData);
    
        //
        // An initilization method used to read the casing exception table
        // and set up m_pExceptionHeader and m_pExceptionData.
        //
        BOOL GetExceptionHeader();
        
    private:

        //
        // The default NativeTextInfo which is used for the default linguistic
        // casing for most of the cultures.
        //
        NativeTextInfo*  m_pDefaultNativeTextInfo; 
        
        //
        // The number of cultures which have exceptions.
        //
        LONG m_nExceptionCount;
        
        //
        // An array which points to casing tables for cultures which have exceptions.
        // The size of this array is dynamicly decided by m_nExceptionCount.
        //
        NativeTextInfo** m_ppExceptionTextInfoArray;
        
        LPBYTE  m_pTextInfoTable;
        LPBYTE  m_pTextInfoExceptionTable;
        LPBYTE m_pUpperCaseTable;
        LPBYTE m_pLowerCaseTable;
        LPBYTE m_pTitleCasingTable;
        PlaneOffset* m_pPlaneOffset;

};
#endif  // __DEFAULT_CASING_TABLE_H
