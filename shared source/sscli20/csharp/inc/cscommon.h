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
// File: cscommon.h
//
// ===========================================================================

#ifndef __CSCOMMON_H__
#define __CSCOMMON_H__

// round up to a power of 2
// use macros for compile time constants only
// use inline functions for most cases
#define ROUNDUP_MASK(value, mask) (((value) + (mask)) & ~(mask))
#define ROUNDUP(value, radix) ROUNDUP_MASK(value, (radix - 1))
#define ROUNDUP_2(value) ROUNDUP(value, 2)
#define ROUNDUP_4(value) ROUNDUP(value, 4)
#define ROUNDUP_8(value) ROUNDUP(value, 8)
#define ROUNDUP_16(value) ROUNDUP(value, 16)
#define ROUNDUP_32(value) ROUNDUP(value, 32)
#define ROUNDUP_64(value) ROUNDUP(value, 64)
#define ROUNDUP_128(value) ROUNDUP(value, 128)
#define ROUNDUP_PTR(value) ROUNDUP(value, sizeof(void*));

template <class T>
inline T RoundUp(T value, T radix)
{
    T mask = (T) (radix - 1);
    return static_cast<T> (((value) + (mask)) & ~(mask));
}

template <class T>
inline T RoundUp2(T value)
{
    return RoundUp(value, static_cast<T> (2));
}

template <class T>
inline T RoundUp4(T value)
{
    return RoundUp(value, static_cast<T> (4));
}

template <class T>
inline T RoundUp8(T value)
{
    return RoundUp(value, static_cast<T> (8));
}

template <class T>
inline T RoundUp16(T value)
{
    return RoundUp(value, static_cast<T> (16));
}

template <class T>
inline T RoundUp32(T value)
{
    return RoundUp(value, static_cast<T> (32));
}

template <class T>
inline T RoundUp64(T value)
{
    return RoundUp(value, static_cast<T> (64));
}

template <class T>
inline T RoundUp128(T value)
{
    return RoundUp(value, static_cast<T> (128));
}

template <class T>
inline T RoundUpAllocSize(T value)
{
#ifdef ALIGN_ACCESS
    // Note: allocation alignment is not equal to sizeof(void*) on some platforms (e.g. SPARC)
    T alignmask = ALIGN_ACCESS;
#else
    T alignmask = sizeof(void*);
#endif
    return RoundUp(value, static_cast<T> (alignmask));
}

template <class T>
inline T RoundUpAllocSizeTo(T value, int access)
{
    return RoundUp(value, static_cast<T> (access));
}

template <typename Type, long length>
long ArrayLength(Type (&)[length])
{
    return length;
}

template <typename A, typename B>
class CPairData
{
private:
#ifdef DEBUG
    bool m_fInited;
#endif
    A m_a;
    B m_b;

public:
    CPairData()
#ifdef DEBUG
        : m_fInited(false)
#endif
    {
    }

    CPairData(A a, B b) : 
#ifdef DEBUG
        m_fInited(true), 
#endif
        m_a(a), m_b(b) 
    {
    }

    const A& GetFirst() const 
    {
#ifdef DEBUG
        ASSERT(m_fInited);
#endif
        return m_a;
    }

    const B& GetSecond() const 
    {
#ifdef DEBUG
        ASSERT(m_fInited);
#endif
        return m_b;
    }
};

#ifndef CLIENT_IS_CSC
#ifndef CLIENT_IS_ALINK
#endif
#endif

inline int OnCriticalInternalError()
{
    // Force an AV.
    return *((volatile int*)0);
}

#ifdef DEBUG
#define RETAILVERIFY(fTest) ASSERT(fTest)
#else // !DEBUG
#define RETAILVERIFY(fTest) (!(fTest) ? OnCriticalInternalError() : 0)
#endif


__forceinline size_t SizeMul(size_t a, size_t b) {
    size_t c = a * b;
    RETAILVERIFY(!b || c / b == a);
    return c;
}

__forceinline size_t SizeAdd(size_t a, size_t b) {
    size_t c = a + b;
    RETAILVERIFY(c >= a);
    return c;
}

__forceinline int IndexAdd(int a, int b) {
    int c = a + b;
    RETAILVERIFY(c >= 0);
    return c;
}

template <typename T, typename U, int CCH_BUFFER>
__forceinline T FetchAtIndex(T (&arr)[CCH_BUFFER], U index)
{
    RETAILVERIFY((U)(int)index == index && (int)index >= 0 && (int)index < CCH_BUFFER);
    return arr[index];
}

template <typename T, typename U>
__forceinline T FetchAtIndex(T *arr, U index, int max)
{
    RETAILVERIFY((U)(int)index == index && (int)index >= 0  && (int)index < max);
    return arr[index];
}

template <typename T, typename U, typename S, int CCH_BUFFER>
__forceinline void StoreAtIndex(T (&arr)[CCH_BUFFER], U index, S value)
{
    RETAILVERIFY((U)(int)index == index && (int)index >= 0 && (int)index < CCH_BUFFER);
    arr[index] = value;
}

template <typename T, typename U, typename S>
__forceinline void StoreAtIndex(T * arr, U index, int max, S value)
{
    RETAILVERIFY((U)(int)index == index && (int)index >= 0  && (int)index < max);
    arr[index] = value;
}


#define STACK_ALLOC(t,n) (t*)_alloca (SizeMul(n, sizeof(t)))
#define EnsureFailedWinErrAndExit(expr) { hr = HRESULT_FROM_WIN32(expr); if (SUCCEEDED(hr)) hr = E_UNEXPECTED; goto LExit; }


inline HRESULT AllocateBSTR (PCWSTR pszText, BSTR *pbstrOut)
{
    *pbstrOut = SysAllocString (pszText ? pszText : L"");
    return *pbstrOut == NULL ? E_OUTOFMEMORY : S_OK;
}


template <typename T>
class BufferWithLen
{
public:
    BufferWithLen(T * memory, size_t count) {
        RETAILVERIFY((size_t)(ULONG)count == count);
        this->memory = (count > 0 ? memory : NULL);
        this->count = count;
    }
    BufferWithLen() {
        memory = NULL;
        count = 0;
    }
    BufferWithLen<T> wcschr(WCHAR ch)
    {
        RETAILVERIFY(memory);
        T * found = ::wcschr(memory, ch);
        if (found)
            return CreateTraversal().MoveForward(found - memory);
        return BufferWithLen<T>();
    }
    template <size_t CCH_BUFFER>
    BufferWithLen(T (&sz)[CCH_BUFFER]) {
        RETAILVERIFY(CCH_BUFFER > 1);
        this->memory = sz;
        this->count = CCH_BUFFER;
    }
    static BufferWithLen<T> CreateFrom(T* _memory) {
        BufferWithLen<T> rval(_memory, (_memory ? (1 + wcslen(_memory)) : 0));
        return rval;
    }
    T operator [] (int index) const {
        RETAILVERIFY(memory);
        RETAILVERIFY(index >= 0 && (size_t)index < count);
        return memory[index];
    }
    T operator [] (size_t index) const {
        RETAILVERIFY(memory);
        RETAILVERIFY(index >= 0 && index < count);
        return memory[index];
    }
    bool operator== (const BufferWithLen<T> op2) const
    {
        return memory == op2.GetData();
    }
    operator const T* () const {
        RETAILVERIFY(!memory || count > 0);
        return memory;
    }
    template <typename U>
    BufferWithLen<T> operator+ (U op2) const
    {
        return CreateTraversal().MoveForward(op2);
    }
    BufferWithLen<T> operator+ (size_t op2) const
    {
        return CreateTraversal().MoveForward(op2);
    }   
#ifdef _MSC_VER
    // on other platforms, ULONG and size_t are the same type
    BufferWithLen<T> operator+ (ULONG op2) const
    {
        return CreateTraversal().MoveForward(op2);
    }   
#endif
    BufferWithLen<T> operator+ (int op2) const
    {
        return CreateTraversal().MoveForward(op2);
    }
    const T* Str() const {
        RETAILVERIFY(!memory || count > 0);
        return memory;
    }
    void SetAt(size_t index, T t) const {
        RETAILVERIFY(memory);
        RETAILVERIFY(index >= 0 && index < count);
        memory[index] = t;
    }
    T* GetData() const {
        RETAILVERIFY(!memory || count > 0);
        return memory;
    }
    ULONG Count() const {
        return (ULONG)count;
    }
    ULONG Size() const {
        size_t cb = SizeMul(count, sizeof(T));
        RETAILVERIFY((ULONG)cb == cb);
        return (ULONG)cb;
    }
    template <typename U>
    void CopyFrom(const BufferWithLen<U> source) const {
        RETAILVERIFY(memory);
        RETAILVERIFY(source.Size() <= Size());
        memcpy(memory, source.GetData(), min(Size(), source.Size()));
    }
    template <typename U>
    void CopyFrom(const BufferWithLen<U> source, size_t elements) const {
        RETAILVERIFY(memory);
        RETAILVERIFY(elements <= source.Count());
        memcpy(memory, source.GetData(), SizeMul(elements, sizeof(U)));
    }
    void InitWith(int i) const {
        memset(memory, i, Size());
    }
    BufferWithLen<T> MoveForward(size_t elementCount)
    {
        RETAILVERIFY(elementCount <= Count());
        if (count <= elementCount) {
            count = 0;
            memory = NULL;
        }
        else {
            count -= elementCount;
            memory += elementCount;
        }
        return *this;
    }
    void AddAndIncrement(T newElement)
    {
        RETAILVERIFY(Count() > 0);
        SetAt(0, (T)newElement);
        MoveForward(1);
    }
    BufferWithLen<T> CreateTraversal() const {
        return BufferWithLen<T>(memory, Count());
    }
    bool IsValid() {
        return memory != NULL;
    }
    void Invalidate() {
        memory = NULL;
        count = 0;
    }
protected:
    T * memory;
    size_t count;
};



template <typename T>
class VSFreer : public BufferWithLen<T>
{
public:
    VSFreer() {
        origMemory = NULL;
    }
    ~VSFreer() {
        Clear();
    }
    void Clear() {
        if (origMemory) {
            VSFree(origMemory);
            origMemory = NULL;
            this->Invalidate();
        }
    }
    HRESULT AllocCount(size_t count)
    {
        ASSERT(!origMemory);
        if (count) {
            origMemory = this->memory = (T*) VSAlloc(SizeMul(count, sizeof(T)));
            return this->memory ? ((this->count = count), S_OK) : E_OUTOFMEMORY;
        } else {
            origMemory = this->memory = 0;
            this->count = 0;
            return S_OK;
        }
    }
    void TransferOwnershipFrom(VSFreer<T> & source)
    {
        Clear();
        this->memory = source.memory;
        this->count = source.count;
        this->origMemory = source.origMemory;
        source.Invalidate();
        source.origMemory = NULL;
    }
    HRESULT AllocFromFile(HANDLE hFile, DWORD fileSize) {
        RETAILVERIFY(sizeof(T) == 1);
        HRESULT hr;
        DWORD cbRead;
        if (FAILED(hr = AllocCount(SizeAdd(fileSize, 1)))) goto LExit;
        if (!ReadFile (hFile, this->GetData(), fileSize, &cbRead, NULL) || (cbRead != fileSize))
        {
            EnsureFailedWinErrAndExit(GetLastError())
        }
        this->memory[fileSize] = 0;
        this->count = fileSize;
        if (this->count == 0) this->memory = NULL;
LExit:
        return hr;
    }
private:
    T * origMemory;
};

typedef VSFreer<WCHAR> WCAllocBuffer;
typedef VSFreer<char> CAllocBuffer;
typedef BufferWithLen<WCHAR> WCBuffer;
typedef BufferWithLen<char> CBuffer;

#endif // __CSCOMMON_H__
