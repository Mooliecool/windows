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
// NibbleStream reader and writer.

#ifndef _NIBBLESTREAM_H_
#define _NIBBLESTREAM_H_

#include "contract.h"
#include "ex.h"

typedef BYTE NIBBLE;

//-----------------------------------------------------------------------------
// Helpers for compression routines.
//-----------------------------------------------------------------------------
// This class allows variable-length compression of DWORDs.
//
// A value can be stored using one or more nibbles. 3 bits of a nibble are used 
// to store 3 bits of the value, and the top bit indicates if  the following nibble 
// contains rest of the value. If the top bit is not set, then this
// nibble is the last part of the value. 
// The higher bits of the value are written out first, and the lowest 3 bits
// are written out last.
//
// In the encoded stream of bytes, the lower nibble of a byte is used before 
// the high nibble.
//
// A binary value ABCDEFGHI (where A is the highest bit) is encoded as
// the follow two bytes : 1DEF1ABC XXXX0GHI
//
// Examples :
// 0            => X0
// 1            => X1
//
// 7            => X7
// 8            => 09
// 9            => 19
//
// 0x3F (63)    => 7F
// 0x40 (64)    => F9 X0
// 0x41 (65)    => F9 X1
//
// 0x1FF (511)  => FF X7
// 0x200 (512)  => 89 08
// 0x201 (513)  => 89 18

class NibbleWriter
{
public:
    // We pass in a stack allocated buffer that we know is large enough
    // (via a conservative estimate).
    // When we're done, we just need to get the final number of bytes we used.
    NibbleWriter(void * pStackBuffer, size_t size)
    {
        LEAF_CONTRACT;
        _ASSERTE(pStackBuffer != NULL);
        
        m_pBuffer = (BYTE*) pStackBuffer;
        m_cBytes = size;
        m_cNibble = 0;
    }

    size_t GetFinalByteCount()
    {
        LEAF_CONTRACT;
        return (m_cNibble / 2) + 1;
    }
    void * GetFinalBuffer()
    {
        LEAF_CONTRACT;
        return m_pBuffer;
    }

//.............................................................................
// Writer methods
//.............................................................................


    // Write a single nibble to the stream.
    void WriteNibble(NIBBLE i)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        _ASSERTE(i <= 0xF);

        // Bufer should have been allocated large enough to hold data.
        if (!(m_cNibble / 2 < m_cBytes))
        {
            _ASSERTE(false && !"Buffer too small");
            EX_THROW(HRException, (ERROR_INSUFFICIENT_BUFFER));
        }
        
        BYTE * p = &m_pBuffer[m_cNibble / 2];
        if ((m_cNibble & 1) == 0)
        {
            // Use the low nibble first
            *p = i;
        }
        else
        {
            // Use the high nibble after the high nibble is used
            *p |= (i << 4);
        }

        m_cNibble++;
    }

    static NIBBLE GetNibble(DWORD dw, int iNibble)
    {
        LEAF_CONTRACT;
        return (NIBBLE) ((dw >> (iNibble * 4)) & 0x7);
    }

    // Write a word (16 bits = 4 nibbles).
    void WriteU16(WORD i)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        WriteNibble(GetNibble(i, 0));
        WriteNibble(GetNibble(i, 1));
        WriteNibble(GetNibble(i, 2));
        WriteNibble(GetNibble(i, 3));
    }

    void WritePtr(void * p)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        ULONG_PTR u = (ULONG_PTR) p;
        for(size_t i = 0; i < sizeof(u) * 2; i++)
        {
            WriteNibble(GetNibble(u, (int) i));
        }
    }

    // Write an unsigned int via variable length nibble encoding.
    // We use the bit scheme:
    // 0ABC (if 0 <= dw <= 0x7)
    // 1ABC 0DEF (if 0 <= dw <= 0x7f)
    // 1ABC 1DEF 0GHI (if 0 <= dw <= 0x7FF)
    // etc..

    void WriteEncodedU32(DWORD dw)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        // Note we must write this out with the low terminating nibble (0ABC) last b/c the
        // reader gets nibbles in the same order we write them.
        int i = 0;
        while ((dw >> i) > 7)
        {
            i+= 3;
        }
        while(i > 0)
        {
            WriteNibble((NIBBLE) ((dw >> i) & 0x7) | 0x8);
            i -= 3;
        }
        WriteNibble((NIBBLE) dw & 0x7);        
    }

    // Write a signed 32 bit value.
    void WriteEncodedI32(int x)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        DWORD dw = (x < 0) ? (((-x) << 1) + 1) : (x << 1);
        WriteEncodedU32(dw);
    };



protected:
    BYTE * m_pBuffer;
    size_t m_cBytes; // size of buffer.
    size_t m_cNibble; // Which nibble are we at?
};

//-----------------------------------------------------------------------------

class NibbleReader
{
public:
    NibbleReader(void * pStackBuffer, size_t size)
    {
        LEAF_CONTRACT;
        _ASSERTE(pStackBuffer != NULL);
        
        m_pBuffer = (BYTE*) pStackBuffer;
        m_cBytes = size;
        m_cNibble = 0;
    }

    // Get the index of the next Byte.
    // This tells us how many bytes (rounding up to whole bytes) have been read.
    // This is can be used to extra raw byte data that may be embedded on a byte boundary in the nibble stream.
    size_t GetNextByteIndex()
    {
        LEAF_CONTRACT;
        return (m_cNibble / 2) + 1;
    }

    NIBBLE ReadNibble()
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        NIBBLE i = 0;
        // Bufer should have been allocated large enough to hold data.
        if (!(m_cNibble / 2 < m_cBytes))
        {
            // We should never get here in a normal retail scenario.
            // We could wind up here if somebody provided us invalid data (maybe by corrupting an ngenned image).
            EX_THROW(HRException, (E_INVALIDARG));
        }
        
        BYTE p = m_pBuffer[m_cNibble / 2];
        if ((m_cNibble & 1) == 0)
        {
            // Read the low nibble first
            i = (NIBBLE) (p & 0xF);
        }
        else
        {
            // Read the high nibble after the high nibble has been read
            i = (NIBBLE) (p >> 4) & 0xF;
        }
        m_cNibble++;

        return i;
    }

    // Read an unsigned int that was encoded via variable length nibble encoding
    // from NibbleWriter::WriteEncodedU32.    
    DWORD ReadEncodedU32()
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        DWORD dw =0;

        NIBBLE n;
        do
        {
            n = ReadNibble();
            dw = (dw << 3) + (n & 0x7);
        } while((n & 0x8) > 0);

        return dw;
    }

    int ReadEncodedI32()
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        DWORD dw = ReadEncodedU32();
        int x = dw >> 1;
        return (dw & 1) ? (-x) : (x);
    }

    void* ReadPtr()
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        ULONG_PTR u = 0;
        for(size_t i = 0; i < sizeof(u) * 2; i++)
        {
            NIBBLE n = ReadNibble();
            u = (u << 2) + n;
        }
        return (void*) u;
    }


protected:
    BYTE * m_pBuffer;
    size_t m_cBytes; // size of buffer.
    size_t m_cNibble; // Which nibble are we at?
};



#endif // _NIBBLESTREAM_H_
