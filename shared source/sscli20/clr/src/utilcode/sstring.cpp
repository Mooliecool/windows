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
// ---------------------------------------------------------------------------
// SString.cpp
// ---------------------------------------------------------------------------

#include "stdafx.h"
#include "sstring.h"
#include "ex.h"
#include "holder.h"

#if defined(_MSC_VER)
#pragma inline_depth (25)
#endif

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

// Have one internal, well-known, literal for the empty string.
BYTE SString::s_EmptyBuffer[2] = { 0 };


UINT SString::s_ACP = 0;
BOOL SString::s_IsANSIMultibyte = TRUE;

#ifndef DACCESS_COMPILE
static BYTE s_EmptySpace[sizeof(SString)] = { 0 };
#endif // DACCESS_COMPILE

LCID SString::s_defaultLCID = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);

SPTR_IMPL(SString,SString,s_Empty);

void SString::Startup()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (s_ACP == 0)
    {
        CPINFO info;
        
        s_ACP = GetACP();
        if (GetCPInfo(s_ACP, &info) && info.MaxCharSize == 1)
            s_IsANSIMultibyte = FALSE;

#ifndef DACCESS_COMPILE
        s_Empty = PTR_SString(new (s_EmptySpace) SString());
        s_Empty->SetNormalized();
#endif // DACCESS_COMPILE
    }
}

CHECK SString::CheckStartup()
{
    WRAPPER_CONTRACT;

    CHECK(s_Empty != NULL);
    CHECK_OK;
}

//-----------------------------------------------------------------------------
// Case insensitive helpers.
//-----------------------------------------------------------------------------

static WCHAR MapChar(WCHAR wc, LCID lcid)
{
    WRAPPER_CONTRACT;

    WCHAR                     wTmp;

    wTmp = toupper(wc);

    return wTmp;
}

#define IS_UPPER_A_TO_Z(x) (((x) >= L'A') && ((x) <= L'Z'))
#define IS_LOWER_A_TO_Z(x) (((x) >= L'a') && ((x) <= L'z'))
#define CAN_SIMPLE_UPCASE(x) (((x)&~0x7f) == 0)
#define SIMPLE_UPCASE(x) (IS_LOWER_A_TO_Z(x) ? ((x) - L'a' + L'A') : (x))
#define SIMPLE_DOWNCASE(x) (IS_UPPER_A_TO_Z(x) ? ((x) - L'A' + L'a') : (x))

/* static */
int SString::CaseCompareHelper(const WCHAR *buffer1, const WCHAR *buffer2, COUNT_T count, LCID lcid, BOOL stopOnNull, BOOL stopOnCount)
{
    LEAF_CONTRACT;

    _ASSERTE(stopOnNull || stopOnCount);

    const WCHAR *buffer1End = buffer1 + count;
    int diff = 0;

    while (!stopOnCount || (buffer1 < buffer1End))
    {
        WCHAR ch1 = *buffer1++;
        WCHAR ch2 = *buffer2++;
        diff = ch1 - ch2;
        if ((ch1 == 0) || (ch2 == 0)) 
        {
            if  (diff != 0 || stopOnNull) 
            {
                break;
            }
        }
        else 
        {
            if (diff != 0)
            {
                diff = ((CAN_SIMPLE_UPCASE(ch1) ? SIMPLE_UPCASE(ch1) : MapChar(ch1, lcid))
                        - (CAN_SIMPLE_UPCASE(ch2) ? SIMPLE_UPCASE(ch2) : MapChar(ch2, lcid)));
            }
            if (diff != 0) 
            {
                break;
            }
        }
    }

    return diff;
}

#define IS_LOWER_A_TO_Z_ANSI(x) (((x) >= 'a') && ((x) <= 'z'))
#define CAN_SIMPLE_UPCASE_ANSI(x) (((x) >= 0x20) && ((x) <= 0x7f))
#define SIMPLE_UPCASE_ANSI(x) (IS_LOWER_A_TO_Z(x) ? ((x) - 'a' + 'A') : (x))


/* static */
int SString::CaseCompareHelperA(const CHAR *buffer1, const CHAR *buffer2, COUNT_T count, LCID lcid, BOOL stopOnNull, BOOL stopOnCount)
{
    LEAF_CONTRACT;
    
    _ASSERTE(stopOnNull || stopOnCount);

    const CHAR *buffer1End = buffer1 + count;
    int diff = 0;

    while (!stopOnCount || (buffer1 < buffer1End))
    {
        CHAR ch1 = *buffer1;
        CHAR ch2 = *buffer2;
        if ((ch1 == 0) || (ch2 == 0)) 
        {
            diff = ch1 - ch2;
            if  (diff != 0 || stopOnNull) 
            {
                break;
            }
            buffer1++;
            buffer2++;
        }
        else
        {
            diff = ch1 - ch2;
            if (diff != 0) 
            {
                diff = (SIMPLE_UPCASE_ANSI(ch1) - SIMPLE_UPCASE_ANSI(ch2));
                if (diff != 0)
                {
                    break;
                }
            }
            buffer1++;
            buffer2++;
        }
    }
    return diff;
}

static int CaseHashHelper(const WCHAR *buffer, COUNT_T count, LCID lcid)
{
    LEAF_CONTRACT;

    const WCHAR *bufferEnd = buffer + count;
    ULONG hash = 5381;

    while (buffer < bufferEnd)
    {
        WCHAR ch = *buffer++;
        ch = CAN_SIMPLE_UPCASE(ch) ? SIMPLE_UPCASE(ch) : MapChar(ch, lcid);

        hash = (((hash << 5) + hash) ^ ch);
    }

    return hash;
}

static int CaseHashHelperA(const CHAR *buffer, COUNT_T count)
{
    LEAF_CONTRACT;

    const CHAR *bufferEnd = buffer + count;
    ULONG hash = 5381;

    while (buffer < bufferEnd)
    {
        CHAR ch = *buffer++;
        ch = SIMPLE_UPCASE_ANSI(ch);

        hash = (((hash << 5) + hash) ^ ch);
    }

    return hash;
}

//-----------------------------------------------------------------------------
// Set this string to a copy of the unicode string
//-----------------------------------------------------------------------------
void SString::Set(const WCHAR *string)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        THROWS;
    }
    CONTRACT_END;

    if (string == NULL || *string == 0)
        Clear();
    else
    {
        Resize((COUNT_T) wcslen(string), REPRESENTATION_UNICODE);
        wcscpy_s(GetRawUnicode(), GetBufferSizeInCharIncludeNullChar(), string);
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to a copy of the first count characters of the given
// unicode string.
//-----------------------------------------------------------------------------
void SString::Set(const WCHAR *string, COUNT_T count)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        PRECONDITION(CheckCount(count));
        THROWS;
    }
    CONTRACT_END;

    if (count == 0)
        Clear();
    else
    {
        Resize(count, REPRESENTATION_UNICODE);
        wcsncpy_s(GetRawUnicode(), GetBufferSizeInCharIncludeNullChar(), string, count);
        GetRawUnicode()[count] = 0;
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to a copy of the given ansi string
//-----------------------------------------------------------------------------
void SString::SetASCII(const ASCII *string)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        PRECONDITION(CheckASCIIString(string));
        THROWS;
    }
    CONTRACT_END;

    if (string == NULL || *string == 0)
        Clear();
    else
    {
        Resize((COUNT_T) strlen(string), REPRESENTATION_ASCII);
        strcpy_s(GetRawUTF8(), GetBufferSizeInCharIncludeNullChar(), string);
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to a copy of the first count characters of the given
// ascii string
//-----------------------------------------------------------------------------
void SString::SetASCII(const ASCII *string, COUNT_T count)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        PRECONDITION(CheckASCIIString(string, count));
        PRECONDITION(CheckCount(count));
        THROWS;
    }
    CONTRACT_END;

    if (count == 0)
        Clear();
    else
    {
        Resize(count, REPRESENTATION_ASCII);
        strncpy_s(GetRawASCII(), GetBufferSizeInCharIncludeNullChar(), string, count);
        GetRawASCII()[count] = 0;
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to a copy of the given UTF8 string
//-----------------------------------------------------------------------------
void SString::SetUTF8(const UTF8 *string)
{
    CONTRACT_VOID
    {
        // !!! Check for illegal UTF8 encoding?
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        THROWS;
    }
    CONTRACT_END;

    if (string == NULL || *string == 0)
        Clear();
    else
    {
        Resize((COUNT_T) strlen(string), REPRESENTATION_UTF8);
        strcpy_s(GetRawUTF8(), GetBufferSizeInCharIncludeNullChar(), string);
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to a copy of the first count characters of the given
// UTF8 string.
//-----------------------------------------------------------------------------
void SString::SetUTF8(const UTF8 *string, COUNT_T count)
{
    CONTRACT_VOID
    {
        // !!! Check for illegal UTF8 encoding?
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        PRECONDITION(CheckCount(count));
        THROWS;
    }
    CONTRACT_END;

    if (count == 0)
        Clear();
    else
    {
        Resize(count, REPRESENTATION_UTF8);
        strncpy_s(GetRawUTF8(), GetBufferSizeInCharIncludeNullChar(), string, count);
        GetRawUTF8()[count] = 0;
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to a copy of the given ANSI string
//-----------------------------------------------------------------------------
void SString::SetANSI(const ANSI *string)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        THROWS;
    }
    CONTRACT_END;

    if (string == NULL || *string == 0)
        Clear();
    else
    {
        Resize((COUNT_T) strlen(string), REPRESENTATION_ANSI);
        strcpy_s(GetRawANSI(), GetBufferSizeInCharIncludeNullChar(), string);
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to a copy of the first count characters of the given
// ANSI string.
//-----------------------------------------------------------------------------
void SString::SetANSI(const ANSI *string, COUNT_T count)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        PRECONDITION(CheckCount(count));
        THROWS;
    }
    CONTRACT_END;

    if (count == 0)
        Clear();
    else
    {
        Resize(count, REPRESENTATION_ANSI);
        strncpy_s(GetRawANSI(), GetBufferSizeInCharIncludeNullChar(), string, count);
        GetRawANSI()[count] = 0;
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to the given unicode character
//-----------------------------------------------------------------------------
void SString::Set(WCHAR character)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        THROWS;
    }
    CONTRACT_END;

    if (character == 0)
        Clear();
    else
    {
        Resize(1, REPRESENTATION_UNICODE);
        GetRawUnicode()[0] = character;
        GetRawUnicode()[1] = 0;
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to the given UTF8 character
//-----------------------------------------------------------------------------
void SString::SetUTF8(CHAR character)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        THROWS;
    }
    CONTRACT_END;

    if (character == 0)
        Clear();
    else
    {
        Resize(1, REPRESENTATION_UTF8);
        GetRawUTF8()[0] = character;
        GetRawUTF8()[1] = 0;
    }

    RETURN;
}


//-----------------------------------------------------------------------------
// Set this string to the given ansi literal.
// This will share the memory and not make a copy.
//-----------------------------------------------------------------------------
void SString::SetLiteral(const ASCII *literal)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(literal));
        PRECONDITION(CheckASCIIString(literal));
        THROWS;
    }
    CONTRACT_END;

    Set(SString(Literal, literal));

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to the given unicode literal.
// This will share the memory and not make a copy.
//-----------------------------------------------------------------------------
void SString::SetLiteral(const WCHAR *literal)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(literal));
        THROWS;
    }
    CONTRACT_END;

    Set(SString(Literal, literal));

    RETURN;
}

//-----------------------------------------------------------------------------
// Hash the string contents
//-----------------------------------------------------------------------------
ULONG SString::Hash() const
{
    CONTRACT(ULONG)
    {
        INSTANCE_CHECK;
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    ConvertToUnicode();

    RETURN HashString(GetRawUnicode());
}

//-----------------------------------------------------------------------------
// Hash the string contents
//-----------------------------------------------------------------------------
ULONG SString::HashCaseInsensitive(LCID lcid) const
{
    CONTRACT(ULONG)
    {
        INSTANCE_CHECK;
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    ConvertToIteratable();

    ULONG result;

    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
    case REPRESENTATION_EMPTY:
        result = CaseHashHelper(GetRawUnicode(), GetRawCount(), lcid);
        break;

    case REPRESENTATION_ASCII:
        result = CaseHashHelperA(GetRawASCII(), GetRawCount());
        break;

    default:
        UNREACHABLE();
    }

    RETURN result;
}

//-----------------------------------------------------------------------------
// Truncate this string to count characters.
//-----------------------------------------------------------------------------
void SString::Truncate(const Iterator &i)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i));
        POSTCONDITION(GetRawCount() == i - Begin());
        THROWS;
    }
    CONTRACT_END;

    CONSISTENCY_CHECK(IsFixedSize());

    COUNT_T size = i - Begin();

    Resize(size, GetRepresentation(), PRESERVE);

    i.Resync(this, (BYTE *) (GetRawUnicode() + size));

    RETURN;
}

//-----------------------------------------------------------------------------
// Convert the ASCII representation for this String to Unicode. We can do this
// quickly and in-place (if this == &dest), which is why it is optimized.
//-----------------------------------------------------------------------------
void SString::ConvertASCIIToUnicode(SString &dest) const
{
    CONTRACT_VOID
    {
        PRECONDITION(IsRepresentation(REPRESENTATION_ASCII));
        POSTCONDITION(dest.IsRepresentation(REPRESENTATION_UNICODE));
        THROWS;
    }
    CONTRACT_END;

    // Handle the empty case.
    if (IsEmpty())
    {
        dest.Clear();
        RETURN;
    }

    CONSISTENCY_CHECK(CheckPointer(GetRawASCII()));
    CONSISTENCY_CHECK(GetRawCount() > 0);

    // If dest is the same as this, then we need to preserve on resize. 
    dest.Resize(GetRawCount(), REPRESENTATION_UNICODE,
                this == &dest ? PRESERVE : DONT_PRESERVE);

    // Make sure the buffer is big enough.
    CONSISTENCY_CHECK(dest.GetAllocation() > (GetRawCount() * sizeof(WCHAR)));

    // This is a poor man's widen. Since we know that the representation is ASCII,
    // we can just pad the string with a bunch of zero-value bytes. Of course,
    // we move from the end of the string to the start so that we can convert in
    // place (in the case that &dest == this).
    WCHAR *outBuf = dest.GetRawUnicode() + dest.GetRawCount();
    ASCII *inBuf = GetRawASCII() + GetRawCount();

    while (GetRawASCII() <= inBuf)
    {
        CONSISTENCY_CHECK(dest.GetRawUnicode() <= outBuf);
        // The casting zero-extends the value, thus giving us the zero-valued byte.
        *outBuf = (WCHAR) *inBuf;
        outBuf--;
        inBuf--;
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Convert the internal representation for this String to Unicode.
//-----------------------------------------------------------------------------
void SString::ConvertToUnicode() const
{
    CONTRACT_VOID
    {
        POSTCONDITION(IsRepresentation(REPRESENTATION_UNICODE));
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    if (!IsRepresentation(REPRESENTATION_UNICODE))
    {
        if (IsRepresentation(REPRESENTATION_ASCII))
        {
            ConvertASCIIToUnicode(*(const_cast<SString *>(this)));
        }
        else
        {
            StackSString s;
            ConvertToUnicode(s);
            PREFIX_ASSUME(!s.IsImmutable());
            (const_cast<SString*>(this))->Set(s);
        }
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Convert the internal representation for this String to Unicode, while
// preserving the iterator if the conversion is done.
//-----------------------------------------------------------------------------
void SString::ConvertToUnicode(const CIterator &i) const
{
    CONTRACT_VOID
    {
        PRECONDITION(i.Check());
        POSTCONDITION(IsRepresentation(REPRESENTATION_UNICODE));
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    if (!IsRepresentation(REPRESENTATION_UNICODE))
    {
        CONSISTENCY_CHECK(IsFixedSize());

        COUNT_T index = 0;
        // Get the current index of the iterator
        if (i.m_ptr != NULL)
        {
            CONSISTENCY_CHECK(GetCharacterSizeShift() == 0);
            index = (COUNT_T) (i.m_ptr - m_buffer);
        }

        if (IsRepresentation(REPRESENTATION_ASCII))
        {
            ConvertASCIIToUnicode(*(const_cast<SString *>(this)));
        }
        else
        {
            StackSString s;
            ConvertToUnicode(s);
            (const_cast<SString*>(this))->Set(s);
        }

        // Move the iterator to the new location.
        if (i.m_ptr != NULL)
        {
            i.Resync(this, (BYTE *) (GetRawUnicode() + index));
        }
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Set s to be a copy of this string's contents, but in the unicode format.
//-----------------------------------------------------------------------------
void SString::ConvertToUnicode(SString &s) const
{
    CONTRACT_VOID
    {
        PRECONDITION(s.Check());
        POSTCONDITION(s.IsRepresentation(REPRESENTATION_UNICODE));
        THROWS;
    }
    CONTRACT_END;

    int page = 0;

    switch (GetRepresentation())
    {
    case REPRESENTATION_EMPTY:
        s.Clear();
        RETURN;

    case REPRESENTATION_UNICODE:
        s.Set(*this);
        RETURN;

    case REPRESENTATION_UTF8:
        page = CP_UTF8;
        break;

    case REPRESENTATION_ASCII:
        ConvertASCIIToUnicode(s);
        RETURN;

    case REPRESENTATION_ANSI:
        page = CP_ACP;
        break;

    default:
        UNREACHABLE();
    }

    COUNT_T length = WszMultiByteToWideChar(page, 0, GetRawANSI(), GetRawCount()+1, 0, 0);
    if (length == 0)
        ThrowLastError();

    s.Resize(length-1, REPRESENTATION_UNICODE);

    length = WszMultiByteToWideChar(page, 0, GetRawANSI(), GetRawCount()+1, s.GetRawUnicode(), length);
    if (length == 0)
        ThrowLastError();

    RETURN;
}

//-----------------------------------------------------------------------------
// Set s to be a copy of this string's contents, but in the ANSI format.
//-----------------------------------------------------------------------------
void SString::ConvertToANSI(SString &s) const
{
    CONTRACT_VOID
    {
        PRECONDITION(s.Check());
        POSTCONDITION(s.IsRepresentation(REPRESENTATION_ANSI));
        THROWS;
    }
    CONTRACT_END;

    switch (GetRepresentation())
    {
    case REPRESENTATION_EMPTY:
        s.Clear();
        RETURN;

    case REPRESENTATION_ASCII:
    case REPRESENTATION_ANSI:
        s.Set(*this);
        RETURN;

    case REPRESENTATION_UTF8:
        // No direct conversion to ANSI
        ConvertToUnicode();
        // fall through

    case REPRESENTATION_UNICODE:
        break;

    default:
        UNREACHABLE();
    }

    COUNT_T length = WszWideCharToMultiByte(CP_ACP, 0, GetRawUnicode(), GetRawCount()+1,
                                        NULL, 0, NULL, NULL);

    s.Resize(length-1, REPRESENTATION_ANSI);

    length = WszWideCharToMultiByte(CP_ACP, 0, GetRawUnicode(), GetRawCount()+1,
                                    s.GetRawANSI(), length, NULL, NULL);
    if (length == 0)
        ThrowLastError();

    RETURN;
}

//-----------------------------------------------------------------------------
// Set s to be a copy of this string's contents, but in the utf8 format.
//-----------------------------------------------------------------------------
void SString::ConvertToUTF8(SString &s) const
{
    CONTRACT_VOID
    {
        PRECONDITION(s.Check());
        POSTCONDITION(s.IsRepresentation(REPRESENTATION_UTF8));
        THROWS;
    }
    CONTRACT_END;

    switch (GetRepresentation())
    {
    case REPRESENTATION_EMPTY:
        s.Clear();
        RETURN;

    case REPRESENTATION_ASCII:
    case REPRESENTATION_UTF8:
        s.Set(*this);
        RETURN;

    case REPRESENTATION_ANSI:
        // No direct conversion from ANSI to UTF8
        ConvertToUnicode();
        // fall through

    case REPRESENTATION_UNICODE:
        break;

    default:
        UNREACHABLE();
    }

    COUNT_T length = WszWideCharToMultiByte(CP_UTF8, 0, GetRawUnicode(), GetRawCount()+1,
                                            NULL, 0, NULL, NULL);

    s.Resize(length-1, REPRESENTATION_UTF8);

    length = WszWideCharToMultiByte(CP_UTF8, 0, GetRawUnicode(), GetRawCount()+1,
                                    s.GetRawUTF8(), length, NULL, NULL);
    if (length == 0)
        ThrowLastError();

    RETURN;
}

//-----------------------------------------------------------------------------
// Replace a single character with another character.
//-----------------------------------------------------------------------------
void SString::Replace(const Iterator &i, WCHAR c)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i, 1));
        POSTCONDITION(Match(i, c));
        THROWS;
    }
    CONTRACT_END;

    if (IsRepresentation(REPRESENTATION_ASCII) && ((c&~0x7f) == 0))
    {
        *(BYTE*)i.m_ptr = (BYTE) c;
    }
    else
    {
        ConvertToUnicode(i);

        *(USHORT*)i.m_ptr = c;
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Replace the substring specified by position, length with the given string s.
//-----------------------------------------------------------------------------
void SString::Replace(const Iterator &i, COUNT_T length, const SString &s)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i, length));
        PRECONDITION(s.Check());
        POSTCONDITION(Match(i, s));
        THROWS;
    }
    CONTRACT_END;

    Representation representation = GetRepresentation();
    if (representation == REPRESENTATION_EMPTY)
    {
        // This special case contains some optimizations (like literal sharing).
        Set(s);
        ConvertToIteratable();
        i.Resync(this, m_buffer);
    }
    else
    {
        StackSString temp;
        const SString &source = GetCompatibleString(s, temp, i);

        COUNT_T deleteSize = length<<GetCharacterSizeShift();
        COUNT_T insertSize = source.GetRawCount()<<source.GetCharacterSizeShift();

        SBuffer::Replace(i, deleteSize, insertSize);
        SBuffer::Copy(i, source.m_buffer, insertSize);
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// Find s in this string starting at i. Return TRUE & update iterator if found.
//-----------------------------------------------------------------------------
BOOL SString::Find(CIterator &i, const SString &s) const
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(s.Check());
        POSTCONDITION(RETVAL == Match(i, s));
        THROWS_UNLESS_BOTH_NORMALIZED(s);
    }
    CONTRACT_END;

    // Get a compatible string from s
    StackSString temp;
    const SString &source = GetCompatibleString(s, temp, i);

    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
        {
            COUNT_T count = source.GetRawCount();
            const WCHAR *start = i.GetUnicode();
            const WCHAR *end = GetUnicode() + GetRawCount() - count;
            while (start <= end)
            {
                if (wcsncmp(start, source.GetRawUnicode(), count) == 0)
                {
                    i.Resync(this, (BYTE*) start);
                    RETURN TRUE;
                }
                start++;
            }
        }
        break;

    case REPRESENTATION_ANSI:
    case REPRESENTATION_ASCII:
        {
            COUNT_T count = source.GetRawCount();
            const CHAR *start = i.GetASCII();
            const CHAR *end = GetRawASCII() + GetRawCount() - count;
            while (start <= end)
            {
                if (strncmp(start, source.GetRawASCII(), count) == 0)
                {
                    i.Resync(this, (BYTE*) start);
                    RETURN TRUE;
                }
                start++;
            }
        }
        break;

    case REPRESENTATION_EMPTY:
        {
            if (source.GetRawCount() == 0)
                RETURN TRUE;
        }
        break;

    case REPRESENTATION_UTF8:
    default:
        UNREACHABLE();
    }

    RETURN FALSE;
}

//-----------------------------------------------------------------------------
// Find s in this string starting at i. Return TRUE & update iterator if found.
//-----------------------------------------------------------------------------
BOOL SString::Find(CIterator &i, WCHAR c) const
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i));
        POSTCONDITION(RETVAL == Match(i, c));
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    // Get a compatible string
    if (c & ~0x7f)
        ConvertToUnicode(i);

    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
        {
            const WCHAR *start = i.GetUnicode();
            const WCHAR *end = GetUnicode() + GetRawCount() - 1;
            while (start <= end)
            {
                if (*start == c)
                {
                    i.Resync(this, (BYTE*) start);
                    RETURN TRUE;
                }
                start++;
            }
        }
        break;

    case REPRESENTATION_ANSI:
    case REPRESENTATION_ASCII:
        {
            const CHAR *start = i.GetASCII();
            const CHAR *end = GetRawASCII() + GetRawCount() - 1;
            while (start <= end)
            {
                if (*start == c)
                {
                    i.Resync(this, (BYTE*) start);
                    RETURN TRUE;
                }
                start++;
            }
        }
        break;

    case REPRESENTATION_EMPTY:
        break;

    case REPRESENTATION_UTF8:
    default:
        UNREACHABLE();
    }

    RETURN FALSE;
}

//-----------------------------------------------------------------------------
// Find s in this string, working backwards staring at i.
// Return TRUE and update iterator if found.
//-----------------------------------------------------------------------------
BOOL SString::FindBack(CIterator &i, const SString &s) const
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(s.Check());
        POSTCONDITION(RETVAL == Match(i, s));
        THROWS_UNLESS_BOTH_NORMALIZED(s);
    }
    CONTRACT_END;

    // Get a compatible string from s
    StackSString temp;
    const SString &source = GetCompatibleString(s, temp, i);

    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
        {
            COUNT_T count = source.GetRawCount();
            const WCHAR *start = GetRawUnicode() + GetRawCount() - count;
            if (start > i.GetUnicode())
                start = i.GetUnicode();
            const WCHAR *end = GetRawUnicode();

            while (start >= end)
            {
                if (wcsncmp(start, source.GetRawUnicode(), count) == 0)
                {
                    i.Resync(this, (BYTE*) start);
                    RETURN TRUE;
                }
                start--;
            }
        }
        break;

    case REPRESENTATION_ANSI:
    case REPRESENTATION_ASCII:
        {
            COUNT_T count = source.GetRawCount();
            const CHAR *start = GetRawASCII() + GetRawCount() - count;
            if (start > i.GetASCII())
                start = i.GetASCII();
            const CHAR *end = GetRawASCII();

            while (start >= end)
            {
                if (strncmp(start, source.GetRawASCII(), count) == 0)
                {
                    i.Resync(this, (BYTE*) start);
                    RETURN TRUE;
                }
                start--;
            }
        }
        break;

    case REPRESENTATION_EMPTY:
        {
            if (source.GetRawCount() == 0)
                RETURN TRUE;
        }
        break;

    case REPRESENTATION_UTF8:
    default:
        UNREACHABLE();
    }

    RETURN FALSE;
}

//-----------------------------------------------------------------------------
// Find s in this string, working backwards staring at i.
// Return TRUE and update iterator if found.
//-----------------------------------------------------------------------------
BOOL SString::FindBack(CIterator &i, WCHAR c) const
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i));
        POSTCONDITION(RETVAL == Match(i, c));
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    // Get a compatible string from s
    if (c & ~0x7f)
        ConvertToUnicode(i);

    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
        {
            const WCHAR *start = GetRawUnicode() + GetRawCount() - 1;
            if (start > i.GetUnicode())
                start = i.GetUnicode();
            const WCHAR *end = GetRawUnicode();

            while (start >= end)
            {
                if (*start == c)
                {
                    i.Resync(this, (BYTE*) start);
                    RETURN TRUE;
                }
                start--;
            }
        }
        break;

    case REPRESENTATION_ANSI:
    case REPRESENTATION_ASCII:
        {
            const CHAR *start = GetRawASCII() + GetRawCount() - 1;
            if (start > i.GetASCII())
                start = i.GetASCII();
            const CHAR *end = GetRawASCII();

            while (start >= end)
            {
                if (*start == c)
                {
                    i.Resync(this, (BYTE*) start);
                    RETURN TRUE;
                }
                start--;
            }
        }
        break;

    case REPRESENTATION_EMPTY:
        break;

    case REPRESENTATION_UTF8:
    default:
        UNREACHABLE();
    }

    RETURN FALSE;
}

//-----------------------------------------------------------------------------
// Returns TRUE if this string begins with the contents of s
//-----------------------------------------------------------------------------
BOOL SString::BeginsWith(const SString &s) const
{
    WRAPPER_CONTRACT;

    return Match(Begin(), s);
}

//-----------------------------------------------------------------------------
// Returns TRUE if this string ends with the contents of s
//-----------------------------------------------------------------------------
BOOL SString::EndsWith(const SString &s) const
{
    WRAPPER_CONTRACT;

    // Need this check due to iterator arithmetic below.
    if (GetCount() < s.GetCount())
    {
        return FALSE;
    }

    return Match(End() - s.GetCount(), s);
}

//-----------------------------------------------------------------------------
// Compare this string's contents to s's contents.
// The comparison does not take into account localization issues like case folding.
// Return 0 if equal, <0 if this < s, >0 is this > s. (same as strcmp).
//-----------------------------------------------------------------------------
int SString::Compare(const SString &s) const
{
    CONTRACT(int)
    {
        INSTANCE_CHECK;
        PRECONDITION(s.Check());
        THROWS_UNLESS_BOTH_NORMALIZED(s);
    }
    CONTRACT_END;

    StackSString temp;
    const SString &source = GetCompatibleString(s, temp);

    COUNT_T smaller;
    int equals = 0;
    int result = 0;

    if (GetRawCount() < source.GetRawCount())
    {
        smaller = GetRawCount();
        equals = -1;
    }
    else if (GetRawCount() > source.GetRawCount())
    {
        smaller = source.GetRawCount();
        equals = 1;
    }
    else
    {
        smaller = GetRawCount();
        equals = 0;
    }

    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
        result = wcsncmp(GetRawUnicode(), source.GetRawUnicode(), smaller);
        break;

    case REPRESENTATION_ASCII:
    case REPRESENTATION_ANSI:
        result = strncmp(GetRawASCII(), source.GetRawASCII(), smaller);
        break;

    case REPRESENTATION_EMPTY:
        result = 0;
        break;

    default:
    case REPRESENTATION_UTF8:
        UNREACHABLE();
    }

    if (result == 0)
        RETURN equals;
    else
        RETURN result;
}

//-----------------------------------------------------------------------------
// Compare this string's contents to s's contents.
// Return 0 if equal, <0 if this < s, >0 is this > s. (same as strcmp).
//-----------------------------------------------------------------------------

int SString::CompareCaseInsensitive(const SString &s, LCID lcid) const
{
    CONTRACT(int)
    {
        INSTANCE_CHECK;
        PRECONDITION(s.Check());
        THROWS_UNLESS_BOTH_NORMALIZED(s);
    }
    CONTRACT_END;

    StackSString temp;
    const SString &source = GetCompatibleString(s, temp);

    COUNT_T smaller;
    int equals = 0;
    int result = 0;

    if (GetRawCount() < source.GetRawCount())
    {
        smaller = GetRawCount();
        equals = -1;
    }
    else if (GetRawCount() > source.GetRawCount())
    {
        smaller = source.GetRawCount();
        equals = 1;
    }
    else
    {
        smaller = GetRawCount();
        equals = 0;
    }

    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
        result = CaseCompareHelper(GetRawUnicode(), source.GetRawUnicode(), smaller, lcid, FALSE, TRUE);
        break;

    case REPRESENTATION_ASCII:
    case REPRESENTATION_ANSI:
        result = CaseCompareHelperA(GetRawASCII(), source.GetRawASCII(), smaller, lcid, FALSE, TRUE);
        break;

    case REPRESENTATION_EMPTY:
        result = 0;
        break;

    default:
    case REPRESENTATION_UTF8:
        UNREACHABLE();
    }

    if (result == 0)
        RETURN equals;
    else
        RETURN result;
}

//-----------------------------------------------------------------------------
// Compare this string's contents to s's contents.
// The comparison does not take into account localization issues like case folding.
// Return 1 if equal, 0 if not.
//-----------------------------------------------------------------------------
BOOL SString::Equals(const SString &s) const
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        PRECONDITION(s.Check());
        THROWS_UNLESS_BOTH_NORMALIZED(s);
        FAULTS_UNLESS_BOTH_NORMALIZED(s, ThrowOutOfMemory());
    }
    CONTRACT_END;

    StackSString temp;
    const SString &source = GetCompatibleString(s, temp);

    COUNT_T count = GetRawCount();

    if (count != source.GetRawCount())
        RETURN FALSE;

    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
        RETURN (wcsncmp(GetRawUnicode(), source.GetRawUnicode(), count) == 0);

    case REPRESENTATION_ASCII:
    case REPRESENTATION_ANSI:
        RETURN (strncmp(GetRawASCII(), source.GetRawASCII(), count) == 0);

    case REPRESENTATION_EMPTY:
        RETURN TRUE;

    default:
    case REPRESENTATION_UTF8:
        UNREACHABLE();
    }

    RETURN FALSE;
}

//-----------------------------------------------------------------------------
// Compare this string's contents case insensitively to s's contents.
// Return 1 if equal, 0 if not.
//-----------------------------------------------------------------------------
BOOL SString::EqualsCaseInsensitive(const SString &s, LCID lcid) const
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        PRECONDITION(s.Check());
        THROWS_UNLESS_BOTH_NORMALIZED(s);
        FAULTS_UNLESS_BOTH_NORMALIZED(s, ThrowOutOfMemory());
    }
    CONTRACT_END;

    StackSString temp;
    const SString &source = GetCompatibleString(s, temp);

    COUNT_T count = GetRawCount();

    if (count != source.GetRawCount())
        RETURN FALSE;

    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
        RETURN (CaseCompareHelper(GetRawUnicode(), source.GetRawUnicode(), count, lcid, FALSE, TRUE) == 0);

    case REPRESENTATION_ASCII:
    case REPRESENTATION_ANSI:
        RETURN (CaseCompareHelperA(GetRawASCII(), source.GetRawASCII(), count, lcid, FALSE, TRUE) == 0);

    case REPRESENTATION_EMPTY:
        RETURN TRUE;

    default:
    case REPRESENTATION_UTF8:
        UNREACHABLE();
    }

    RETURN FALSE;
}

//-----------------------------------------------------------------------------
// Compare s's contents to the substring starting at position
// The comparison does not take into account localization issues like case folding.
// Return TRUE if equal, FALSE if not
//-----------------------------------------------------------------------------
BOOL SString::Match(const CIterator &i, const SString &s) const
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(s.Check());
        THROWS_UNLESS_BOTH_NORMALIZED(s);
    }
    CONTRACT_END;

    StackSString temp;
    const SString &source = GetCompatibleString(s, temp, i);

    COUNT_T remaining = End() - i;
    COUNT_T count = source.GetRawCount();

    if (remaining < count)
        RETURN FALSE;

    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
        RETURN (wcsncmp(i.GetUnicode(), source.GetRawUnicode(), count) == 0);

    case REPRESENTATION_ASCII:
    case REPRESENTATION_ANSI:
        RETURN (strncmp(i.GetASCII(), source.GetRawASCII(), count) == 0);

    case REPRESENTATION_EMPTY:
        RETURN TRUE;

    default:
    case REPRESENTATION_UTF8:
        UNREACHABLE();
    }

    RETURN FALSE;
}

//-----------------------------------------------------------------------------
// Compare s's contents case insensitively to the substring starting at position
// Return TRUE if equal, FALSE if not
//-----------------------------------------------------------------------------
BOOL SString::MatchCaseInsensitive(const CIterator &i, const SString &s, LCID lcid) const
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(s.Check());
        THROWS_UNLESS_BOTH_NORMALIZED(s);
    }
    CONTRACT_END;

    StackSString temp;
    const SString &source = GetCompatibleString(s, temp, i);

    COUNT_T remaining = End() - i;
    COUNT_T count = source.GetRawCount();

    if (remaining < count)
        RETURN FALSE;

    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
    case REPRESENTATION_ANSI:
        RETURN (CaseCompareHelper(i.GetUnicode(), source.GetRawUnicode(), count, lcid, FALSE, TRUE) == 0);

    case REPRESENTATION_ASCII:
        RETURN (CaseCompareHelperA(i.GetASCII(), source.GetRawASCII(), count, lcid, FALSE, TRUE) == 0);

    case REPRESENTATION_EMPTY:
        RETURN TRUE;

    default:
    case REPRESENTATION_UTF8:
        UNREACHABLE();
    }

    RETURN FALSE;
}

//-----------------------------------------------------------------------------
// Compare c case insensitively to the character at position
// Return TRUE if equal, FALSE if not
//-----------------------------------------------------------------------------
BOOL SString::MatchCaseInsensitive(const CIterator &i, WCHAR c, LCID lcid) const
{
    CONTRACT(BOOL)
    {
        GC_NOTRIGGER;
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i));
        NOTHROW;
    }
    CONTRACT_END;

    // End() will not throw here
    CONTRACT_VIOLATION(ThrowsViolation);
    if (i >= End())
        RETURN FALSE;

    WCHAR test = i[0];

    RETURN (test == c
            || ((CAN_SIMPLE_UPCASE(test) ? SIMPLE_UPCASE(test) : MapChar(test, lcid))
                == (CAN_SIMPLE_UPCASE(c) ? SIMPLE_UPCASE(c) : MapChar(c, lcid))));
}

//-----------------------------------------------------------------------------
// Get a const pointer to the internal buffer as an ANSI string.
//-----------------------------------------------------------------------------
const CHAR *SString::GetANSI(AbstractScratchBuffer &scratch) const
{
    CONTRACT(const CHAR *)
    {
        INSTANCE_CHECK_NULL;
        THROWS;
    }
    CONTRACT_END;

    if (this == NULL)
        RETURN NULL;

    if (IsRepresentation(REPRESENTATION_ANSI))
        RETURN GetRawANSI();

    ConvertToANSI(scratch);
    RETURN ((SString&)scratch).GetRawANSI();
}

//-----------------------------------------------------------------------------
// Get a const pointer to the internal buffer as a UTF8 string.
//-----------------------------------------------------------------------------
const UTF8 *SString::GetUTF8(AbstractScratchBuffer &scratch) const
{
    CONTRACT(const UTF8 *)
    {
        INSTANCE_CHECK_NULL;
        THROWS;
    }
    CONTRACT_END;

    if (this == NULL)
        RETURN NULL;

    if (IsRepresentation(REPRESENTATION_UTF8))
        RETURN GetRawUTF8();

    ConvertToUTF8(scratch);
    RETURN ((SString&)scratch).GetRawUTF8();
}

//-----------------------------------------------------------------------------
// Safe version of sprintf.
// Prints formatted ansi text w/ var args to this buffer.
//-----------------------------------------------------------------------------
void SString::Printf(const CHAR *format, ...)
{
    WRAPPER_CONTRACT;

    va_list args;
    va_start(args, format);
    VPrintf(format, args);
    va_end(args);
}

#ifdef _DEBUG
//
static void CheckForFormatStringGlobalizationIssues(const SString &format, const SString &result)
{
    CONTRACTL
    {
        THROWS;
    }
    CONTRACTL_END;

    BOOL fDangerousFormat = FALSE;

    // Check whether the format string contains the %S formatting specifier
    SString::CIterator itrFormat = format.Begin();
    while (*itrFormat)
    {
        if (*itrFormat++ == '%')
        {
            if (*itrFormat++ == 'S')
            {
                fDangerousFormat = TRUE;
                break;
            }
        }
    }

    if (fDangerousFormat)
    {
        BOOL fNonAsciiUsed = FALSE;

        // Now check whether there are any non-ASCII characters in the output.

        // Check whether the result contains non-Ascii characters
        SString::CIterator itrResult = format.Begin();
        while (*itrResult)
        {
            if (*itrResult++ > 127)
            {
                fNonAsciiUsed = TRUE;
                break;
            }
        }

        CONSISTENCY_CHECK_MSGF(!fNonAsciiUsed,
            ("Non-ASCII string was produced by %%S format specifier. This is likely globalization bug."
            "To fix this, change the format string to %%s and do the correct encoding at the Printf callsite"));
    }
}
#endif

#ifndef EBADF
#define EBADF 9
#endif

#ifndef ENOMEM
#define ENOMEM 12
#endif

#ifndef ERANGE
#define ERANGE 34
#endif

void SString::VPrintf(const CHAR *format, va_list args)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(format));
        THROWS;
    }
    CONTRACT_END;

    // sprintf gives us no means to know how many characters are written
    // other than guessing and trying

    if (GetRawCount() > 0)
    {
        // First, try to use the existing buffer
        int result = _vsnprintf_s(GetRawANSI(), GetRawCount()+1, _TRUNCATE, format, args);

        if (result >=0)
        {
            // Succeeded in writing. Now resize - 
            Resize(result, REPRESENTATION_ANSI, PRESERVE);
            INDEBUG(CheckForFormatStringGlobalizationIssues(SString(Ansi, format), *this));
            RETURN;
        }
    }

    // Make a guess how long the result will be (note this will be doubled)

    COUNT_T guess = (COUNT_T) strlen(format)+1;
    if (guess < GetRawCount())
        guess = GetRawCount();
    if (guess < MINIMUM_GUESS)
        guess = MINIMUM_GUESS;

    while (TRUE)
    {
        // Double the previous guess - eventually we will get enough space
        guess *= 2;
        Resize(guess, REPRESENTATION_ANSI);

        // Clear errno to avoid false alarms
        errno = 0;

        int result = _vsnprintf_s(GetRawANSI(), GetRawCount()+1, _TRUNCATE, format, args);

        if (result >= 0)
        {
            // Succeed in writing. Shrink the buffer to fit exactly.
            Resize(result, REPRESENTATION_ANSI, PRESERVE);
            INDEBUG(CheckForFormatStringGlobalizationIssues(SString(Ansi, format), *this));
            RETURN;
        }

        if (errno==ENOMEM)
        {
            ThrowOutOfMemory();
        }
        else
        if (errno!=0 && errno!=EBADF && errno!=ERANGE)
        {
            CONSISTENCY_CHECK_MSG(FALSE, "_vsnprintf failed. Potential globalization bug.");
            ThrowHR(HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION));
        }
    }
    RETURN;
}

void SString::Printf(const WCHAR *format, ...)
{
    WRAPPER_CONTRACT;

    va_list args;
    va_start(args, format);
    VPrintf(format, args);
    va_end(args);
}

void SString::PPrintf(const WCHAR *format, ...)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(format));
        THROWS;
    }
    CONTRACT_END;

    va_list argItr;
    va_start(argItr, format);
    PVPrintf(format, argItr);
    va_end(argItr);    

    RETURN;
}

void SString::VPrintf(const WCHAR *format, va_list args)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(format));
        THROWS;
    }
    CONTRACT_END;

    // sprintf gives us no means to know how many characters are written
    // other than guessing and trying

    if (GetRawCount() > 0)
    {
        // First, try to use the existing buffer
        int result = _vsnwprintf_s(GetRawUnicode(), GetRawCount()+1, _TRUNCATE, format, args);

        if (result >= 0)
        {
            // succeeded
            Resize(result, REPRESENTATION_UNICODE, PRESERVE);
            INDEBUG(CheckForFormatStringGlobalizationIssues(SString(format), *this));
            RETURN;
        }
    }

    // Make a guess how long the result will be (note this will be doubled)

    COUNT_T guess = (COUNT_T) wcslen(format)+1;
    if (guess < GetRawCount())
        guess = GetRawCount();
    if (guess < MINIMUM_GUESS)
        guess = MINIMUM_GUESS;

    while (TRUE)
    {
        // Double the previous guess - eventually we will get enough space
        guess *= 2;
        Resize(guess, REPRESENTATION_UNICODE);

        // Clear errno to avoid false alarms
        errno = 0;

        int result = _vsnwprintf_s(GetRawUnicode(), GetRawCount()+1, _TRUNCATE, format, args);

        if (result >= 0)
        {
            Resize(result, REPRESENTATION_UNICODE, PRESERVE);
            INDEBUG(CheckForFormatStringGlobalizationIssues(SString(format), *this));
            RETURN;
        }

        if (errno==ENOMEM)
        {
            ThrowOutOfMemory();
        }
        else
        if (errno!=0 && errno!=EBADF && errno!=ERANGE)
        {
            CONSISTENCY_CHECK_MSG(FALSE, "_vsnwprintf failed. Potential globalization bug.");
            ThrowHR(HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION));
        }
    }
    RETURN;
}

void SString::PVPrintf(const WCHAR *format, va_list args)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(format));
        THROWS;
    }
    CONTRACT_END;

    // sprintf gives us no means to know how many characters are written
    // other than guessing and trying

    if (GetRawCount() > 0)
    {
        // First, try to use the existing buffer
        int result = _vsnwprintf_s(GetRawUnicode(), GetRawCount()+1, _TRUNCATE, format, args);
        if (result >= 0)
        {
            // succeeded
            Resize(result, REPRESENTATION_UNICODE, PRESERVE);
            INDEBUG(CheckForFormatStringGlobalizationIssues(SString(format), *this));
            RETURN;
        }
    }

    // Make a guess how long the result will be (note this will be doubled)

    COUNT_T guess = (COUNT_T) wcslen(format)+1;
    if (guess < GetRawCount())
        guess = GetRawCount();
    if (guess < MINIMUM_GUESS)
        guess = MINIMUM_GUESS;

    while (TRUE)
    {
        // Double the previous guess - eventually we will get enough space
        guess *= 2;
        Resize(guess, REPRESENTATION_UNICODE, DONT_PRESERVE);

        // Clear errno to avoid false alarms
        errno = 0;

        int result = _vsnwprintf_s(GetRawUnicode(), GetRawCount()+1, _TRUNCATE, format, args);

        if (result >= 0)
        {
            Resize(result, REPRESENTATION_UNICODE, PRESERVE);
            INDEBUG(CheckForFormatStringGlobalizationIssues(SString(format), *this));
            RETURN;
        }

        if (errno==ENOMEM)
        {
            ThrowOutOfMemory();
        }
        else
        if (errno!=0 && errno!=EBADF && errno!=ERANGE)
        {
            CONSISTENCY_CHECK_MSG(FALSE, "_vsnwprintf failed. Potential globalization bug.");
            ThrowHR(HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION));
        }
    }
    RETURN;
}

void SString::AppendPrintf(const CHAR *format, ...)
{
    WRAPPER_CONTRACT;

    va_list args;
    va_start(args, format);
    AppendVPrintf(format, args);
    va_end(args);
}

void SString::AppendVPrintf(const CHAR *format, va_list args)
{
    WRAPPER_CONTRACT;

    StackSString s;
    s.VPrintf(format, args);
    Append(s);
}

void SString::AppendPrintf(const WCHAR *format, ...)
{
    WRAPPER_CONTRACT;

    va_list args;
    va_start(args, format);
    AppendVPrintf(format, args);
    va_end(args);
}

void SString::AppendVPrintf(const WCHAR *format, va_list args)
{
    WRAPPER_CONTRACT;

    StackSString s;
    s.VPrintf(format, args);
    Append(s);
}

//----------------------------------------------------------------------------
// LoadResource - moved to sstring_com.cpp
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Format the message and put the contents in this string
//----------------------------------------------------------------------------

BOOL SString::FormatMessage(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId,
                            const SString &arg1, const SString &arg2,
                            const SString &arg3, const SString &arg4,
                            const SString &arg5, const SString &arg6,
                            const SString &arg7, const SString &arg8,
                            const SString &arg9, const SString &arg10)
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        THROWS;
    }
    CONTRACT_END;

    const WCHAR *args[] = {arg1.GetUnicode(), arg2.GetUnicode(), arg3.GetUnicode(), arg4.GetUnicode(),
                           arg5.GetUnicode(), arg6.GetUnicode(), arg7.GetUnicode(), arg8.GetUnicode(),
                           arg9.GetUnicode(), arg10.GetUnicode()};

    if (GetRawCount() > 0)
    {
        // First, try to use our existing buffer to hold the result.
        Resize(GetRawCount(), REPRESENTATION_UNICODE);

        DWORD result = ::WszFormatMessage(dwFlags | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                                          lpSource, dwMessageId, dwLanguageId,
                                          GetRawUnicode(), GetRawCount()+1, (va_list*)args);

        // Although we cannot directly detect truncation, we can tell if we
        // used up all the space (in which case we will assume truncation.)

        if (result != 0 && result < GetRawCount())
        {
            if (GetRawUnicode()[result-1] == L' ')
            {
                GetRawUnicode()[result-1] = L'\0';
                result -= 1;
            }
            Resize(result, REPRESENTATION_UNICODE, PRESERVE);
            RETURN TRUE;
        }
    }

    // We don't have enough space in our buffer, do dynamic allocation.
    WCHAR *string = NULL;

    DWORD result = ::WszFormatMessage(dwFlags | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                                      lpSource, dwMessageId, dwLanguageId,
                                      (LPWSTR) &string, 0, (va_list*)args);

    LocalAllocHolder holder((HLOCAL) string);

    if (result == 0)
        RETURN FALSE;
    else
    {
        if (string[result-1] == L' ')
            string[result-1] = L'\0';

        Set(string);
        RETURN TRUE;
    }
}


void SString::MakeFullNamespacePath(const SString &nameSpace, const SString &name)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        THROWS;
    }
    CONTRACT_END;

    if (nameSpace.GetRepresentation() == REPRESENTATION_UTF8
        && name.GetRepresentation() == REPRESENTATION_UTF8)
    {
        const UTF8 *ns = nameSpace.GetRawUTF8();
        const UTF8 *n = name.GetRawUTF8();
        COUNT_T count = ns::GetFullLength(ns, n)-1;
        Resize(count, REPRESENTATION_UTF8);
        ns::MakePath(GetRawUTF8(), count+1, ns, n);
    }
    else
    {
        const WCHAR *ns = nameSpace;
        const WCHAR *n = name;
        COUNT_T count = ns::GetFullLength(ns, n)-1;
        Resize(count, REPRESENTATION_UNICODE);
        ns::MakePath(GetRawUnicode(), count+1, ns, n);
    }

    RETURN;
}



//----------------------------------------------------------------------------
// Private helper.
// Check to see if the string fits the suggested representation
//----------------------------------------------------------------------------
BOOL SString::IsRepresentation(Representation representation) const
{
    CONTRACT(BOOL)
    {
        PRECONDITION(CheckRepresentation(representation));
        NOTHROW;
    }
    CONTRACT_END;

    Representation currentRepresentation = GetRepresentation();

    if (currentRepresentation == representation)
        RETURN TRUE;

    // If we have an empty representation, we match everything
    if (currentRepresentation == REPRESENTATION_EMPTY)
        RETURN TRUE;

    // If we're a 1 byte charset, there are some more chances to match
    if (currentRepresentation != REPRESENTATION_UNICODE
        && representation != REPRESENTATION_UNICODE)
    {
        // If we're ASCII, we can be any 1 byte rep
        if (currentRepresentation == REPRESENTATION_ASCII)
            RETURN TRUE;

        // We really want to be ASCII - scan to see if we qualify
        if (ScanASCII())
            RETURN TRUE;
    }

    // Sorry, must convert.
    RETURN FALSE;
}

//----------------------------------------------------------------------------
// Private helper.
// Get the contents of the given string in a form which is compatible with our
// string (and is in a fixed character set.)  Updates the given iterator
// if necessary to keep it in sync.
//----------------------------------------------------------------------------
const SString &SString::GetCompatibleString(const SString &s, SString &scratch, const CIterator &i) const
{
    CONTRACTL
    {
        PRECONDITION(s.Check());
        PRECONDITION(scratch.Check());
        PRECONDITION(scratch.CheckEmpty());
        THROWS_UNLESS_BOTH_NORMALIZED(s);
    }
    CONTRACTL_END;

    // Since we have an iterator, we should be fixed size already
    CONSISTENCY_CHECK(IsFixedSize());

    switch (GetRepresentation())
    {
    case REPRESENTATION_EMPTY:
        return s;

    case REPRESENTATION_ASCII:
        if (s.IsRepresentation(REPRESENTATION_ASCII))
            return s;

        // We can't in general convert to ASCII, so try unicode.
        ConvertToUnicode(i);
        // fall through

    case REPRESENTATION_UNICODE:
        if (s.IsRepresentation(REPRESENTATION_UNICODE))
            return s;

        s.ConvertToUnicode(scratch);
        return scratch;

    case REPRESENTATION_UTF8:
    case REPRESENTATION_ANSI:
        // These should all be impossible since we have an CIterator on us.
    default:
        UNREACHABLE_MSG("Unexpected string representation");
    }

    return s;
}

//----------------------------------------------------------------------------
// Private helper.
// Get the contents of the given string in a form which is compatible with our
// string (and is in a fixed character set.)
// May convert our string to unicode.
//----------------------------------------------------------------------------
const SString &SString::GetCompatibleString(const SString &s, SString &scratch) const
{
    CONTRACTL
    {
        PRECONDITION(s.Check());
        PRECONDITION(scratch.Check());
        PRECONDITION(scratch.CheckEmpty());
        THROWS_UNLESS_BOTH_NORMALIZED(s);
    }
    CONTRACTL_END;

    // First, make sure we have a fixed size.
    ConvertToFixed();

    switch (GetRepresentation())
    {
    case REPRESENTATION_EMPTY:
        return s;

    case REPRESENTATION_ANSI:
        if (s.IsRepresentation(REPRESENTATION_ANSI))
            return s;

        s.ConvertToANSI(scratch);
        return scratch;

    case REPRESENTATION_ASCII:
        if (s.IsRepresentation(REPRESENTATION_ASCII))
            return s;

        // We can't in general convert to ASCII, so try unicode.
        ConvertToUnicode();
        // fall through

    case REPRESENTATION_UNICODE:
        if (s.IsRepresentation(REPRESENTATION_UNICODE))
            return s;

        s.ConvertToUnicode(scratch);
        return scratch;

    case REPRESENTATION_UTF8:
    default:
        UNREACHABLE();
    }

    return s;
}

//----------------------------------------------------------------------------
// Private helper.
// If we have a 1 byte representation, scan the buffer to see if we can gain
// some conversion flexibility by labelling it ASCII
//----------------------------------------------------------------------------
BOOL SString::ScanASCII() const
{
    CONTRACT(BOOL)
    {
        POSTCONDITION(IsRepresentation(REPRESENTATION_ASCII) || IsASCIIScanned());
        NOTHROW;
    }
    CONTRACT_END;

    if (!IsASCIIScanned())
    {
        const CHAR *c = GetRawANSI();
        const CHAR *cEnd = c + GetRawCount();
        while (c < cEnd)
        {
            if (*c & 0x80)
                break;
            c++;
        }
        if (c == cEnd)
        {
            const_cast<SString *>(this)->SetRepresentation(REPRESENTATION_ASCII);
            RETURN TRUE;
        }
        else
            const_cast<SString *>(this)->SetASCIIScanned();
    }
    RETURN FALSE;
}

//----------------------------------------------------------------------------
// Private helper.
// Resize updates the geometry of the string and ensures that
// the space can be written to.
// count - number of characters (not including null) to hold
// preserve - if we realloc, do we copy data from old to new?
//----------------------------------------------------------------------------

void SString::Resize(COUNT_T count, SString::Representation representation, Preserve preserve)
{
    CONTRACT_VOID
    {
        PRECONDITION(CountToSize(count) >= count);
        POSTCONDITION(IsRepresentation(representation));
        POSTCONDITION(GetRawCount() == count);
        if (count == 0) NOTHROW; else THROWS;
    }
    CONTRACT_END;

    // If we are resizing to zero, Clear is more efficient
    if (count == 0)
    {
        Clear();
    }
    else
    {
        SetRepresentation(representation);

        COUNT_T size = CountToSize(count);

        // detect overflow
        if (size < count)
            ThrowOutOfMemory();

        ClearNormalized();

        SBuffer::Resize(size, preserve);

        if (IsImmutable())
            EnsureMutable();

        NullTerminate();
    }

    RETURN;
}

//-----------------------------------------------------------------------------
// This is essentially a specialized version of the above for size 0
//-----------------------------------------------------------------------------
void SString::Clear()
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        POSTCONDITION(IsEmpty());
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACT_END;

    SetRepresentation(REPRESENTATION_EMPTY);

    if (IsImmutable())
    {
        // Use shared empty string rather than allocating a new buffer
        SBuffer::SetImmutable(s_EmptyBuffer, sizeof(s_EmptyBuffer));
    }
    else
    {
        // Leave allocated buffer for future growth
        SBuffer::TweakSize(sizeof(WCHAR));
        GetRawUnicode()[0] = 0;
    }

    RETURN;
}


#ifdef DACCESS_COMPILE

void*
SString::DacGetRawContent() const
{
    if (IsEmpty())
    {
        return NULL;
    }

    switch (GetRepresentation())
    {
    case REPRESENTATION_EMPTY:
        return NULL;

    case REPRESENTATION_UNICODE:
        return DacInstantiateStringW((TADDR)m_buffer,
                                     GetCount(), true);

    case REPRESENTATION_UTF8:
    case REPRESENTATION_ASCII:
    case REPRESENTATION_ANSI:
        return DacInstantiateStringA((TADDR)m_buffer,
                                     GetCount(), true);

    default:
        DacNotImpl();
        return NULL;
    }
}

bool
SString::DacGetUnicode(COUNT_T bufChars,
                       __out_z __inout_ecount(bufChars) WCHAR *buffer,
                       COUNT_T *needChars) const
{
    PVOID content;
    int page = CP_ACP;

    if (IsEmpty() ||
        GetRepresentation() == REPRESENTATION_EMPTY)
    {
        if (needChars)
        {
            *needChars = 1;
        }
        if (buffer && bufChars)
        {
            buffer[0] = 0;
        }
        return true;
    }

    content = SBuffer::DacGetRawContent();
    switch (GetRepresentation())
    {
    case REPRESENTATION_UNICODE:
        if (needChars)
        {
            *needChars = GetCount() + 1;
        }
        if (buffer && bufChars)
        {
            if (bufChars > GetCount() + 1)
            {
                bufChars = GetCount() + 1;
            }
            memcpy(buffer, content, bufChars * sizeof(*buffer));
            buffer[bufChars - 1] = 0;
        }
        return true;

    case REPRESENTATION_UTF8:
        page = CP_UTF8;
    case REPRESENTATION_ASCII:
    case REPRESENTATION_ANSI:
        // page defaults to CP_ACP.
        if (needChars)
        {
            *needChars = WszMultiByteToWideChar(page, 0,
                                                (PSTR)content, -1,
                                                NULL, 0);
        }
        if (buffer && bufChars)
        {
            if (!WszMultiByteToWideChar(page, 0,
                                        (PSTR)content, -1,
                                        buffer, bufChars))
            {
                return false;
            }
        }
        return true;

    default:
        DacNotImpl();
        return false;
    }
}

#endif // #ifdef DACCESS_COMPILE
