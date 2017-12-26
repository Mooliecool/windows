//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  File and directory path utilities.
//
//-------------------------------------------------------------------------------------------------

#pragma once

#define _DEFAULTPATHSEPARATOR   L'\\'
#define _DEFAULTEXTSEPARATOR    L'.'

//---------------------------------------------------------------------------
// @@ Path utilities
//
// These are general purpose file-path manipulation routines.  The convention
// is that the caller must allocate enough space for the path.  Typically this
// is MAX_PATH characters.
//
// Whenever a directory is specified, then the trailing backslash (\) should
// not be included as part of the string.
//---------------------------------------------------------------------------

// A cate----zation of path types
enum PathType
{
    PathUNC,    // fully qualified UNC path
    PathFull,   // fully qualified path
    PathRelative,   // relative path
    PathEmpty,
    PathFileURL
};

// Given a path, return one of the PathType constants.  A path is considered
// to be full if it starts with <any-char>:.  A path is a UNC path if it
// starts with \\.  All other paths are considered to be relative paths.
PathType PathGuessType(
    const WCHAR * wszPath,
    const WCHAR wchPathSeparator = _DEFAULTPATHSEPARATOR);

// Given a directory or filename, add a directory or filename to the path.
// This is like WszCat, only it makes sure that a '\\' separator is added
// if necessary.  The return value is wszPath passed in.  For the general
// case, the wszPath buffer should be MAX_PATH * 2 characters.
// [....]: 10/26/2004:  It's wrong to assume that a path is a given size.  Added a specific parameter to cover this.
WCHAR * PathCatName(
    _Inout_opt_cap_(cchPathBuffer)_Prepost_count_(cchPathBuffer)WCHAR * wszPath,
    size_t cchPathBuffer,
    _In_z_ const WCHAR * wszName,
    const WCHAR wchPathSeparator = _DEFAULTPATHSEPARATOR);

// Given a full or partial file / directory path, return a pointer to the
// the filename / directory name portion of the path.
//   "c:\bar\foo.txt"   returns "foo.txt"
//   "c:\"        returns    ""
//   "c:foo"        returns "foo"
//   "c:..\foo"        returns "foo"
//   "c:.."        returns ".."
//   "\\bar\foo"        returns "foo"
//   "\\bar\foo\----" returns "\----"
//   "bar\foo.txt"    returns "bar\foo.txt"
//   "foo.txt"        returns "foo.txt"
WCHAR * PathFindName(
    const WCHAR * wszPath,
    const WCHAR wchPathSeparator = _DEFAULTPATHSEPARATOR);

// Given a full or partial file / directory path, return a pointer to
// the extension portion of the filename.  This includes the period.
// If there is no extension, then the pointer is to the end of the
// string.
//   "c:\bar\foo.txt"      returns ".txt"
//   "c:\"          returns ""
//   "c:\foo"          returns ""
//   "c:\bar\foo.bar.txt" returns ".txt"
//   "c:\bar.foo\bar"      returns ""
WCHAR * PathFindExt(
    const WCHAR * wszPath,
    const WCHAR wchPathSeparator = _DEFAULTPATHSEPARATOR,
    const WCHAR wchExtSeparator = _DEFAULTEXTSEPARATOR);

// Given a full or partial file / directory path, return a pointer to the
// the first character in the path that does not correspond to the device:
//   "c:\bar\foo.txt"   returns "\bar\foo.txt"
//   "c:\"        returns    "\"
//   "c:foo"        returns "foo"
//   "c:..\foo"        returns "..\foo"
//   "c:.."        returns ".."
//   "\\bar\foo"        returns ""
//   "\\bar\foo\----" returns "\----"
//   "bar\foo.txt"    returns "bar\foo.txt"
//   "foo.txt"        returns "foo.txt"
WCHAR * PathFindDevice(
    const WCHAR * wszPath,
    const WCHAR wchPathSeparator = _DEFAULTPATHSEPARATOR);

// Given a path pointing to a file or directory, shorten the path string to
// to be just the path of the parent directory.  If there is no parent
// directory, then it has no effect on the buffer.  The pointer passed
// in is returned.  In no cases will the truncated path have a '\\' at
// the end of it. For example:
//
//    "c:\a\b\c"  returns "c:\a\b"
//    "\\a\\b"      returns "\\a\\b"
//    "\\a\\b\c"  returns "\\a\\b"
//    "c:\a\b"      returns "c:\a"
//    "c:\"      returns "c:
//    "c:\a"      returns "c:
//
// This returns whether or not the buffer was modified.
BOOL PathMakeDir(
    _Inout_opt_z_ WCHAR * wszPath,
    const WCHAR wchPathSeparator = _DEFAULTPATHSEPARATOR);

// Takes the (possibly-qualified) file name in pwszFileName and puts a canonical version
// of the file name (no path, all lowercase) into pwszCanonicalBuf, which must be at
// least cch Unicode characters in size.  The function modifies and returns pwszCanonicalBuf.
WCHAR * GetCanonicalFileName(
    _In_opt_z_ const WCHAR * pwszFileName,
    _Out_opt_cap_(cch)WCHAR * pwszCanonicalBuf,
    unsigned cch,
    const WCHAR wchPathSeparator = _DEFAULTPATHSEPARATOR);

unsigned VB_W_GetCommandLine(
    _Out_opt_cap_(SizeOfBuffer)WCHAR * pwszBufferForCommandLine,
    unsigned SizeOfBuffer);

bool IsIllegalFileNameChar(const WCHAR wch);
