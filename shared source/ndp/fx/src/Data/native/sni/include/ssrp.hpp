#ifndef _SSRP_HPP_
#define _SSRP_HPP_

namespace SSRP 
{
	DWORD SsrpGetInfo( __in LPSTR szServer, __in LPSTR szInstance, __inout ProtList *pProtocolList );
	DWORD SsrpEnumCore(LPSTR , char * , DWORD *, bool );
	bool GetAdminPort( const char *szServer, const char *szInstance, __inout USHORT *pPort );
};

#endif
