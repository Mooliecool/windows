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
#ifndef __HASHNODE_H_INCLUDED__
#define __HASHNODE_H_INCLUDED__

class CHashNode {
    public:
        CHashNode();
        virtual ~CHashNode();
    
        static HRESULT Create(LPCWSTR pwzSource, CHashNode **ppHashNode);

        BOOL IsDuplicate(LPCWSTR pwzStr) const;

    private:
        HRESULT Init(LPCWSTR pwzSource);

    private:
        LPWSTR                     _pwzSource;

};

#endif

