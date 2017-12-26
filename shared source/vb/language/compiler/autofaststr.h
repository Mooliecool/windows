//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// This is a template-based implementation of AutoFastStr Class. AutoFastStr is derived from 
// FastStr Class. User can opt any memory allocation mechanism (like NorlsAllocWrapper, CompilerAlloc 
// etc.,) to store string.
//
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
//
// AutoFastStr gives user an option to select memory allocation types for storing string. User 
// have to provide an object of memory allocator (like NorlsAllocWrapper, CompilerAlloc etc.,) 
// while declaring an object of AutoFastStr.
// AutoFastStr uses all string comparison capabilities provided by FastStr. 
//
//-------------------------------------------------------------------------------------------------
template <class Alloc = VBAllocWrapper>
class AutoFastStr : public FastStr
{
public:

    // Constructor with allocator only
    AutoFastStr(_In_ Alloc allocator = Alloc());

    // Constructor with String and Allocator
    AutoFastStr(
        _In_opt_z_ const wchar_t * wczStr, 
        _In_ Alloc allocator = Alloc());

    // Copy Constructor: Creates a shallow copy and increments the reference count
    AutoFastStr(_In_ const AutoFastStr &rhsAutoFastStr);

    // Destructor: Free memory if string is not being referenced by some other object. 
    ~AutoFastStr();

    // Assignment Operator: Initializes the object with received string. 
    AutoFastStr<Alloc> & operator=(_In_opt_z_ const wchar_t *wczStr);

    // Assignment Operator: Initializes the object with right-hand side object. 
    AutoFastStr<Alloc> & operator=(_In_ const AutoFastStr<Alloc> &rhs);

    // Free the string unless being used by some other object
    virtual __override void Release();

private:

    // Stores allocator 
    Alloc m_alloc;

    // String's Reference count 
    unsigned * m_referenceCount;

    // Allocates memory using allocator and copies string in the memory 
    wchar_t * CopyString(_In_opt_z_ const wchar_t * wczStr);

    //Initializes the class members
    virtual __override void Init(_In_opt_z_ const wchar_t * wczStr);


};

// Constructor
template <class Alloc>
AutoFastStr<Alloc>::AutoFastStr(_In_ Alloc allocator) :
    FastStr(),
    m_alloc(allocator),
    m_referenceCount(NULL)
{
}

// Constructor with String and Allocator
template <class Alloc>
AutoFastStr<Alloc>::AutoFastStr(
    _In_opt_z_ const wchar_t * wczStr, 
    _In_ Alloc allocator) : 
    FastStr(),
    m_alloc(allocator)
{
    Init(wczStr); 
}

// Copy Constructor: Creates a shallow copy and increments the reference count
template <class Alloc>
AutoFastStr<Alloc>::AutoFastStr(_In_ const AutoFastStr &rhsAutoFastStr) :
    m_alloc(rhsAutoFastStr.m_alloc)
{
    if (rhsAutoFastStr.m_referenceCount)
    {
        m_wczStr   = rhsAutoFastStr.m_wczStr; 
        m_hashCode = rhsAutoFastStr.m_hashCode;
        m_length   = rhsAutoFastStr.m_length;
        m_referenceCount  = rhsAutoFastStr.m_referenceCount;
        (*m_referenceCount)++;
    }
    else
    {
        // Initialize with NULL
        Init(NULL);
    }
}

//---------------------------------------------------------------------------------------------
//
// Destructor: Free memory if string is not being referenced by some other object. 
// 
template <class Alloc>
AutoFastStr<Alloc>::~AutoFastStr()
{
    Release();
}

//---------------------------------------------------------------------------------------------
//
// Assignment Operator: Initializes the object with received string. 
// If left-hand side object already points to a string then this function  
// releases the previous string before assigning to the new string and increments  
// the reference count
//
template <class Alloc>
AutoFastStr<Alloc> & AutoFastStr<Alloc>::operator=(_In_opt_z_ const wchar_t *wczStr)
{
    // Since wczStr may be the same pointer as m_wczStr. So save the 
    // string before releasing the left-hand side object. 

    AutoFastStr<Alloc> tmp(wczStr, m_alloc);

    *this = tmp;

    return *this;
}

//---------------------------------------------------------------------------------------------
//
// Assignment Operator: Initializes the object with right-hand side object. 
// If left-hand side object already points to a string then this function  
// releases the previous string before assigning to the new string and increments  
// the reference count
//
template <class Alloc>
AutoFastStr<Alloc> & AutoFastStr<Alloc>::operator=(_In_ const AutoFastStr<Alloc> &rhs)
{
    if (m_wczStr == rhs.m_wczStr)
    {
        return *this;
    }

    //Reset the class members
    Release();

    // Shallow copy
    m_wczStr   = rhs.m_wczStr; 
    m_hashCode = rhs.m_hashCode;
    m_length   = rhs.m_length;
    m_alloc    = rhs.m_alloc;
    m_referenceCount  = rhs.m_referenceCount;

    // Before incrementing the reference count, make sure rhs is already initialized 
    // with a string
    if (m_referenceCount)
    {
        (*m_referenceCount)++;
    }

    return *this;
}

// Free the string unless being used by some other object
template <class Alloc>
//virtual 
void AutoFastStr<Alloc>::Release()
{
    if (m_referenceCount == NULL)
    {
        return;
    }

    // Because we set m_referenceCount to NULL when *m_referenceCount was 0 after decrement.
    VSASSERT(*m_referenceCount != 0, "Reference Count cannot be zero");

    (*m_referenceCount)--;
    if (*m_referenceCount == 0) 
    {
        if (m_wczStr)
        {
            m_alloc.DeAllocateArray(const_cast< wchar_t * >(m_wczStr));
        }
        m_alloc.DeAllocate(m_referenceCount);
    }

    // Set the pointers to NULL
    m_wczStr = NULL;
    m_referenceCount = NULL;
}

// Allocate memory using 'alloc' and copies string in the memory 
template <class Alloc>
wchar_t * AutoFastStr<Alloc>::CopyString(_In_opt_z_ const wchar_t * wczStr)
{
    if (wczStr == NULL)
    {
        return NULL;
    }

    // One extra byte for NULL terminator
    wchar_t * wczReturnStr = m_alloc.AllocateArray<wchar_t>(wcslen(wczStr)+1);
    VSASSERT(wczReturnStr != NULL, "Cannot allocate memory");
    wcsncpy(wczReturnStr, wczStr, wcslen(wczStr)+1);
    return wczReturnStr;
}

//Initializes the class members
template <class Alloc>
//virtual 
void AutoFastStr<Alloc>::Init(_In_opt_z_ const wchar_t * wczStr)
{
    if (wczStr != NULL)
    {
        m_wczStr   = CopyString(wczStr); 
        m_length   = wcslen(m_wczStr);
        m_hashCode = GetHashCode(m_wczStr, m_length);

        m_referenceCount = m_alloc.Allocate<unsigned>();
        VSASSERT(m_referenceCount != NULL, "Cannot allocate memory");
        *m_referenceCount = 1;
    }
    else
    {
        m_wczStr   = NULL; 
        m_length   = 0;
        m_hashCode = 0;
        m_referenceCount = NULL;
    }
}
