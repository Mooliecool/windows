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

/**************************************************************************/
/* a type parameter list */

#ifndef TYPAR_H
#define TYPAR_H
#include "binstr.h"

extern unsigned int g_uCodePage;

class TyParDescr
{
public:
    TyParDescr()
    {
        m_pbsBounds = NULL;
        m_wzName = NULL;
        m_dwAttrs = 0;
    };
    ~TyParDescr()
    {
        delete m_pbsBounds;
        delete [] m_wzName;
        m_lstCA.RESET(true);
    };
    void Init(BinStr* bounds, char* name, DWORD attrs)
    {
        m_pbsBounds = bounds;
        ULONG               cTemp = (ULONG)strlen(name)+1;
        m_wzName = new WCHAR[cTemp];
        if(m_wzName)
        {
            memset(m_wzName,0,sizeof(WCHAR)*cTemp);
            WszMultiByteToWideChar(g_uCodePage,0,name,-1,m_wzName,cTemp);
        }
        m_dwAttrs = attrs;
    };
    BinStr* Bounds() { return m_pbsBounds; };
    WCHAR*  Name() { return m_wzName; };
    DWORD   Attrs() { return m_dwAttrs; };
    CustomDescrList* CAList() { return &m_lstCA; };
private:
    BinStr* m_pbsBounds;
    WCHAR*  m_wzName;
    DWORD   m_dwAttrs;
    CustomDescrList m_lstCA;
};

class TyParList {
public:
	TyParList(DWORD a, BinStr* b, char* n, TyParList* nx = NULL) 
    { 
        bound  = (b == NULL) ? new BinStr() : b;
        bound->appendInt32(0); // zero terminator
		attrs = a; name = n; next = nx;
	};
	~TyParList() 
    { 
        if(bound) delete bound;
		if (next) delete next; 
	};
    int Count()
    {
        TyParList* tp = this;
        int n;
        for(n = 1; (tp = tp->next) != NULL; n++);
        return n;
    };
    int IndexOf(char* name)
    {
        TyParList* tp = this;
        int n;
        int ret = -1;
        for(n=0; tp != NULL; n++, tp = tp->next)
        {
            if(tp->name == NULL)
            {
                if(name == NULL) ret = n;
            }
            else
            {
                if(name == NULL) continue;
                if(0 == strcmp(name,tp->name)) ret = n;
            }
        }
        return ret;
    };
    int ToArray(BinStr ***bounds, LPCWSTR** names, DWORD **attrs)
    {   
        int n = Count();
        BinStr **b = new BinStr* [n];
        LPCWSTR *nam = new LPCWSTR [n];
        DWORD *attr = attrs ? new DWORD [n] : NULL;
        TyParList *tp = this;
        int i = 0;
        while (tp)
        {
            ULONG               cTemp = (ULONG)strlen(tp->name)+1;
            WCHAR*              wzDllName = new WCHAR [cTemp];
            // Convert name to UNICODE
            memset(wzDllName,0,sizeof(WCHAR)*cTemp);
            WszMultiByteToWideChar(g_uCodePage,0,tp->name,-1,wzDllName,cTemp);
            nam[i] = (LPCWSTR)wzDllName;
            b[i] = tp->bound;
            if (attr) 
                attr[i] = tp->attrs;
            tp->bound = 0; // to avoid deletion by destructor
            i++;
            tp = tp->next;
        }
        *bounds = b;
        *names = nam;          
        if (attrs)
            *attrs = attr;
        return n;
    };
    int ToArray(TyParDescr **ppTPD)
    {
        int n = Count();
        TyParDescr *pTPD = NULL;
        if(n)
        {
            pTPD = new TyParDescr[n];
            if(pTPD)
            {
                int i = 0;
                TyParList *tp = this;
                while (tp)
                {
                    pTPD[i].Init(tp->bound,tp->name,tp->attrs);
                    tp->bound = 0; // to avoid deletion by destructor
                    i++;
                    tp = tp->next;
                }
            }
        }
        *ppTPD = pTPD;
        return n;
    };
    TyParList* Next() { return next; };
    BinStr* Bound() { return bound; };
private:
	BinStr* bound;
    char*   name;
    TyParList* next;
    DWORD   attrs;
};

typedef TyParList* pTyParList;

#endif

