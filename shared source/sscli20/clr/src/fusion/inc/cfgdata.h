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
#ifndef __CFGDATA_H_INCLUDED__
#define __CFGDATA_H_INCLUDED__

#include "list.h"

class CQualifyAssembly {
    public:
        CQualifyAssembly()
        : _pwzPartialName(NULL)
        , _pNameFull(NULL)
        {
        }

        virtual ~CQualifyAssembly()
        {
            SAFEDELETEARRAY(_pwzPartialName);
            SAFERELEASE(_pNameFull);
        }

    public:
        LPWSTR                            _pwzPartialName;
        IAssemblyName                    *_pNameFull;
};

class CBindingRedir {
    public:
        CBindingRedir()
        : _pwzVersionOld(NULL)
        , _pwzVersionNew(NULL)
        {
        }
            
        virtual ~CBindingRedir()
        {
            SAFEDELETEARRAY(_pwzVersionOld);
            SAFEDELETEARRAY(_pwzVersionNew);
        }

    public:
        LPWSTR                            _pwzVersionOld;
        LPWSTR                            _pwzVersionNew;
};

class CCodebaseHint {
    public:
        CCodebaseHint()
        : _pwzVersion(NULL)
        , _pwzCodebase(NULL)
        {
        }
       
        virtual ~CCodebaseHint()
        {
            SAFEDELETEARRAY(_pwzVersion);
            SAFEDELETEARRAY(_pwzCodebase);
        }

    public:
        LPWSTR                            _pwzVersion;
        LPWSTR                            _pwzCodebase;
};


class CAsmBindingInfo {
    public:
        CAsmBindingInfo()
        : _pwzName(NULL)
        , _pwzPublicKeyToken(NULL)
        , _pwzCulture(NULL)
        , _peKind(peNone)
        , _bApplyPublisherPolicy(TRUE)
        {
        }

        virtual ~CAsmBindingInfo()
        {
            LISTNODE                            pos = NULL;
            CBindingRedir                      *pRedir = NULL;
            CCodebaseHint                      *pCB = NULL;
            
            SAFEDELETEARRAY(_pwzName);
            SAFEDELETEARRAY(_pwzPublicKeyToken);
            SAFEDELETEARRAY(_pwzCulture);
            
            pos = _listBindingRedir.GetHeadPosition();
            while (pos) {
                pRedir = _listBindingRedir.GetNext(pos);
                SAFEDELETE(pRedir);
            }

            _listBindingRedir.RemoveAll();

            pos = _listCodebase.GetHeadPosition();
            while (pos) {
                pCB = _listCodebase.GetNext(pos);
                SAFEDELETE(pCB);
            }

            _listCodebase.RemoveAll();
        }

        static LONG ComparePE(CAsmBindingInfo *pAsmInfo1, CAsmBindingInfo *pAsmInfo2)
        {
            if (pAsmInfo1->_peKind == pAsmInfo2->_peKind) {
                return 0;
            }

            if (pAsmInfo1->_peKind > pAsmInfo2->_peKind) {
                return 1;
            }

            return -1;
        }

    public:
        LPWSTR                            _pwzName;
        LPWSTR                            _pwzPublicKeyToken;
        LPWSTR                            _pwzCulture;
        PEKIND                            _peKind;  
        BOOL                              _bApplyPublisherPolicy;
        List<CBindingRedir *>             _listBindingRedir;
        List<CCodebaseHint *>             _listCodebase;
};

#endif

