#ifndef _REG_HPP_
#define _REG_HPP_

DWORD GetProtocolList( __inout ProtList * pProtList, 
							   const char * szServer,
							   const char * szOriginalServer );


DWORD GetProtocolDefaults( 	__out ProtElem * pProtElem,
									const char * pszProtocol,
									const char * szServer );

DWORD GetUserInstanceDllPath( __out_bcount(cchDllPathSize) LPSTR szDllPath, 
									__in DWORD cchDllPathSize,
									__out DWORD* pErrorState);

namespace LastConnectCache
{
	void Initialize();
	
	void Shutdown();

	BOOL GetEntry( const char * szAlias, __out ProtElem * pProtElem );

	void SetEntry( const char * szAlias, __in ProtElem * pProtElem );

	void RemoveEntry( const char * szAlias );

}

#endif
