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

#if !defined( __STREAMUTIL_H__ )
#define __STREAMUTIL_H__

namespace StreamUtil
{

//  Write data to stream and advance the totalBytes counter
//
inline
HRESULT WriteToStream( IStream * strm, void const * data, ULONG sizeInBytes, ULONG * totalBytes = NULL )
{
    HRESULT hr = strm->Write( data, sizeInBytes, NULL );
    if ( SUCCEEDED( hr ) && totalBytes != NULL )
        *totalBytes += sizeInBytes;
    return hr;
}


//  Write a POD to stream
//
template < typename T >
HRESULT WritePODToStream( IStream * strm, T val, ULONG * totalBytes )
{
    return WriteToStream( strm, & val, sizeof( val ), totalBytes );
}


//  Write concrete data types to stream
//  Add additional overloads as needed
//

inline
HRESULT WriteToStream( IStream * strm, int val, ULONG * totalBytes = NULL )
{
    return WritePODToStream( strm, val, totalBytes );
}


inline
HRESULT WriteToStream( IStream * strm, DWORD val, ULONG * totalBytes = NULL )
{
    return WritePODToStream( strm, val, totalBytes );
}


inline
HRESULT WriteToStream( IStream * strm, WORD val, ULONG * totalBytes = NULL )
{
    return WritePODToStream( strm, val, totalBytes );
}


inline
HRESULT WriteToStream( IStream * strm, BYTE val, ULONG * totalBytes = NULL )
{
    return WritePODToStream( strm, val, totalBytes );
}


//  Align to DWORD boundary
//
inline
HRESULT AlignDWORD( IStream * strm, ULONG * totalBytes )
{
    HRESULT hr = S_OK;

    ULONG aligned = *totalBytes + 3 & ~3;
    if ( aligned != *totalBytes )
    {
        DWORD data = 0;
        hr = WriteToStream( strm, & data, aligned - *totalBytes, totalBytes );
    }
    
    return hr;
}


//  Get stream position
//
inline
HRESULT GetPos( IStream * strm, ULONG * pos )
{
    LARGE_INTEGER temp = { {0} };
    ULARGE_INTEGER ul_pos = { {0} };
    HRESULT hr = strm->Seek( temp, STREAM_SEEK_CUR, & ul_pos );
    * pos = ul_pos.u.LowPart;
    return hr;
}


class NullStream : public IStream
{
public:
    NullStream()
        : m_pos( 0 )
    {}
    
    ULONG STDMETHODCALLTYPE AddRef()
    {
        _ASSERTE( false );
        return 0;
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        _ASSERTE( false );
        return 0;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID, PVOID* )
    {
        _ASSERTE( false );
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead)
    {
        _ASSERTE( false );
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Write(const void  *pv, ULONG cb, ULONG *pcbWritten)
    {
        m_pos += cb;
        if ( pcbWritten )
            *pcbWritten = cb;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove,DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
    {
        if ( dwOrigin != STREAM_SEEK_CUR || dlibMove.QuadPart != 0 || plibNewPosition == NULL )
            return E_NOTIMPL;

        plibNewPosition->u.HighPart = 0;
        plibNewPosition->u.LowPart = m_pos;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize)
    {
        _ASSERTE( false );
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CopyTo(
        IStream     *pstm,
        ULARGE_INTEGER cb,
        ULARGE_INTEGER *pcbRead,
        ULARGE_INTEGER *pcbWritten)
    {
        _ASSERTE( false );
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Commit(
        DWORD       grfCommitFlags)
    {
        _ASSERTE( false );
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Revert()
    {
        _ASSERTE( false );
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE LockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD       dwLockType)
    {
        _ASSERTE( false );
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE UnlockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD       dwLockType)
    {
        _ASSERTE( false );
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Stat(
        STATSTG     *pstatstg,
        DWORD       grfStatFlag)
    {
        _ASSERTE( false );
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Clone(
        IStream     **ppstm)
    {
        _ASSERTE( false );
        return E_NOTIMPL;
    }

private:
    ULONG m_pos;
};

}

#endif

