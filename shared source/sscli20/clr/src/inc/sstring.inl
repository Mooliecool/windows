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

#ifndef _SSTRING_INL_
#define _SSTRING_INL_

#include "sstring.h"

#if defined(_MSC_VER)
#pragma inline_depth (20)
#endif

// ---------------------------------------------------------------------------
// Inline implementations. Pay no attention to that man behind the curtain.
// ---------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Default constructor. Sets the string to the empty string.
//----------------------------------------------------------------------------
inline SString::SString()
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    // The postcondition causes is to call goop that will trigger a violation
    // and we really don't care as it's debug only
    CONTRACT_VIOLATION(SOToleranceViolation);

    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        POSTCONDITION(IsEmpty());
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACT_END;

    RETURN;
}

inline SString::SString(void *buffer, COUNT_T size)
  : SBuffer(Prealloc, buffer, size)
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(buffer));
        PRECONDITION(CheckSize(size));
        POSTCONDITION(IsEmpty());
        NOTHROW;
    }
    CONTRACT_END;

    if (size < sizeof(WCHAR))
    {
        // Ignore the useless buffer
        SetImmutable(s_EmptyBuffer, sizeof(s_EmptyBuffer));
    }
    else
    {
        SBuffer::TweakSize(sizeof(WCHAR));
        GetRawUnicode()[0] = 0;
    }

    RETURN;
}

inline SString::SString(const SString &s)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(s.Check());
        POSTCONDITION(Equals(s));
        THROWS;
    }
    CONTRACT_END;

    Set(s);

    RETURN;
}

inline SString::SString(const SString &s1, const SString &s2)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(s1.Check());
        PRECONDITION(s2.Check());
        THROWS;
    }
    CONTRACT_END;

    Set(s1, s2);

    RETURN;
}

inline SString::SString(const SString &s1, const SString &s2, const SString &s3)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(s1.Check());
        PRECONDITION(s2.Check());
        PRECONDITION(s3.Check());
        THROWS;
    }
    CONTRACT_END;

    Set(s1, s2, s3);

    RETURN;
}

inline SString::SString(const SString &s1, const SString &s2, const SString &s3, const SString &s4)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(s1.Check());
        PRECONDITION(s2.Check());
        PRECONDITION(s3.Check());
        PRECONDITION(s4.Check());
        THROWS;
    }
    CONTRACT_END;

    Set(s1, s2, s3, s4);

    RETURN;
}

inline SString::SString(const SString &s, const CIterator &i, COUNT_T count)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(s.Check());
        PRECONDITION(i.Check());
        PRECONDITION(CheckCount(count));
        POSTCONDITION(s.Match(i, *this));
        POSTCONDITION(GetRawCount() == count);
        THROWS;
    }
    CONTRACT_END;

    Set(s, i, count);

    RETURN;
}

inline SString::SString(const SString &s, const CIterator &start, const CIterator &end)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(s.Check());
        PRECONDITION(start.Check());
        PRECONDITION(s.CheckIteratorRange(start));
        PRECONDITION(end.Check());
        PRECONDITION(s.CheckIteratorRange(end));
        PRECONDITION(start <= end);
        POSTCONDITION(s.Match(start, *this));
        POSTCONDITION(GetRawCount() == (COUNT_T) (end - start));
        THROWS;
    }
    CONTRACT_END;

    Set(s, start, end);

    RETURN;
}

inline SString::SString(const WCHAR *string)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        THROWS;
    }
    CONTRACT_END;

    Set(string);

    RETURN;
}

inline SString::SString(const WCHAR *string, COUNT_T count)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        PRECONDITION(CheckCount(count));
        THROWS;
    }
    CONTRACT_END;

    Set(string, count);

    RETURN;
}

inline SString::SString(enum tagASCII, const ASCII *string)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        PRECONDITION(CheckASCIIString(string));
        THROWS;
    }
    CONTRACT_END;

    SetASCII(string);

    RETURN;
}

inline SString::SString(enum tagASCII, const ASCII *string, COUNT_T count)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        PRECONDITION(CheckASCIIString(string, count));
        PRECONDITION(CheckCount(count));
        THROWS;
    }
    CONTRACT_END;

    SetASCII(string, count);

    RETURN;
}

inline SString::SString(tagUTF8 dummytag, const UTF8 *string)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        // !!! Check for illegal UTF8 encoding?
        PRECONDITION(CheckPointer(string, NULL_OK));
        THROWS;
    }
    CONTRACT_END;

    SetUTF8(string);

    RETURN;
}

inline SString::SString(tagUTF8 dummytag, const UTF8 *string, COUNT_T count)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        // !!! Check for illegal UTF8 encoding?
        PRECONDITION(CheckPointer(string, NULL_OK));
        PRECONDITION(CheckCount(count));
        THROWS;
    }
    CONTRACT_END;

    SetUTF8(string, count);

    RETURN;
}

inline SString::SString(tagANSI dummytag, const ANSI *string)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        THROWS;
    }
    CONTRACT_END;

    SetANSI(string);

    RETURN;
}

inline SString::SString(tagANSI dummytag, const ANSI *string, COUNT_T count)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(string, NULL_OK));
        PRECONDITION(CheckCount(count));
        THROWS;
    }
    CONTRACT_END;

    SetANSI(string, count);

    RETURN;
}

inline SString::SString(WCHAR character)
  : SBuffer(Immutable, s_EmptyBuffer, sizeof(s_EmptyBuffer))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        THROWS;
    }
    CONTRACT_END;

    Set(character);

    RETURN;
}

inline SString::SString(tagLiteral dummytag, const ASCII *literal)
  : SBuffer(Immutable, (const BYTE *) literal, (COUNT_T) (strlen(literal)+1)*sizeof(CHAR))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(literal));
        PRECONDITION(CheckASCIIString(literal));
        NOTHROW;
    }
    CONTRACT_END;

    SetRepresentation(REPRESENTATION_ASCII);

    RETURN;
}

inline SString::SString(tagLiteral dummytag, const WCHAR *literal)
  : SBuffer(Immutable, (const BYTE *) literal, (COUNT_T) (wcslen(literal)+1)*sizeof(WCHAR))
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(literal));
        NOTHROW;
    }
    CONTRACT_END;

    SetRepresentation(REPRESENTATION_UNICODE);
    SetNormalized();

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to s
// s - source string
//-----------------------------------------------------------------------------
inline void SString::Set(const SString &s)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(s.Check());
        POSTCONDITION(Equals(s));
        THROWS;
    }
    CONTRACT_END;

    SBuffer::Set(s);
    SetRepresentation(s.GetRepresentation());
    ClearNormalized();

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to concatenation of s1 and s2
//-----------------------------------------------------------------------------
inline void SString::Set(const SString &s1, const SString &s2)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(s1.Check());
        PRECONDITION(s2.Check());
        THROWS;
    }
    CONTRACT_END;

    Preallocate(s1.GetCount() + s2.GetCount());

    Set(s1);
    Append(s2);

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to concatenation of s1, s2, and s3
//-----------------------------------------------------------------------------
inline void SString::Set(const SString &s1, const SString &s2, const SString &s3)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(s1.Check());
        PRECONDITION(s2.Check());
        PRECONDITION(s3.Check());
        THROWS;
    }
    CONTRACT_END;

    Preallocate(s1.GetCount() + s2.GetCount() + s3.GetCount());

    Set(s1);
    Append(s2);
    Append(s3);

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to concatenation of s1, s2, s3, and s4
//-----------------------------------------------------------------------------
inline void SString::Set(const SString &s1, const SString &s2, const SString &s3, const SString &s4)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(s1.Check());
        PRECONDITION(s2.Check());
        PRECONDITION(s3.Check());
        PRECONDITION(s4.Check());
        THROWS;
    }
    CONTRACT_END;

    Preallocate(s1.GetCount() + s2.GetCount() + s3.GetCount() + s4.GetCount());

    Set(s1);
    Append(s2);
    Append(s3);
    Append(s4);

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to the substring from s.
// s - the source string
// start - the character to start at
// length - number of characters to copy from s.
//-----------------------------------------------------------------------------
inline void SString::Set(const SString &s, const CIterator &i, COUNT_T count)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(s.Check());
        PRECONDITION(i.Check());
        PRECONDITION(CheckCount(count));
        POSTCONDITION(s.Match(i, *this));
        POSTCONDITION(GetRawCount() == count);
        THROWS;
    }
    CONTRACT_END;

    Resize(count, s.GetRepresentation());
    SBuffer::Copy(SBuffer::Begin(), i.m_ptr, count<<i.m_characterSizeShift);
    NullTerminate();

    RETURN;
}

//-----------------------------------------------------------------------------
// Set this string to the substring from s.
// s - the source string
// start - the position to start
// end - the position to end (exclusive)
//-----------------------------------------------------------------------------
inline void SString::Set(const SString &s, const CIterator &start, const CIterator &end)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(s.Check());
        PRECONDITION(start.Check());
        PRECONDITION(s.CheckIteratorRange(start));
        PRECONDITION(end.Check());
        PRECONDITION(s.CheckIteratorRange(end));
        PRECONDITION(end >= start);
        POSTCONDITION(s.Match(start, *this));
        POSTCONDITION(GetRawCount() == (COUNT_T) (end - start));
        THROWS;
    }
    CONTRACT_END;

    Set(s, start, end - start);

    RETURN;
}

// Return a global empty string
inline const SString &SString::Empty()
{
    CONTRACTL
    {
        // POSTCONDITION(RETVAL.IsEmpty());
        PRECONDITION(CheckStartup());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return *s_Empty;
}

// Get a const pointer to the internal buffer as a unicode string.
inline const WCHAR *SString::GetUnicode() const
{
    CONTRACT(const WCHAR *)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;
    
    if (this == NULL)
        RETURN NULL;

    ConvertToUnicode();

    RETURN GetRawUnicode();
}

// Normalize the string to unicode.  This will make many operations nonfailing.
inline void SString::Normalize() const
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        POSTCONDITION(IsNormalized());
        THROWS_UNLESS_NORMALIZED;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    ConvertToUnicode();
    SetNormalized();

    RETURN;
}

// Get a const pointer to the internal buffer as a unicode string.
inline const WCHAR *SString::GetUnicode(const CIterator &i) const
{
    CONTRACT(const WCHAR *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i));
        THROWS_UNLESS_NORMALIZED;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    PRECONDITION(CheckPointer(this));
    
    ConvertToUnicode(i);

    RETURN i.GetUnicode();
}

// Append s to the end of this string.
inline void SString::Append(const SString &s)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(s.Check());
        THROWS;
    }
    CONTRACT_END;

    Insert(End(), s);

    RETURN;
}

inline void SString::Append(const WCHAR *string)
{    
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(string));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(string);
    Append(s);

    RETURN;
}

inline void SString::AppendASCII(const CHAR *string)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(string));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(SString::Ascii, string);
    Append(s);

    RETURN;
}

inline void SString::AppendUTF8(const CHAR *string)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(string));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(SString::Utf8, string);
    Append(s);

    RETURN;
}

inline void SString::Append(const WCHAR c)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(c);
    Append(s);

    RETURN;
}

inline void SString::AppendUTF8(const CHAR c)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(SString::Utf8, c);
    Append(s);

    RETURN;
}

// Turn this on to test that these if you are testing common scenarios dealing with
// ASCII strings that do not touch the cases where this family of function differs
// in behavior for expected reasons.
//#define VERIFY_CRT_EQUIVALNCE 1

// Helpers for CRT function equivalance.
/* static */
inline int __cdecl SString::_stricmp(const CHAR *buffer1, const CHAR *buffer2) {
    WRAPPER_CONTRACT;
    int returnValue = CaseCompareHelperA(buffer1, buffer2, 0, s_defaultLCID, TRUE, FALSE);
#ifdef VERIFY_CRT_EQUIVALNCE
    _ASSERTE((returnValue == 0) == (::_stricmp(buffer1, buffer2) == 0));
#endif
    return returnValue;

}

/* static */
inline int __cdecl SString::_strnicmp(const CHAR *buffer1, const CHAR *buffer2, COUNT_T count) {
    WRAPPER_CONTRACT;
    int returnValue = CaseCompareHelperA(buffer1, buffer2, count, s_defaultLCID, TRUE, TRUE);
#ifdef VERIFY_CRT_EQUIVALNCE
    _ASSERTE((returnValue == 0) == (::_strnicmp(buffer1, buffer2, count) == 0));
#endif
    return returnValue;
}

/* static */
inline int __cdecl SString::_wcsicmp(const WCHAR *buffer1, const WCHAR *buffer2) {
    WRAPPER_CONTRACT;
    int returnValue = CaseCompareHelper(buffer1, buffer2, 0, s_defaultLCID, TRUE, FALSE);
#ifdef VERIFY_CRT_EQUIVALNCE
    _ASSERTE((returnValue == 0) == (::_wcsicmp(buffer1, buffer2) == 0));
#endif
    return returnValue;

}

/* static */
inline int __cdecl SString::_wcsnicmp(const WCHAR *buffer1, const WCHAR *buffer2, COUNT_T count) {
    WRAPPER_CONTRACT;
    int returnValue = CaseCompareHelper(buffer1, buffer2, count, s_defaultLCID, TRUE, TRUE);
#ifdef VERIFY_CRT_EQUIVALNCE
    _ASSERTE((returnValue == 0) == (::_wcsnicmp(buffer1, buffer2, count) == 0));
#endif
    return returnValue;
}


inline ULONG SString::HashCaseInsensitive() const
{
	WRAPPER_CONTRACT;
	return HashCaseInsensitive(s_defaultLCID);
}

inline int SString::CompareCaseInsensitive(const SString &s) const
{
	WRAPPER_CONTRACT;
	return CompareCaseInsensitive(s, s_defaultLCID);
}

inline BOOL SString::EqualsCaseInsensitive(const SString &s) const
{
	WRAPPER_CONTRACT;
	return EqualsCaseInsensitive(s, s_defaultLCID);
}

inline BOOL SString::MatchCaseInsensitive(const CIterator &i, const SString &s) const
{
	WRAPPER_CONTRACT;
	return MatchCaseInsensitive(i, s, s_defaultLCID);
}

inline BOOL SString::MatchCaseInsensitive(const CIterator &i, WCHAR c) const
{
	WRAPPER_CONTRACT;
	return MatchCaseInsensitive(i, c, s_defaultLCID);
}

inline BOOL SString::Match(const CIterator &i, WCHAR c) const
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
    RETURN (i < End() && i[0] == c);
}

inline BOOL SString::Skip(CIterator &i, const SString &s) const
{
    CONTRACT(BOOL)
    {
        GC_NOTRIGGER;
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(s.Check());
        THROWS_UNLESS_BOTH_NORMALIZED(s);
    }
    CONTRACT_END;

    if (Match(i, s))
    {
        i += s.GetRawCount();
        RETURN TRUE;
    }
    else
        RETURN FALSE;
}

inline BOOL SString::Skip(CIterator &i, WCHAR c) const
{
    CONTRACT(BOOL)
    {
        GC_NOTRIGGER;
        INSTANCE_CHECK;
        PRECONDITION(CheckIteratorRange(i));
        NOTHROW;
    }
    CONTRACT_END;

    if (Match(i, c))
    {
        i++;
        RETURN TRUE;
    }
    else
        RETURN FALSE;
}

// Find string within this string. Return TRUE and update iterator if found
inline BOOL SString::Find(CIterator &i, const WCHAR *string) const
{    
    CONTRACT(BOOL)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(CheckPointer(string));
        POSTCONDITION(RETVAL == Match(i, SString(string)));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(string);
    RETURN Find(i, s);
}

inline BOOL SString::FindASCII(CIterator &i, const CHAR *string) const
{    
    CONTRACT(BOOL)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(CheckPointer(string));
        POSTCONDITION(RETVAL == Match(i, SString(SString::Ascii, string)));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(SString::Ascii, string);
    RETURN Find(i, s);
}

inline BOOL SString::FindUTF8(CIterator &i, const CHAR *string) const
{    
    CONTRACT(BOOL)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(CheckPointer(string));
        POSTCONDITION(RETVAL == Match(i, SString(SString::Ascii, string)));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(SString::Utf8, string);
    RETURN Find(i, s);
}

inline BOOL SString::FindBack(CIterator &i, const WCHAR *string) const
{    
    CONTRACT(BOOL)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(CheckPointer(string));
        POSTCONDITION(RETVAL == Match(i, SString(string)));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(string);
    RETURN FindBack(i, s);
}

inline BOOL SString::FindBackASCII(CIterator &i, const CHAR *string) const
{    
    CONTRACT(BOOL)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(CheckPointer(string));
        POSTCONDITION(RETVAL == Match(i, SString(SString::Ascii, string)));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(SString::Ascii, string);
    RETURN FindBack(i, s);
}

inline BOOL SString::FindBackUTF8(CIterator &i, const CHAR *string) const
{    
    CONTRACT(BOOL)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(CheckPointer(string));
        POSTCONDITION(RETVAL == Match(i, SString(SString::Ascii, string)));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(SString::Utf8, string);
    RETURN FindBack(i, s);
}

// Insert string at iterator position
inline void SString::Insert(const Iterator &i, const SString &s)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(s.Check());
        THROWS;
    }
    CONTRACT_END;

    Replace(i, 0, s);

    RETURN;
}

inline void SString::Insert(const Iterator &i, const WCHAR *string)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(CheckPointer(string));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(string);
    Replace(i, 0, s);

    RETURN;
}

inline void SString::InsertASCII(const Iterator &i, const CHAR *string)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(CheckPointer(string));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(SString::Ascii, string);
    Replace(i, 0, s);

    RETURN;
}

inline void SString::InsertUTF8(const Iterator &i, const CHAR *string)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i));
        PRECONDITION(CheckPointer(string));
        THROWS;
    }
    CONTRACT_END;

    StackSString s(SString::Utf8, string);
    Replace(i, 0, s);

    RETURN;
}

// Delete string at iterator position
inline void SString::Delete(const Iterator &i, COUNT_T length)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckIteratorRange(i, length));
        THROWS;
    }
    CONTRACT_END;

    Replace(i, length, SString());

    RETURN;
}

// Preallocate some space for the string buffer
inline void SString::Preallocate(COUNT_T characters) const
{
    WRAPPER_CONTRACT;

    // Assume unicode since we may get converted
    SBuffer::Preallocate(characters * sizeof(WCHAR));
}

// Trim unused space from the buffer
inline void SString::Trim() const
{
    WRAPPER_CONTRACT;

    if (GetRawCount() == 0)
    {
        // Share the global empty string buffer.
        const_cast<SString *>(this)->SBuffer::SetImmutable(s_EmptyBuffer, sizeof(s_EmptyBuffer));
    }
    else
    {
        SBuffer::Trim();
    }
}

// RETURN true if the string is empty.
inline BOOL SString::IsEmpty() const
{ 
    CONTRACT(BOOL)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACT_END;

    RETURN (GetRawCount() == 0);
}

// RETURN true if the string rep is ASCII.
inline BOOL SString::IsASCII() const
{ 
    CONTRACT(BOOL)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACT_END;

    RETURN IsRepresentation(REPRESENTATION_ASCII);
}

// Get the number of characters in the string (excluding the terminating NULL)
inline COUNT_T SString::GetCount() const
{ 
    CONTRACT(COUNT_T)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        POSTCONDITION(CheckCount(RETVAL));
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    ConvertToFixed();

    RETURN SizeToCount(GetSize());
}

// Private helpers:
// Return the current size of the string (even if it is multibyte)
inline COUNT_T SString::GetRawCount() const
{
    WRAPPER_CONTRACT;

    return SizeToCount(GetSize());
}

// Private helpers:
// get string contents as a particular character set:

inline ASCII *SString::GetRawASCII() const
{
    LEAF_CONTRACT;

    return (ASCII *) m_buffer;
}

inline UTF8 *SString::GetRawUTF8() const
{
    LEAF_CONTRACT;

    return (UTF8 *) m_buffer;
}

inline ANSI *SString::GetRawANSI() const
{
    LEAF_CONTRACT;

    return (ANSI *) m_buffer;
}

inline WCHAR *SString::GetRawUnicode() const
{
    LEAF_CONTRACT;

    return (WCHAR *) m_buffer;
}

// Private helper:
// get the representation (ansi, unicode, utf8)
inline SString::Representation SString::GetRepresentation() const 
{ 
    WRAPPER_CONTRACT;

    return (Representation) SBuffer::GetRepresentationField();
}

// Private helper.
// Set the representation.
inline void SString::SetRepresentation(SString::Representation representation)
{ 
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckRepresentation(representation));
        POSTCONDITION(GetRepresentation() == representation);
        NOTHROW;
    }
    CONTRACT_END;

    SBuffer::SetRepresentationField((int) representation);

    RETURN;
}

// Private helper:
// Get the amount to shift the byte size to get a character count
inline int SString::GetCharacterSizeShift() const 
{ 
    WRAPPER_CONTRACT;

    // Note that the flag is backwards; we want the default
    // value to match the default representation (empty)
    return (GetRepresentation()&REPRESENTATION_SINGLE_MASK) == 0;
}

//----------------------------------------------------------------------------
// Private helper.
// We know the buffer should be m_count characters. Place a null terminator
// in the buffer to make our internal string null-terminated at that length.
//----------------------------------------------------------------------------
FORCEINLINE void SString::NullTerminate()
{
    CONTRACT_VOID
    {
        POSTCONDITION(CheckPointer(this));
        NOTHROW;
    }
    CONTRACT_END;

    BYTE *end = m_buffer + GetSize();

    if (GetRepresentation()&REPRESENTATION_SINGLE_MASK)
    {
        ((CHAR *)end)[-1] = 0;
    }
    else
    {
        ((WCHAR *)end)[-1] = 0;
    }

    RETURN;
}

//----------------------------------------------------------------------------
// private helper
// Return true if the string is a literal.
// A literal string has immutable memory. 
//----------------------------------------------------------------------------
inline BOOL SString::IsLiteral() const 
{
    WRAPPER_CONTRACT;

    return SBuffer::IsImmutable() && (m_buffer != s_EmptyBuffer);
}

//----------------------------------------------------------------------------
// private helper:
// RETURN true if the string allocated (and should delete) its buffer.
// IsAllocated() will RETURN false for Literal strings and 
// stack-based strings (the buffer is on the stack)
//----------------------------------------------------------------------------
inline BOOL SString::IsAllocated() const
{
    WRAPPER_CONTRACT;

    return SBuffer::IsAllocated();
}

//----------------------------------------------------------------------------
// Return true after we call OpenBuffer(), but before we close it.
// All SString operations are illegal while the buffer is open.
//----------------------------------------------------------------------------
#if _DEBUG    
inline BOOL SString::IsBufferOpen() const
{
    WRAPPER_CONTRACT;

    return SBuffer::IsOpened();
}
#endif

//----------------------------------------------------------------------------
// Return true if we've scanned the string to see if it is in the ASCII subset.
//----------------------------------------------------------------------------
inline BOOL SString::IsASCIIScanned() const
{    
    WRAPPER_CONTRACT;

    return SBuffer::IsFlag1();
}

//----------------------------------------------------------------------------
// Set that we've scanned the string to see if it is in the ASCII subset.
//----------------------------------------------------------------------------
inline void SString::SetASCIIScanned() const
{    
    WRAPPER_CONTRACT;

    const_cast<SString *>(this)->SBuffer::SetFlag1();
}

//----------------------------------------------------------------------------
// Return true if we've normalized the string to unicode
//----------------------------------------------------------------------------
inline BOOL SString::IsNormalized() const
{    
    WRAPPER_CONTRACT;

    return SBuffer::IsFlag3();
}

//----------------------------------------------------------------------------
// Set that we've normalized the string to unicode
//----------------------------------------------------------------------------
inline void SString::SetNormalized() const
{    
    WRAPPER_CONTRACT;

    const_cast<SString *>(this)->SBuffer::SetFlag3();
}

//----------------------------------------------------------------------------
// Clear normalization
//----------------------------------------------------------------------------
inline void SString::ClearNormalized() const
{    
    WRAPPER_CONTRACT;

    const_cast<SString *>(this)->SBuffer::ClearFlag3();
}

//----------------------------------------------------------------------------
// Private helper.
// Check to see if the string representation has single byte size
//----------------------------------------------------------------------------
inline BOOL SString::IsSingleByte() const
{
    CONTRACT(BOOL)
    {
        NOTHROW;
    }
    CONTRACT_END;

    RETURN ((GetRepresentation()&REPRESENTATION_SINGLE_MASK) != 0);
}

//----------------------------------------------------------------------------
// Private helper.
// Check to see if the string representation has fixed size characters
//----------------------------------------------------------------------------
inline BOOL SString::IsFixedSize() const
{
    CONTRACT(BOOL)
    {
        NOTHROW;
    }
    CONTRACT_END;

    if (GetRepresentation()&REPRESENTATION_VARIABLE_MASK)
        RETURN ((GetRepresentation() == REPRESENTATION_ANSI) && !s_IsANSIMultibyte);
    else
        RETURN TRUE;
}

//----------------------------------------------------------------------------
// Private helper.
// Check to see if the string representation is appropriate for iteration
//----------------------------------------------------------------------------
inline BOOL SString::IsIteratable() const
{
    CONTRACT(BOOL)
    {
        NOTHROW;
    }
    CONTRACT_END;

    // Note that in many cases ANSI may be fixed width.  However we 
    // currently still do not allow iterating on them, because we would have to
    // do character-by-character conversion on a character dereference (which must
    // go to unicode) .  We may want to adjust this going forward to 
    // depending on perf in the non-ASCII but fixed width ANSI case.

    RETURN ((GetRepresentation()&REPRESENTATION_VARIABLE_MASK) == 0);
}

//----------------------------------------------------------------------------
// Private helper
// Return the size of the given string in bytes
// in the given representation.
// count does not include the null-terminator, but the RETURN value does.
//----------------------------------------------------------------------------
inline COUNT_T SString::CountToSize(COUNT_T count) const
{    
    CONTRACT(COUNT_T)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckCount(count));
        POSTCONDITION(SizeToCount(RETVAL) == count);
        NOTHROW;
    }
    CONTRACT_END;

    RETURN (count+1) << GetCharacterSizeShift();
}

//----------------------------------------------------------------------------
// Private helper.
// Return the maxmimum count of characters that could fit in a buffer of 
// 'size' bytes in the given representation.
// 'size' includes the null terminator, but the RETURN value does not.
//----------------------------------------------------------------------------
inline COUNT_T SString::SizeToCount(COUNT_T size) const
{
    CONTRACT(COUNT_T)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckSize(size));
        POSTCONDITION(CountToSize(RETVAL) == size);
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACT_END;

    RETURN (size >> GetCharacterSizeShift()) - 1;
}

//----------------------------------------------------------------------------
// Private helper.
// Return the maxmimum count of characters that could fit in the current
// buffer including NULL terminator. 
//----------------------------------------------------------------------------
inline COUNT_T SString::GetBufferSizeInCharIncludeNullChar() const
{
    CONTRACT(COUNT_T)
    {
        GC_NOTRIGGER;
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACT_END;

    RETURN (GetSize() >> GetCharacterSizeShift());
}


 
//----------------------------------------------------------------------------
// Assert helper
// Asser that the iterator is within the given string.
//----------------------------------------------------------------------------
inline CHECK SString::CheckIteratorRange(const CIterator &i) const
{
    CANNOT_HAVE_CONTRACT;
    CHECK(i >= Begin());
    CHECK(i <= End()); // Note that it's OK to look at the terminating null
    CHECK_OK;
}

//----------------------------------------------------------------------------
// Assert helper
// Asser that the iterator is within the given string.
//----------------------------------------------------------------------------
inline CHECK SString::CheckIteratorRange(const CIterator &i, COUNT_T length) const
{
    CANNOT_HAVE_CONTRACT;
    CHECK(i >= Begin());
    CHECK(i + length <= End());  // Note that it's OK to look at the terminating null
    CHECK_OK;
}

//----------------------------------------------------------------------------
// Assert that the string is empty
//----------------------------------------------------------------------------
inline CHECK SString::CheckEmpty() const
{
    CANNOT_HAVE_CONTRACT;
    CHECK(IsEmpty());
    CHECK_OK;
}

//----------------------------------------------------------------------------
// Check the range of a count
//----------------------------------------------------------------------------
inline CHECK SString::CheckCount(COUNT_T count) 
{
    CANNOT_HAVE_CONTRACT;
    CHECK(CheckSize(count*sizeof(WCHAR)));
    CHECK_OK;
}

//----------------------------------------------------------------------------
// Check the representation field
//----------------------------------------------------------------------------
inline CHECK SString::CheckRepresentation(int representation)
{
    CANNOT_HAVE_CONTRACT;
    CHECK(representation == REPRESENTATION_EMPTY
          || representation == REPRESENTATION_UNICODE
          || representation == REPRESENTATION_ASCII
          || representation == REPRESENTATION_UTF8
          || representation == REPRESENTATION_ANSI);
    CHECK((representation & REPRESENTATION_MASK) == representation);

    CHECK_OK;
}

#if CHECK_INVARIANTS
//----------------------------------------------------------------------------
// Assert helper. Check that the string only uses the ASCII subset of 
// codes.
//----------------------------------------------------------------------------
inline CHECK SString::CheckASCIIString(const CHAR *string)
{
    CANNOT_HAVE_CONTRACT;
    if (string != NULL)
        CHECK(CheckASCIIString(string, (int) strlen(string)));
    CHECK_OK;
}

inline CHECK SString::CheckASCIIString(const CHAR *string, COUNT_T count)
{
    CANNOT_HAVE_CONTRACT;
#if _DEBUG
    const CHAR *sEnd = string + count;
    while (string < sEnd)
    {
        CHECK_MSG((*string & 0x80) == 0x00, "Found non-ASCII character in string.");
        string++;
    }
#endif
    CHECK_OK;
}

//----------------------------------------------------------------------------
// Check routine and invariants.
//----------------------------------------------------------------------------

inline CHECK SString::Check() const
{
    CANNOT_HAVE_CONTRACT;
    CHECK(SBuffer::Check());
    CHECK_OK;
}

inline CHECK SString::Invariant() const
{
    CANNOT_HAVE_CONTRACT;
    CHECK(SBuffer::Invariant());
    CHECK_OK;
}

inline CHECK SString::InternalInvariant() const
{
    CANNOT_HAVE_CONTRACT;
    CHECK(SBuffer::InternalInvariant());
    CHECK(SBuffer::GetSize() >= 2);
    if (IsNormalized())
        CHECK(IsRepresentation(REPRESENTATION_UNICODE));
    CHECK_OK;
}
#endif  // CHECK_INVARIANTS

//----------------------------------------------------------------------------
// Return a writeable buffer that can store 'countChars'+1 unicode characters.
// Call CloseBuffer when done.
//----------------------------------------------------------------------------    
inline WCHAR *SString::OpenUnicodeBuffer(COUNT_T countChars)
{
    CONTRACT(WCHAR*)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckCount(countChars));
#if _DEBUG
        POSTCONDITION(IsBufferOpen());
#endif
        POSTCONDITION(GetRawCount() == countChars);
        POSTCONDITION(GetRepresentation() == REPRESENTATION_UNICODE || countChars == 0);
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS;
    }
    CONTRACT_END;

    OpenBuffer(REPRESENTATION_UNICODE, countChars);
    RETURN GetRawUnicode();
}

//----------------------------------------------------------------------------
// Return a writeable buffer that can store 'countChars'+1 ansi characters.
// Call CloseBuffer when done.
//----------------------------------------------------------------------------
inline ANSI *SString::OpenANSIBuffer(COUNT_T countChars)
{
    CONTRACT(ANSI*)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckCount(countChars));
#if _DEBUG
        POSTCONDITION(IsBufferOpen());
#endif
        POSTCONDITION(GetRawCount() == countChars);
        POSTCONDITION(GetRepresentation() == REPRESENTATION_ANSI || countChars == 0);
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS;
    }
    CONTRACT_END;

    OpenBuffer(REPRESENTATION_ANSI, countChars);
    RETURN GetRawANSI();
}

//----------------------------------------------------------------------------
// Return a writeable buffer that can store 'countChars'+1 ansi characters.
// Call CloseBuffer when done.
//----------------------------------------------------------------------------
inline UTF8 *SString::OpenUTF8Buffer(COUNT_T countBytes)
{
    CONTRACT(UTF8*)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckCount(countBytes));
#if _DEBUG
        POSTCONDITION(IsBufferOpen());
#endif
        POSTCONDITION(GetRawCount() == countBytes);
        POSTCONDITION(GetRepresentation() == REPRESENTATION_UTF8 || countBytes == 0);
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS;
    }
    CONTRACT_END;

    OpenBuffer(REPRESENTATION_UTF8, countBytes);
    RETURN GetRawUTF8();
}

//----------------------------------------------------------------------------
// Private helper to open a raw buffer.
// Called by public functions to open the buffer in the specific 
// representation.
// While the buffer is opened, all other operations are illegal. Call 
// CloseBuffer() when done.
//----------------------------------------------------------------------------
inline void SString::OpenBuffer(SString::Representation representation, COUNT_T countChars)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION_MSG(!IsBufferOpen(), "Can't nest calls to OpenBuffer()");
        PRECONDITION(CheckRepresentation(representation));
        PRECONDITION(CheckSize(countChars));
#if _DEBUG
        POSTCONDITION(IsBufferOpen());
#endif
        POSTCONDITION(GetRawCount() == countChars);
        POSTCONDITION(GetRepresentation() == representation || countChars == 0);
        THROWS;
    }
    CONTRACT_END;

    Resize(countChars, representation);

    SBuffer::OpenRawBuffer(CountToSize(countChars));

    RETURN;
}

//----------------------------------------------------------------------------
// Get the max size that can be passed to OpenUnicodeBuffer without causing 
// allocations.
//----------------------------------------------------------------------------
inline COUNT_T SString::GetUnicodeAllocation()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
    } 
    CONTRACTL_END;

    COUNT_T allocation = GetAllocation();
    return ( (allocation > sizeof(WCHAR)) 
        ? (allocation - sizeof(WCHAR)) / sizeof(WCHAR) : 0 );
}

//----------------------------------------------------------------------------
// Close an open buffer. Assumes that we wrote exactly number of characters 
// we requested in OpenBuffer.
//----------------------------------------------------------------------------
inline void SString::CloseBuffer()
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
#if _DEBUG
        PRECONDITION_MSG(IsBufferOpen(), "Can only CloseBuffer() after a call to OpenBuffer()");
#endif
        POSTCONDITION(CheckPointer(this));
        THROWS;
    }
    CONTRACT_END;

    SBuffer::CloseRawBuffer();
    NullTerminate();

    RETURN;
}

//----------------------------------------------------------------------------
// CloseBuffer() tells the SString that we're done using the unsafe buffer.
// countChars is the count of characters actually used (so we can set m_count).
// This is important if we request a buffer larger than what we actually 
// used.
//----------------------------------------------------------------------------
inline void SString::CloseBuffer(COUNT_T finalCount)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
#if _DEBUG
        PRECONDITION_MSG(IsBufferOpen(), "Can only CloseBuffer() after a call to OpenBuffer()");
#endif
        PRECONDITION(CheckSize(finalCount));
        POSTCONDITION(CheckPointer(this));
        POSTCONDITION(GetRawCount() == finalCount);
        THROWS;
    }
    CONTRACT_END;

    SBuffer::CloseRawBuffer(CountToSize(finalCount));
    NullTerminate();

    RETURN;
}

//----------------------------------------------------------------------------
// EnsureWritable
// Ensures that the buffer is writable
//----------------------------------------------------------------------------
inline void SString::EnsureWritable() const
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        POSTCONDITION(!IsLiteral());
        THROWS;
    }
    CONTRACT_END;

    if (IsLiteral())
        const_cast<SString *>(this)->Resize(GetRawCount(), GetRepresentation(), PRESERVE);

    RETURN;
}

//-----------------------------------------------------------------------------
// Convert the internal representation to be a fixed size 
//-----------------------------------------------------------------------------
inline void SString::ConvertToFixed() const
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        POSTCONDITION(IsFixedSize());
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    // If we're already fixed size, great.
    if (IsFixedSize())
        RETURN;

    // See if we can coerce it to ASCII.
    if (ScanASCII())
        RETURN;

    // Convert to unicode then.
    ConvertToUnicode();

    RETURN;
}

//-----------------------------------------------------------------------------
// Convert the internal representation to be an iteratable one (current 
// requirements here are that it be trivially convertable to unicode chars.)
//-----------------------------------------------------------------------------
inline void SString::ConvertToIteratable() const
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        POSTCONDITION(IsIteratable());
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    // If we're already iteratable, great.
    if (IsIteratable())
        RETURN;

    // See if we can coerce it to ASCII.
    if (ScanASCII())
        RETURN;

    // Convert to unicode then.
    ConvertToUnicode();

    RETURN;
}

//-----------------------------------------------------------------------------
// Create iterators on the string.
//-----------------------------------------------------------------------------

inline SString::UIterator SString::BeginUnicode()
{
    CONTRACT(SString::UIterator)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        POSTCONDITION(CheckValue(RETVAL));
        THROWS;
    }
    CONTRACT_END;

    ConvertToUnicode();
    EnsureWritable();

    RETURN UIterator(this, 0);
}
        
inline SString::UIterator SString::EndUnicode()
{
    CONTRACT(SString::UIterator)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        POSTCONDITION(CheckValue(RETVAL));
        THROWS;
    }
    CONTRACT_END;

    ConvertToUnicode();
    EnsureWritable();

    RETURN UIterator(this, GetCount());
}

//-----------------------------------------------------------------------------
// Create CIterators on the string.
//-----------------------------------------------------------------------------

FORCEINLINE SString::CIterator SString::Begin() const
{
    CONTRACT(SString::CIterator)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        POSTCONDITION(CheckValue(RETVAL));
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    ConvertToIteratable();

    RETURN CIterator(this, 0);
}
        
FORCEINLINE SString::CIterator SString::End() const
{
    CONTRACT(SString::CIterator)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        POSTCONDITION(CheckValue(RETVAL));
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    ConvertToIteratable();

    RETURN CIterator(this, GetCount());
}

//-----------------------------------------------------------------------------
// Create Iterators on the string.
//-----------------------------------------------------------------------------

FORCEINLINE SString::Iterator SString::Begin() 
{
    CONTRACT(SString::Iterator)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        POSTCONDITION(CheckValue(RETVAL));
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    ConvertToIteratable();
    EnsureMutable();

    RETURN Iterator(this, 0);
}
        
FORCEINLINE SString::Iterator SString::End() 
{
    CONTRACT(SString::Iterator)
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        POSTCONDITION(CheckValue(RETVAL));
        THROWS_UNLESS_NORMALIZED;
    }
    CONTRACT_END;

    ConvertToIteratable();
    EnsureMutable();

    RETURN Iterator(this, GetCount());
}

//-----------------------------------------------------------------------------
// CIterator support routines
//-----------------------------------------------------------------------------

inline SString::Index::Index()
{
    LEAF_CONTRACT;
}

inline SString::Index::Index(SString *string, SCOUNT_T index)
  : SBuffer::Index(string, index<<string->GetCharacterSizeShift())
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(string));
        PRECONDITION(string->IsIteratable());
        PRECONDITION(DoCheck(0));
        POSTCONDITION(CheckPointer(this));
        // POSTCONDITION(Subtract(string->Begin()) == index); contract violation - fix later
        NOTHROW;
    }
    CONTRACT_END;
        
    m_characterSizeShift = string->GetCharacterSizeShift();

    RETURN;
}

inline BYTE &SString::Index::GetAt(SCOUNT_T delta) const
{
    LEAF_CONTRACT;

    return m_ptr[delta<<m_characterSizeShift];
}

inline void SString::Index::Skip(SCOUNT_T delta)
{
    LEAF_CONTRACT;

    m_ptr += (delta<<m_characterSizeShift);
}

inline SCOUNT_T SString::Index::Subtract(const Index &i) const
{
    LEAF_CONTRACT;

    return (SCOUNT_T) ((m_ptr - i.m_ptr)>>m_characterSizeShift);
}

inline CHECK SString::Index::DoCheck(SCOUNT_T delta) const
{
    CANNOT_HAVE_CONTRACT;
#if _DEBUG
    const SString *string = (const SString *) GetContainerDebug();

    CHECK(m_ptr + (delta<<m_characterSizeShift) >= string->m_buffer);
    CHECK(m_ptr + (delta<<m_characterSizeShift) < string->m_buffer + string->GetSize());
#endif
    CHECK_OK;
}

inline void SString::Index::Resync(const SString *string, BYTE *ptr) const
{
    WRAPPER_CONTRACT;

    SBuffer::Index::Resync(string, ptr);

    const_cast<SString::Index*>(this)->m_characterSizeShift = string->GetCharacterSizeShift();
}


inline const WCHAR *SString::Index::GetUnicode() const
{
    LEAF_CONTRACT;

    return (const WCHAR *) m_ptr;
}

inline const CHAR *SString::Index::GetASCII() const
{
    LEAF_CONTRACT;

    return (const CHAR *) m_ptr;
}

inline WCHAR SString::Index::operator*() const
{
    WRAPPER_CONTRACT;

    if (m_characterSizeShift == 0)
        return *(CHAR*)&GetAt(0);
    else
        return *(WCHAR*)&GetAt(0);
}

inline void SString::Index::operator->() const
{ 
    LEAF_CONTRACT;
}

inline WCHAR SString::Index::operator[](int index) const
{
    WRAPPER_CONTRACT;

    if (m_characterSizeShift == 0)
        return *(CHAR*)&GetAt(index);
    else
        return *(WCHAR*)&GetAt(index);
}

//-----------------------------------------------------------------------------
// Iterator support routines
//-----------------------------------------------------------------------------

inline SString::UIndex::UIndex()
{
    LEAF_CONTRACT;
}

inline SString::UIndex::UIndex(SString *string, SCOUNT_T index)
  : SBuffer::Index(string, index*sizeof(WCHAR))
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(string));
        PRECONDITION(string->IsRepresentation(REPRESENTATION_UNICODE));
        PRECONDITION(DoCheck(0));
        POSTCONDITION(CheckPointer(this));
        NOTHROW;
    }
    CONTRACT_END;

    RETURN;
}

inline WCHAR &SString::UIndex::GetAt(SCOUNT_T delta) const
{
    LEAF_CONTRACT;

    return ((WCHAR*)m_ptr)[delta];
}

inline void SString::UIndex::Skip(SCOUNT_T delta)
{
    LEAF_CONTRACT;

    m_ptr += delta * sizeof(WCHAR);
}

inline SCOUNT_T SString::UIndex::Subtract(const UIndex &i) const
{
    WRAPPER_CONTRACT;

    return (SCOUNT_T) (GetUnicode() - i.GetUnicode());
}

inline CHECK SString::UIndex::DoCheck(SCOUNT_T delta) const
{
    CANNOT_HAVE_CONTRACT;
#if _DEBUG
    const SString *string = (const SString *) GetContainerDebug();

    CHECK(GetUnicode() + delta >= string->GetRawUnicode());
    CHECK(GetUnicode() + delta <= string->GetRawUnicode() + string->GetCount());
#endif

    CHECK_OK;
}

inline WCHAR *SString::UIndex::GetUnicode() const
{
    LEAF_CONTRACT;

    return (WCHAR*) m_ptr;
}

//-----------------------------------------------------------------------------
// Opaque scratch buffer class routines
//-----------------------------------------------------------------------------
inline SString::AbstractScratchBuffer::AbstractScratchBuffer(void *buffer, COUNT_T size)
  : SString(buffer, size)
{
    CONTRACT_VOID
    {
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(buffer));
        PRECONDITION(CheckCount(size));
        NOTHROW;
    }
    CONTRACT_END;

    RETURN;
}

#endif  // _SSTRING_INL_

