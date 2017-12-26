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
/*============================================================
**
** Class:  Path
**
**
** Purpose: A collection of path manipulation methods.
**
**
===========================================================*/

using System;
using System.Security.Permissions;
using Win32Native = Microsoft.Win32.Win32Native;
using System.Text;
using System.Runtime.InteropServices;
using System.Security;
using System.Security.Cryptography;
using System.Runtime.CompilerServices;
using System.Globalization;
using System.Runtime.Versioning;

namespace System.IO {
    // Provides methods for processing directory strings in an ideally
    // cross-platform manner.  Most of the methods don't do a complete
    // full parsing (such as examining a UNC hostname), but they will
    // handle most string operations.  
    // 
    // File names cannot contain backslash (\), slash (/), colon (:),
    // asterick (*), question mark (?), quote ("), less than (<;), 
    // greater than (>;), or pipe (|).  The first three are used as directory
    // separators on various platforms.  Asterick and question mark are treated
    // as wild cards.  Less than, Greater than, and pipe all redirect input
    // or output from a program to a file or some combination thereof.  Quotes
    // are special.
    // 
    // We are guaranteeing that Path.SeparatorChar is the correct 
    // directory separator on all platforms, and we will support 
    // Path.AltSeparatorChar as well.  To write cross platform
    // code with minimal pain, you can use slash (/) as a directory separator in
    // your strings.
     // Class contains only static data, no need to serialize
    [ComVisible(true)]
    public static class Path
    {
        // Platform specific directory separator character.  This is backslash
        // ('\') on Windows, slash ('/') on Unix, and colon (':') on Mac.
        // 
#if !PLATFORM_UNIX        
        public static readonly char DirectorySeparatorChar = '\\';
#else
        public static readonly char DirectorySeparatorChar = '/';
#endif // !PLATFORM_UNIX
        
        // Platform specific alternate directory separator character.  
        // This is backslash ('\') on Unix, and slash ('/') on Windows 
        // and MacOS.
        // 
#if !PLATFORM_UNIX        
        public static readonly char AltDirectorySeparatorChar = '/';
#else
        public static readonly char AltDirectorySeparatorChar = '\\';
#endif // !PLATFORM_UNIX
    
        // Platform specific volume separator character.  This is colon (':')
        // on Windows and MacOS, and slash ('/') on Unix.  This is mostly
        // useful for parsing paths like "c:\windows" or "MacVolume:System Folder".  
        // 
#if !PLATFORM_UNIX
        public static readonly char VolumeSeparatorChar = ':';
#else
        public static readonly char VolumeSeparatorChar = '/';
#endif // !PLATFORM_UNIX        
        
        // Platform specific invalid list of characters in a path.
        // See the "Naming a File" MSDN conceptual docs for more details on
        // what is valid in a file name (which is slightly different from what
        // is legal in a path name).
        // Note: This list is duplicated in CheckInvalidPathChars
        [Obsolete("Please use GetInvalidPathChars or GetInvalidFileNameChars instead.")]
        public static readonly char[] InvalidPathChars = { '\"', '<', '>', '|', '\0', (Char)1, (Char)2, (Char)3, (Char)4, (Char)5, (Char)6, (Char)7, (Char)8, (Char)9, (Char)10, (Char)11, (Char)12, (Char)13, (Char)14, (Char)15, (Char)16, (Char)17, (Char)18, (Char)19, (Char)20, (Char)21, (Char)22, (Char)23, (Char)24, (Char)25, (Char)26, (Char)27, (Char)28, (Char)29, (Char)30, (Char)31 };


        private static readonly char[] RealInvalidPathChars = { '\"', '<', '>', '|', '\0', (Char)1, (Char)2, (Char)3, (Char)4, (Char)5, (Char)6, (Char)7, (Char)8, (Char)9, (Char)10, (Char)11, (Char)12, (Char)13, (Char)14, (Char)15, (Char)16, (Char)17, (Char)18, (Char)19, (Char)20, (Char)21, (Char)22, (Char)23, (Char)24, (Char)25, (Char)26, (Char)27, (Char)28, (Char)29, (Char)30, (Char)31 };

        private static readonly char[] InvalidFileNameChars = { '\"', '<', '>', '|', '\0', (Char)1, (Char)2, (Char)3, (Char)4, (Char)5, (Char)6, (Char)7, (Char)8, (Char)9, (Char)10, (Char)11, (Char)12, (Char)13, (Char)14, (Char)15, (Char)16, (Char)17, (Char)18, (Char)19, (Char)20, (Char)21, (Char)22, (Char)23, (Char)24, (Char)25, (Char)26, (Char)27, (Char)28, (Char)29, (Char)30, (Char)31, ':', '*', '?', '\\', '/' };

#if !PLATFORM_UNIX
        public static readonly char PathSeparator = ';';
#else
        public static readonly char PathSeparator = ':';
#endif // !PLATFORM_UNIX

        // Make this public sometime.
        // MaxPath accounts for the null-terminating character, for example, the maximum path on the D drive is "D:<256 chars>\0". 
        internal static readonly int MaxPath = 260;
    
        // Changes the extension of a file path. The path parameter
        // specifies a file path, and the extension parameter
        // specifies a file extension (with a leading period, such as
        // ".exe" or ".cs").
        //
        // The function returns a file path with the same root, directory, and base
        // name parts as path, but with the file extension changed to
        // the specified extension. If path is null, the function
        // returns null. If path does not contain a file extension,
        // the new file extension is appended to the path. If extension
        // is null, any exsiting extension is removed from path.
        //
        public static String ChangeExtension(String path, String extension) {
            if (path != null) {
                CheckInvalidPathChars(path);
    
                String s = path;
                for (int i = path.Length; --i >= 0;) {
                    char ch = path[i];
                    if (ch == '.') {
                        s = path.Substring(0, i);
                        break;
                    }
                    if (ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar) break;
                }
                if (extension != null && path.Length != 0) {
                    if (extension.Length == 0 || extension[0] != '.') {
                        s = s + ".";
                    }
                    s = s + extension;
                }
                return s;
            }
            return null;
        }

        
        // Returns the directory path of a file path. This method effectively
        // removes the last element of the given file path, i.e. it returns a
        // string consisting of all characters up to but not including the last
        // backslash ("\") in the file path. The returned value is null if the file
        // path is null or if the file path denotes a root (such as "\", "C:", or
        // "\\server\share").
        //
        public static String GetDirectoryName(String path) {
            if (path != null) {
                CheckInvalidPathChars(path);
                path = FixupPath(path);
                int root = GetRootLength(path);
                int i = path.Length;
                if (i > root) {
                    i = path.Length;
                    if (i == root) return null;
                    while (i > root && path[--i] != DirectorySeparatorChar && path[i] != AltDirectorySeparatorChar);
                    return path.Substring(0, i);
                }
            }
            return null;
        }

        // Gets the length of the root DirectoryInfo or whatever DirectoryInfo markers
        // are specified for the first part of the DirectoryInfo name.
        // 
        internal static int GetRootLength(String path) {
            CheckInvalidPathChars(path);
            
            int i = 0;
            int length = path.Length;

#if !PLATFORM_UNIX
            if (length >= 1 && (IsDirectorySeparator(path[0]))) {
                // handles UNC names and directories off current drive's root.
                i = 1;
                if (length >= 2 && (IsDirectorySeparator(path[1]))) {
                    i = 2;
                    int n = 2;
                    while (i < length && ((path[i] != DirectorySeparatorChar && path[i] != AltDirectorySeparatorChar) || --n > 0)) i++;
                }
            }
            else if (length >= 2 && path[1] == VolumeSeparatorChar) {
                // handles A:\foo.
                i = 2;
                if (length >= 3 && (IsDirectorySeparator(path[2]))) i++;
            }
            return i;
#else    
            if (length >= 1 && (IsDirectorySeparator(path[0]))) {
                i = 1;
            }
            return i;
#endif // !PLATFORM_UNIX
        }

        internal static bool IsDirectorySeparator(char c) {
            return (c==DirectorySeparatorChar || c == AltDirectorySeparatorChar);
        }


        public static char[] GetInvalidPathChars()
        {
            return (char[]) RealInvalidPathChars.Clone();
        }

        public static char[] GetInvalidFileNameChars()
        {
            return (char[]) InvalidFileNameChars.Clone();
        }

        // Returns the extension of the given path. The returned value includes the
        // period (".") character of the extension except when you have a terminal period when you get String.Empty, such as ".exe" or
        // ".cpp". The returned value is null if the given path is
        // null or if the given path does not include an extension.
        //
        public static String GetExtension(String path) {
            if (path==null)
                return null;

            CheckInvalidPathChars(path);
            int length = path.Length;
            for (int i = length; --i >= 0;) {
                char ch = path[i];
                if (ch == '.')
                {
                    if (i != length - 1)
                        return path.Substring(i, length - i);
                    else
                        return String.Empty;
                }
                if (ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar)
                    break;
            }
            return String.Empty;
        }

        // Expands the given path to a fully qualified path. The resulting string
        // consists of a drive letter, a colon, and a root relative path. This
        // function does not verify that the resulting path 
        // refers to an existing file or directory on the associated volume.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String GetFullPath(String path) {
            String fullPath = GetFullPathInternal(path);
            new FileIOPermission( FileIOPermissionAccess.PathDiscovery, new String[] { fullPath }, false, false ).Demand();
            return fullPath;
        }

        // This method is package access to let us quickly get a string name
        // while avoiding a security check.  This also serves a slightly
        // different purpose - when we open a file, we need to resolve the
        // path into a fully qualified, non-relative path name.  This
        // method does that, finding the current drive &; directory.  But
        // as long as we don't return this info to the user, we're good.  However,
        // the public GetFullPath does need to do a security check.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static String GetFullPathInternal(String path) {
            if (path == null)
                throw new ArgumentNullException("path");

            String newPath = NormalizePath(path, true);

            return newPath;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static String NormalizePath(String path, bool fullCheck) {
            return NormalizePathFast(path, fullCheck);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static String NormalizePathSlow(String path, bool fullCheck) {
            BCLDebug.Assert(path != null, "path can't be null");
            // If we're doing a full path check, trim whitespace and look for
            // illegal path characters.

            if (fullCheck) {
                // Trim whitespace off the end of the string.
                path = path.TrimEnd();

                // Look for illegal path characters.
                CheckInvalidPathChars(path);
            }

            int index = 0;
            char[] newBuffer = new char[MaxPath];
            int newBufferIndex = 0;
            char[] finalBuffer = null;
            uint numSpaces = 0;
            uint numDots = 0;
            bool fixupDirectorySeparator = false;
            // Number of significant chars other than potentially suppressible
            // dots and spaces since the last directory or volume separator char
            uint numSigChars = 0;
            int lastSigChar = -1; // Index of last significant character.
            // Whether this segment of the path (not the complete path) started
            // with a volume separator char.  Reject "c:...".
            bool startedWithVolumeSeparator = false;
            bool firstSegment = true;
            bool mightBeShortFileName = false;

#if !PLATFORM_UNIX
            // Win9x fixup - //server/share becomes c://server/share.
            // This prevents our code from turning "\\server" into "\server".
            // On Win9x, //server/share becomes c://server/share
            if (path.Length > 0 && (path[0] == DirectorySeparatorChar || path[0] == AltDirectorySeparatorChar)) {
                newBuffer[newBufferIndex++] = '\\';
                index++;
                lastSigChar = 0;
            }
#endif

            // Normalize the string, stripping out redundant dots, spaces, and 
            // slashes.
            while (index < path.Length) {
                char currentChar = path[index];

                // We handle both directory separators and dots specially.  For 
                // directory separators, we consume consecutive appearances.  
                // For dots, we consume all dots beyond the second in 
                // succession.  All other characters are added as is.  In 
                // addition we consume all spaces after the last other char
                // in a directory name up until the directory separator.

                if (currentChar == DirectorySeparatorChar || currentChar == AltDirectorySeparatorChar) {
                    // If we have a path like "123.../foo", remove the trailing dots.
                    // However, if we found "c:\temp\..\bar" or "c:\temp\...\bar", don't.
                    // Also remove trailing spaces from both files & directory names.
                    // This was agreed on with the OS team to fix undeletable directory
                    // names ending in spaces.

                    // If we saw a '\' as the previous last significant character and
                    // are simply going to write out dots, suppress them.
                    // If we only contain dots and slashes though, only allow
                    // a string like [dot]+ [space]*.  Ignore everything else.
                    // Legal: "\.. \", "\...\", "\. \"
                    // Illegal: "\.. .\", "\. .\", "\ .\"
                    if (numSigChars == 0) {
                        // Dot and space handling
                        if (numDots > 0) {
                            // Look for ".[space]*" or "..[space]*"
                            int start = lastSigChar + 1;
                            if (path[start] != '.')
                                throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

                            // Only allow "[dot]+[space]*", and normalize the 
                            // legal ones to "." or ".."
                            if (numDots >= 2) {
                                // Reject "C:..."
                                if (startedWithVolumeSeparator && numDots > 2)

                                    throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

                                if (path[start + 1] == '.') {
                                    // Search for a space in the middle of the
                                    // dots and throw
                                    for(int i=start + 2; i < start + numDots; i++) {
                                        if (path[i] != '.')
                                            throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));
                                    }

                                    numDots = 2;
                                }
                                else {
                                    if (numDots > 1)
                                        throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));
                                    numDots = 1;
                                }
                            }
                                    
                            if (newBufferIndex + numDots + 1 >= MaxPath)
                                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));

                            if (numDots == 2) {
                                newBuffer[newBufferIndex++] = '.';
                            }

                            newBuffer[newBufferIndex++] = '.';
                            fixupDirectorySeparator = false;

                            // Continue in this case, potentially writing out '\'.
                        }

                        if (numSpaces > 0 && firstSegment) {
                            // Handle strings like " \\server\share".
                            if (index + 1 < path.Length && 
                                (path[index + 1] == DirectorySeparatorChar || path[index + 1] == AltDirectorySeparatorChar))
                            {
                                newBuffer[newBufferIndex++] = DirectorySeparatorChar;
                            }
                        }
                    }
                    numDots = 0;
                    numSpaces = 0;  // Suppress trailing spaces

                    if (!fixupDirectorySeparator) {
                        fixupDirectorySeparator = true;

                        if (newBufferIndex + 1 >= MaxPath)
                            throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));

                        newBuffer[newBufferIndex++] = DirectorySeparatorChar;
                    }
                    numSigChars = 0;
                    lastSigChar = index;
                    startedWithVolumeSeparator = false;
                    firstSegment = false;

#if !PLATFORM_UNIX
                    // For short file names, we must try to expand each of them as
                    // soon as possible.  We need to allow users to specify a file
                    // name that doesn't exist using a path with short file names
                    // in it, such as this for a temp file we're trying to create:
                    // C:\DOCUME~1\USERNA~1.RED\LOCALS~1\Temp\bg3ylpzp
                    // We could try doing this afterwards piece by piece, but it's
                    // probably a lot simpler to do it here.
                    if (mightBeShortFileName) {
                        newBuffer[newBufferIndex] = '\0';
                        TryExpandShortFileName(newBuffer, ref newBufferIndex, MAX_PATH);
                        mightBeShortFileName = false;
                    }
#endif
                } // if (Found directory separator)
                else if (currentChar == '.') {
                    // Reduce only multiple .'s only after slash to 2 dots. For
                    // instance a...b is a valid file name.
                    numDots++;
                    // Don't flush out non-terminal spaces here, because they may in
                    // the end not be significant.  Turn "c:\ . .\foo" -> "c:\foo"
                    // which is the conclusion of removing trailing dots & spaces,
                    // as well as folding multiple '\' characters.
                }
                else if (currentChar == ' ') {
                    numSpaces++;
                }
                else {  // Normal character logic
#if !PLATFORM_UNIX
                    if (currentChar == '~')
                        mightBeShortFileName = true;
#endif

                    fixupDirectorySeparator = false;

#if !PLATFORM_UNIX
                    // To reject strings like "C:...\foo" and "C  :\foo"
                    if (firstSegment && currentChar == VolumeSeparatorChar) {
                        // Only accept "C:", not "c :" or ":"
                        // Get a drive letter or ' ' if index is 0.
                        char driveLetter = (index > 0) ? path[index-1] : ' ';
                        bool validPath = ((numDots == 0) && (numSigChars >= 1) && (driveLetter != ' '));
                        if (!validPath)
                            throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

                        startedWithVolumeSeparator = true;
                        // We need special logic to make " c:" work, we should not fix paths like "  foo::$DATA"
                        if (numSigChars > 1) { // Common case, simply do nothing
                            uint spaceCount = 0; // How many spaces did we write out, numSpaces has already been reset.
                            while((spaceCount < newBufferIndex) && newBuffer[spaceCount] == ' ')
                                spaceCount++;
                            if (numSigChars - spaceCount == 1) {
                                newBuffer[0] = driveLetter; // Overwrite spaces, we need a special case to not break "  foo" as a relative path.
                                newBufferIndex=1;
                            }
                        }
                        numSigChars = 0;
                    }
                    else
#endif // !PLATFORM_UNIX
                    {
                        numSigChars += 1 + numDots + numSpaces;
                    }

                    // Copy any spaces & dots since the last significant character
                    // to here.  Note we only counted the number of dots & spaces,
                    // and don't know what order they're in.  Hence the copy.
                    if (numDots > 0 || numSpaces > 0) {
                        int numCharsToCopy = (lastSigChar >= 0) ? index - lastSigChar - 1 : index;
                        if (numCharsToCopy > 0) {
                            path.CopyTo(lastSigChar + 1, newBuffer, newBufferIndex, numCharsToCopy);
                            newBufferIndex += numCharsToCopy;
                        }
                        numDots = 0;
                        numSpaces = 0;
                    }

                    if (newBufferIndex + 1 >= MaxPath)
                        throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));
                        
                    newBuffer[newBufferIndex++] = currentChar;
                    lastSigChar = index;
                }
                
                index++;
            } // end while

            // Drop any trailing dots and spaces from file & directory names, EXCEPT
            // we MUST make sure that "C:\foo\.." is correctly handled.
            // Also handle "C:\foo\." -> "C:\foo", while "C:\." -> "C:\"
            if (numSigChars == 0) {
                if (numDots > 0) {
                    // Look for ".[space]*" or "..[space]*"
                    int start = lastSigChar + 1;
                    if (path[start] != '.')
                        throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

                    // Only allow "[dot]+[space]*", and normalize the 
                    // legal ones to "." or ".."
                    if (numDots >= 2) {
                        // Reject "C:..."
                        if (startedWithVolumeSeparator && numDots > 2)
                            throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

                        if (path[start + 1] == '.') {
                            // Search for a space in the middle of the
                            // dots and throw
                            for(int i=start + 2; i < start + numDots; i++) {
                                if (path[i] != '.')
                                    throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));
                            }
                            
                            numDots = 2;
                        }
                        else {
                            if (numDots > 1)
                                throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));
                            numDots = 1;
                        }
                    }

                    if (newBufferIndex + numDots >= MaxPath)
                        throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));

                    if (numDots == 2) {
                        newBuffer[newBufferIndex++] = '.';
                    }

                    newBuffer[newBufferIndex++] = '.';
                }
            } // if (numSigChars == 0)

            // If we ended up eating all the characters, bail out.
            if (newBufferIndex == 0)
                throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

            BCLDebug.Assert(newBufferIndex <= MaxPath, "Overflowed temporary path buffer" );
            newBuffer[newBufferIndex] = '\0';

            // Disallow URL's here.  Some of our other Win32 API calls will reject
            // them later, so we might be better off rejecting them here.
            // Note we've probably turned them into "file:\D:\foo.tmp" by now.
            // But for compatibility, ensure that callers that aren't doing a 
            // full check aren't rejected here.
            if (fullCheck && 
                (CharArrayStartsWithOrdinal(newBuffer, newBufferIndex, "http:", false) ||
                 CharArrayStartsWithOrdinal(newBuffer, newBufferIndex, "file:", false)))
                throw new ArgumentException(Environment.GetResourceString("Argument_PathUriFormatNotSupported"));
            
#if !PLATFORM_UNIX
            // If the last part of the path (file or directory name) had a tilde,
            // expand that too.
            if (mightBeShortFileName) {
                TryExpandShortFileName(newBuffer, ref newBufferIndex, MaxPath);
            }
#endif

            // Call the Win32 API to do the final canonicalization step.
            int result = 1;
            char[] pFinal;
            int len;

            if (fullCheck) {
                
                finalBuffer = new char[MaxPath + 1];
                result = Win32Native.GetFullPathName(newBuffer, MaxPath + 1, finalBuffer, IntPtr.Zero);
                
                // If success, the return buffer length does not account for the terminating null character.
                // If failure, the return buffer length does account for the path + the terminating null character.
                if (result > MaxPath) {
                    finalBuffer = new char[result];
                    result = Win32Native.GetFullPathName(newBuffer, result, finalBuffer, IntPtr.Zero);

                    // Fullpath is genuinely long
                    if (result > MaxPath)
                        throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));
                }

                BCLDebug.Assert(result <= MaxPath, "did we accidently remove a PathTooLongException check?");
                if (result == 0 && newBuffer[0] != '\0') {
                    __Error.WinIOError();
                }
                else if (result < MaxPath)  // May be necessary for empty strings
                    finalBuffer[result] = '\0';
                pFinal = finalBuffer;
                len = result;

#if !PLATFORM_UNIX
                // If we called GetFullPathName with something like "foo" and our
                // command window was in short file name mode (ie, by running edlin or
                // DOS versions of grep, etc), we might have gotten back a short file
                // name.  So, check to see if we need to expand it.
                mightBeShortFileName = false;
                for(uint i=0; i<len && !mightBeShortFileName; i++) {
                    if (finalBuffer[i] == '~')
                        mightBeShortFileName = true;
                }
                if (mightBeShortFileName) {
                    bool r = TryExpandShortFileName(finalBuffer, ref len, MaxPath);
                    if (!r) {
                        int lastSlash = Array.LastIndexOf(finalBuffer, DirectorySeparatorChar, len - 1, len);
                        if (lastSlash >= 0) {
                            BCLDebug.Assert(lastSlash < len, "path unexpectedly ended in a '\'");
                            char[] savedName = new char[len - lastSlash - 1];
                            Array.Copy(finalBuffer, lastSlash + 1, savedName, 0, len - lastSlash - 1);
                            finalBuffer[lastSlash] = '\0';
                            r = TryExpandShortFileName(finalBuffer, ref lastSlash, MaxPath);
                            
                            // Clean up changes made to the finalBuffer.
                            finalBuffer[lastSlash] = DirectorySeparatorChar;
                            
                            Array.Copy(savedName, 0, finalBuffer, lastSlash + 1, savedName.Length);
                            if (r)
                                len = lastSlash + 1 + savedName.Length;
                        }
                    }
                }
#endif
            }
            else {
                pFinal = newBuffer;
                len = newBufferIndex;
            }

            if (result != 0) {
                /* Throw an ArgumentException for paths like \\, \\server, \\server\
                   This check can only be properly done after normalizing, so
                   \\foo\.. will be properly rejected.  Also, reject \\?\GLOBALROOT\
                   (an internal kernel path) because it provides aliases for drives. */
                if (pFinal[0] == '\\' && pFinal[1] == '\\') {
                    int startIndex = 2;
                    while (startIndex < result) {
                        if (pFinal[startIndex] == '\\') {
                            startIndex++;
                            break;
                        }
                        else {
                            startIndex++;
                        }
                    }
                    if (startIndex == result)
                        throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegalUNC"));

                    // Check for \\?\Globalroot, an internal mechanism to the kernel
                    // that provides aliases for drives and other undocumented stuff.
                    // The kernel team won't even describe the full set of what
                    // is available here - we don't want managed apps mucking 
                    // with this for security reasons.
                    if (CharArrayStartsWithOrdinal(pFinal, len, "\\\\?\\globalroot", true))
                        throw new ArgumentException(Environment.GetResourceString("Arg_PathGlobalRoot"));
                }
            }

            // Check our result and form the managed string as necessary.
            
            if (len >= MaxPath)
                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));

            if (result == 0) {
                int errorCode = Marshal.GetLastWin32Error();
                if (errorCode == 0)
                    errorCode = Win32Native.ERROR_BAD_PATHNAME;
                __Error.WinIOError(errorCode, path);
                return null;  // Unreachable - silence a compiler error.
            }

            return new String(pFinal, 0, len);
        }

        private static bool CharArrayStartsWithOrdinal(char[] array, int numChars, String compareTo, bool ignoreCase)
        {
            if (numChars < compareTo.Length)
                return false;

            if (ignoreCase) {
                String s = new String(array, 0, compareTo.Length);
                return compareTo.Equals(s, StringComparison.OrdinalIgnoreCase);
            }
            else {
                for(int i=0; i<compareTo.Length; i++) {
                    if (array[i] != compareTo[i]) {
                        return false;
                    }
                }
                return true;
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private static bool TryExpandShortFileName(char[] buffer, ref int bufferLength, int maxBufferSize)
        {
            BCLDebug.Assert(buffer != null, "buffer can't be null");

            // Allocate on the heap instead of the stack to simplify Win9x code path.
            char[] shortFileNameBuffer = new char[MaxPath + 1];

            int r = Win32Native.GetLongPathName(buffer, shortFileNameBuffer, MaxPath);
            
            if (r >= MaxPath)
                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));
            if (r == 0) {
                // Note: GetLongPathName will return ERROR_INVALID_FUNCTION on a 
                // path like \\.\PHYSICALDEVICE0 - some device driver doesn't 
                // support GetLongPathName on that string.  This behavior is 
                // by design, according to the Core File Services team.
                // We also get ERROR_NOT_ENOUGH_QUOTA in SQL_CLR_STRESS runs
                // intermittently on paths like D:\DOCUME~1\user\LOCALS~1\Temp\
                return false;
            }

            
            Buffer.BlockCopy(shortFileNameBuffer, 0, buffer, 0, 2 * r);
            bufferLength = r;
            buffer[bufferLength] = '\0';
            return true;
        }
        
        // This will update the path stack pointer after checking to ensure the index is bounded with in MaxPath
        private static unsafe void SafeSetStackPointerValue(char* buffer, int index, char value)
        {
            if (index >= MaxPath)
                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));

            buffer[index] = value;
        }
        
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        // This version allocates the path buffer on the stack instead of heap for perf
        internal unsafe static String NormalizePathFast(String path, bool fullCheck) {
            BCLDebug.Assert(path != null, "path can't be null");
            // If we're doing a full path check, trim whitespace and look for
            // illegal path characters.

            // Win9x: to fixup path to replace multiple slashes with a single slash
            if (fullCheck) {
                // Trim whitespace off the end of the string.
                path = path.TrimEnd();

                // Look for illegal path characters.
                CheckInvalidPathChars(path);
            }

            int index = 0;
            // Allocating this on the stack instead of heap for workingset/perf gain. 
            // We need to be careful when we are indexing on the pointer as the operations 
            // are unsafe and not protected by bounds check.
            char *newBuffer = stackalloc char[MaxPath]; 
            int newBufferIndex = 0;
            uint numSpaces = 0;
            uint numDots = 0;
            bool fixupDirectorySeparator = false;
            // Number of significant chars other than potentially suppressible
            // dots and spaces since the last directory or volume separator char
            uint numSigChars = 0;
            int lastSigChar = -1; // Index of last significant character.
            // Whether this segment of the path (not the complete path) started
            // with a volume separator char.  Reject "c:...".
            bool startedWithVolumeSeparator = false;
            bool firstSegment = true;
            bool mightBeShortFileName = false;

#if !PLATFORM_UNIX
            // Win9x fixup - //server/share becomes c://server/share.
            // This prevents our code from turning "\\server" into "\server".
            // On Win9x, //server/share becomes c://server/share
            if (path.Length > 0 && (path[0] == DirectorySeparatorChar || path[0] == AltDirectorySeparatorChar)) {
                SafeSetStackPointerValue(newBuffer, newBufferIndex++, '\\');
                index++;
                lastSigChar = 0;
            }
#endif

            // Normalize the string, stripping out redundant dots, spaces, and 
            // slashes.
            while (index < path.Length) {
                char currentChar = path[index];

                // We handle both directory separators and dots specially.  For 
                // directory separators, we consume consecutive appearances.  
                // For dots, we consume all dots beyond the second in 
                // succession.  All other characters are added as is.  In 
                // addition we consume all spaces after the last other char
                // in a directory name up until the directory separator.

                if (currentChar == DirectorySeparatorChar || currentChar == AltDirectorySeparatorChar) {
                    // If we have a path like "123.../foo", remove the trailing dots.
                    // However, if we found "c:\temp\..\bar" or "c:\temp\...\bar", don't.
                    // Also remove trailing spaces from both files & directory names.
                    // This was agreed on with the OS team to fix undeletable directory
                    // names ending in spaces.

                    // If we saw a '\' as the previous last significant character and
                    // are simply going to write out dots, suppress them.
                    // If we only contain dots and slashes though, only allow
                    // a string like [dot]+ [space]*.  Ignore everything else.
                    // Legal: "\.. \", "\...\", "\. \"
                    // Illegal: "\.. .\", "\. .\", "\ .\"
                    if (numSigChars == 0) {
                        // Dot and space handling
                        if (numDots > 0) {
                            // Look for ".[space]*" or "..[space]*"
                            int start = lastSigChar + 1;
                            if (path[start] != '.')
                                throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

                            // Only allow "[dot]+[space]*", and normalize the 
                            // legal ones to "." or ".."
                            if (numDots >= 2) {
                                // Reject "C:..."
                                if (startedWithVolumeSeparator && numDots > 2)

                                    throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

                                if (path[start + 1] == '.') {
                                    // Search for a space in the middle of the
                                    // dots and throw
                                    for(int i=start + 2; i < start + numDots; i++) {
                                        if (path[i] != '.')
                                            throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));
                                    }

                                    numDots = 2;
                                }
                                else {
                                    if (numDots > 1)
                                        throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));
                                    numDots = 1;
                                }
                            }
                                    
                            if (numDots == 2) {
                                SafeSetStackPointerValue(newBuffer, newBufferIndex++, '.');
                            }

                            SafeSetStackPointerValue(newBuffer, newBufferIndex++, '.');
                            fixupDirectorySeparator = false;

                            // Continue in this case, potentially writing out '\'.
                        }

                        if (numSpaces > 0 && firstSegment) {
                            // Handle strings like " \\server\share".
                            if (index + 1 < path.Length && 
                                (path[index + 1] == DirectorySeparatorChar || path[index + 1] == AltDirectorySeparatorChar))
                            {
                                SafeSetStackPointerValue(newBuffer, newBufferIndex++, DirectorySeparatorChar);
                            }
                        }
                    }
                    numDots = 0;
                    numSpaces = 0;  // Suppress trailing spaces

                    if (!fixupDirectorySeparator) {
                        fixupDirectorySeparator = true;
                        SafeSetStackPointerValue(newBuffer, newBufferIndex++, DirectorySeparatorChar);
                    }
                    numSigChars = 0;
                    lastSigChar = index;
                    startedWithVolumeSeparator = false;
                    firstSegment = false;

#if !PLATFORM_UNIX
                    // For short file names, we must try to expand each of them as
                    // soon as possible.  We need to allow people to specify a file
                    // name that doesn't exist using a path with short file names
                    // in it, such as this for a temp file we're trying to create:
                    // C:\DOCUME~1\USERNA~1.RED\LOCALS~1\Temp\bg3ylpzp
                    // We could try doing this afterwards piece by piece, but it's
                    // probably a lot simpler to do it here.
                    if (mightBeShortFileName) {
                        SafeSetStackPointerValue(newBuffer, newBufferIndex, '\0');
                        TryExpandShortFileName(newBuffer, ref newBufferIndex, MAX_PATH);
                        mightBeShortFileName = false;
                    }
#endif
                } // if (Found directory separator)
                else if (currentChar == '.') {
                    // Reduce only multiple .'s only after slash to 2 dots. For
                    // instance a...b is a valid file name.
                    numDots++;
                    // Don't flush out non-terminal spaces here, because they may in
                    // the end not be significant.  Turn "c:\ . .\foo" -> "c:\foo"
                    // which is the conclusion of removing trailing dots & spaces,
                    // as well as folding multiple '\' characters.
                }
                else if (currentChar == ' ') {
                    numSpaces++;
                }
                else {  // Normal character logic
#if !PLATFORM_UNIX
                    if (currentChar == '~')
                        mightBeShortFileName = true;
#endif

                    fixupDirectorySeparator = false;

#if !PLATFORM_UNIX
                    // To reject strings like "C:...\foo" and "C  :\foo"
                    if (firstSegment && currentChar == VolumeSeparatorChar) {
                        // Only accept "C:", not "c :" or ":"
                        // Get a drive letter or ' ' if index is 0.
                        char driveLetter = (index > 0) ? path[index-1] : ' ';
                        bool validPath = ((numDots == 0) && (numSigChars >= 1) && (driveLetter != ' '));
                        if (!validPath)
                            throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

                        startedWithVolumeSeparator = true;
                        // We need special logic to make " c:" work, we should not fix paths like "  foo::$DATA"
                        if (numSigChars > 1) { // Common case, simply do nothing
                            uint spaceCount = 0; // How many spaces did we write out, numSpaces has already been reset.
                            while((spaceCount < newBufferIndex) && newBuffer[spaceCount] == ' ')
                                spaceCount++;
                            if (numSigChars - spaceCount == 1) {
                                //Safe to update stack ptr directly
                                newBuffer[0] = driveLetter; // Overwrite spaces, we need a special case to not break "  foo" as a relative path.
                                newBufferIndex=1;
                            }
                        }
                        numSigChars = 0;
                    }
                    else 
#endif // !PLATFORM_UNIX
                    {
                        numSigChars += 1 + numDots + numSpaces;
                    }

                    // Copy any spaces & dots since the last significant character
                    // to here.  Note we only counted the number of dots & spaces,
                    // and don't know what order they're in.  Hence the copy.
                    if (numDots > 0 || numSpaces > 0) {
                        int numCharsToCopy = (lastSigChar >= 0) ? index - lastSigChar - 1 : index;
                        if (numCharsToCopy > 0) {
                            for (int i=0; i<numCharsToCopy; i++) {
                                SafeSetStackPointerValue(newBuffer, newBufferIndex++, path[lastSigChar + 1 + i]);
                            }
                        }
                        numDots = 0;
                        numSpaces = 0;
                    }

                    SafeSetStackPointerValue(newBuffer, newBufferIndex++, currentChar);
                    lastSigChar = index;
                }
                
                index++;
            } // end while

            // Drop any trailing dots and spaces from file & directory names, EXCEPT
            // we MUST make sure that "C:\foo\.." is correctly handled.
            // Also handle "C:\foo\." -> "C:\foo", while "C:\." -> "C:\"
            if (numSigChars == 0) {
                if (numDots > 0) {
                    // Look for ".[space]*" or "..[space]*"
                    int start = lastSigChar + 1;
                    if (path[start] != '.')
                        throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

                    // Only allow "[dot]+[space]*", and normalize the 
                    // legal ones to "." or ".."
                    if (numDots >= 2) {
                        // Reject "C:..."
                        if (startedWithVolumeSeparator && numDots > 2)
                            throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

                        if (path[start + 1] == '.') {
                            // Search for a space in the middle of the
                            // dots and throw
                            for(int i=start + 2; i < start + numDots; i++) {
                                if (path[i] != '.')
                                    throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));
                            }
                            
                            numDots = 2;
                        }
                        else {
                            if (numDots > 1)
                                throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));
                            numDots = 1;
                        }
                    }

                    if (numDots == 2) {
                        SafeSetStackPointerValue(newBuffer, newBufferIndex++, '.');
                    }

                    SafeSetStackPointerValue(newBuffer, newBufferIndex++, '.');
                }
            } // if (numSigChars == 0)

            // If we ended up eating all the characters, bail out.
            if (newBufferIndex == 0)
                throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegal"));

            // Null terminate the buffer. We will allow at most a path of length MaxPath-1 (accounting for the null terminating char). 
            SafeSetStackPointerValue(newBuffer, newBufferIndex, '\0');

            // Disallow URL's here.  Some of our other Win32 API calls will reject
            // them later, so we might be better off rejecting them here.
            // Note we've probably turned them into "file:\D:\foo.tmp" by now.
            // But for compatibility, ensure that callers that aren't doing a 
            // full check aren't rejected here.
            if (fullCheck && 
                (CharArrayStartsWithOrdinal(newBuffer, newBufferIndex, "http:", false) ||
                 CharArrayStartsWithOrdinal(newBuffer, newBufferIndex, "file:", false)))
                throw new ArgumentException(Environment.GetResourceString("Argument_PathUriFormatNotSupported"));
            
#if !PLATFORM_UNIX
            // If the last part of the path (file or directory name) had a tilde,
            // expand that too.
            if (mightBeShortFileName) {
                TryExpandShortFileName(newBuffer, ref newBufferIndex, MaxPath);
            }
#endif

            // Call the Win32 API to do the final canonicalization step.
            int result = 1;
            char *pFinal;
            int len;

            if (fullCheck) {
                
                
                char *finalBuffer = stackalloc char[MaxPath + 1];
                result = Win32Native.GetFullPathName(newBuffer, MaxPath + 1, finalBuffer, IntPtr.Zero);
                
                // If success, the return buffer length does not account for the terminating null character.
                // If in-sufficient buffer, the return buffer length does account for the path + the terminating null character.
                // If failure, the return buffer length is zero 
                if (result > MaxPath) {
                    char *tempBuffer = stackalloc char[result];
                    finalBuffer = tempBuffer;
                    result = Win32Native.GetFullPathName(newBuffer, result, finalBuffer, IntPtr.Zero);
                }

                // Fullpath is genuinely long
                if (result >= MaxPath)
                    throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));
                
                BCLDebug.Assert(result < MaxPath, "did we accidently remove a PathTooLongException check?");
                if (result == 0 && newBuffer[0] != '\0') {
                    __Error.WinIOError();
                }
                else if (result < MaxPath) {
                    // Null terminate explicitly (may be only needed for some cases such as empty strings)
                    // GetFullPathName return length doesn't account for null terminating char...
                    finalBuffer[result] = '\0'; // Safe to write directly as result is < MaxPath
                }
                
                pFinal = finalBuffer;
                // Doesn't account for null terminating char. Think of this as the last
                // valid index into the buffer but not the length of the buffer
                len = result;   

#if !PLATFORM_UNIX
                // If we called GetFullPathName with something like "foo" and our
                // command window was in short file name mode (ie, by running edlin or
                // DOS versions of grep, etc), we might have gotten back a short file
                // name.  So, check to see if we need to expand it.
                mightBeShortFileName = false;
                for(uint i=0; i<len && !mightBeShortFileName; i++) {
                    if (finalBuffer[i] == '~')
                        mightBeShortFileName = true;
                }

                if (mightBeShortFileName) {
                    bool r = TryExpandShortFileName(finalBuffer, ref len, MaxPath);
                    if (!r) {
                        int lastSlash = -1;
                        
                        for (int i=len-1; i>=0; i--) { 
                            if (finalBuffer[i] == DirectorySeparatorChar) {
                                lastSlash = i;
                                break;
                            }
                        }

                        if (lastSlash >= 0) {
                            
                            // This bounds check is for safe memcpy but we should never get this far 
                            if (len >= MaxPath)
                                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));

                            int lenSavedName = len - lastSlash - 1; 
                            BCLDebug.Assert(lastSlash < len, "path unexpectedly ended in a '\'");
                            char* savedName = stackalloc char[lenSavedName];

                            Buffer.memcpy(finalBuffer, lastSlash + 1, savedName, 0, lenSavedName);
                            
                            SafeSetStackPointerValue(finalBuffer, lastSlash, '\0');
                            r = TryExpandShortFileName(finalBuffer, ref lastSlash, MaxPath);
                            
                            // Clean up changes made to the finalBuffer.
                            SafeSetStackPointerValue(finalBuffer, lastSlash, DirectorySeparatorChar);
                            
                            if (lastSlash + 1 + lenSavedName >= MaxPath)
                                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));
                            
                            Buffer.memcpy(savedName, 0, finalBuffer, lastSlash + 1, lenSavedName);
                            
                            if (r)
                                len = lastSlash + 1 + lenSavedName;
                        }
                    }
                }
#endif
            }
            else {
                pFinal = newBuffer;
                len = newBufferIndex;
            }

            if (result != 0) {
                /* Throw an ArgumentException for paths like \\, \\server, \\server\
                   This check can only be properly done after normalizing, so
                   \\foo\.. will be properly rejected.  Also, reject \\?\GLOBALROOT\
                   (an internal kernel path) because it provides aliases for drives. */
                if (pFinal[0] == '\\' && pFinal[1] == '\\') {
                    int startIndex = 2;
                    while (startIndex < result) {
                        if (pFinal[startIndex] == '\\') {
                            startIndex++;
                            break;
                        }
                        else {
                            startIndex++;
                        }
                    }
                    if (startIndex == result)
                        throw new ArgumentException(Environment.GetResourceString("Arg_PathIllegalUNC"));

                    // Check for \\?\Globalroot, an internal mechanism to the kernel
                    // that provides aliases for drives and other undocumented stuff.
                    // The kernel team won't even describe the full set of what
                    // is available here - we don't want managed apps mucking 
                    // with this for security reasons.
                    if (CharArrayStartsWithOrdinal(pFinal, len, "\\\\?\\globalroot", true))
                        throw new ArgumentException(Environment.GetResourceString("Arg_PathGlobalRoot"));
                }
            }

            // Check our result and form the managed string as necessary.
            if (len >= MaxPath)
                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));

            if (result == 0) {
                int errorCode = Marshal.GetLastWin32Error();
                if (errorCode == 0)
                    errorCode = Win32Native.ERROR_BAD_PATHNAME;
                __Error.WinIOError(errorCode, path);
                return null;  // Unreachable - silence a compiler error.
            }

            return new String(pFinal, 0, len);
        }

        private static unsafe bool CharArrayStartsWithOrdinal(char* array, int numChars, String compareTo, bool ignoreCase)
        {
            if (numChars < compareTo.Length)
                return false;
            if (ignoreCase) {
                String s = new String(array, 0, compareTo.Length);
                return compareTo.Equals(s, StringComparison.OrdinalIgnoreCase);
            }
            else {
                for(int i=0; i<compareTo.Length; i++) {
                    if (array[i] != compareTo[i]) {
                        return false;
                    }
                }
                return true;
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private unsafe static bool TryExpandShortFileName(char* buffer, ref int bufferLength, int maxBufferSize)
        {
            BCLDebug.Assert(buffer != null, "buffer can't be null");

            char* shortFileNameBuffer = stackalloc char[MaxPath + 1];

            int r = Win32Native.GetLongPathName(buffer, shortFileNameBuffer, MaxPath);
            
            // If success, the return buffer length does not account for the terminating null character.
            // If in-sufficient buffer, the return buffer length does account for the path + the terminating null character.
            // If failure, the return buffer length is zero 
            if (r >= MaxPath)
                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));
            if (r == 0) {
                // Note: GetLongPathName will return ERROR_INVALID_FUNCTION on a 
                // path like \\.\PHYSICALDEVICE0 - some device driver doesn't 
                // support GetLongPathName on that string.  This behavior is 
                // by design, according to the Core File Services team.
                // We also get ERROR_NOT_ENOUGH_QUOTA in SQL_CLR_STRESS runs
                // intermittently on paths like D:\DOCUME~1\user\LOCALS~1\Temp\
                return false;
            }

            // Safe to copy as we have already done MaxPath bound checking 
            Buffer.memcpy(shortFileNameBuffer, 0, buffer, 0, r);
            bufferLength = r;
            // We should explicitly null terminate as in some cases the long version of the path 
            // might actually be shorter than what we started with because of Win32's normailization
            // Safe to write directly as bufferLength is guaranteed to be < MaxPath
            buffer[bufferLength] = '\0';  
            return true;
        }

        // Win9x: to fixup path to replace multiple slashes with a single slash
        // This is marked as exposing no resources because it calls 
        // NormalizePath and passes in false for fullCheck.  This is mostly
        // used to simply clean up the format of the string, but not to fully
        // qualify it.
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        internal static String FixupPath(String path)
        {
            String newPath = NormalizePath(path, false);
            return newPath;
        }
        
        // Returns the name and extension parts of the given path. The resulting
        // string contains the characters of path that follow the last
        // backslash ("\"), slash ("/"), or colon (":") character in 
        // path. The resulting string is the entire path if path 
        // contains no backslash after removing trailing slashes, slash, or colon characters. The resulting 
        // string is null if path is null.
        //
        public static String GetFileName(String path) {
          if (path != null) {
                CheckInvalidPathChars(path);
    
                int length = path.Length;
                for (int i = length; --i >= 0;) {
                    char ch = path[i];
                    if (ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar)
                        return path.Substring(i + 1, length - i - 1);

                }
            }
            return path;
        }

        public static String GetFileNameWithoutExtension(String path) {
            path = GetFileName(path);
            if (path != null)
            {
                int i;
                if ((i=path.LastIndexOf('.')) == -1)
                    return path; // No path extension found
                else
                    return path.Substring(0,i);
            }
            return null;
         }



        // Returns the root portion of the given path. The resulting string
        // consists of those rightmost characters of the path that constitute the
        // root of the path. Possible patterns for the resulting string are: An
        // empty string (a relative path on the current drive), "\" (an absolute
        // path on the current drive), "X:" (a relative path on a given drive,
        // where X is the drive letter), "X:\" (an absolute path on a given drive),
        // and "\\server\share" (a UNC path for a given server and share name).
        // The resulting string is null if path is null.
        //
        public static String GetPathRoot(String path) {
            if (path == null) return null;
            path = FixupPath(path);
            return path.Substring(0, GetRootLength(path));
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String GetTempPath()
        {
            new EnvironmentPermission(PermissionState.Unrestricted).Demand();
            StringBuilder sb = new StringBuilder(MAX_PATH);
            uint r = Win32Native.GetTempPath(MAX_PATH, sb);
            String path = sb.ToString();
            if (r==0) __Error.WinIOError();
            path = GetFullPathInternal(path);
            return path;
        }
                
        // Returns a cryptographically strong random 8.3 string that can be 
        // used as either a folder name or a file name.
        public static String GetRandomFileName()
        {
            // 5 bytes == 40 bits == 40/5 == 8 chars in our encoding
            // This gives us exactly 8 chars. We want to avoid the 8.3 short name issue
            byte[] key = new byte[10];
            RNGCryptoServiceProvider rng = new RNGCryptoServiceProvider();
            rng.GetBytes(key);
            // rndCharArray is expected to be 16 chars
            char[] rndCharArray = System.IO.IsolatedStorage.IsolatedStorage.ToBase32StringSuitableForDirName(key).ToCharArray();
            rndCharArray[8] = '.';
            return new String(rndCharArray, 0, 12);
        }

        // Returns a unique temporary file name, and creates a 0-byte file by that
        // name on disk.
        [ResourceExposure(ResourceScope.AppDomain)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String GetTempFileName()
        {
            String path = GetTempPath();

            // Since this can write to the temp directory and theoretically 
            // cause a denial of service attack, demand FileIOPermission to 
            // that directory.
            new FileIOPermission(FileIOPermissionAccess.Write, path).Demand();

            StringBuilder sb = new StringBuilder(MAX_PATH);
            uint r = Win32Native.GetTempFileName(path, "tmp", 0, sb);
            if (r==0) __Error.WinIOError();
            return sb.ToString();
        }
    
        // Tests if a path includes a file extension. The result is
        // true if the characters that follow the last directory
        // separator ('\\' or '/') or volume separator (':') in the path include 
        // a period (".") other than a terminal period. The result is false otherwise.
        //
        public static bool HasExtension(String path) {
            if (path != null) {
                CheckInvalidPathChars(path);
                
                for (int i = path.Length; --i >= 0;) {
                    char ch = path[i];
                    if (ch == '.') {
                        if ( i != path.Length - 1)
                            return true;
                        else
                            return false;
                    }
                    if (ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar) break;
                }
            }
            return false;
        }
    
    
        // Tests if the given path contains a root. A path is considered rooted
        // if it starts with a backslash ("\") or a drive letter and a colon (":").
        //
        public static bool IsPathRooted(String path) {
            if (path != null) {
                CheckInvalidPathChars(path);
    
                int length = path.Length;
                if ((length >= 1 && (path[0] == DirectorySeparatorChar || path[0] == AltDirectorySeparatorChar))
#if !PLATFORM_UNIX                       
                    || (length >= 2 && path[1] == VolumeSeparatorChar)
#endif
                    ) return true;
            }
            return false;
        }

        public static String Combine(String path1, String path2) {
            if (path1==null || path2==null)
                throw new ArgumentNullException((path1==null) ? "path1" : "path2");
            CheckInvalidPathChars(path1);
            CheckInvalidPathChars(path2);
            
            if (path2.Length == 0)
                return path1;

            if (path1.Length == 0)
                return path2;
                
            if (IsPathRooted(path2))
                return path2;

            char ch = path1[path1.Length - 1];
            if (ch != DirectorySeparatorChar && ch != AltDirectorySeparatorChar && ch != VolumeSeparatorChar) 
                return path1 + DirectorySeparatorChar + path2;
            return path1 + path2;
        }


        // ".." can only be used if it is specified as a part of a valid File/Directory name. We disallow
        //  the user being able to use it to move up directories. Here are some examples eg 
        //    Valid: a..b  abc..d
        //    Invalid: ..ab   ab..  ..   abc..d\abc..
        //
        internal static void CheckSearchPattern(String searchPattern)
        {
            int index;
            while ((index = searchPattern.IndexOf("..", StringComparison.Ordinal)) != -1) {
                    
                 if (index + 2 == searchPattern.Length) // Terminal ".." . Files names cannot end in ".."
                    throw new ArgumentException(Environment.GetResourceString("Arg_InvalidSearchPattern"));
                
                 if ((searchPattern[index+2] ==  DirectorySeparatorChar)
                    || (searchPattern[index+2] == AltDirectorySeparatorChar))
                    throw new ArgumentException(Environment.GetResourceString("Arg_InvalidSearchPattern"));
                
                searchPattern = searchPattern.Substring(index + 2);
            }

        }

        internal static void CheckInvalidPathChars(String path)
        {
#if PLATFORM_UNIX            
            if( path.StartsWith("\\\\"))
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidPathChars"));
#endif // PLATFORM_UNIX

            for (int i = 0; i < path.Length; i++)
            {
                int c = path[i];

                // Note: This list is duplicated in static char[] InvalidPathChars
                if (c == '\"' || c == '<' || c == '>' || c == '|' || c < 32)
                    throw new ArgumentException(Environment.GetResourceString("Argument_InvalidPathChars"));
            }
        }

        
        internal static String InternalCombine(String path1, String path2) {
            if (path1==null || path2==null)
                throw new ArgumentNullException((path1==null) ? "path1" : "path2");
            CheckInvalidPathChars(path1);
            CheckInvalidPathChars(path2);
            
            if (path2.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_PathEmpty"), "path2");
            if (IsPathRooted(path2))
                throw new ArgumentException(Environment.GetResourceString("Arg_Path2IsRooted"), "path2");
            int i = path1.Length;
            if (i == 0) return path2;
            char ch = path1[i - 1];
            if (ch != DirectorySeparatorChar && ch != AltDirectorySeparatorChar && ch != VolumeSeparatorChar) 
                return path1 + DirectorySeparatorChar + path2;
            return path1 + path2;
        }

    
        // Windows API definitions
        internal const int MAX_PATH = 260;  // From WinDef.h
        internal const int MAX_DIRECTORY_PATH = 248;   // cannot create directories greater than 248 characters
    }
}
