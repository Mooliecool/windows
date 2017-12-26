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
// MetaModelRO.h -- header file for Read-Only compressed COM+ metadata.
//
// Used by the EE.
//
//*****************************************************************************
#ifndef _METAMODELRO_H_
#define _METAMODELRO_H_

#if _MSC_VER >= 1100
 # pragma once
#endif

#include "metamodel.h"

//*****************************************************************************
// A read-only MiniMd.  This is the fastest and smallest possible MiniMd,
//  and as such, is the preferred EE metadata provider.
//*****************************************************************************
// Pointer to a table.
#define _TBLPTR(tbl) m_pTable[TBL_##tbl##]

template <class MiniMd> class CLiteWeightStgdb;
class CMiniMdRW;
class MDInternalRO;
class CMiniMd : public CMiniMdTemplate<CMiniMd>
{
public:
    friend class CLiteWeightStgdb<CMiniMd>;
    friend class CMiniMdTemplate<CMiniMd>;
    friend class CMiniMdRW;
    friend class MDInternalRO;

    HRESULT InitOnMem(void *pBuf, ULONG ulBufLen);
    HRESULT PostInit(int iLevel);  // higher number : more checking


    BOOL _IsValidToken(             // true if tk is valid token
        mdToken     tk)                     // [IN] token to be checked
    {
        BOOL        bRet = FALSE;           // default to invalid token

        if (TypeFromToken(tk) == mdtString)
        {
            // need to check the user string heap
            if (m_USBlobs.IsValidCookie(RidFromToken(tk)))
                bRet = true;
        }
        else
            // Base type doesn't know about m_USBlobs
            bRet = _IsValidTokenBase(tk);
        
        return bRet;
    } // CMiniMDRO::_IsValidToken


    FORCEINLINE void *GetUserString(ULONG ix, ULONG *pLen)
    { return m_USBlobs.GetBlob(ix, pLen); }

    // Hot table info.
#include <pshpack2.h>
    class HotTableHeader
    {
    private:
        ULONG       m_cHotRecs;
        ULONG       m_offsFirstLevelTable;
        ULONG       m_offsSecondLevelTable;
        ULONG       m_offsHotData;
        WORD        m_shiftCount;
    public:
        ULONG       HotRecs()
        {
            return  m_cHotRecs;
        }
        bool        HasFirstLevelTable()
        {
            return  m_offsFirstLevelTable != 0;
        }
        WORD        *FirstLevelTable()
        {
            _ASSERTE(HasFirstLevelTable());
            return (WORD *)((BYTE *)this + m_offsFirstLevelTable);
        }
        bool        HasSecondLevelTable()
        {
            return  m_offsSecondLevelTable != 0;
        }
        BYTE        *SecondLevelTable()
        {
            _ASSERTE(HasSecondLevelTable());
            return (BYTE *)this + m_offsSecondLevelTable;
        }
        BYTE        *HotData()
        {
            return  (BYTE *)this + m_offsHotData;
        }
        WORD        ShiftCount()
        {
            return m_shiftCount;
        }
        ULONG       Mask()
        {
            return (1UL<<m_shiftCount)-1;
        }
        BYTE *Align4(BYTE *p)
        {
            return (BYTE *)((size_t)p + 3 & ~3);
        }
        void        CheckHeader()
        {
            _ASSERTE(HotRecs() > 0 && HotRecs() <= USHRT_MAX);
            if (!HasFirstLevelTable())
            {
                _ASSERTE(!HasSecondLevelTable());
                _ASSERTE(HotData() == Align4((BYTE *)this + sizeof(*this)));
            }
            else
            {
                _ASSERTE(FirstLevelTable() == (WORD *)((BYTE *)this + sizeof(*this)));
                _ASSERTE(HasSecondLevelTable() && SecondLevelTable() == (BYTE *)(FirstLevelTable() + ((SIZE_T)1UL<<ShiftCount()) + 1));
                _ASSERTE(HotData() == Align4(SecondLevelTable() + HotRecs()));
            }
        }
    };
#include <poppack.h>

    class   HotTableDirectory
    {
    private:
        ULONG   magic;  // must be 'HOMD'
        LONG    m_offsTableHeader[TBL_COUNT];
    public:
        FORCEINLINE bool HasHotTableHeader(int ixTbl)
        {
            return m_offsTableHeader[ixTbl] != 0;
        }
        FORCEINLINE HotTableHeader *TableHeader(int ixTbl)
        {
            LONG offs = m_offsTableHeader[ixTbl];
            _ASSERTE(offs != 0);
            return (HotTableHeader *)((BYTE *)this + offs);
        }
        void CheckTables()
        {
            _ASSERTE(magic == 0x484f4e44);
            for (int i = 0; i < TBL_COUNT; i++)
            {
                if (HasHotTableHeader(i))
                {
                    TableHeader(i)->CheckHeader();
                }
            }
        }
    };

protected:
    // Table info.
    BYTE        *m_pTable[TBL_COUNT];
    HotTableDirectory *m_hotTableDirectory;

    HRESULT SetTablePointers(BYTE *pBase, ULONG cbData);

    void *FindHotRow(ULONG ixTbl,ULONG rid);

    StgPoolReadOnly m_Guids;            // Heaps
    StgPoolReadOnly m_Strings;          //  for
    StgBlobPoolReadOnly m_Blobs;        //   this
    StgBlobPoolReadOnly m_USBlobs;      //    MiniMd

    //*************************************************************************
    // Overridables -- must be provided in derived classes.
    FORCEINLINE LPCUTF8 Impl_GetString(ULONG ix)
    { return m_Strings.GetStringReadOnly(ix); }
    HRESULT Impl_GetStringW(ULONG ix, __inout_ecount (cchBuffer) LPWSTR szOut, ULONG cchBuffer, ULONG *pcchBuffer);
    FORCEINLINE void Impl_GetGuid(ULONG ix, GUID *pGuid)
    { 
        // Add void* casts so that the compiler can't make assumptions about alignment.
        CopyMemory((void*)pGuid, (void*)m_Guids.GetGuid(ix), sizeof(GUID));
        SwapGuid(pGuid);
    }
    FORCEINLINE void *Impl_GetBlob(ULONG ix, ULONG *pLen)
    { return m_Blobs.GetBlob(ix, pLen); }

    // Row from RID, RID from row.
    FORCEINLINE void *Impl_GetRow(ULONG ixTbl,ULONG rid) 
    {   // Want a valid RID here.  If this fires, check the calling code for an invalid token.
        _ASSERTE(rid >= 1 && rid <= m_Schema.m_cRecs[ixTbl] && "Caller error:  you passed an invalid token to the metadata!!");
        // Table pointer points before start of data.  Allows using RID as
        // an index, without adjustment.
        if (m_hotTableDirectory != NULL && m_hotTableDirectory->HasHotTableHeader(ixTbl))
            return FindHotRow(ixTbl, rid);
        else
            return m_pTable[ixTbl] + (rid * m_TableDefs[ixTbl].m_cbRec);
    }
    RID Impl_GetRidForRow(const void *pRow, ULONG ixTbl);

    // Validation.
    int Impl_IsValidPtr(const void *pRow, int ixTbl);

    // Count of rows in tbl2, pointed to by the column in tbl.
    int Impl_GetEndRidForColumn(const void *pRec, int ixtbl, CMiniColDef &def, int ixtbl2);

    FORCEINLINE RID Impl_SearchTable(ULONG ixTbl, CMiniColDef sColumn, ULONG ixCol, ULONG ulTarget)
    { return vSearchTable(ixTbl, sColumn, ulTarget); }

    // given a rid to the Property table, find an entry in PropertyMap table that contains the back pointer
    // to its typedef parent
    RID FindPropertyMapParentOfProperty(RID rid)
    { return vSearchTableNotGreater(TBL_PropertyMap, _COLDEF(PropertyMap,PropertyList), rid); }

    HRESULT FindParentOfPropertyHelper(
        mdProperty  pr,
        mdTypeDef   *ptd)
    {
        HRESULT     hr = NOERROR;

        RID         ridPropertyMap;
        PropertyMapRec *pRec;

        ridPropertyMap = FindPropertyMapParentOfProperty( RidFromToken( pr ) );
        pRec = getPropertyMap( ridPropertyMap );
        *ptd = getParentOfPropertyMap( pRec );

        RidToToken(*ptd, mdtTypeDef);

        return hr;
    } // HRESULT CMiniMdRW::FindParentOfPropertyHelper()
    
    // given a rid to the Event table, find an entry in EventMap table that contains the back pointer
    // to its typedef parent
    RID FindEventMapParentOfEvent(RID rid)
    { return vSearchTableNotGreater(TBL_EventMap, _COLDEF(EventMap, EventList), rid); }

    HRESULT FindParentOfEventHelper(
        mdEvent  pr,
        mdTypeDef   *ptd)
    {
        HRESULT     hr = NOERROR;

        RID         ridEventMap;
        EventMapRec *pRec;

        ridEventMap = FindEventMapParentOfEvent( RidFromToken( pr ) );
        pRec = getEventMap( ridEventMap );
        *ptd = getParentOfEventMap( pRec );

        RidToToken(*ptd, mdtTypeDef);

        return hr;
    } // HRESULT CMiniMdRW::FindParentOfEventHelper()
    
    FORCEINLINE int Impl_IsRo() 
    { return 1; }
    //*************************************************************************

    HRESULT CommonEnumCustomAttributeByName( // S_OK or error.
        mdToken     tkObj,                  // [IN] Object with Custom Attribute.
        LPCUTF8     szName,                 // [IN] Name of desired Custom Attribute.
        bool        fStopAtFirstFind,       // [IN] just find the first one
        HENUMInternal* phEnum);             // enumerator to fill up

    HRESULT CommonGetCustomAttributeByName( // S_OK or error.
        mdToken     tkObj,                  // [IN] Object with Custom Attribute.
        LPCUTF8     szName,                 // [IN] Name of desired Custom Attribute.
        const void  **ppData,               // [OUT] Put pointer to data here.
        ULONG       *pcbData);              // [OUT] Put size of data here.

};


#endif // _METAMODELRO_H_
