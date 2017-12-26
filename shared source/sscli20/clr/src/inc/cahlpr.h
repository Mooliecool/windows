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
// File: CAHLPR.H
//
//
//*****************************************************************************
#ifndef __CAHLPR_H__
#define __CAHLPR_H__

//*****************************************************************************
// This class assists in the parsing of CustomAttribute blobs.
//*****************************************************************************
struct CaValue
{
    union
    {
        signed __int8       i1;
        unsigned __int8     u1;
        signed __int16      i2;
        unsigned __int16    u2;
        signed __int32      i4;
        unsigned __int32    u4;
        signed __int64      i8;
        unsigned __int64    u8;
        float               r4;
        double              r8;
        struct
        {
            LPCUTF8         pStr;
            ULONG           cbStr;
        } str;
    };
    unsigned __int8         tag;
};


class CustomAttributeParser {
public:
    CustomAttributeParser(              // Constructor for CustomAttributeParser.
        const void *pvBlob,             // Pointer to the CustomAttribute blob.
        ULONG   cbBlob)                 // Size of the CustomAttribute blob.
     :  m_pbCur(reinterpret_cast<const BYTE*>(pvBlob)),
        m_pbBlob(reinterpret_cast<const BYTE*>(pvBlob)),
        m_cbBlob(cbBlob)
    {
        LEAF_CONTRACT;
    }

    signed __int8    GetI1()
    {
        LEAF_CONTRACT;
        signed __int8 tmp = *reinterpret_cast<const signed __int8*>(m_pbCur);
        m_pbCur += sizeof(signed __int8);
        return tmp;
    }
    unsigned __int8  GetU1()
    {
        LEAF_CONTRACT;
        unsigned __int8 tmp = *reinterpret_cast<const unsigned __int8*>(m_pbCur);
        m_pbCur += sizeof(unsigned __int8);
        return tmp;
    }

    signed __int16   GetI2()
    {
        LEAF_CONTRACT;
        signed __int16 tmp = GET_UNALIGNED_VAL16(m_pbCur);
        m_pbCur += sizeof(signed __int16);
        return tmp;
    }
    unsigned __int16 GetU2()
    {
        LEAF_CONTRACT;
        unsigned __int16 tmp = GET_UNALIGNED_VAL16(m_pbCur);
        m_pbCur += sizeof(unsigned __int16 );
        return tmp;
    }

    signed __int32   GetI4()
    {
        LEAF_CONTRACT;
        signed __int32 tmp = GET_UNALIGNED_VAL32(m_pbCur);
        m_pbCur += sizeof(signed __int32 );
        return tmp;
    }
    unsigned __int32 GetU4()
    {
        LEAF_CONTRACT;
        unsigned __int32 tmp = GET_UNALIGNED_VAL32(m_pbCur);
        m_pbCur += sizeof(unsigned __int32 );
        return tmp;
    }

    signed __int64   GetI8()
    {
        LEAF_CONTRACT;
        signed __int64 tmp = GET_UNALIGNED_VAL64(m_pbCur);
        m_pbCur += sizeof(signed __int64 );
        return tmp;
    }
    unsigned __int64 GetU8()
    {
        LEAF_CONTRACT;
        unsigned __int64 tmp = GET_UNALIGNED_VAL64(m_pbCur);
        m_pbCur += sizeof(unsigned __int64 );
        return tmp;
    }

    float            GetR4()
    {
        LEAF_CONTRACT;
        __int32 tmp = GET_UNALIGNED_VAL32(m_pbCur);
        _ASSERTE(sizeof(__int32) == sizeof(float));
        m_pbCur += sizeof(float);
        return (float &)tmp;
    }
    double           GetR8()
    {
        LEAF_CONTRACT;
        __int64 tmp = GET_UNALIGNED_VAL64(m_pbCur);
        _ASSERTE(sizeof(__int64) == sizeof(double));
        m_pbCur += sizeof(double);
        return (double &)tmp;
    }


    HRESULT GetI1(signed __int8 *pVal)
    {
        WRAPPER_CONTRACT;

        if (BytesLeft() < (int) sizeof(signed __int8))
            return META_E_CA_INVALID_BLOB;
        *pVal = GetI1();
        return S_OK;
    }
    HRESULT GetU1(unsigned __int8 *pVal)
    {
        WRAPPER_CONTRACT;

        if (BytesLeft() < (int) sizeof(unsigned __int8))
            return META_E_CA_INVALID_BLOB;
        *pVal = GetU1();
        return S_OK;
    }

    HRESULT GetI2(signed __int16 *pVal)
    {
        WRAPPER_CONTRACT;

        if (BytesLeft() < (int) sizeof(signed __int16))
            return META_E_CA_INVALID_BLOB;
        *pVal = GetI2();
        return S_OK;
    }
    HRESULT GetU2(unsigned __int16 *pVal)
    {
        WRAPPER_CONTRACT;

        if (BytesLeft() < (int) sizeof(unsigned __int16))
            return META_E_CA_INVALID_BLOB;
        *pVal = GetU2();
        return S_OK;
    }

    HRESULT GetI4(signed __int32 *pVal)
    {
        WRAPPER_CONTRACT;

        if (BytesLeft() < (int) sizeof(signed __int32))
            return META_E_CA_INVALID_BLOB;
        *pVal = GetI4();
        return S_OK;
    }
    HRESULT GetU4(unsigned __int32 *pVal)
    {
        WRAPPER_CONTRACT;

        if (BytesLeft() < (int) sizeof(unsigned __int32))
            return META_E_CA_INVALID_BLOB;
        *pVal = GetU4();
        return S_OK;
    }

    HRESULT GetI8(signed __int64 *pVal)
    {
        WRAPPER_CONTRACT;

        if (BytesLeft() < (int) sizeof(signed __int64))
            return META_E_CA_INVALID_BLOB;
        *pVal = GetI8();
        return S_OK;
    }
    HRESULT GetU8(unsigned __int64 *pVal)
    {
        WRAPPER_CONTRACT;

        if (BytesLeft() < (int) sizeof(unsigned __int64))
            return META_E_CA_INVALID_BLOB;
        *pVal = GetU8();
        return S_OK;
    }

    HRESULT GetR4(float *pVal)
    {
        WRAPPER_CONTRACT;

        if (BytesLeft() < (int) sizeof(float))
            return META_E_CA_INVALID_BLOB;
        *pVal = GetR4();
        return S_OK;
    }
    HRESULT GetR8(double *pVal)
    {
        WRAPPER_CONTRACT;

        if (BytesLeft() < (int) sizeof(double))
            return META_E_CA_INVALID_BLOB;
        *pVal = GetR8();
        return S_OK;
    }


    short GetProlog()
    {
        WRAPPER_CONTRACT;

        m_pbCur = m_pbBlob;
        return GetI2();
    }

    int GetTagType()
    {
        WRAPPER_CONTRACT;

        return GetU1();
    }

    ULONG PeekStringLength()
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

        ULONG cb; 
        UnpackValue(m_pbCur, &cb);
        return cb;
    }

    LPCUTF8 GetString(ULONG *pcbString)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

        // Get the length, pointer to data following the length.
        const BYTE *pb = UnpackValue(m_pbCur, pcbString);
        m_pbCur = pb;
        // If null pointer is coded, no data follows length.
        if (*pcbString == (ULONG) -1)
            return (0);
        // Adjust current pointer for string data.
        m_pbCur += *pcbString;
        // Return pointer to string data.
        return (reinterpret_cast<LPCUTF8>(pb));
    }

    ULONG GetArraySize ()
    {
        WRAPPER_CONTRACT;

        ULONG cb;
        m_pbCur = UnpackValue(m_pbCur, &cb);
        return cb;
    }

    int BytesLeft()
    {
        LEAF_CONTRACT; 
        return (int)(m_cbBlob - (m_pbCur - m_pbBlob));
    }

private:
    const BYTE  *m_pbCur;
    const BYTE  *m_pbBlob;
    ULONG       m_cbBlob;

    const BYTE *UnpackValue(                // Uppack a coded integer.
        const BYTE  *pBytes,                // First byte of length.
        ULONG       *pcb)                   // Put the value here.
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_FORBID_FAULT;

        int iLeft = BytesLeft();
        if (iLeft < 1)
        {
            *pcb = (ULONG) -1;
            return 0;
        }
        if ((*pBytes & 0x80) == 0x00)       // 0??? ????
        {
            *pcb = (*pBytes & 0x7f);
            return pBytes + 1;
        }

        if ((*pBytes & 0xC0) == 0x80)       // 10?? ????
        {
            if (iLeft < 2)
            {
                *pcb = (ULONG) -1;
                return 0;
            }
            *pcb = ((*pBytes & 0x3f) << 8 | *(pBytes+1));
            return pBytes + 2;
        }

        if ((*pBytes & 0xE0) == 0xC0)       // 110? ????
        {
            if (iLeft < 4)
            {
                *pcb = (ULONG) -1;
                return 0;
            }
            *pcb = ((*pBytes & 0x1f) << 24 | *(pBytes+1) << 16 | *(pBytes+2) << 8 | *(pBytes+3));
            return pBytes + 4;
        }

        if (*pBytes == 0xff)                // Special value for "NULL pointer"
        {
            *pcb = (ULONG) -1;
            return pBytes + 1;
        }

        _ASSERTE(!"Unexpected packed value");
        *pcb = (ULONG) -1;
        return pBytes + 1;
    } // ULONG UnpackValue()
};

#endif // __CAHLPR_H__
