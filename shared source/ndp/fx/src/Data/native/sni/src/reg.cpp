//****************************************************************************
//              Copyright (c) Microsoft Corporation.
//
// @File: open.hpp
// @Owner: nantu, petergv
// @Test: milu
//
// <owner current="true" primary="true">nantu</owner>
// <owner current="true" primary="false">petergv</owner>
//
// Purpose: Connection string parsing
//
// Notes: Everything in this file is shared with dbnetlib.dll ( dbnetlib has a separate copy of this file ).
//        So, special attention is needed whenever anything in this file needs to be changed.
//
//          
// @EndHeader@
//****************************************************************************

#include "snipch.hpp"
#include "ssrp.hpp"
#include "NLregC.h"

#define MAX_CACHEENTRY_LENGTH ( 11 + 1                         \
							    + MAX_PROTOCOLNAME_LENGTH + 1  \
							    + MAX_NAME_SIZE + 1            \
                                + 11 )		

#define DEFAULT_PROTOCOLS	TEXT("sm\0")  \
							TEXT("tcp\0") \
							TEXT("np\0")

#define DEFAULT_PROTOCOLS_SANDBOX	TEXT("np\0")

namespace LastConnectCache 
{

// Linked List impl. for LastConnectCache
//
class Cache;

class CacheItem
{
	friend class Cache;

    LPSTR m_szValName;  // Name of the cache value
    LPSTR m_szValue;    // Cache value content
    CacheItem * m_pNext;       // Pointer to next element in list

public:

	CacheItem():
		m_pNext(0),
		m_szValName(0),
		m_szValue(0)
	{
	}
	~CacheItem()
	{
		delete m_szValName;	//szValue is inside this also
	}

	BOOL SetValue( const char *szValName, const char *szValue)
	{
		BidxScopeAutoSNI2( SNIAPI_TAG _T( "szValName: \"%hs\", szValue: \"%hs\"\n"), 
						szValName, szValue);
	
		int cchValName=(int)strlen(szValName) + 1;
		int cchVal=(int)strlen(szValue) + 1;

   		m_szValName = NewNoX(gpmo) char[ cchValName + cchVal];
		if( !m_szValName )
		{
			BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{BOOL}\n"), FALSE);
			return FALSE;
		}
		
		m_szValue = m_szValName + cchValName;
		(void)StringCchCopyA(m_szValName,cchValName,szValName);
		(void)StringCchCopyA(m_szValue,cchVal,szValue);

		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{BOOL}\n"), TRUE);

		return TRUE;
	}
	
	inline BOOL CopyValue(__out_ecount(cchDest) char *szDest, DWORD cchDest)
	{
		return SUCCEEDED(StringCchCopyA(szDest, cchDest, m_szValue));
	}
};


class Cache{

	CacheItem *pHead;
public:
	Cache():
		pHead(0)
	{
	}
	
	~Cache()
	{
		Cleanup();
	};

	void Cleanup()
	{
		BidxScopeAutoSNI0( SNIAPI_TAG _T( "\n") );
		
		CacheItem *pNext=pHead;
		while(pHead){
			pNext = pHead->m_pNext;
			delete pHead;
			pHead=pNext;
		}
	}
	BOOL Insert( const char *szValName, const char *szValue)
	{
		BidxScopeAutoSNI2( SNIAPI_TAG _T( "szValName: \"%hs\", szValue: \"%hs\"\n"), 
						szValName, szValue);
		
		if(MAX_CACHEENTRY_LENGTH<strlen(szValue))
		{			
			BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{BOOL}\n"), FALSE);
			return FALSE;
		}
	
	    CacheItem * pNewItem = NewNoX(gpmo) CacheItem();

		if( !pNewItem )
		{
			BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{BOOL}\n"), FALSE);
			return FALSE;
		}

	    	if( !pNewItem->SetValue(szValName, szValue ))
	    	{
	    		delete pNewItem;
			BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{BOOL}\n"), FALSE);
	    		return FALSE;
	    	}

		pNewItem->m_pNext = pHead;			//it points to old first link
		pHead = pNewItem;			//now first points to this

		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{BOOL}\n"), TRUE);
		return TRUE;
	}

	// Find item in the cache
	CacheItem * Find( const char * szValName)   
	{                           
		BidxScopeAutoSNI1( SNIAPI_TAG _T( "szValName: \"%hs\"\n"), szValName);

		CacheItem *pCurrent;

		for ( pCurrent = pHead; pCurrent; pCurrent = pCurrent->m_pNext )
OACR_WARNING_PUSH
OACR_WARNING_DISABLE(SYSTEM_LOCALE_MISUSE , " INTERNATIONALIZATION BASELINE AT KATMAI RTM. FUTURE ANALYSIS INTENDED. ")
			if ( CSTR_EQUAL == CompareStringA(LOCALE_SYSTEM_DEFAULT,
									 NORM_IGNORECASE|NORM_IGNOREWIDTH,
									 pCurrent->m_szValName, -1,
									 szValName, -1))
OACR_WARNING_POP
				break;

		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%p{CacheItem *}\n"), pCurrent);
		
		return pCurrent;
	}

	// Remove item from the cache
	BOOL Remove(const char * szValName)           
	{
		BidxScopeAutoSNI1( SNIAPI_TAG _T( "szValName: \"%hs\"\n"), szValName);
		
		CacheItem ** ppCurrentItem = &pHead;

		for ( ; *ppCurrentItem ; ppCurrentItem = &(*ppCurrentItem)->m_pNext)
		{

OACR_WARNING_PUSH
OACR_WARNING_DISABLE(SYSTEM_LOCALE_MISUSE , " INTERNATIONALIZATION BASELINE AT KATMAI RTM. FUTURE ANALYSIS INTENDED. ")
			if ( CSTR_EQUAL == CompareStringA(LOCALE_SYSTEM_DEFAULT,
									 NORM_IGNORECASE|NORM_IGNOREWIDTH,
									 (*ppCurrentItem)->m_szValName, -1,
									 szValName, -1))
OACR_WARNING_POP
			{
				CacheItem * pDeleteItem = *ppCurrentItem;

				*ppCurrentItem = (*ppCurrentItem)->m_pNext;
				delete pDeleteItem;
				BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{BOOL}\n"), TRUE);
				return TRUE;
			}
		}

		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{BOOL}\n"), FALSE);
		
		return FALSE;
	}
	
};

Cache *pgLastConnectCache;         // the LastConnectCache

SNICritSec 	* critsecCache = 0;

#ifndef SNIX
//	For SNIX version, see below
//
void Initialize()
{
	BidxScopeAutoSNI0( SNIAPI_TAG _T( "\n"));

	BidTraceU1( SNI_BID_TRACE_ON, SNI_TAG _T("pgLastConnectCache: %p\n"), pgLastConnectCache);

	Assert( !pgLastConnectCache );

	LPSTR szValueNameList = 0;
	LPSTR szValue = 0;

	// Initialize in memory Cache
	pgLastConnectCache = NewNoX(gpmo) Cache();

	if( !pgLastConnectCache )
	{
		goto ErrorExit;
	}
	
	if( ERROR_SUCCESS != SNICritSec::Initialize(&critsecCache))
	{
		goto ErrorExit;
	}

	// Initialize persistent cache in registry.
	// First, create LastConnectionCache if it doesnt exist yet.
	// In case of failure, we go ahead silently.
	CScreateLastConnectionCache(FALSE);
	
	//Second, Read in cached values from the registry. 
	DWORD dwcbValueNameListLen = 0; 
	DWORD dwcbMaxValueLen = 0;

	if ( ERROR_SUCCESS != CSgetCachedValueList( NULL,
											    &dwcbValueNameListLen, 
												&dwcbMaxValueLen ) )
	{
		goto ErrorExit1;
	}

	 // Bump up the size to alloc space for '\0'
	if (FAILED (DWordAdd(dwcbMaxValueLen, 1, &dwcbMaxValueLen)))
		goto ErrorExit1;

	szValueNameList = NewNoX(gpmo) char[ dwcbValueNameListLen ];
	if( NULL == szValueNameList )
	{
		goto ErrorExit1;
	}

	if ( ERROR_SUCCESS != CSgetCachedValueList( szValueNameList,
												&dwcbValueNameListLen, 
												NULL ) )
	{
		goto ErrorExit1;
	}

	szValue     = NewNoX(gpmo) char [ dwcbMaxValueLen ];
	if( NULL == szValue )
	{
		goto ErrorExit1;
	}

	for ( char * szValueName = szValueNameList; 
		0 != *szValueName; 
		szValueName += _tcslen( szValueName ) + 1 )
	{
		if ( ERROR_SUCCESS == CSgetCachedValue( szValueName,
												szValue, 
												dwcbMaxValueLen ) )
        {
            // There is a valid registry entry
            // So, insert into the LastConnectCache
            if( !pgLastConnectCache->Insert( szValueName, szValue) )
            {
				goto ErrorExit1;
            }
        }       
    }

ErrorExit1:
	
	delete [] szValueNameList;
	delete [] szValue;

	BidTraceU0( SNI_BID_TRACE_ON,RETURN_TAG _T("success\n"));

	return;

ErrorExit:

	DeleteCriticalSection(&critsecCache);
	if(pgLastConnectCache)
		delete pgLastConnectCache;

	pgLastConnectCache = 0;
		
	BidTraceU0( SNI_BID_TRACE_ON,RETURN_TAG _T("fail\n"));

	return;
}
#endif	//	#ifndef SNIX

void Shutdown()
{
	BidxScopeAutoSNI0( SNIAPI_TAG _T( "\n"));

	BidTraceU1( SNI_BID_TRACE_ON, SNI_TAG _T("pgLastConnectCache: %p\n"), pgLastConnectCache);

	if( !pgLastConnectCache )
	{
		return;
	}

	DeleteCriticalSection(&critsecCache);

	delete pgLastConnectCache;

	pgLastConnectCache = 0;
	
	return;
}

#ifndef SNIX
//	For SNIX version, see below
//
void RemoveEntry( const char *szAlias)
{
	BidxScopeAutoSNI1( SNIAPI_TAG _T( "szAlias: '%hs'\n"), szAlias);

	BidTraceU1( SNI_BID_TRACE_ON, SNI_TAG _T("pgLastConnectCache: %p\n"), pgLastConnectCache);

	if( !pgLastConnectCache )
	{
		return;
	}
	
	CAutoSNICritSec a_csCache( critsecCache, SNI_AUTOCS_DO_NOT_ENTER );

	a_csCache.Enter();

    if( pgLastConnectCache->Remove( szAlias) )
    {
		LONG ret;
		
	    // Remove from registry
		ret = CSdeleteCachedValue( const_cast<char *>(szAlias) );

		if( ERROR_SUCCESS != ret )
    	{
			BidTrace1(ERROR_TAG _T("registry: %d{WINERR}\n"), ret);
	    }
	}

	a_csCache.Leave(); 

	return;
}
#endif	//	#ifndef SNIX

BOOL GetEntry( const char *szAlias, __out ProtElem *pProtElem)
{
	BidxScopeAutoSNI2( SNIAPI_TAG _T( "szAlias: '%hs', pProtElem: %p\n"), szAlias, pProtElem);

	BidTraceU1( SNI_BID_TRACE_ON, SNI_TAG _T("pgLastConnectCache: %p\n"), pgLastConnectCache);

	if( !pgLastConnectCache )
	{
		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{BOOL}\n"), FALSE);

		return FALSE;
	}
	

	CAutoSNICritSec a_csCache( critsecCache, SNI_AUTOCS_DO_NOT_ENTER );

	a_csCache.Enter();

    char szCacheInfo[MAX_CACHEENTRY_LENGTH+1];
    CacheItem * pItem;

    // Look for item in the cache
    if( (pItem = pgLastConnectCache->Find(szAlias)) == NULL)
    {
		a_csCache.Leave(); 
        goto ErrorExit;
    }
    else    
    {
        BOOL fSuccess = pItem->CopyValue(szCacheInfo, 
			sizeof(szCacheInfo)/sizeof(szCacheInfo[0]));  
		
		a_csCache.Leave(); 

		if( !fSuccess )
		{
			goto ErrorExit;
		}
    }

    // We may have a blank value, check for that
    if( !szCacheInfo[0] )
        goto ErrorExit;

	char *pPtr;
	char *pEnd;

	pPtr=szCacheInfo;

    // This is a check to ensure we do not break clients
    // which have older entries in their LastConnect cache
    if( pPtr[0] < 'A' )     // Okay, its a version number
    {
        // Get version info
		pEnd=strchr(pPtr,':');
		if(!pEnd)
			goto ErrorExit;
        
        *pEnd = 0;
        pPtr = pEnd+1;

    }
    // Otherwise return ERROR_FAIL - this will force it to be
    // set in the cache the next time
    else
    {
        goto ErrorExit;
    }

	char *pszProtName = 0;
	
	if(pEnd=strchr(pPtr,':')){
		*pEnd=0;
		if(strlen(pPtr)>MAX_PROTOCOLNAME_LENGTH)
			goto ErrorExit;

		pszProtName = pPtr;
		
		pPtr = pEnd+1;
	}
	else
		goto ErrorExit;

    switch(	 pszProtName[0] )
    {
        case 'n':   // Named Pipe

            pProtElem->SetProviderNum(NP_PROV);
            if( FAILED( StringCchCopyA(
				pProtElem->Np.Pipe, CCH_ANSI_STRING(pProtElem->Np.Pipe), pPtr ) ) )
            {
				goto ErrorExit;
            }

            break;

        case 'v':   // VIA

            pProtElem->SetProviderNum(VIA_PROV);
            if( FAILED( StringCchCopyA(
				pProtElem->Via.Host, CCH_ANSI_STRING(pProtElem->Via.Host), pPtr ) ) )
            {
				goto ErrorExit;
            }

            break;

        case 't':   // Tcp

			pProtElem->SetProviderNum(TCP_PROV);
			if(pEnd=strchr(pPtr,','))
			{
				if( sizeof(pProtElem->Tcp.szPort) <= strlen(pEnd+1))
					goto ErrorExit;

				if(Strtoi(pEnd+1)==0)
					goto ErrorExit;

				// We verified the destination size above, so ignore 
				// the return value.  
				(void)StringCchCopyA( pProtElem->Tcp.szPort, CCH_ANSI_STRING(pProtElem->Tcp.szPort), pEnd+1);
			}
			else
				goto ErrorExit;

            break;

        case 'l':   // Lpc

            pProtElem->SetProviderNum(SM_PROV);
            if( FAILED( StringCchCopyA( pProtElem->Sm.Alias, CCH_ANSI_STRING(pProtElem->Sm.Alias), pPtr ) ) )
            {
				goto ErrorExit;
            }

            break;
    
        default:
            goto ErrorExit;
    }

	BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{BOOL}\n"), TRUE);

	return TRUE;

ErrorExit:

	BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{BOOL}\n"), FALSE);

    return FALSE;
}

#ifndef SNIX
//	For SNIX version, see below
//
void SetEntry( const char *szAlias, __in ProtElem *pProtElem)
{
	BidxScopeAutoSNI2( SNIAPI_TAG _T( "szAlias: '%hs', pProtElem: %p\n"), szAlias, pProtElem);

	BidTraceU1( SNI_BID_TRACE_ON, SNI_TAG _T("pgLastConnectCache: %p\n"), pgLastConnectCache);

	if( !pgLastConnectCache )
	{
		return;
	}

	Assert( strchr(szAlias,'\\'));
	
    char szCacheVal[MAX_CACHEENTRY_LENGTH+1];

    // Enter critical section
	CAutoSNICritSec a_csCache( critsecCache, SNI_AUTOCS_DO_NOT_ENTER );

	a_csCache.Enter();

	Assert(pProtElem);
	
    switch(pProtElem->GetProviderNum())
    {
        case TCP_PROV:   // Tcp
            if( FAILED( StringCchPrintfA(
				szCacheVal, CCH_ANSI_STRING(szCacheVal), "0:tcp:%s,%s", pProtElem->m_szServerName, pProtElem->Tcp.szPort ) ) )
            {
				goto ErrorExit;
            }
            break;

        case NP_PROV:   // Named Pipe
            if( FAILED( StringCchPrintfA(
				szCacheVal, CCH_ANSI_STRING(szCacheVal), "0:np:%s", pProtElem->Np.Pipe ) ) )
            {
				goto ErrorExit;
            }
            break;

        case SM_PROV:   //Lpc
			///does shared memory use cache?
            if( FAILED( StringCchPrintfA(
				szCacheVal, CCH_ANSI_STRING(szCacheVal), "0:lpc:%s", pProtElem->Sm.Alias ) ) )
            {
				goto ErrorExit;
            }
            break;

        case VIA_PROV:   // Via
        	/// this is not useful information to cache
            if( FAILED( StringCchPrintfA(szCacheVal, CCH_ANSI_STRING(szCacheVal), "0:via:%s", pProtElem->Via.Host ) ) )
            {
				goto ErrorExit;
            }
            break;

        default:
			// this assertion is used to catch improper function usages.
			Assert( 0 && "providerNum: invalid value\n" );
			BidTrace0(ERROR_TAG _T("providerNum: invalid value\n"));
			goto ErrorExit;

    }

    unsigned int chCacheVal;

    chCacheVal=(unsigned int)strlen(szCacheVal);

    Assert(chCacheVal<=MAX_CACHEENTRY_LENGTH);

    if(MAX_CACHEENTRY_LENGTH<chCacheVal)
		goto ErrorExit;
    
	pgLastConnectCache->Remove(szAlias);

    if( !pgLastConnectCache->Insert( szAlias, szCacheVal) )
    {
    	goto ErrorExit;
    }

    // Set in registry
    DWORD dwValueLen;

    dwValueLen = (DWORD)(strlen(szCacheVal) * sizeof(char));


	if( ERROR_SUCCESS != CSsetCachedValue( const_cast<char *>(szAlias), 
										   szCacheVal ) )											
    {
        goto ErrorExit;
    }

	BidTraceU0( SNI_BID_TRACE_ON, SNI_TAG _T("success\n"));
	
ErrorExit:

	a_csCache.Leave(); 

    return;

}
#endif	//	#ifndef SNIX


#ifdef SNIX
//	SNIX version, which does not use or manipulate registry
//
void Initialize()
{
	BidxScopeAutoSNI0( SNIAPI_TAG _T( "\n"));

	BidTraceU1( SNI_BID_TRACE_ON, SNI_TAG _T("pgLastConnectCache: %p\n"), pgLastConnectCache);

	Assert( !pgLastConnectCache );

    // Initialize Cache
    pgLastConnectCache = NewNoX(gpmo) Cache();

	if( !pgLastConnectCache )
	{
		goto ErrorExit;
	}
	
	if( ERROR_SUCCESS != SNICritSec::Initialize(&critsecCache))
	{
		goto ErrorExit;
	}
	
	BidTraceU0( SNI_BID_TRACE_ON,RETURN_TAG _T("success\n"));

	return;

ErrorExit:

	DeleteCriticalSection(&critsecCache);

	delete pgLastConnectCache;

	pgLastConnectCache = 0;
		
	BidTraceU0( SNI_BID_TRACE_ON,RETURN_TAG _T("fail\n"));

	return;
}

//	SNIX version, which does not use or manipulate registry
//
void RemoveEntry( const char *szAlias)
{
	BidxScopeAutoSNI1( SNIAPI_TAG _T( "szAlias: '%hs'\n"), szAlias);

	BidTraceU1( SNI_BID_TRACE_ON, SNI_TAG _T("pgLastConnectCache: %p\n"), pgLastConnectCache);

	if( !pgLastConnectCache )
	{
		return;
	}
	
	CAutoSNICritSec a_csCache( critsecCache, SNI_AUTOCS_DO_NOT_ENTER );

	a_csCache.Enter();

    pgLastConnectCache->Remove( szAlias); 

	a_csCache.Leave(); 

	return;
}

//	SNIX version, which does not use or manipulate registry
//
void SetEntry( const char *szAlias, ProtElem *pProtElem)
{
	BidxScopeAutoSNI2( SNIAPI_TAG _T( "szAlias: '%hs', pProtElem: %p\n"), szAlias, pProtElem);

	BidTraceU1( SNI_BID_TRACE_ON, SNI_TAG _T("pgLastConnectCache: %p\n"), pgLastConnectCache);

	if( !pgLastConnectCache )
	{
		return;
	}

	Assert( strchr(szAlias,'\\'));
	
    char szCacheVal[MAX_CACHEENTRY_LENGTH+1];

    // Enter critical section
	CAutoSNICritSec a_csCache( critsecCache, SNI_AUTOCS_DO_NOT_ENTER );

	a_csCache.Enter();

	Assert(pProtElem);
	
    switch(pProtElem->GetProviderNum())
    {
        case TCP_PROV:   // Tcp
            if( FAILED( StringCchPrintfA(
				szCacheVal, CCH_ANSI_STRING(szCacheVal), "0:tcp:%s,%s", pProtElem->m_szServerName, pProtElem->Tcp.szPort ) ) )
            {
				goto ErrorExit;
            }
            break;

        case NP_PROV:   // Named Pipe
            if( FAILED( StringCchPrintfA(
				szCacheVal, CCH_ANSI_STRING(szCacheVal), "0:np:%s", pProtElem->Np.Pipe ) ) )
            {
				goto ErrorExit;
            }
            break;

        case SM_PROV:   //Lpc
			///does shared memory use cache?
            if( FAILED( StringCchPrintfA(
				szCacheVal, CCH_ANSI_STRING(szCacheVal), "0:lpc:%s", pProtElem->Sm.Alias ) ) )
            {
				goto ErrorExit;
            }
            break;

        case VIA_PROV:   // Via
        	/// this is not useful information to cache
            if( FAILED( StringCchPrintfA(szCacheVal, CCH_ANSI_STRING(szCacheVal), "0:via:%s", pProtElem->Via.Host ) ) )
            {
				goto ErrorExit;
            }
            break;

        default:
			// this assertion is used to catch improper function usages.
			Assert( 0 && "providerNum: invalid value\n" );
			BidTrace0(ERROR_TAG _T("providerNum: invalid value\n"));
			goto ErrorExit;

    }

    unsigned int chCacheVal;

    chCacheVal=(unsigned int)strlen(szCacheVal);

    Assert(chCacheVal<=MAX_CACHEENTRY_LENGTH);

    if(MAX_CACHEENTRY_LENGTH<chCacheVal)
		goto ErrorExit;
    
	pgLastConnectCache->Remove(szAlias);

    if( !pgLastConnectCache->Insert( szAlias, szCacheVal) )
    {
    	goto ErrorExit;
    }

	BidTraceU0( SNI_BID_TRACE_ON, SNI_TAG _T("success\n"));
	
ErrorExit:

	a_csCache.Leave(); 

    return;

}
#endif	//	#ifdef SNIX


} // namespace LastConnectCache 


DWORD GetProtocolEnum( const char    * pszProtocol,
						   __out ProviderNum * pProtNum )
{
	BidxScopeAutoSNI2( SNIAPI_TAG _T( "pszProtocol: '%hs', pProtNum: %p\n"), pszProtocol, pProtNum);

	if( !_stricmp("TCP", pszProtocol) )
	{
		*pProtNum = TCP_PROV;
	}
	else if( !_stricmp("NP", pszProtocol) )
	{
		*pProtNum = NP_PROV;
	}
	else if( !_stricmp("SM", pszProtocol) )
	{
		*pProtNum = SM_PROV;
	}
	else if( !_stricmp("VIA", pszProtocol) )
	{
		*pProtNum = VIA_PROV;
	}
	else
	{
		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), ERROR_FAIL);

		return ERROR_FAIL;
	}
	
	BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), ERROR_SUCCESS);

	return ERROR_SUCCESS;
}

#ifndef SNIX
//	For SNIX version, see below
//
DWORD GetProtocolDefaults( 	__out ProtElem * pProtElem,
									const char * pszProtocol,
									const char * szServer )
{
	BidxScopeAutoSNI3( SNIAPI_TAG _T( "pProtElem: %p, pszProtocol: '%hs', szServer: '%hs'\n"), pProtElem, pszProtocol, szServer);

	LONG       lResult;
	char     * szFoundChar = NULL;
	ProviderNum    eProviderNum;
	DWORD  nlReturn = ERROR_FAIL;
	LPSTR szClusEnv = NULL;
	bool  fSetDefaultValues = false;

	CS_PROTOCOL_PROPERTY ProtocolProperty;

	nlReturn = GetProtocolEnum( pszProtocol,
							    &eProviderNum );

	if( nlReturn != ERROR_SUCCESS )
		goto ErrorExit;

	pProtElem->SetProviderNum(eProviderNum);

	//
	// some of default parameters below only apply to default server instance
	// for example tcp port is one of them
	// In cases where default parameter doesn't make sense we skip processing
	//
	switch( eProviderNum )
	{
		case TCP_PROV:
				
			if( g_fSandbox )
			{
				fSetDefaultValues = true;
			}
			else		
			{	
				// Get the protocol Properties
				//
				lResult = CSgetProtocolProperty( (char *)pszProtocol,
												 CS_PROP_TCP_DEFAULT_PORT,
												 &ProtocolProperty );

				if( (ERROR_SUCCESS == lResult) && 
					(REG_DWORD == ProtocolProperty.dwPropertyType) )
				{
					(void)StringCchPrintfA(pProtElem->Tcp.szPort,CCH_ANSI_STRING(pProtElem->Tcp.szPort),"%d",ProtocolProperty.PropertyValue.dwDoubleWordValue);
				}
				else	// We had a problem so just use known default
				{
					fSetDefaultValues = true;
				}
			}

			if( fSetDefaultValues )
			{
				(void)StringCchCopyA(pProtElem->Tcp.szPort,CCH_ANSI_STRING(pProtElem->Tcp.szPort),"1433");
			}

			break;

		case NP_PROV:	
			// _dupenv_s() returns non-zero for failure.  
			//
			if( _dupenv_s(&szClusEnv, NULL, "_CLUSTER_NETWORK_NAME_" ) )
			{
				goto ErrorExit; 
			}
			
			(void)StringCchCopyA( pProtElem->Np.Pipe, CCH_ANSI_STRING(pProtElem->Np.Pipe), "\\\\" );

			// If string passed in matches Computername AND
			// the cluster env var is NOT set, then put "." in pipe name
OACR_WARNING_PUSH
OACR_WARNING_DISABLE(SYSTEM_LOCALE_MISUSE , " INTERNATIONALIZATION BASELINE AT KATMAI RTM. FUTURE ANALYSIS INTENDED. ")
			if( CSTR_EQUAL == CompareStringA(LOCALE_SYSTEM_DEFAULT,
									 NORM_IGNORECASE|NORM_IGNOREWIDTH,
									 szServer, -1,
									 gszComputerName, -1)&&
				((NULL == szClusEnv) || !_stricmp(szClusEnv, ""))
			  )
OACR_WARNING_POP
			{
				(void)StringCchCatA( pProtElem->Np.Pipe, CCH_ANSI_STRING(pProtElem->Np.Pipe), "." );
			}

			// In all other cases leave as is
			else
			{
				if( FAILED( StringCchCatA( 
					pProtElem->Np.Pipe, CCH_ANSI_STRING(pProtElem->Np.Pipe), szServer ) ) )
				{
					goto ErrorExit; 
				}
			}

			if( FAILED( StringCchCatA( 
				pProtElem->Np.Pipe, CCH_ANSI_STRING(pProtElem->Np.Pipe), "\\PIPE\\" ) ) )
			{
				goto ErrorExit; 
			}
			
			if( g_fSandbox )
			{
				// If registry access is set to false, set the default values.
				fSetDefaultValues = true;
			}
			else
			{
				// Get the protocol Properties
				//
				lResult = CSgetProtocolProperty( (char *)pszProtocol,
                                                 CS_PROP_NP_DEFAULT_PIPE,
                                                 &ProtocolProperty );

				if( (ERROR_SUCCESS == lResult) &&
					(REG_SZ == ProtocolProperty.dwPropertyType) )
				{
					if( FAILED( StringCchCatA( pProtElem->Np.Pipe, CCH_ANSI_STRING(pProtElem->Np.Pipe), 
						    ProtocolProperty.PropertyValue.szStringValue ) ) )
					{
						goto ErrorExit; 
					}					    
				}
				else	// We had a problem so just use known default
				{
					fSetDefaultValues = true;	    
				}
			}
		
			if( fSetDefaultValues )
			{
				if( FAILED( StringCchCatA( pProtElem->Np.Pipe, CCH_ANSI_STRING(pProtElem->Np.Pipe), "sql\\query" ) ) )
				{
					goto ErrorExit; 
				}		
			}

			break;

		case SM_PROV:
			if( FAILED( StringCchCopyA( 
				pProtElem->Sm.Alias, CCH_ANSI_STRING(pProtElem->Sm.Alias), szServer ) ) )
			{
				goto ErrorExit; 
			}				

			break;

		case VIA_PROV:	// Can be Giganet or Servernet

			// Get the protocol Properties
			//

			//	Hard-code the vendor name to "QLogic".  
			//	In Yukon/Whidbey (SNAC/SNIX) the only supported 
			//	vendor is QLogic.  
			//
			(void)StringCchCopyA( pProtElem->Via.Vendor, CCH_ANSI_STRING(pProtElem->Via.Vendor), CS_VALUE_VIA_VENDOR_NAME_QLOGIC );
			
			// Copy the server name into the host
			if( FAILED( StringCchCopyA( 
				pProtElem->Via.Host, CCH_ANSI_STRING(pProtElem->Via.Host), szServer ) ) )
			{
				goto ErrorExit; 
			}					    				

			if( g_fSandbox )
			{
				fSetDefaultValues = true;
			}
			else
			{
				lResult = CSgetProtocolProperty( (char *)pszProtocol,
												 CS_PROP_VIA_DEFAULT_PORT,
												 &ProtocolProperty );

				if( (ERROR_SUCCESS == lResult) && 
					(REG_SZ == ProtocolProperty.dwPropertyType) &&
					ProtocolProperty.PropertyValue.szStringValue[0] &&
					strchr(ProtocolProperty.PropertyValue.szStringValue, ':') )
				{
					LPSTR szTmp = strchr(ProtocolProperty.PropertyValue.szStringValue, ':');
					int iPort = atoi(szTmp+1);

					if( (0 <= iPort) && (USHRT_MAX >= iPort) )
					{
						pProtElem->Via.Port = static_cast<USHORT>(iPort); 
					}
					else
					{
						fSetDefaultValues = true;
					}
				}
				else
				{
					fSetDefaultValues = true;
				}
			}

			if( fSetDefaultValues )
			{
				pProtElem->Via.Port = 1433;

				// XXX - this is currently an empty string
				pProtElem->Via.Param[0] = 0;
			}			

			break;

		default:
			// this assertion is used to catch improper function usages.
			Assert( 0 && "providerNum: invalid value\n" );
			BidTrace0(ERROR_TAG _T("providerNum: invalid value\n"));
			goto ErrorExit;
	}

	if( NULL != szClusEnv )
	{
		free(szClusEnv); 
	}

	BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), ERROR_SUCCESS);

	return ERROR_SUCCESS;

ErrorExit:

	if( NULL != szClusEnv )
	{
		free(szClusEnv); 
	}

	BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), ERROR_FAIL);

	return ERROR_FAIL;
}


//	For SNIX versions, see below
//
DWORD GetProtocolList( 	__inout ProtList * pProtList, 
							const char * szServer,
							const char * szOriginalServer )
{
	BidxScopeAutoSNI3( SNIAPI_TAG _T( "pProtList: %p, szServer: '%hs', szOriginalServer: '%hs'\n"), pProtList, szServer, szOriginalServer);

	LONG       lResult      = 0;
	DWORD      dwBufferSize = 0;
	TCHAR     * pszProtocolOrder = 0;
	TCHAR		* pszProt;
	ProtElem * newProtElem;
	DWORD  nlReturn;
	DWORD dwRead = 0;
	DWORD dwLen = 0;
	HRESULT hr;

	if( g_fSandbox )
	{
		dwBufferSize = sizeof ( DEFAULT_PROTOCOLS_SANDBOX );
		pszProt = DEFAULT_PROTOCOLS_SANDBOX;
	}
	else
	{
		lResult = CSgetProtocolOrder( NULL, 
			                          &dwBufferSize );

		//	If the protocol order list was not found or it's 
		//	empty, use a default list.  cliconfg sets an empty 
		//	list to a single NULL-terminating character, 
		//	Yukon's WMI-based snap-in based on NLregC sets it 
		//	to two NULL-terminating characters.  
		//
		if( (lResult != ERROR_SUCCESS) || (dwBufferSize < 3 * sizeof(TCHAR)) )
		{
			pszProt = DEFAULT_PROTOCOLS; 
			lResult = ERROR_SUCCESS; 
		}
		else
		{
			//	dwBufferSize is in bytes.  Divide it by the character 
			//	size rounding it up.  
			//
			pszProt = pszProtocolOrder = 
				NewNoX(gpmo) TCHAR[ (dwBufferSize + sizeof(TCHAR) - 1) / sizeof(TCHAR) ];

			if( pszProtocolOrder == NULL )
			{
				pszProt = DEFAULT_PROTOCOLS; 
				lResult = ERROR_SUCCESS; 
			}
			else
			{
				lResult = CSgetProtocolOrder( pszProtocolOrder,
					                          &dwBufferSize );

				if( lResult != ERROR_SUCCESS )
				{
					pszProt = DEFAULT_PROTOCOLS; 
					lResult = ERROR_SUCCESS; 
				}
			}
		}
	} // else of if( g_fSandbox )

	// Go through each protocol and add to our protocol list
	// Checking that we don't read beyond the data is ONLY for PREfix.  This check is
	// guaranteed NOT to fire because we add the terminators to the string ourselves.
	// Apply the length check, then check for terminator.
	for( ; dwRead < dwBufferSize && *pszProt != 0x00; )
	{
		// If the protocol is shared memory and its NOT a 
		// local host, then just continue
		if( !_stricmp(pszProt, "sm") && !IsLocalHost(szServer) )
		{
			// Do nothing.
		}
		else
		{
			// Do not insert the same protocol multiple times
			ProviderNum eProviderNum;

			nlReturn = GetProtocolEnum( pszProt,
										&eProviderNum );

			// Skip unknown protocols.  
			if( nlReturn == ERROR_SUCCESS && NULL == pProtList->Find(eProviderNum) )
			{
				newProtElem = NewNoX(gpmo) ProtElem();

				if( !newProtElem )
					goto ErrorExit;

				if( ERROR_SUCCESS != newProtElem->Init( szServer, szOriginalServer ) )
				{
					delete newProtElem;
					goto ErrorExit;
				}

				nlReturn = GetProtocolDefaults( newProtElem,
												   pszProt,
												    szServer );

				if( nlReturn == ERROR_FAIL )
				{
					delete newProtElem;
					goto ErrorExit;
				}
					
				pProtList->AddTail( newProtElem );
			}
		}
		// move to next protocol in the null-null-terminated list
		dwLen = _tcslen( pszProt ) + 1;
		pszProt += dwLen;
		// dwBufferSize is a byte-count, so multiply by sizeof TCHAR
		hr = DWordMult(dwLen,sizeof(TCHAR),&dwLen);
		if (FAILED(hr))
		{
			BidTraceU1( SNI_BID_TRACE_ON, ERROR_TAG _T("dwLen: %d{DWORD}, "), dwLen);
			goto ErrorExit;
		}
		hr = DWordAdd(dwRead, dwLen, &dwRead);
		if (FAILED(hr))
		{
			BidTraceU2( SNI_BID_TRACE_ON, ERROR_TAG _T("dwLen: %d{DWORD}, dwRead: %d{DWORD}"), dwLen, dwRead);
			goto ErrorExit;
		}

	}	// for(;;)

	delete [] pszProtocolOrder;

	BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), ERROR_SUCCESS);

	return ERROR_SUCCESS;

ErrorExit:

	delete [] pszProtocolOrder;

	BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), ERROR_FAIL);

	return ERROR_FAIL;

}

#else	//	#ifndef SNIX

/////////////////////////// SNIX ///////////////////////////
//
//	Below are SNIX versions of functions which use or manipulate 
//	registry entries.  The SNIX versions do not use or manipulate 
//	registry, and use hard-coded defaults if needed.  Function 
//	which do not access registry are common, in the upper parts 
//	of the file.    
//
////////////////////////////////////////////////////////////


//	SNIX uses hard-coded values for protocol order instead of 
//	reading registry via NLreg libraries.  The non-Win9x default
//	is used also by MDAC in case the registry list is not 
//	present, or if it is empty.  
//
#define DEFAULT_PROTOCOLS_WIN9X	"tcp\0" \
								"np\0"

extern BOOL gfIsWin9x;


//	SNIX version, which does not use or manipulate registry
//
DWORD GetProtocolDefaults( 	ProtElem * pProtElem,
									const char * pszProtocol,
									const char * szServer )
{
	BidxScopeAutoSNI3( SNIAPI_TAG _T( "pProtElem: %p, pszProtocol: '%hs', szServer: '%hs'\n"), pProtElem, pszProtocol, szServer);

	LONG       lResult;
	char     * szFoundChar = NULL;
	ProviderNum    eProviderNum;
	DWORD  nlReturn = ERROR_FAIL;
	LPSTR szClusEnv = NULL;

	CS_PROTOCOL_PROPERTY ProtocolProperty;

	nlReturn = GetProtocolEnum( pszProtocol,
							    &eProviderNum );

	if( nlReturn != ERROR_SUCCESS )
		goto ErrorExit;

	pProtElem->SetProviderNum(eProviderNum);

	//
	// some of default parameters below only apply to default server instance
	// for example tcp port is one of them
	// In cases where default parameter doesn't make sense we skip processing
	//

	switch( eProviderNum )
	{
		case TCP_PROV:
			(void)StringCchCopyA(pProtElem->Tcp.szPort,CCH_ANSI_STRING(pProtElem->Tcp.szPort),"1433");
			break;

		case NP_PROV:
			// _dupenv_s() returns non-zero for failure.  
			//
			if( _dupenv_s(&szClusEnv, NULL, "_CLUSTER_NETWORK_NAME_" ) )
			{
				goto ErrorExit; 
			}
			
			(void)StringCchCopyA( pProtElem->Np.Pipe, CCH_ANSI_STRING(pProtElem->Np.Pipe), "\\\\" );

			// If string passed in matches Computername AND
			// the cluster env var is NOT set, then put "." in pipe name
			if( CSTR_EQUAL == CompareStringA(LOCALE_SYSTEM_DEFAULT,
									 NORM_IGNORECASE|NORM_IGNOREWIDTH,
									 szServer, -1,
									 gszComputerName, -1) &&
				((NULL == szClusEnv) || !_stricmp(szClusEnv, ""))
			  )
			{
				(void)StringCchCatA( pProtElem->Np.Pipe, CCH_ANSI_STRING(pProtElem->Np.Pipe), "." );
			}

			// In all other cases leave as is
			else
			{
				if( FAILED( StringCchCatA( 
					pProtElem->Np.Pipe, CCH_ANSI_STRING(pProtElem->Np.Pipe), szServer ) ))
				{
					goto ErrorExit; 
				}
			}

			if( FAILED( StringCchCatA( 
				pProtElem->Np.Pipe, CCH_ANSI_STRING(pProtElem->Np.Pipe), "\\PIPE\\sql\\query" ) ) )
			{
				goto ErrorExit; 
			}
			break;

		case SM_PROV:
			if( FAILED( StringCchCopyA( 
				pProtElem->Sm.Alias, CCH_ANSI_STRING(pProtElem->Sm.Alias), szServer ) ) )
			{
				goto ErrorExit; 
			}					    				
			break;

		case VIA_PROV:	// Can be Giganet or Servernet

			// 






			(void)StringCchCopyA( pProtElem->Via.Vendor, CCH_ANSI_STRING(pProtElem->Via.Vendor), CS_VALUE_VIA_VENDOR_NAME_QLOGIC );
			
			// Copy the server name into the host
			if( FAILED( StringCchCopyA( 
				pProtElem->Via.Host, CCH_ANSI_STRING(pProtElem->Via.Host), szServer ) ) )
			{
				goto ErrorExit; 
			}					    				

			lResult = CSgetProtocolProperty( (char *)pszProtocol,
										 CS_PROP_VIA_DEFAULT_PORT,
										 &ProtocolProperty );

			if( (ERROR_SUCCESS == lResult) && 
				(REG_SZ == ProtocolProperty.dwPropertyType) &&
				ProtocolProperty.PropertyValue.szStringValue[0] &&
				strchr(ProtocolProperty.PropertyValue.szStringValue, ':') )
			{
				LPSTR szTmp = strchr(ProtocolProperty.PropertyValue.szStringValue, ':');
				int iPort = atoi(szTmp+1);
				
				if( (0 <= iPort) && (USHRT_MAX >= iPort) )
				{
					pProtElem->Via.Port = static_cast<USHORT>(iPort); 
				}
				else
				{
					pProtElem->Via.Port = 1433;
				}
			}
			else
				pProtElem->Via.Port = 1433;

			break;
		default:
			// this assertion is used to catch improper function usages.
			Assert( 0 && "providerNum: invalid value\n" );
			BidTrace0(ERROR_TAG _T("providerNum: invalid value\n"));
			goto ErrorExit;
	}

	if( NULL != szClusEnv )
	{
		free(szClusEnv); 
	}

	BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), ERROR_SUCCESS);

	return ERROR_SUCCESS;

ErrorExit:

	if( NULL != szClusEnv )
	{
		free(szClusEnv); 
	}

	BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), ERROR_FAIL);

	return ERROR_FAIL;
}

//	SNIX version, which does not use or manipulate registry
//
DWORD GetProtocolList( 	ProtList * pProtList, 
							const char * szServer,
							const char * szOriginalServer )
{
	BidxScopeAutoSNI3( SNIAPI_TAG _T( "pProtList: %p, szServer: '%hs', szOriginalServer: '%hs'\n"), pProtList, szServer, szOriginalServer);

	LONG       lResult      = 0;
	TCHAR		* pszProt;
	ProtElem * newProtElem;
	DWORD  nlReturn;

	if( !gfIsWin9x )
		pszProt = DEFAULT_PROTOCOLS; 
	else
		pszProt = DEFAULT_PROTOCOLS_WIN9X; 

	// Go through each protocol and add to our protocol list
	//
	for( ;*pszProt != 0x00;							// are we at end of protocol list?
		  pszProt += _tcslen( pszProt ) + 1 )	// position to next protocol
	{
		// If the protocol is shared memory and its NOT a 
		// local host, then just continue
		if( !_stricmp(pszProt, "sm") && !IsLocalHost(szServer) )
			continue;
		
		newProtElem = NewNoX(gpmo) ProtElem();

		if( !newProtElem )
			goto ErrorExit;

		if( ERROR_SUCCESS != newProtElem->Init( szServer, szOriginalServer ) )
		{
			delete newProtElem;
			goto ErrorExit;
		}

		nlReturn = GetProtocolDefaults( newProtElem,
										   pszProt,
										    szServer );

		if( nlReturn == ERROR_FAIL )
		{
			delete newProtElem;
			goto ErrorExit;
		}
			
		pProtList->AddTail( newProtElem );

	}	// for(;;)

	BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), ERROR_SUCCESS);

	return ERROR_SUCCESS;

ErrorExit:

	BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), ERROR_FAIL);

	return ERROR_FAIL;

}
#endif	//	#ifdef SNIX

// Local DB Functions

//	Gets the dll path of the latest sqlUserInstance.dll installed
//	from the registry.
//
DWORD GetUserInstanceDllPath(LPSTR szDllPath, DWORD cchDllPathSize, DWORD* pErrorState)
{
	return CSgetUserInstanceDllPath(szDllPath, cchDllPathSize,(LocalDBErrorState*)pErrorState);
}



