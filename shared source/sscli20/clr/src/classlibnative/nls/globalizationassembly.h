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
#ifndef _NLS_ASSEMBLY
#define _NLS_ASSEMBLY

class SortingTable;

class NativeGlobalizationAssembly : public NLSTable {
public:
    static void AddToList(NativeGlobalizationAssembly* pNGA);
    static NativeGlobalizationAssembly *FindGlobalizationAssembly(Assembly *targetAssembly);

    NativeGlobalizationAssembly(Assembly* pAssembly);
public:

    SortingTable* m_pSortingTable;

private:
    // Use the following two to construct a linked list of the NativeGlboalizationAssembly.
    // We will use this linked list to shutdown the NativeGlobalizationAssembly
    // ever created in the system.

    // The head of the linked list.
    static NativeGlobalizationAssembly* m_pHead;
    // The current node of the linked list.
    static NativeGlobalizationAssembly* m_pCurrent;
    
    //The next GlobalizationAssembly in the list.
    NativeGlobalizationAssembly* m_pNext;
};
#endif
