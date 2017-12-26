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
//
// class.hpp
//

#ifndef _CLASS_HPP
#define _CLASS_HPP

class PermissionDecl;
class PermissionSetDecl;

extern unsigned int g_uCodePage;
extern WCHAR    wzUniBuf[];
extern DWORD    dwUniBuf;

class Class
{
public:
	Class * m_pEncloser;
	char  *	m_szFQN;
    DWORD   m_dwFQN;
    unsigned m_Hash;
    mdTypeDef m_cl;
    mdTypeRef m_crExtends;
    mdTypeRef *m_crImplements;
    TyParDescr* m_TyPars;
    DWORD   m_NumTyPars;
    DWORD   m_Attr;
    DWORD   m_dwNumInterfaces;
	DWORD	m_dwNumFieldsWithOffset;
    PermissionDecl* m_pPermissions;
    PermissionSetDecl* m_pPermissionSets;
	ULONG	m_ulSize;
	ULONG	m_ulPack;
	BOOL	m_bIsMaster;
    BOOL    m_fNew;
    BOOL    m_fNewMembers;

    MethodList			m_MethodList;
    //MethodSortedList    m_MethodSList;
    FieldDList          m_FieldDList;	
    EventDList          m_EventDList;
    PropDList           m_PropDList;
	CustomDescrList     m_CustDList;

    Class(char* pszFQN)
    {
		m_pEncloser = NULL;
        m_cl = mdTypeDefNil;
        m_crExtends = mdTypeRefNil;
        m_NumTyPars = 0;
        m_TyPars = NULL;
        m_dwNumInterfaces = 0;
		m_dwNumFieldsWithOffset = 0;
		m_crImplements = NULL;
		m_szFQN = pszFQN;
        m_dwFQN = pszFQN ? (DWORD)strlen(pszFQN) : 0;
        m_Hash = pszFQN ? hash((BYTE*)pszFQN, m_dwFQN, 10) : 0;

        m_Attr = tdPublic;

		m_bIsMaster  = TRUE;
        m_fNew = TRUE;

        m_pPermissions = NULL;
        m_pPermissionSets = NULL;

		m_ulPack = 0;
		m_ulSize = 0xFFFFFFFF;
    }
	
	~Class()
	{
		delete [] m_szFQN;
        delete [] m_crImplements;
        delete [] m_TyPars;
	}

    int FindTyPar(WCHAR* wz)
    {
        int i,retval=-1;
        for(i=0; i < (int)m_NumTyPars; i++)
        {
            if(!wcscmp(wz,m_TyPars[i].Name()))
            {
                retval = i;
                break;
            }
        }
        return retval;
    };
    int FindTyPar(char* sz)
    {
        if(sz)
        {
            wzUniBuf[0] = 0;
            WszMultiByteToWideChar(g_uCodePage,0,sz,-1,wzUniBuf,dwUniBuf);
            return FindTyPar(wzUniBuf);
        }
        else return -1;
    };
    int ComparedTo(Class* T) { return (m_Hash == T->m_Hash) ? 0 : ((m_Hash > T->m_Hash) ? 1 : -1); };
};


#endif /* _CLASS_HPP */

