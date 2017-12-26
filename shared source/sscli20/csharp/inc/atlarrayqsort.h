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

#ifndef __ATL_ARRAY_QSORT_H__
#define __ATL_ARRAY_QSORT_H__

////////////////////////////////////////////////////////////////////////////////
// AtlArrayQSort
//
// Call qsort on an ATL Array (type safe)

template<typename T>
void AtlArrayQSort(CAtlArray<T> &ar, int (__cdecl *func)(const T *, const T *))
{
    if (ar.GetCount() <= 1)
        return;

    qsort(ar.GetData(), ar.GetCount(), sizeof(T), (int(_cdecl *)(const void *, const void *))func);
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
T * AtlArrayBSearch(CAtlArray<T> &ar, const T *pElem, int (__cdecl *func)(const T *, const T *))
{
    return (T *)bsearch(pElem, ar.GetData(), ar.GetCount(), sizeof(T), (int(_cdecl *)(const void *, const void *))func);
}

template<typename T1, typename T2>
interface IComparer
{
    virtual long Compare(const T1& t1, const T2& t2) = 0;
};

////////////////////////////////////////////////////////////////////////////////

//Finds the first element in the list that matches the search criteria. Useful
//for when the list contains duplicates

template<typename T1, typename T2>
long AtlArrayStableBinarySearch(CAtlArray<T1>& ar, const T2& elem, IComparer<T2,T1>* pComparer)
{
    long low = 0;
    long high = (long)ar.GetCount() - 1;
    while (low < high)
    {
        //dont' touch this code under penalty of death.  It's subtle!
        //(Talk to renaud if yoiu absolutely have to)
        long median = (low + high) / 2;
        long compareResult = pComparer->Compare(elem, ar[median]);
        if (compareResult > 0)
        {
            low = median + 1;
        }
        else
        {
            high = median;
        }
    }

    // min is position of 1st elem or insertion pos
    return low;
}

#endif // __ATL_ARRAY_QSORT_H__
