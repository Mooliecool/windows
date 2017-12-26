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
// MetaModelRO.cpp -- Read-only implementation of compressed COM+ metadata.
//
//*****************************************************************************
#include "stdafx.h"

#include "metamodelro.h"
#include <posterror.h>
#include <corerror.h>
#include "metadatatracker.h"

//*****************************************************************************
// Set the pointers to consecutive areas of a large buffer.
//*****************************************************************************
HRESULT CMiniMd::SetTablePointers(
    BYTE        *pBase,
    ULONG       cbData)
{
    ULONG       ulOffset = 0;           // Offset so far in the table.
    int         i;                      // Loop control.

	for (i=0; i<TBL_COUNT; ++i)
	{
        METADATATRACKER_ONLY(MetaDataTracker::NoteSection(i, pBase + ulOffset, m_TableDefs[i].m_cbRec * m_Schema.m_cRecs[i], m_TableDefs[i].m_cbRec));
        // Table pointer points before start of data.  Allows using RID as
        //  an index, without adjustment.
        m_pTable[i] = pBase + ulOffset - m_TableDefs[i].m_cbRec;
        ulOffset += m_TableDefs[i].m_cbRec * m_Schema.m_cRecs[i];
    }

    if (ulOffset > cbData)
        return PostError(CLDB_E_FILE_CORRUPT);

    return S_OK;
} // HRESULT CMiniMd::SetTablePointers()

//*****************************************************************************
// Given a table index and a rid, find the row, looking first in hot metadata
//*****************************************************************************
void * CMiniMd::FindHotRow(ULONG ixTbl, ULONG rid)
{
    // our caller should make sure of this.
    _ASSERTE(m_hotTableDirectory != NULL && m_hotTableDirectory->HasHotTableHeader(ixTbl));

    // fetch the hot table header for this table.
    HotTableHeader *hotTableHeader = m_hotTableDirectory->TableHeader(ixTbl);
    if (hotTableHeader->HasFirstLevelTable())
    {
        // fetch the first level table
        WORD *firstLevelTable = hotTableHeader->FirstLevelTable();

        // find the high order bits of the rid
        BYTE bRid = (BYTE)(rid >> hotTableHeader->ShiftCount());

        // use the low order bits of the rid to index into
        // the first level table.
        int i = firstLevelTable[rid & hotTableHeader->Mask()];
        int end = firstLevelTable[(rid & hotTableHeader->Mask()) + 1];

        // the generation logic should make sure either both tables are present
        // or both absent.
        _ASSERTE(hotTableHeader->HasSecondLevelTable());

        // fetch second level table
        BYTE *secondLevelTable = hotTableHeader->SecondLevelTable();

        // look for the high order bits of the rid in the second level table.
        // search is linear, but should be short on average.
        for ( ; i < end; i++)
        {
            if (secondLevelTable[i] == bRid)
            {
                // i is the index of the hot row we're looking for.

                // in checked build, make sure this agrees with what we get from the cold data.
                _ASSERTE(memcmp(hotTableHeader->HotData() + (i * m_TableDefs[ixTbl].m_cbRec),
                                m_pTable[ixTbl] + (rid * m_TableDefs[ixTbl].m_cbRec),
                                m_TableDefs[ixTbl].m_cbRec) == 0);

                return hotTableHeader->HotData() + (i * m_TableDefs[ixTbl].m_cbRec);
            }
        }
    }
    else
    {
        // no first level table - this implies the whole table is replicated
        // in the hot section. simply multiply and fetch the right record.
        // hot indices are 0-based, rids are 1-base, so need to subtract 1 from rid.

        // in checked build, make sure this agrees with what we get from the cold data.
        _ASSERTE(memcmp(hotTableHeader->HotData() + ((rid - 1) * m_TableDefs[ixTbl].m_cbRec),
                        m_pTable[ixTbl] + (rid * m_TableDefs[ixTbl].m_cbRec),
                        m_TableDefs[ixTbl].m_cbRec) == 0);

        return hotTableHeader->HotData() + ((rid - 1) * m_TableDefs[ixTbl].m_cbRec);
    }
    // not found in hot data - return address in cold data.
    return m_pTable[ixTbl] + (rid * m_TableDefs[ixTbl].m_cbRec);
}

//*****************************************************************************
// Given a buffer that contains a MiniMd, init to read it.
//*****************************************************************************
HRESULT CMiniMd::InitOnMem(
    void        *pvBuf,                 // The buffer.    
    ULONG       ulBufLen)               // Size of the buffer..
{
    HRESULT     hr = S_OK;
    ULONG cbData;
    BYTE *pBuf = reinterpret_cast<BYTE*>(pvBuf);

    // Uncompress the schema from the buffer into our structures.
    IfFailGo(SchemaPopulate(pvBuf, ulBufLen, &cbData));

    // There shouldn't be any pointer tables.
    if (m_Schema.m_cRecs[TBL_MethodPtr] || m_Schema.m_cRecs[TBL_FieldPtr])
    {
#ifdef _DEBUG
        if(REGUTIL::GetConfigDWORD(L"AssertOnBadImageFormat", 0))
            _ASSERTE( !"Trying to open Read/Write format as ReadOnly!");
#endif
        return PostError(CLDB_E_FILE_CORRUPT);
    }

    // initialize the pointers to the rest of the data.
    IfFailGo(SetTablePointers(pBuf + Align4(cbData), ulBufLen-cbData));

ErrExit:
    return hr;
} // HRESULT CMiniMd::InitOnMem()

//*****************************************************************************
// Validate cross-stream consistency.
//*****************************************************************************
HRESULT CMiniMd::PostInit(
    int         iLevel)
{
    HRESULT     hr = S_OK;
    return hr;
} // HRESULT CMiniMd::PostInit()

//*****************************************************************************
// Given a pointer to a row, what is the RID of the row?
//*****************************************************************************
RID CMiniMd::Impl_GetRidForRow(         // RID corresponding to the row pointer.
    const void  *pvRow,                 // Pointer to the row.
    ULONG       ixTbl)                  // Which table.
{
    _ASSERTE(isValidPtr(pvRow, ixTbl));

    const BYTE *pRow = reinterpret_cast<const BYTE*>(pvRow);

    // check if the address is in the cold section of the data
    if (m_pTable[ixTbl] + 1 <= pRow  && pRow <= m_pTable[ixTbl] + m_Schema.m_cRecs[ixTbl]*m_TableDefs[ixTbl].m_cbRec)
    {
    	// Offset in the table.
	    size_t cbDiff = pRow - m_pTable[ixTbl];

    	// Index of row.  Table pointer points before start of data, so RID can
	    //  be used/generated directly as an index.
    	return (RID)(cbDiff / m_TableDefs[ixTbl].m_cbRec);
    }
    else
    {
        // is there a hot section as well?
        if (m_hotTableDirectory != NULL && m_hotTableDirectory->HasHotTableHeader(ixTbl))
        {
            // fetch the hot table header and the start of the hot data
            HotTableHeader *hotTableHeader = m_hotTableDirectory->TableHeader(ixTbl);
            BYTE *hotData = hotTableHeader->HotData();

            // is the address in the hot section?
            if (hotData <= pRow  && pRow < hotData + hotTableHeader->HotRecs()*m_TableDefs[ixTbl].m_cbRec)
            {
                // figure out the hot index
                size_t hotIndex = (pRow - hotData)/m_TableDefs[ixTbl].m_cbRec;

                // if additional first and second level tables are missing, rid is simply hotIndex + 1
                if (!hotTableHeader->HasFirstLevelTable())
                    return (RID) (hotIndex + 1);

                // otherwise look for matching entry in first level table
                int i;
                for (i = 0; hotTableHeader->FirstLevelTable()[i] <= hotIndex; i++)
                    ;
                i--;

                // index in first level table is low order bits of rid, second level table contains high order bits.
                return (RID)((i & hotTableHeader->Mask()) + (hotTableHeader->SecondLevelTable()[hotIndex] << hotTableHeader->ShiftCount()));
            }
        }
    }
    _ASSERTE(!"pvRow out of range in call to Impl_GetRidForRow");
    return 1;
} // RID CMiniMd::Impl_GetRidForRow()


//*****************************************************************************
// Test that a pointer points to the start of a record in a given table.
//*****************************************************************************
int CMiniMd::Impl_IsValidPtr(               // True if pointer is valid for table.
    const void  *pvRow,                 // Pointer to test.
    int         ixTbl)                  // Table pointer should be in.
{
	const BYTE *pRow = reinterpret_cast<const BYTE*>(pvRow);
	// Should point within the table.

    size_t cbDiff;
    // is the row in the cold metadata section of the table?
	if (pRow > m_pTable[ixTbl] && pRow <= m_pTable[ixTbl] + m_TableDefs[ixTbl].m_cbRec * m_Schema.m_cRecs[ixTbl])
        cbDiff = pRow - m_pTable[ixTbl];
    else
    {
        // is there a hot section as well?
        if (m_hotTableDirectory != NULL && m_hotTableDirectory->HasHotTableHeader(ixTbl))
        {
            // fetch the hot table header and the start of the hot data
            HotTableHeader *hotTableHeader = m_hotTableDirectory->TableHeader(ixTbl);
            BYTE *hotData = hotTableHeader->HotData();

            // is the address in the hot section?
            if (pRow >= hotData && pRow < hotData + m_TableDefs[ixTbl].m_cbRec * hotTableHeader->HotRecs())
                cbDiff = pRow - hotData;
            else
                return false;
        }
        else
            return false;
    }

	// Should point directly at a row.
	if ((cbDiff % m_TableDefs[ixTbl].m_cbRec) != 0)
		return false;

    return true;
} // int CMiniMd::Impl_IsValidPtr()


//*****************************************************************************
// converting a ANSI heap string to unicode string to an output buffer
//*****************************************************************************
HRESULT CMiniMd::Impl_GetStringW(ULONG ix, __inout_ecount (cchBuffer) LPWSTR szOut, ULONG cchBuffer, ULONG *pcchBuffer)
{
    LPCSTR      szString;               // Single byte version.
    int         iSize;                  // Size of resulting string, in wide chars.
    HRESULT     hr = NOERROR;

    szString = getString(ix);

    if ( *szString == 0 )
    {
        // If emtpy string "", return pccBuffer 0
        if ( szOut && cchBuffer )
            szOut[0] = L'\0';
        if ( pcchBuffer )
            *pcchBuffer = 0;
        goto ErrExit;
    }
    if (!(iSize=::WszMultiByteToWideChar(CP_UTF8, 0, szString, -1, szOut, cchBuffer)))
    {
        // What was the problem?
        DWORD dwNT = GetLastError();

        // Not truncation?
        if (dwNT != ERROR_INSUFFICIENT_BUFFER)
            IfFailGo( HRESULT_FROM_NT(dwNT) );

        // Truncation error; get the size required.
        if (pcchBuffer)
            *pcchBuffer = ::WszMultiByteToWideChar(CP_UTF8, 0, szString, -1, szOut, 0);

        IfFailGo( CLDB_S_TRUNCATION );
    }
    if (pcchBuffer)
        *pcchBuffer = iSize;

ErrExit:
    return hr;
} // HRESULT CMiniMd::Impl_GetStringW()


//*****************************************************************************
// Given a table with a pointer (index) to a sequence of rows in another 
//  table, get the RID of the end row.  This is the STL-ish end; the first row
//  not in the list.  Thus, for a list of 0 elements, the start and end will
//  be the same.
//*****************************************************************************
int CMiniMd::Impl_GetEndRidForColumn(   // The End rid.
    const void  *pvRec,                 // Row that references another table.
    int         ixTbl,                  // Table containing the row.
    CMiniColDef &def,                   // Column containing the RID into other table.
    int         ixTbl2)                 // The other table.
{
    RID lastRid = m_Schema.m_cRecs[ixTbl];
    RID rid = Impl_GetRidForRow(pvRec, ixTbl);

    ULONG ixEnd;

	// Last rid in range from NEXT record, or count of table, if last record.
	_ASSERTE(rid <= lastRid);
	if (rid < lastRid)
		ixEnd = getIX(Impl_GetRow(ixTbl, rid+1), def);
	else	// Convert count to 1-based rid.
		ixEnd = m_Schema.m_cRecs[ixTbl2] + 1;

    return ixEnd;
} // int CMiniMd::Impl_GetEndRidForColumn()


//*****************************************************************************
// return all found CAs in an enumerator
//*****************************************************************************
HRESULT CMiniMd::CommonEnumCustomAttributeByName( // S_OK or error.
    mdToken     tkObj,                  // [IN] Object with Custom Attribute.
    LPCUTF8     szName,                 // [IN] Name of desired Custom Attribute.
    bool        fStopAtFirstFind,       // [IN] just find the first one
    HENUMInternal* phEnum)              // enumerator to fill up
{
    HRESULT     hr = S_OK;              // A result.
    HRESULT     hrRet = S_FALSE;        // Assume that we won't find any 
    ULONG       ridStart, ridEnd;       // Loop start and endpoints.

    _ASSERTE(phEnum != NULL);

    memset(phEnum, 0, sizeof(HENUMInternal));

    HENUMInternal::InitDynamicArrayEnum(phEnum);

    phEnum->m_tkKind = mdtCustomAttribute;

    // Get the list of custom values for the parent object.

    ridStart = getCustomAttributeForToken(tkObj, &ridEnd);
    if (ridStart == 0)
        return S_FALSE;

    // Look for one with the given name.
    for (; ridStart < ridEnd; ++ridStart)
    {
        IfFailGoto(CompareCustomAttribute( tkObj, szName, ridStart), ErrExit);
        if (hr == S_OK)
        {
            // If here, found a match.
            hrRet = S_OK;
            IfFailGo( HENUMInternal::AddElementToEnum(
                phEnum, 
                TokenFromRid(ridStart, mdtCustomAttribute)));
            if (fStopAtFirstFind)
                goto ErrExit;
        }
    }

ErrExit:
    if (FAILED(hr))
        return hr;
    return hrRet;

}   // CommonEnumCustomAttributeByName


//*****************************************************************************
// return just the blob value of the first found CA matching the query.
//*****************************************************************************
HRESULT CMiniMd::CommonGetCustomAttributeByName( // S_OK or error.
    mdToken     tkObj,                  // [IN] Object with Custom Attribute.
    LPCUTF8     szName,                 // [IN] Name of desired Custom Attribute.
    const void  **ppData,               // [OUT] Put pointer to data here.
    ULONG       *pcbData)               // [OUT] Put size of data here.
{
    HRESULT         hr;
    ULONG           cbData;
    HENUMInternal   hEnum;
    mdCustomAttribute ca = 0;
    CustomAttributeRec *pRec;

    hr = CommonEnumCustomAttributeByName(tkObj, szName, true, &hEnum);
    if (hr != S_OK)
        goto ErrExit;

    if (ppData)
    {
        // now get the record out.
        if (pcbData == 0)
            pcbData = &cbData;

        HENUMInternal::EnumNext(&hEnum, &ca);
        pRec = getCustomAttribute(RidFromToken(ca));
        *ppData = getValueOfCustomAttribute(pRec, pcbData);
    }
ErrExit:
    HENUMInternal::ClearEnum(&hEnum);
    return hr;
}   // CommonGetCustomAttributeByName


// eof ------------------------------------------------------------------------
    
