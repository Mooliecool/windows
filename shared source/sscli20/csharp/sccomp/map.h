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
// and #pragma warnings
// ===========================================================================

#ifndef __map_h__
#define __map_h__

struct NAME;

template<class T> struct MAPABLE : T {
    long    mapLine;
};

struct PPLINE {
    bool    fDefaultLine;
    long    newLine;
    NAME    *pFilename;
};

struct PPWARNING {
    bool    bDisable;      // true for #pragma warning disable, false for #pragma warning restore

    // INVARIANT(!bDssable || cntWarnings > 0)
    WORD    cntWarnings;

    // INVARIANT(lengthof(pWarnings) == cntWarnings)
    WORD *pWarnings;
};

struct XMLMAP {
    long    srcLine;
    long    colAdjust;
};

template<class T, int S = 10> class CMapBase {
public:

    CMapBase(MEMHEAP * allocator) : m_allocator(allocator), m_array(NULL), m_iCount(0), m_iLast(-1) {};
    ~CMapBase() { Clear(); }

    // Frees memory and does other cleanup
    virtual void Clear();
    __inline bool IsEmpty() { return m_array == NULL && m_iCount == 0; }
    long GetCount(long beforeLine = LONG_MAX) { return FindClosestIndexBefore(beforeLine) + 1; }

    template<int U>
    void Copy(const CMapBase<T,U> * oldMap, long startLine = -1, long endLine = LONG_MAX, long delta = 0){
        long startIndex = oldMap->FindClosestIndexAfter(startLine);
        if (startIndex == -1 || oldMap->m_array[startIndex].mapLine >= endLine) {
            return;
        }

        long endIndex = oldMap->FindClosestIndexBefore(endLine);
        ASSERT(endIndex != -1 && oldMap->m_array[endIndex].mapLine > startLine);

        for (;startIndex <= endIndex; startIndex++)
            AppendMap(oldMap->m_array[startIndex].mapLine + delta, oldMap->m_array[startIndex]);
    }

    // Does not free any memory or do any cleanup!!!
    void Remove(long startLine = -1, long endLine = LONG_MAX);

protected:
    void AppendMap(long mapLine, const T & data);
    void GrowMap();
    long FindIndex(long mapLine) const;
    long FindClosestIndexBefore(long mapLine) const;
    long FindClosestIndexBeforeLinear(long mapLine, int min, int max) const;
    long FindClosestIndexBeforeBSearch(long mapLine, int min, int max) const;
    long FindClosestIndexAfter(long mapLine) const;
    long FindClosestIndexAfterLinear(long mapLine, int min, int max) const;
    long FindClosestIndexAfterBSearch(long mapLine, int min, int max) const;

    MEMHEAP     *m_allocator;
    MAPABLE<T>  *m_array;
    long         m_iCount;
    long         m_iLast;
};


////////////////////////////////////////////////////////////////////////////////
// CLineMap
//
// Map of {Line # in source file => PPLINE entry}
// Whenever a #line directive is encountered in a source file, we add an entry
// to the map.
//  #line default:               Add an entry (curLine, <curLine + 1>, NULL)
//  #line hidden :               Add an entry (curLine, -1, NULL)
//  #line <filename> <line#>:    Add an entry (curLine, <line#>, <filename>)

class CLineMap : public CMapBase<PPLINE> {
public:
    CLineMap(MEMHEAP *allocator) : CMapBase<PPLINE>(allocator) {};
    ~CLineMap() { Clear(); }

    void AddMap(long oldLine, bool fDefaultLine, long newLine, NAME* pFilename);

    void HideLines(long oldLine) 
    { 
        AddMap(oldLine, false/*fDefaultLine*/, -1, NULL); 
    }

    __inline long Map(long oldLine, NAME** /* out */ ppFilename, bool* /* out */ pbIsHidden, bool* /* out */ pbIsMapped) {
        if (m_array == NULL) {
            if (pbIsHidden) *pbIsHidden = false;
            if (pbIsMapped) *pbIsMapped = false;
            return oldLine;
        } else
            return InternalMap( oldLine, ppFilename, pbIsHidden, pbIsMapped);
    }

    __inline void Map(POSDATA * /* in,out */ pos, NAME** /* out */ ppFilename, bool* /* out */ pbIsHidden, bool* /* out */ pbIsMapped) {
        if (m_array != NULL)
            pos->iLine = InternalMap( pos->iLine, ppFilename, pbIsHidden, pbIsMapped);
        else 
        {
            if (pbIsHidden) *pbIsHidden = false;
            if (pbIsMapped) *pbIsMapped = false;
        }
    }

    MEMHEAP *GetAllocator() const 
    { 
        return m_allocator; 
    }

    bool ContainsEntry(int iStartLine, int iEndLine);
    void RemoveFrom(int iStartLine);
    void ApplyDelta(int iStartLine, int iDeltaLine);


private:
    long InternalMap(long oldLine, NAME** /* out */ ppFilename, bool* /* out */ pbIsHidden, bool* /* out */ pbIsMapped);
};

class CWarningMap : public CMapBase<PPWARNING> {
public:
    CWarningMap(MEMHEAP *allocator) : CMapBase<PPWARNING>(allocator) {};

    virtual void Clear();

    void AddWarning(long srcLine, const bool bDisable, const WORD cntWarnings, WORD * pWarnings);

    bool IsWarningDisabled(const WORD number, const long startLine, const long endLine);

    bool IsWarningChanged(const WORD * const number, const long startLine, const long endLine);
};


class CXMLMap : public CMapBase<XMLMAP> {
public:
    CXMLMap() : CMapBase<XMLMAP>(NULL) {};
    CXMLMap(MEMHEAP *allocator) : CMapBase<XMLMAP>(allocator) {};
    ~CXMLMap() { Clear(); }

    void Init(MEMHEAP * allocator) { m_allocator = allocator; }

    void AddMap(long xmlLine, long srcLine, long colAdjust);

    __inline POSDATA Map(long xmlLine, long xmlColumn) const {
        long index = FindIndex(xmlLine);
        if (index == -1)
            return POSDATA(); // Mapping a line that somehow has no info!!!!
        else
            return POSDATA(m_array[index].srcLine, xmlColumn + m_array[index].colAdjust);
    }

    __inline long Count() const { return m_iLast + 1; }
};

#endif //__map_h__
