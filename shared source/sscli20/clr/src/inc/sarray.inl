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
// --------------------------------------------------------------------------------
// SArray.inl
// --------------------------------------------------------------------------------

#ifndef _SARRAY_INL_
#define _SARRAY_INL_

#include "sarray.h"

template <typename ELEMENT, BOOL BITWISE_COPY>
inline SArray<ELEMENT, BITWISE_COPY>::SArray()
  : m_buffer()
{
    LEAF_CONTRACT;
}
    
template <typename ELEMENT, BOOL BITWISE_COPY>
inline SArray<ELEMENT, BITWISE_COPY>::SArray(COUNT_T count)
  : m_buffer(count * sizeof(ELEMENT))
{
    WRAPPER_CONTRACT;
    ConstructBuffer(Begin(), count);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline SArray<ELEMENT, BITWISE_COPY>::SArray(ELEMENT * begin, ELEMENT * end)
  : m_buffer((end - begin) * sizeof(ELEMENT))
{
    WRAPPER_CONTRACT;
    CopyConstructBuffer(Begin(), end - begin, begin);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline SArray<ELEMENT, BITWISE_COPY>::SArray(void *prealloc, COUNT_T count)
  : m_buffer(SBuffer::Prealloc, prealloc, count*sizeof(ELEMENT))
{
    LEAF_CONTRACT;
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Clear()
{
    WRAPPER_CONTRACT;
    DestructBuffer(Begin(), GetCount());
    m_buffer.Clear();
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline ELEMENT* SArray<ELEMENT, BITWISE_COPY>::OpenRawBuffer(COUNT_T elementCount)
{
    WRAPPER_CONTRACT;
    return (ELEMENT*)m_buffer.OpenRawBuffer(elementCount * sizeof(ELEMENT));
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline ELEMENT* SArray<ELEMENT, BITWISE_COPY>::OpenRawBuffer()
{
    WRAPPER_CONTRACT;
    return (ELEMENT*)m_buffer.OpenRawBuffer(GetCount() * sizeof(ELEMENT));
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::CloseRawBuffer(COUNT_T finalElementCount)
{
    WRAPPER_CONTRACT;
    m_buffer.CloseRawBuffer(finalElementCount * sizeof(ELEMENT));
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::CloseRawBuffer()
{
    WRAPPER_CONTRACT;
    m_buffer.CloseRawBuffer();
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Set(const SArray<ELEMENT, BITWISE_COPY> &array)
{
    WRAPPER_CONTRACT;
    if (BITWISE_COPY)
    {
        m_buffer.Set(array.m_buffer);
    }
    else
    {
        DestructBuffer(Begin(), GetCount());
        m_buffer.SetSize(0);
        m_buffer.SetSize(array.m_buffer.GetSize());
        CopyConstructBuffer(Begin(), GetCount(), array.GetElements());
    }
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline COUNT_T SArray<ELEMENT, BITWISE_COPY>::GetCount() const
{
    WRAPPER_CONTRACT;
    return m_buffer.GetSize()/sizeof(ELEMENT);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline BOOL SArray<ELEMENT, BITWISE_COPY>::IsEmpty() const
{
    WRAPPER_CONTRACT;
    return GetCount() == 0;
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::SetCount(COUNT_T count)
{
    WRAPPER_CONTRACT;
    COUNT_T oldCount = GetCount();
    if (count > oldCount)
        ConstructBuffer(Begin() + oldCount, count - oldCount);
        
    m_buffer.SetSize(count*sizeof(ELEMENT));

    if (oldCount > count)
        DestructBuffer(Begin() + count, oldCount - count);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline COUNT_T SArray<ELEMENT, BITWISE_COPY>::GetAllocation() const
{
    WRAPPER_CONTRACT;
    return m_buffer.GetAllocation() / sizeof(ELEMENT);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Preallocate(int count) const
{
    WRAPPER_CONTRACT;
    m_buffer.Preallocate(count * sizeof(ELEMENT));
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Trim() const
{
    WRAPPER_CONTRACT;
    m_buffer.Trim();
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Copy(const Iterator &to, const Iterator &from, COUNT_T size)
{
    WRAPPER_CONTRACT;

    DestructBuffer(to, size);

    CopyConstructBuffer(to, size, from);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Move(const Iterator &to, const Iterator &from, COUNT_T size)
{

    DestructBuffer(to, size);

    SBuffer::Move(to, from, size*sizeof(ELEMENT));

    ConstructBuffer(from, size);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Copy(const Iterator &i, const ELEMENT *source, COUNT_T size)
{
    WRAPPER_CONTRACT;
    DestructBuffer(i, size);

    CopyConstructBuffer(i, size, source);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Copy(void *dest, const Iterator &i, COUNT_T size)
{
    WRAPPER_CONTRACT;

    m_buffer.Copy(dest, i.m_i, size*sizeof(ELEMENT));
}


template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Insert(const Iterator &i)
{
    WRAPPER_CONTRACT;
    Replace(i, 0, 1);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Delete(const Iterator &i)
{
    WRAPPER_CONTRACT;
    Replace(i, 1, 0);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Insert(const Iterator &i, COUNT_T count)
{
    WRAPPER_CONTRACT;
    Replace(i, 0, count);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::Delete(const Iterator &i, COUNT_T count)
{
    Delete(i, 0, count);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>:: Replace(const Iterator &i, COUNT_T deleteCount, COUNT_T insertCount)
{
    WRAPPER_CONTRACT;
    DestructBuffer(i, deleteCount);

    m_buffer.Replace(i.m_i, deleteCount*sizeof(ELEMENT), insertCount*sizeof(ELEMENT));

    ConstructBuffer(i, insertCount);
}


template <typename ELEMENT, BOOL BITWISE_COPY>
inline ELEMENT &SArray<ELEMENT, BITWISE_COPY>::operator[](int index)
{
    WRAPPER_CONTRACT;
    return *(GetElements() + index);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline const ELEMENT &SArray<ELEMENT, BITWISE_COPY>::operator[](int index) const
{
    WRAPPER_CONTRACT;
    return *(GetElements() + index);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline ELEMENT &SArray<ELEMENT, BITWISE_COPY>::operator[](COUNT_T index)
{
    WRAPPER_CONTRACT;
    return *(GetElements() + index);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline const ELEMENT &SArray<ELEMENT, BITWISE_COPY>::operator[](COUNT_T index) const
{
    return *(GetElements() + index);
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline ELEMENT *SArray<ELEMENT, BITWISE_COPY>::GetElements() const
{
    LEAF_CONTRACT;
    return (ELEMENT *) (const BYTE *) m_buffer;
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::ConstructBuffer(const Iterator &i, COUNT_T size)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    if (!BITWISE_COPY)
    {
        ELEMENT *start = GetElements() + (i - Begin());
        ELEMENT *end = start + size;

        while (start < end)
        {
            new (start) ELEMENT();
            start++;
        }
    }
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::CopyConstructBuffer(const Iterator &i, COUNT_T size, const ELEMENT *from)
{
    ptrdiff_t start_offset = i - Begin();
    ELEMENT *p = (ELEMENT *) m_buffer.OpenRawBuffer(m_buffer.GetSize()) + start_offset;

    if (BITWISE_COPY)
    {
        memmove(p, from, size * sizeof(ELEMENT));
    }
    else
    {
        ELEMENT *start = (ELEMENT *) p;
        ELEMENT *end = (ELEMENT *) (p + size);

        while (start < end)
        {
            new (start) ELEMENT(*from);

            start++;
            from++;
        }
    }

    m_buffer.CloseRawBuffer();
}

template <typename ELEMENT, BOOL BITWISE_COPY>
inline void SArray<ELEMENT, BITWISE_COPY>::DestructBuffer(const Iterator &i, COUNT_T size)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    if (!BITWISE_COPY)
    {
        ELEMENT *start = GetElements() + (i - Begin());
        ELEMENT *end = start + size;

        while (start < end)
        {
            start->ELEMENT::~ELEMENT();

            start++;
        }
    }
}

template <typename ELEMENT, COUNT_T SIZE, BOOL BITWISE_COPY>
inline InlineSArray<ELEMENT, SIZE, BITWISE_COPY>::InlineSArray()
  : SArray<ELEMENT, BITWISE_COPY>((void*)m_prealloc, SIZE)
{
    LEAF_CONTRACT;
}

#endif  // _SARRAY_INL_
