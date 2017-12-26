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
// File: InternalOnly.cpp
//
//*****************************************************************************
#ifndef __InternalOnly_h__
#define __InternalOnly_h__

#ifndef BadError
#define BadError(hr) (hr)
#endif

#ifndef PostError
#define PostError(hr) (hr)
#endif

#ifndef OutOfMemory
#define OutOfMemory() (E_OUTOFMEMORY)
#endif

/* ------------------------------------------------------------------------- *
 * Global variable declarations
 * ------------------------------------------------------------------------- */
// Every time an EnC happens, we bump this up.                                
extern ULONG g_EditAndContinueCounter;




class GetActiveFunctionsDebuggerCommand : public DebuggerCommand
{
public:
    GetActiveFunctionsDebuggerCommand(const WCHAR *name, int minMatchLength = 0);
    void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args);
    void Help(Shell *shell);
    const WCHAR *ShortHelp(Shell *shell);
};


class EditAndContinueDebuggerCommand : public DebuggerCommand
{
public:
    EditAndContinueDebuggerCommand(const WCHAR *name, int minMatchLength = 0);
    void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args);
    void Help(Shell *shell);
    const WCHAR *ShortHelp(Shell *shell);
};

class CompileForEditAndContinueCommand : public DebuggerCommand
{
public:
    CompileForEditAndContinueCommand(const WCHAR *name, int minMatchLength = 0);
    void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args);
    void Help(Shell *shell);
    const WCHAR *ShortHelp(Shell *shell);
};

class DisassembleDebuggerCommand : public DebuggerCommand
{
public:
    DisassembleDebuggerCommand(const WCHAR *name, int minMatchLength = 0);
    void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args);
    void Help(Shell *shell);
    const WCHAR *ShortHelp(Shell *shell);
};


class ClearUnmanagedExceptionCommand : public DebuggerCommand
{
public:
    ClearUnmanagedExceptionCommand(const WCHAR *name, int minMatchLength = 0);
    void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args);
    void Help(Shell *shell);
    const WCHAR *ShortHelp(Shell *shell);
};

// Unmanaged commands
class UnmanagedThreadsDebuggerCommand : public DebuggerCommand
{
private:
	BOOL  m_unmanaged;
public:
    UnmanagedThreadsDebuggerCommand(const WCHAR *name, int minMatchLength = 0);
    void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args);
    void Help(Shell *shell);
    const WCHAR *ShortHelp(Shell *shell);
};

class UnmanagedWhereDebuggerCommand : public DebuggerCommand
{
private:
	BOOL  m_unmanaged;

public:
    UnmanagedWhereDebuggerCommand(const WCHAR *name, int minMatchLength = 0);
    void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args);
	void Help(Shell *shell);
    const WCHAR *ShortHelp(Shell *shell);
};


//-----------------------------------------------------------------------------
// Misc commands
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Print GC handle
//-----------------------------------------------------------------------------
class PrintGCHandleCommand : public DebuggerCommand
{
public:
    PrintGCHandleCommand(const WCHAR *name, int minMatchLength = 0);
    void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args);
    void Help(Shell *shell);
    const WCHAR *ShortHelp(Shell *shell);
};

//-----------------------------------------------------------------------------
// Call IsTransitionStub at an address
// Also useful for testing a stubmanager
//-----------------------------------------------------------------------------
class IsTransitionStubCommand : public DebuggerCommand
{
public:
    IsTransitionStubCommand(const WCHAR *name, int minMatchLength = 0);
    void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args);
    void Help(Shell *shell);
    const WCHAR *ShortHelp(Shell *shell);
};


//-----------------------------------------------------------------------------
// Dump everything in the world we-know about for a given function.
//-----------------------------------------------------------------------------
class FuncInfoCommand : public DebuggerCommand
{
public:
    FuncInfoCommand(const WCHAR *name, int minMatchLength = 0);
    void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args);
    void Help(Shell *shell);
    const WCHAR *ShortHelp(Shell *shell);

protected:
    void Print(ICorDebugFunction *pFunc, ICorDebugFunction2 *pFunc2, DebuggerShell *shell, ICorDebug *cor);
    HRESULT PrintNativeCode(ICorDebugCode * pCode, DebuggerShell *shell);
    
};



//*****************************************************************************
// Unfortunately the CreateStreamOnHGlobal is a little too smart in that
// it gets its size from GlobalSize.  This means that even if you give it the
// memory for the stream, it has to be globally allocated.  We don't want this
// because we have the stream read only in the middle of a memory mapped file.
// CreateStreamOnMemory and the corresponding, internal only stream object solves
// that problem.
//*****************************************************************************
class CInMemoryStream : public IStream
{
public:
    CInMemoryStream() :
        m_pMem(0),
        m_cbSize(0),
        m_cbCurrent(0),
        m_cRef(1)
    { }

    void InitNew(
        void        *pMem,
        ULONG       cbSize)
    {
        m_pMem = pMem;
        m_cbSize = cbSize;
        m_cbCurrent = 0;
    }

    ULONG STDMETHODCALLTYPE AddRef() {
        return (InterlockedIncrement(&m_cRef));
    }


    ULONG STDMETHODCALLTYPE Release();

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, PVOID *ppOut);

    HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead);

    HRESULT STDMETHODCALLTYPE Write(const void  *pv, ULONG cb, ULONG *pcbWritten);

    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove,DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);

    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize)
    {
        return (BadError(E_NOTIMPL));
    }

    HRESULT STDMETHODCALLTYPE CopyTo(
        IStream     *pstm,
        ULARGE_INTEGER cb,
        ULARGE_INTEGER *pcbRead,
        ULARGE_INTEGER *pcbWritten);

    HRESULT STDMETHODCALLTYPE Commit(
        DWORD       grfCommitFlags)
    {
        return (BadError(E_NOTIMPL));
    }

    HRESULT STDMETHODCALLTYPE Revert()
    {
        return (BadError(E_NOTIMPL));
    }

    HRESULT STDMETHODCALLTYPE LockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD       dwLockType)
    {
        return (BadError(E_NOTIMPL));
    }

    HRESULT STDMETHODCALLTYPE UnlockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD       dwLockType)
    {
        return (BadError(E_NOTIMPL));
    }

    HRESULT STDMETHODCALLTYPE Stat(
        STATSTG     *pstatstg,
        DWORD       grfStatFlag)
    {
        pstatstg->cbSize.QuadPart = m_cbSize;
        return (S_OK);
    }

    HRESULT STDMETHODCALLTYPE Clone(
        IStream     **ppstm)
    {
        return (BadError(E_NOTIMPL));
    }

    static HRESULT CreateStreamOnMemory(           // Return code.
                                 void        *pMem,                  // Memory to create stream on.
                                 ULONG       cbSize,                 // Size of data.
                                 IStream     **ppIStream);            // Return stream object here.

private:
    void        *m_pMem;                // Memory for the read.
    ULONG       m_cbSize;               // Size of the memory.
    ULONG       m_cbCurrent;            // Current offset.
    LONG        m_cRef;                 // Ref count.
};


#endif // __InternalOnly_h__
