//****************************************************************************
//              Copyright (c) Microsoft Corporation.
//
// @File: open.hpp
// @Owner: petergv, nantu
// @Test: milu
//
// <owner current="true" primary="true">petergv</owner>
// <owner current="true" primary="false">nantu</owner>
//
// Purpose: Connection string parsing
//
// Notes: Everything in this file is shared with dbnetlib.dll ( dbnetlib has a separate copy of this file ).
//        So, special attention is needed whenever anything in this file needs to be changed.
//
//          
// @EndHeader@
//****************************************************************************

#ifndef _OPEN_HPP_
#define _OPEN_HPP_

#define MAX_INSTANCENAME_LENGTH 255
#define MAX_ALIAS_LENGTH	(MAX_NAME_SIZE+1+MAX_INSTANCENAME_LENGTH)
#define MAX_PROTOCOLNAME_LENGTH 10
#define MAX_PROTOCOLPARAMETER_LENGTH 255
#define DEFAULT_INSTANCE_NAME "mssqlserver"

#define LOCALDB_WSTR L"(localDB)"
#define LOCALDB_WSTR_LEN sizeof(LOCALDB_WSTR)/sizeof(WCHAR)

#define LOCALDB_INST_WSTR L"(localDB)\\"
#define LOCALDB_INST_WSTR_LEN sizeof(LOCALDB_INST_WSTR)/sizeof(WCHAR)

extern char gszComputerName[];
extern DWORD gdwSvrPortNum;
extern BOOL g_fSandbox;            // Defined in sni.cpp

// Converts from Ascii or Hex to Decimal integer
int Strtoi(const char * szStr);

// Skips white space and sees if theres chars after that
_inline BOOL IsBlank(__in __nullterminated char * pStr)
{
	while(*pStr && (*pStr==' ' || *pStr=='\t'))
		pStr++;
	if(!*pStr)	//end of string
		return true;
	return false;
}

// LocalHost checking
//
inline bool IsLocalHost( const char *szServer)
{
OACR_WARNING_PUSH
OACR_WARNING_DISABLE(SYSTEM_LOCALE_MISUSE , " INTERNATIONALIZATION BASELINE AT KATMAI RTM. FUTURE ANALYSIS INTENDED. ")
	if( strcmp(".",szServer) &&  
		_stricmp_l("(local)",szServer, GetDefaultLocale()) &&
		_stricmp_l("localhost",szServer, GetDefaultLocale()) &&
		CSTR_EQUAL != CompareStringA(LOCALE_SYSTEM_DEFAULT,
									 NORM_IGNORECASE|NORM_IGNOREWIDTH,
									 gszComputerName, -1,
									 szServer, -1))
OACR_WARNING_POP
	{
		return false;
	}
	
	return true;
}

// NOTE: The caller assumes ownership of the dynamically allocated copy
DWORD CopyConnectionString(__in LPCWSTR wszConnect, __out LPWSTR* pwszCopyConnect);

class ConnectParameter {
public:
	char m_szServerName[MAX_NAME_SIZE + 1];
	char m_szOriginalServerName[MAX_NAME_SIZE + 1];
	char m_szInstanceName[MAX_INSTANCENAME_LENGTH + 1];
	char m_szProtocolName[MAX_PROTOCOLNAME_LENGTH + 1];
	char m_szProtocolParameter[MAX_PROTOCOLPARAMETER_LENGTH + 1];
	char m_szAlias[MAX_ALIAS_LENGTH + 1];
	bool m_fCanUseCache;
	bool m_fStandardInstName;
	bool m_fParallel;

	ConnectParameter()
	{
		m_szServerName[0]=0;
		m_szOriginalServerName[0]=0;
		m_szInstanceName[0]=0;
		m_szProtocolName[0]=0;
		m_szProtocolParameter[0]=0;
		m_szAlias[0]=0;
		m_fCanUseCache=true;
		m_fStandardInstName=true;
		m_fParallel=false;
	}

	~ConnectParameter()
	{
	}
			
	DWORD IsLocalDBConnectionString( __in LPCWSTR wszConnect, __out bool* fLocalDB)
	{
		BidxScopeAutoSNI1( SNIAPI_TAG _T( "wszConnect: '%ls'\n"), wszConnect);

		LPWSTR wszCopyConnect = NULL;
		DWORD dwRet = ERROR_SUCCESS;
				
		// Check for Null or empty connection string.
		// Note : We convey that this is not a LocalDB connection string but do not report failure,
		// 		since an empty connection string is valid (connects to localhost). 
		//		
		if(NULL == wszConnect || !wszConnect[0])
		{
			goto Exit;
		}

		// Create a copy of the actual string
		dwRet = CopyConnectionString(wszConnect, &wszCopyConnect);
		if (ERROR_SUCCESS != dwRet)
		{
			// BID already traced in CopyConnectionString
			goto Exit;
		}

		// Trim Leading whitespaces (This is not supposed to fail)
		//
		StrTrimLeftW_Sys(wszCopyConnect);

		if(!_wcsnicmp(wszCopyConnect, LOCALDB_WSTR, LOCALDB_WSTR_LEN))
		{
			// No instance Name specified
			//
			dwRet = ERROR_INVALID_PARAMETER;
			SNI_SET_LAST_ERROR(INVALID_PROV,SNIE_51,dwRet);
			BidTrace1(ERROR_TAG _T("LocalDB: No instance name specified. %d{WINERR}\n"), dwRet);
			goto Exit;
		}
		
		if(!_wcsnicmp(wszCopyConnect, LOCALDB_INST_WSTR, LOCALDB_INST_WSTR_LEN-1))
		{
			DWORD len = (DWORD) wcslenInWChars(wszCopyConnect);
			
			//	Check if instance Name exists
			//
			if((LOCALDB_INST_WSTR_LEN-1) == len)
			{
				// Error case where connection string is 
				// (localdb)\ - missing instance name
				//
				dwRet = ERROR_INVALID_PARAMETER;
				SNI_SET_LAST_ERROR(INVALID_PROV,SNIE_51,dwRet);
				BidTrace1(ERROR_TAG _T("LocalDB: Missing instance name. %d{WINERR}\n"), dwRet); 
				goto Exit;
			}

			*fLocalDB = true;
		}		
Exit:
		if(wszCopyConnect)
		{
			delete [] wszCopyConnect;
			wszCopyConnect = NULL;
		}

		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), dwRet);
		return dwRet;
		
	}
	
	DWORD ParseConnectionString( const char * szConnect, bool fParallel )
	{
		BidxScopeAutoSNI1( SNIAPI_TAG _T( "szConnect: '%hs'\n"), szConnect);

		BOOL  fLastErrorSet = FALSE; 
		char *pPtr;
		char *pEnd;
		BOOL  fHasParameter = FALSE;
		
		m_fParallel = fParallel;
		m_fCanUseCache = m_fCanUseCache && !m_fParallel;
		
		//$ NOTE: this is really the first place where we are manipulating
		// the connection string.  We are going to calculate the length of
		// the string (strlen) and then make a copy of the string for the
		// rest of the processing.  This is pretty important.  It means that
		// we can be assured that what we are processing is a well-formed
		// string.
		DWORD len = (DWORD) strlen (szConnect);
		char *szCopyConnect = NewNoX(gpmo) char[len + 1];
		if( !szCopyConnect )
		{
			SNI_SET_LAST_ERROR( INVALID_PROV, SNIE_SYSTEM, ERROR_OUTOFMEMORY );

			BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), ERROR_OUTOFMEMORY);

			return ERROR_OUTOFMEMORY;
		}
		
		DWORD dwRet = ERROR_INVALID_PARAMETER;
OACR_WARNING_PUSH
OACR_WARNING_DISABLE(SYSTEM_LOCALE_MISUSE , " INTERNATIONALIZATION BASELINE AT KATMAI RTM. FUTURE ANALYSIS INTENDED. ")
		DWORD cch = LCMapStringA(LOCALE_SYSTEM_DEFAULT,
							LCMAP_LOWERCASE,
							szConnect,
							-1,
							szCopyConnect,
							len + 1
							);
OACR_WARNING_POP
		Assert(cch <= len+1);
		if( 0 >= cch || cch > len+1)
		{
			goto ExitFunc;
			
		}		
		szCopyConnect[len] = 0;

		if( ERROR_SUCCESS != StrTrimBoth_Sys(szCopyConnect, len+1 , NULL, " \t", 3))
		{
			goto ExitFunc;
		}

		
		pPtr=szCopyConnect;

		//prefix
		if(pEnd=strchr(pPtr,':'))
		{
			*pEnd = 0;

			//data before ':' can be part of ipv6 address
			Assert( 6<=MAX_PROTOCOLNAME_LENGTH );
			if ((pEnd - pPtr) > MAX_PROTOCOLNAME_LENGTH)
				goto ExitFunc;
			
			//Devnote: If this is IPv6 address, the num of bytes should be less than 5, so 10 bytes is ok.
			(void) StringCchCopyA(m_szProtocolName,CCH_ANSI_STRING(m_szProtocolName),pPtr);
			if( ERROR_SUCCESS != StrTrimBoth_Sys(m_szProtocolName, strlen(m_szProtocolName) + 1 , NULL, " \t", 3))
			{
				goto ExitFunc;
			}
			
			//check if it is an ipv6 address
			if( pEnd==pPtr ||
				strcmp("tcp",m_szProtocolName) &&
				strcmp("np",m_szProtocolName) &&
				strcmp("lpc",m_szProtocolName) &&
				strcmp("via",m_szProtocolName) &&
				strcmp("admin",m_szProtocolName))
			{
				//restore
				*pEnd = ':';	
				*m_szProtocolName = '\0';
			}
			else
			{				
				pPtr = pEnd + 1;
				if( ERROR_SUCCESS != StrTrimBoth_Sys(pPtr, strlen(pPtr)+1, NULL, " \t", 3))
				{
					goto ExitFunc;
				}	
			}
		}

		//	This is a special case where the connection string is either
		//	'\\server\pipe\etc' or 'np:\\server\pipe\etc'
		

		//pipename
OACR_WARNING_PUSH
OACR_WARNING_DISABLE(SYSTEM_LOCALE_MISUSE , " INTERNATIONALIZATION BASELINE AT KATMAI RTM. FUTURE ANALYSIS INTENDED. ")
		if(CSTR_EQUAL == CompareStringA(LOCALE_SYSTEM_DEFAULT,
									  0,
									  pPtr, 2,
									  "\\\\",2))
OACR_WARNING_POP
		{
			// If there is a protocol specified and its not 'np'
			// report an error
			if( !m_szProtocolName[0] )
				(void) StringCchCopyA( m_szProtocolName, CCH_ANSI_STRING(m_szProtocolName),"np");
			else if( strcmp("np", m_szProtocolName) )
				goto ExitFunc;

			if( strlen(pPtr ) > MAX_PROTOCOLPARAMETER_LENGTH )
				goto ExitFunc;			
			
			(void) StringCchCopyA(m_szProtocolParameter,CCH_ANSI_STRING(m_szProtocolParameter),pPtr);				

			char *szServerStart = pPtr+2;
			char *szServerEnd =StrChrA_SYS(szServerStart, (int) strlen(szServerStart),'\\');
			if( !szServerEnd )
				goto ExitFunc;

			*szServerEnd = 0;
			
			if (IsBlank(szServerStart))
				goto ExitFunc;

			// always copy the exact server name (as parsed from the named pipe) to the original name
			(void) StringCchCopyA(m_szOriginalServerName,CCH_ANSI_STRING(m_szOriginalServerName),szServerStart);
			
			if (IsLocalHost(szServerStart))
			{
				(void) StringCchCopyA(m_szServerName,CCH_ANSI_STRING(m_szServerName),gszComputerName);
			}
			else
			{
				if (FAILED (StringCchCopyA(m_szServerName,CCH_ANSI_STRING(m_szServerName),szServerStart)))
					goto ExitFunc;
			}

			//	Parse instance Name from PipeName
			//
			//	Skip the "\pipe\"
			char *szPipeEnd = StrChrA_SYS(szServerEnd+1,(int)strlen(szServerEnd+1),'\\');
			if(!szPipeEnd)
				goto ExitFunc;

			char* szInstStart=0, *szInstEnd=0;

			//	stardard default instance, i.e. \\server\pipe\sql\query
			//
OACR_WARNING_PUSH
OACR_WARNING_DISABLE(SYSTEM_LOCALE_MISUSE , " INTERNATIONALIZATION BASELINE AT KATMAI RTM. FUTURE ANALYSIS INTENDED. ")
			if( CSTR_EQUAL == CompareStringA(LOCALE_SYSTEM_DEFAULT,
									  0,
									  szPipeEnd+1,
									  (int) strlen(szPipeEnd+1),
									  "sql\\query",
									  (int) sizeof("sql\\query")-1))
OACR_WARNING_POP
			{		
				m_szInstanceName[0]=0;
			}			
			// 	Possible standard named instance, i.e. "\\server\pipe\MSSQL$instancename\sql\query".
			//	Use lower case because we just map the szConnect to lower case using LCMapStringA above.
			//	Note that FQDN, pipename and SPN is case in sensitive.
			//
OACR_WARNING_PUSH
OACR_WARNING_DISABLE(SYSTEM_LOCALE_MISUSE , " INTERNATIONALIZATION BASELINE AT KATMAI RTM. FUTURE ANALYSIS INTENDED. ")
			else if(CSTR_EQUAL == CompareStringA(LOCALE_SYSTEM_DEFAULT,
									  0,
									  szPipeEnd+1,
									  (int) sizeof("mssql$")-1,
									  "mssql$",
									  (int) sizeof("mssql$")-1))
OACR_WARNING_POP
			{

				//	standard named instance must finish with "\sql\query".
				//
				szInstEnd= StrChrA_SYS(szPipeEnd+1, (int) strlen(szPipeEnd+1),'\\');				

OACR_WARNING_PUSH
OACR_WARNING_DISABLE(SYSTEM_LOCALE_MISUSE , " INTERNATIONALIZATION BASELINE AT KATMAI RTM. FUTURE ANALYSIS INTENDED. ")
				if( CSTR_EQUAL == CompareStringA(LOCALE_SYSTEM_DEFAULT,
									  0,
									  szInstEnd+1,
									  (int) strlen(szInstEnd+1),
									  "sql\\query",
									  (int) sizeof("sql\\query")-1))
OACR_WARNING_POP
				{
					// standard named instance, i.e. \\server\pipe\MSSQL$instance\sql\query.
					// Find the start of the instance name
					//
					szInstStart = StrChrA_SYS(szPipeEnd+1, (int) strlen(szPipeEnd+1),'$');
					Assert(szInstStart);
					*szInstEnd=0;
					
					if (FAILED (StringCchCopyA(m_szInstanceName,CCH_ANSI_STRING(m_szInstanceName),szInstStart+1)))
						goto ExitFunc;

				}
				// For non-standard pipename, use the string after "pipe\" as the instance name.
				//
				else
				{
					if (FAILED (StringCchPrintf_lA(m_szInstanceName,CCH_ANSI_STRING(m_szInstanceName),"pipe%s", GetDefaultLocale(), szPipeEnd+1)))
						goto ExitFunc;

					m_fStandardInstName=false;
				}
			}
			// For non-standard pipename, use the string after "pipe\" as the instance name.
			//
			else
			{
				if (FAILED (StringCchPrintf_lA(m_szInstanceName,CCH_ANSI_STRING(m_szInstanceName),"pipe%s", GetDefaultLocale(),  szPipeEnd+1)))
					goto ExitFunc;

				m_fStandardInstName=false;
			}				

			m_fCanUseCache = false;

			dwRet = ERROR_SUCCESS;

			goto ExitFunc;
		}


		// parameter.
		if( pEnd = strchr(pPtr,',') )
		{
			*pEnd = 0;

			char * szProtocolParameter = pEnd + 1; 

			if( ERROR_SUCCESS != StrTrimBoth_Sys(szProtocolParameter, strlen(szProtocolParameter)+1, NULL, " \t", 3))
			{
				goto ExitFunc;
			}

			// SQL BU DT 291063: for TCP and only TCP we allow 
			// parameter specification even if protocol is not
			// specified (for backward compatibility with netlibs 
			// 2.x).  
			if(m_szProtocolName[0] == 0)
				(void) StringCchCopyA(m_szProtocolName,CCH_ANSI_STRING(m_szProtocolName),"tcp");

			
			// SQL BU DT 384073: "," is acceptable only for VIA and TCP
			// other protocols may fall through here, force fail exit
			if( strcmp("via", m_szProtocolName) &&  strcmp("tcp", m_szProtocolName) )
			{			
				goto ExitFunc;
			}
			

			// SQL BU DT 291063: 2.8 netlibs allow for an additional
			// TCP parameter ",urgent" but ignores it, and does not even 
			// validate it's actually ",urgent" (Webdata QFE bug 449).  
			// To preserve backward compatibility, SNI will do the same.  
			if( !strcmp("tcp", m_szProtocolName) )
			{
				if( char * pComma = strchr(szProtocolParameter, ',') )
				{
					*pComma = 0; 
					
					if( ERROR_SUCCESS != StrTrimBoth_Sys(szProtocolParameter, strlen(szProtocolParameter)+1, NULL, " \t", 3))
					{
						goto ExitFunc;
					}
				}
			}

			// Error out on blank parameter and ",\"
			if( !(*szProtocolParameter) || *szProtocolParameter == '\\' )
				goto ExitFunc;

			if(strlen(szProtocolParameter) > MAX_PROTOCOLPARAMETER_LENGTH)
				goto ExitFunc;
			
			if( IsBlank(pPtr) )
				goto ExitFunc;

			//Trim off possible "\instancename" in format of "hostname,port\instancename" to
			//preserve MDAC behavior, i.e. ignore instancename when port is present.
			if(pEnd = StrChrA_SYS(szProtocolParameter, (int) strlen(szProtocolParameter),'\\'))
				*pEnd = 0;
			
			(void)StringCchCopyA(m_szProtocolParameter,CCH_ANSI_STRING(m_szProtocolParameter),szProtocolParameter);

			fHasParameter = true;
			m_fCanUseCache = false;
		}

		//instancename
		if( pEnd=StrChrA_SYS(pPtr, (int) strlen(pPtr),'\\'))
		{
			*pEnd = 0;
			if(strlen(pEnd+1) > MAX_INSTANCENAME_LENGTH)
				goto ExitFunc;

			if( IsBlank(pPtr) )
				goto ExitFunc;

			if( ERROR_SUCCESS != StrTrimBoth_Sys(pEnd+1, strlen(pEnd+1)+1, NULL, " \t", 3))
			{
				goto ExitFunc;
			}

			// SQL BU DT 338117: treat missing instance name as default
			// instance to preserve 2.8 netlibs behavior.  
			if( *(pEnd+1) )
			{
				// Per testing it appears that 2.8 did not treat white 
				// space as a default instance.  
				if( IsBlank(pEnd+1) )
					goto ExitFunc;

				//Only save "\instancename" when port number is not available to 
				//preserve MDAC behavior, i.e. ignore instancename when port is present.
				//It is needed in format of "hostname\instancename,port".
				if( !fHasParameter )
				{
					(void)StringCchCopyA(m_szInstanceName,CCH_ANSI_STRING(m_szInstanceName),pEnd+1);
					
				}
			}
		}

		// Instance name "mssqlserver" is reserved for default
		// instance, and is not allowed.  
		if( !strcmp(DEFAULT_INSTANCE_NAME, m_szInstanceName) )
			goto ExitFunc; 			

		if(strlen(pPtr) > MAX_NAME_SIZE)
			goto ExitFunc;


		//server name
		if( ERROR_SUCCESS != StrTrimBoth_Sys(pPtr, strlen(pPtr)+1, NULL, " \t", 3))
		{
			goto ExitFunc;
		}
		
		// always copy the exact server name to the original name
		(void) StringCchCopyA(m_szOriginalServerName,CCH_ANSI_STRING(m_szOriginalServerName),pPtr);
		
		if(IsBlank(pPtr) || IsLocalHost(pPtr))
		{
			// For DAC use "localhost" instead of the server name.  
			if( strcmp("admin", m_szProtocolName) )
			{
				(void)StringCchCopyA(m_szServerName,CCH_ANSI_STRING(m_szServerName),gszComputerName);
			}
			else
			{
				(void)StringCchCopyA(m_szServerName,CCH_ANSI_STRING(m_szServerName),"localhost");
			}
		}
		else
		{
			(void)StringCchCopyA(m_szServerName,CCH_ANSI_STRING(m_szServerName),pPtr);
		}

		// Check if lpc was the protocol and the servername was NOT
		// the local computer - in that case, error out
		if( !strcmp("lpc", m_szProtocolName) && !IsLocalHost(m_szServerName) )
		{
			Assert( dwRet == ERROR_INVALID_PARAMETER );
			
			SNI_SET_LAST_ERROR( INVALID_PROV, SNIE_41, dwRet );

			fLastErrorSet = TRUE; 

			goto ExitFunc;
		}
		
		(void)StringCchCopyA(m_szAlias,CCH_ANSI_STRING(m_szAlias),m_szServerName);

		if(m_szInstanceName[0])
		{
			(void)StringCchCatA(m_szAlias,CCH_ANSI_STRING(m_szAlias),"\\");
			(void)StringCchCatA(m_szAlias,CCH_ANSI_STRING(m_szAlias), m_szInstanceName);
			if( !strcmp("lpc",m_szProtocolName) 
				|| !strcmp("admin",m_szProtocolName))
				m_fCanUseCache = false;
		}
		else
		{
			m_fCanUseCache = false;
		}

		if( fParallel )
		{
			if( !strcmp("np",m_szProtocolName) ||
				!strcmp("via",m_szProtocolName) ||
				!strcmp("lpc",m_szProtocolName) ||
				!strcmp("admin",m_szProtocolName) )
			{
				dwRet = ERROR_INVALID_PARAMETER;
				SNI_SET_LAST_ERROR( INVALID_PROV, SNIE_49, dwRet );
				fLastErrorSet = TRUE; 
				goto ExitFunc;
			}
			else if( '\0' == m_szProtocolName[0] )
			{
				HRESULT hr = StringCchCopyA(m_szProtocolName,CCH_ANSI_STRING(m_szProtocolName),"tcp");
				if( FAILED(hr) )
				{
					// We don't expect this to fail for either of the stated reasons on MSDN...
					Assert( STRSAFE_E_INSUFFICIENT_BUFFER != hr );
					Assert( STRSAFE_E_INVALID_PARAMETER != hr );
					BidTrace1( ERROR_TAG _T("StringCchCopyA failed: %d{HRESULT}.\n"), hr);
					
					dwRet = ERROR_INVALID_PARAMETER;
					SNI_SET_LAST_ERROR( INVALID_PROV, SNIE_10, dwRet );
					fLastErrorSet = TRUE; 
					goto ExitFunc;
				}
			}
		}
		dwRet = ERROR_SUCCESS;

	ExitFunc:

		delete [] szCopyConnect;

		if( ERROR_SUCCESS != dwRet )
		{
			Assert( dwRet == ERROR_INVALID_PARAMETER );

			if( !fLastErrorSet )
			{
				SNI_SET_LAST_ERROR( INVALID_PROV, SNIE_25, dwRet );
			}
		}
		else
		{
			Assert(m_szServerName[0]);
			BidTraceU8( SNI_BID_TRACE_ON, SNI_TAG 
				_T("m_szProtocolName: '%hs', ")
				_T("m_szServerName: '%hs', ")
				_T("m_szInstanceName: '%hs', ")
				_T("m_szProtocolParameter: '%hs', ")
				_T("m_szAlias: '%hs', ")
				_T("m_fCanUseCache: %d{bool}, ")
				_T("m_fStandardInstName: %d{bool}, ")
				_T("m_szOriginalServerName: '%hs'\n"), 
				m_szProtocolName , 
				m_szServerName, 
				m_szInstanceName, 
				m_szProtocolParameter, 
				m_szAlias, 
				m_fCanUseCache,
				m_fStandardInstName,
				m_szOriginalServerName);
		}
		
		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), dwRet);

		return dwRet;
	}
};

class ProtElem
{
private:
	ProviderNum m_ProviderNum;
	
public:

	//	Provider number for setting SNI last error.  
	//	Use m_ProviderNum if equal to INVALID_PROV.  
	//
	ProviderNum m_ProviderToReport;
	char 		m_szServerName[MAX_NAME_SIZE+1];
	char 		m_szOriginalServerName[MAX_NAME_SIZE+1];

	union
	{
		struct
		{
			char szPort[MAX_PROTOCOLPARAMETER_LENGTH+1];
			bool fParallel;
		} Tcp;

		struct
		{
			char Pipe[MAX_PATH+1];
		} Np;

		struct
		{
			char Alias[MAX_ALIAS_LENGTH+1];
		} Sm;

		struct
		{
			USHORT Port;
			char Vendor[MAX_NAME_SIZE+1];
			char Host[MAX_NAME_SIZE+1];
			char Param[MAX_PROTOCOLPARAMETER_LENGTH+1];
		} Via;
	};
	
	ProtElem 	* m_Next;

	ProtElem()
	{
		m_ProviderNum = INVALID_PROV;

		m_ProviderToReport = INVALID_PROV; 

		m_szServerName[0] = 0x00; 
		m_szOriginalServerName[0] = 0x00;
		
		m_Next = 0;
	}

	~ProtElem()
	{
	}

	DWORD Init( const char szServer[], const char szOriginalServerName[] )
	{
		BidxScopeAutoSNI2( SNIAPI_TAG _T( "szServer: '%hs', szOriginalServerName: '%hs'\n"), szServer, szOriginalServerName);

		Assert( szServer[0] );

		DWORD dwRet = (DWORD) StringCchCopyA (m_szServerName, CCH_ANSI_STRING(m_szServerName), szServer);

		if( ERROR_SUCCESS != dwRet )
		{
			goto Exit;
		}

		
		dwRet = (DWORD) StringCchCopyA (m_szOriginalServerName, CCH_ANSI_STRING(m_szOriginalServerName), szOriginalServerName);

	Exit:
		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), dwRet);

		return dwRet;
	}

	void SetProviderNum(ProviderNum providerNum)
	{
		m_ProviderNum = providerNum;

		switch( providerNum )
		{
			case TCP_PROV:
				Tcp.fParallel = false;
				break;
			default:
				// nothing to be done for other protocols
				break;
		}
	}

	ProviderNum GetProviderNum() const
	{
		return m_ProviderNum;
	}

	static DWORD MakeProtElem (__in LPSTR szConnect, __out ProtElem ** ppProtElem)
	{
		BidxScopeAutoSNI2( SNIAPI_TAG _T( "szConnect: '%hs', ppProtElem: %p\n"), szConnect, ppProtElem);

		*ppProtElem = 0;
		
		DWORD dwRet;
		
		ConnectParameter * pConnectParams = NewNoX(gpmo) ConnectParameter;
		if( !pConnectParams )
		{
			dwRet = ERROR_OUTOFMEMORY;

			SNI_SET_LAST_ERROR( INVALID_PROV, SNIE_SYSTEM, dwRet );

			BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), dwRet);

			return dwRet;
		}

		// ProtElem::MakeProtElem is only called by direct consumer calls to SNIOpenSync, which does not take an fParallel parameter - so, always pass False here.
		// While SNIOpenSyncEx calls also go through SNIOpenSync, they will not call MakeProtElem - the ProtElem gets constructed by a different call to ParseConnectionString
		dwRet = pConnectParams->ParseConnectionString( szConnect, false );
		
		if( ERROR_SUCCESS != dwRet )
		{
			delete pConnectParams;

			BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), dwRet);

			return dwRet;
		}

		ProtElem *pProtElem = NewNoX(gpmo) ProtElem();

		if( !pProtElem )
		{
			delete pConnectParams;
			
			dwRet = ERROR_OUTOFMEMORY;

			SNI_SET_LAST_ERROR( INVALID_PROV, SNIE_SYSTEM, dwRet );

			BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), dwRet);

			return dwRet;
		}

		dwRet = pProtElem->Init( pConnectParams->m_szServerName, pConnectParams->m_szOriginalServerName ); 

		if ( ERROR_SUCCESS != dwRet )
		{
			goto ErrorExit; 
		}

		dwRet = ERROR_INVALID_PARAMETER;

		switch( pConnectParams->m_szProtocolName[0] )
		{
			case 't':

				if( strcmp("tcp",pConnectParams->m_szProtocolName))
					goto ErrorExit;

				pProtElem->SetProviderNum(TCP_PROV);

				if(pConnectParams->m_szInstanceName[0])
				{
					goto ErrorExit;
				}
		
				if(pConnectParams->m_szProtocolParameter[0])
				{
					if( 0 == Strtoi(pConnectParams->m_szProtocolParameter))
						goto ErrorExit;

					C_ASSERT(sizeof(pProtElem->Tcp.szPort) == sizeof(pConnectParams->m_szProtocolParameter));
					memcpy(pProtElem->Tcp.szPort, pConnectParams->m_szProtocolParameter, sizeof(pProtElem->Tcp.szPort));
				}
				else 
				{
					goto ErrorExit;
				}

				break;

			default:

				goto ErrorExit;
		}

		dwRet = ERROR_SUCCESS;

	ErrorExit:

		delete pConnectParams;

		if( dwRet == ERROR_SUCCESS )
		{
			*ppProtElem = pProtElem;
		}
		else
		{
			delete pProtElem;
			
			Assert( dwRet == ERROR_INVALID_PARAMETER );

			SNI_SET_LAST_ERROR( INVALID_PROV, SNIE_25, dwRet );
		}
		
		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%d{WINERR}\n"), dwRet);

		return dwRet;
	}

};

class ProtList
{
public:
	ProtElem * Head;

	ProtList():Head(0){}
	~ProtList(){
		DeleteAll();
	}
    void AddTail(__out ProtElem * newElement){
		BidxScopeAutoSNI1( SNIAPI_TAG _T( "newElement: %p{ProtElem*}\n"), newElement);
		
		ProtElem **ppTmp = &Head;
		while(*ppTmp)
			ppTmp=&((*ppTmp)->m_Next);
		*ppTmp=newElement;
		newElement->m_Next=0;
    }

    void AddHead(__out ProtElem * newElement){
		BidxScopeAutoSNI1( SNIAPI_TAG _T( "newElement: %p{ProtElem*}\n"), newElement);
		newElement->m_Next=Head;
		Head=newElement;
    }

    ProtElem * Find(ProviderNum ePN){
		BidxScopeAutoSNI1( SNIAPI_TAG _T( "ePN: %d{ProviderNum}\n"), ePN);
		
		ProtElem *tmpProt = Head;
		while( tmpProt ){
			if( tmpProt->GetProviderNum() == ePN )
				break;
			tmpProt=tmpProt->m_Next;
		}

		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%p{ProtElem*}\n"), tmpProt);

		return tmpProt;
    }

	ProtElem * RemoveProt(ProviderNum ePN){
		BidxScopeAutoSNI1( SNIAPI_TAG _T( "ePN: %d{ProviderNum}\n"), ePN);
		
		ProtElem **ppTmp = &Head;
		ProtElem *pRet = 0;
		while( *ppTmp ){
			if( (*ppTmp)->GetProviderNum() == ePN ){
				pRet = *ppTmp;
				*ppTmp=pRet->m_Next;
				break;
			}
			ppTmp=&((*ppTmp)->m_Next);
		}

		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%p{ProtElem*}\n"), pRet);
		return pRet;
	}

	ProtElem * RemoveFirst(void){
		BidxScopeAutoSNI0( SNIAPI_TAG _T( "\n"));
		
		ProtElem *pRet =Head;
		if(Head)
			Head=Head->m_Next;

		BidTraceU1( SNI_BID_TRACE_ON, RETURN_TAG _T("%p{ProtElem*}\n"), pRet);
		
		return pRet;
	}
	void DeleteAll(){
		BidxScopeAutoSNI0( SNIAPI_TAG _T( "\n"));
		
		ProtElem *pTmp;
		while(pTmp=RemoveFirst())
			delete pTmp;
	}

};

#endif
