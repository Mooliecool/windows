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
// ===========================================================================
// File: map.h
//
// Map for #line pragmas
// ===========================================================================

#include "stdafx.h"

template<typename T>
int __cdecl CompareLongs(const void* first, const void* second) {
    return *(long*)first - ((MAPABLE<T>*)second)->mapLine;
}

template<class T, int S>
void CMapBase<T,S>::Clear() {
    if (m_array != NULL)
        m_allocator->Free(m_array);
    m_array = NULL;
    m_iCount = 0;
    m_iLast = -1;
}


// Need to explicitly instantiate a few members
template void CMapBase<PPLINE>::Clear();
template void CMapBase<PPWARNING>::AppendMap(long, const PPWARNING &);
template long CMapBase<PPWARNING>::FindClosestIndexAfter(long) const;
template long CMapBase<PPWARNING>::FindClosestIndexBefore(long) const;
template void CMapBase<PPWARNING>::Remove(long, long);
template void CMapBase<XMLMAP>::Clear();
template long CMapBase<XMLMAP>::FindIndex(long) const;


/* AddMap - adds a map for the given line
 * Assuming there is sufficient memory, this always succeeds
 * ASSERTs if a map for the given line already exists
 */
template<class T, int S>
void CMapBase<T,S>::AppendMap(long mapLine, const T & data) {
    ASSERT(FindIndex(mapLine) == -1);

    m_iLast++;
    if (m_iLast >= m_iCount)
        GrowMap();

    memcpy(&m_array[m_iLast], &data, sizeof(data));
    m_array[m_iLast].mapLine = mapLine;
    
    // ASSERT it if it's out of order
    // The tokenizer should ALWAYS add maps in order
    ASSERT (m_iLast == 0 || (m_iLast > 0 && m_array[m_iLast-1].mapLine < mapLine));
}

/* AddMap - adds a mapping from oldLine to (newLine, pFilename)
 * Assuming there is sufficient memory, this always succeeds
 * ASSERTs if a map for the given line already exists
 */
void CLineMap::AddMap(long oldLine, bool fDefaultLine, long newLine, NAME* pFilename) {
    PPLINE data = {fDefaultLine, newLine, pFilename};
    CMapBase<PPLINE, 10>::AppendMap(oldLine, data);
}

/* GrowMap - enlarges our map by 10 */
template<class T, int S>
void CMapBase<T,S>::GrowMap() {
    if (m_array == NULL) {
        m_array = (MAPABLE<T>*)m_allocator->Alloc( (m_iCount = S) * sizeof(MAPABLE<T>));
    } else
        m_array = (MAPABLE<T>*)m_allocator->Realloc(m_array, (m_iCount += S) * sizeof(MAPABLE<T>));
}

/* FindIndex - returns an index into our map array
 * if the line has a mapping, returns the index
 * else returns -1
 */
template<class T, int S>
long CMapBase<T,S>::FindIndex(long mapLine) const {
    if (m_array == NULL) return -1;

    void *index = bsearch( &mapLine, m_array, m_iLast + 1, sizeof(m_array[0]), CompareLongs<T>);
    if (index != NULL)
        return (long)((MAPABLE<T>*)index - m_array);
    else
        return -1;
}

/* FindClosestIndexBefore - returns an index into our map array
 * if the line has a mapping, returns the index
 * if not it returns the index of the previous line map
 * if no privous map exists, returns -1
 * (This uses FindClosestIndexBeforeBSearch and FindClosestIndexBeforeLinear to do the searching)
 */
template<class T, int S>
long CMapBase<T,S>::FindClosestIndexBefore(long mapLine) const {
    if (m_array == NULL) return -1;
    
    if (m_iCount <= 12)
        return FindClosestIndexBeforeLinear( mapLine, 0, m_iLast);
    else
        return FindClosestIndexBeforeBSearch( mapLine, 0, m_iLast);
}

/* FindClosestIndexBeforeLinear - returns an index into our map array
 * internal only method, assumes that m_array is valid
 */
template<class T, int S>
long CMapBase<T,S>::FindClosestIndexBeforeLinear(long mapLine, int min, int max) const {

    for(int l = max; l >= min; l--) {
        if (m_array[l].mapLine <= mapLine)
            return l;
    }
    return -1;
}


/* FindClosestIndexBeforeBSearch - returns an index into our map array
 * internal only method, assumes that m_array is valid
 * NOTE: this never actually finds the index, it relies on FindClosestIndexBeforeLinear
 */
template<class T, int S>
long CMapBase<T,S>::FindClosestIndexBeforeBSearch(long mapLine, int min, int max) const {

    if (max - min <= 12)
        return FindClosestIndexBeforeLinear( mapLine, min, max);

    int mid = (max - min) / 2 + min;
    if (m_array[mid].mapLine > mapLine) {
        return FindClosestIndexBeforeBSearch(mapLine, min, mid);
    } else if (m_array[mid].mapLine < mapLine) {
        return FindClosestIndexBeforeBSearch(mapLine, mid, max);
    } else {
        return mid;
    }
}

/* FindClosestIndexAfter - returns an index into our map array
 * if the line has a mapping, returns the index
 * if not it returns the index of the next line map
 * if no next map exists, returns -1
 * (This uses FindClosestIndexAfterBSearch and FindClosestIndexAfterLinear to do the searching)
 */
template<class T, int S>
long CMapBase<T,S>::FindClosestIndexAfter(long mapLine) const {
    if (m_array == NULL) return -1;
    
    if (m_iCount <= 12)
        return FindClosestIndexAfterLinear( mapLine, 0, m_iLast);
    else
        return FindClosestIndexAfterBSearch( mapLine, 0, m_iLast);
}

/* FindClosestIndexAfterLinear - returns an index into our map array
 * internal only method, assumes that m_array is valid
 */
template<class T, int S>
long CMapBase<T,S>::FindClosestIndexAfterLinear(long mapLine, int min, int max) const {

    for(int l = min; l <= max; l++) {
        if (m_array[l].mapLine >= mapLine)
            return l;
    }
    return -1;
}


/* FindClosestIndexAfterBSearch - returns an index into our map array
 * internal only method, assumes that m_array is valid
 * NOTE: this never actually finds the index, it relies on FindClosestIndexAfterLinear
 */
template<class T, int S>
long CMapBase<T,S>::FindClosestIndexAfterBSearch(long mapLine, int min, int max) const {

    if (max - min <= 12)
        return FindClosestIndexAfterLinear( mapLine, min, max);

    int mid = (max - min) / 2 + min;
    if (m_array[mid].mapLine > mapLine) {
        return FindClosestIndexAfterBSearch(mapLine, min, mid);
    } else if (m_array[mid].mapLine < mapLine) {
        return FindClosestIndexAfterBSearch(mapLine, mid, max);
    } else {
        return mid;
    }
}

// Removes elements (probably because they've been copied elsewhere, and shouldn't be cleaned-up
// when this object is destoryed
template <class T, int S>
void CMapBase<T,S>::Remove(long startLine, long endLine) {
    long startIndex = FindClosestIndexAfter(startLine);
    if (startIndex == -1 || m_array[startIndex].mapLine > endLine) {
        return;
    }

    long endIndex = FindClosestIndexBefore(endLine);
    if(endIndex == -1 || m_array[endIndex].mapLine <= startLine) {
        VSFAIL("Bad map");
        return;
    }

    if (endIndex < m_iLast)
        memmove( &m_array[startIndex], &m_array[endIndex+1], sizeof(m_array[0]) * (m_iLast - endIndex));

    m_iLast -= (endIndex + 1 - startIndex);

#ifdef DEBUG
    memset( &m_array[m_iLast + 1], 0, sizeof(m_array[0]) * (m_iCount - m_iLast - 1));
#endif
}

/* InternalMap - the real work horse, maps an oldLine to
 * a new line and filename
 * Faster if ppFilename is NULL because it doesn't find the mapped filename
 * also if there is no filename specified in a previous mapping it
 * does NOT change the value of ppFilename
 */
long CLineMap::InternalMap(long oldLine, NAME** /* out */ ppFilename, bool* /* out */ pbIsHidden, bool * /* out */ pbIsMapped) {
    if (pbIsHidden)
        *pbIsHidden = false;

    if (pbIsMapped)
        *pbIsMapped = false;

    long index = FindClosestIndexBefore(oldLine);
    if (index == -1)
        return oldLine;

    // Are we asking for the line that had the "#line"?
    if (oldLine == m_array[index].mapLine) {
        // backup to the previous one if it exists
        if (index > 0)
            index--;
        else
            // If it doesn't, then the line must be unmapped
            return oldLine;
    }

    // newLine == -1 means this section is 'hidden'
    if (m_array[index].newLine == -1) {
        if (pbIsHidden) *pbIsHidden = true;
        // Backup to get the previous line mapping for the correct line #
        while (m_array[index].newLine == -1) {
            if (index == 0)
                // If a previous one doesn't exist, then the line must be unmapped (hidden, but unmapped)
                return oldLine;
            index--;
        }
    }

    // Calculate the new line number
    long newLine = (oldLine - (m_array[index].mapLine + 1)) + m_array[index].newLine;

    if (ppFilename != NULL) {
        // Now get the filename in effect for this section
        while (index > 0 && m_array[index].pFilename == NULL)
            index--;
        if (index >= 0)
            *ppFilename = m_array[index].pFilename;
    }

    if (!m_array[index].fDefaultLine) {
        if (pbIsMapped) 
            *pbIsMapped = true;
    }

    ASSERT(newLine >= 0);
    return newLine;
}

bool CLineMap::ContainsEntry(int iStartLine, int iEndLine)
{
    int idx1 = FindClosestIndexBefore(iEndLine);
    if (idx1 >= 0 && m_array[idx1].mapLine >= iStartLine)
        return true;

    int idx2 = FindClosestIndexAfter(iStartLine);
    if (idx2>= 0 && m_array[idx2].mapLine <= iEndLine)
        return true;

    return false;
}

void CLineMap::RemoveFrom(int iStartLine)
{
    Remove(iStartLine, LONG_MAX);
}

void CLineMap::ApplyDelta(int iStartLine, int iDeltaLine)
{
    int idx = FindClosestIndexAfter(iStartLine);
    if (idx >= 0)
    {
        for (int i = idx; i <= m_iLast; i++)
        {
            MAPABLE<PPLINE> &ppline = m_array[i];
            ppline.mapLine += iDeltaLine;
            if (ppline.fDefaultLine)
                ppline.newLine += iDeltaLine;
        }
    }
}


// Override CBaseMap::Clear() so we can free up the warning lists
void CWarningMap::Clear() {
    for (long i = 0; i <= m_iLast; i++) {
        if (m_array[i].pWarnings != NULL) {
            m_allocator->Free((void*)m_array[i].pWarnings);
        }
        memset(&m_array[i], 0, sizeof(m_array[i]));
    }
    CMapBase<PPWARNING>::Clear();
}

// adds a #pragma warning to the end of the list
// ASSERTs if a warning for the given line already exists
void CWarningMap::AddWarning(long srcLine, const bool bDisable, const WORD cntWarnings, WORD * pWarnings) {
    PPWARNING w = { bDisable, cntWarnings, NULL};

    if (cntWarnings > 0) {
        w.pWarnings = (WORD*)m_allocator->Alloc(sizeof(WORD) * cntWarnings);
        // insertion/bubble sort
        for (WORD i = 0; i < cntWarnings; i++) {
            WORD insert = pWarnings[i];
            for (WORD j = 0; j < i; j++) {
                if (w.pWarnings[j] > insert) {
                    WORD temp = insert;
                    insert = w.pWarnings[j];
                    w.pWarnings[j] = temp;
                }
            }
            w.pWarnings[i] = insert;
        }
    }

    ASSERT((w.cntWarnings != 0 && w.pWarnings != NULL) || (w.cntWarnings == 0 && w.pWarnings == NULL));

    CMapBase<PPWARNING>::AppendMap(srcLine, w);
}

// Checks to see if the warning is restored anywhere before the end, and disabled anywhere before the start
bool CWarningMap::IsWarningDisabled(const WORD number, const long startLine, const long endLine) {

    long index = FindClosestIndexBefore(endLine);

    // There's no pragmas anywhere before the endLine, so the warning can't be disabled!
    if (index == -1)
        return false;

    while (index > -1) {
        int i = index--;
        if (m_array[i].bDisable) {
            if (m_array[i].mapLine > startLine)
                continue; // Doesn't matter if it was disabled in the middle of the range
            if (m_array[i].cntWarnings == 0) {
                // This means disable everything, so the warning is disabled
                return true;
            } else {
                for (int j = 0; j < m_array[i].cntWarnings; j++) {
                    // linear (but sorted) search because we expect these lists to be short
                    if (m_array[i].pWarnings[j] == number)
                        return true;
                    else if (m_array[i].pWarnings[j] > number)
                        break;
                }
            }
        } else {
            if (m_array[i].cntWarnings == 0) {
                // this means restore everything, so the warning is enabled
                return false;
            } else {
                for (int j = 0; j < m_array[i].cntWarnings; j++) {
                    // linear (but sorted) search because we expect these lists to be short
                    if (m_array[i].pWarnings[j] == number)
                        return false;
                    else if (m_array[i].pWarnings[j] > number)
                        break;
                }
            }
        }
    }

    return false;
}

// Checks to see if the warnings are restored or disabled anywhere between the start and end (inclusive)
// Assumes that numbers is sorted (little-to-big) warning numbers and terminated with 0
bool CWarningMap::IsWarningChanged(const WORD * const numbers, const long startLine, const long endLine) {

    long index = FindClosestIndexAfter(startLine);

    // there's no map after the startLine, or it's after the endLine!!!
    if (index == -1 || m_array[index].mapLine > endLine)
        return false;

    while (index <= m_iLast && m_array[index].mapLine <= endLine) {
        if (m_array[index].cntWarnings == 0)
            return true;

        const WORD * n = numbers;
        for (int j = 0; j < m_array[index].cntWarnings; j++) {
            // linear (but sorted) search because we expect these lists to be short
            while (m_array[index].pWarnings[j] > *n && *n != 0) n++;

            if (*n == 0)
                break;

            if (m_array[index].pWarnings[j] == *n)
                return true;
        }
        index++;
    }

    return false;
}

/* AddMap - adds a mapping from xmlLine to srcLine (plus a column offset)
 * Assuming there is sufficient memory, this always succeeds
 * ASSERTs if a map for the given line already exists
 */
void CXMLMap::AddMap(long xmlLine, long srcLine, long colAdjust) {
    XMLMAP data = {srcLine, colAdjust};
    CMapBase<XMLMAP, 10>::AppendMap(xmlLine, data);
}


