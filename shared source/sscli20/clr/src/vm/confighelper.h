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
//*****************************************************************************
// ConfigHelper.h
// 
//*****************************************************************************
//
// These are unmanaged definitions of interfaces used call Managed Node Factories
// If you make any changes please do corresponding changes in \src\bcl\system\__xmlparser.cs
//
#ifndef _CONFIGHELPER_H
#define _CONFIGHELPER_H

#include <mscoree.h>
#include <xmlparser.h>
#include <mscorcfg.h>
#include "unknwn.h"
#include "../xmlparser/_reference.h"
#include "../xmlparser/_unknown.h"


#include "corffi.h"
#define IConfigHandler IManagedInstanceWrapper



class ConfigFactory : public _unknown<IXMLNodeFactory, &IID_IXMLNodeFactory>
{

public:
    ConfigFactory(IConfigHandler *pFactory);
    ~ConfigFactory();

    HRESULT STDMETHODCALLTYPE NotifyEvent( 
            /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
            /* [in] */ XML_NODEFACTORY_EVENT iEvt);

    HRESULT STDMETHODCALLTYPE BeginChildren( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ XML_NODE_INFO* __RPC_FAR pNodeInfo);
    
    HRESULT STDMETHODCALLTYPE EndChildren( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ BOOL fEmptyNode,
        /* [in] */ XML_NODE_INFO* __RPC_FAR pNodeInfo);
    
    HRESULT STDMETHODCALLTYPE Error( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ HRESULT hrErrorCode,
        /* [in] */ USHORT cNumRecs,
        /* [in] */ XML_NODE_INFO* __RPC_FAR * __RPC_FAR apNodeInfo)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return hrErrorCode;
    }
    
    HRESULT STDMETHODCALLTYPE CreateNode( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ PVOID pNodeParent,
        /* [in] */ USHORT cNumRecs,
        /* [in] */ XML_NODE_INFO* __RPC_FAR * __RPC_FAR apNodeInfo);

public:
    IConfigHandler *m_pManagedFactory;
};

class ConfigNative
{
    static HRESULT RunInternal(IConfigHandler *pFactory, LPCWSTR filename);

public:
    static FCDECL2(void, RunParser, Object* refHandlerUNSAFE, StringObject* strFileNameUNSAFE);
};

#endif //  _CONFIGHELPER_H
