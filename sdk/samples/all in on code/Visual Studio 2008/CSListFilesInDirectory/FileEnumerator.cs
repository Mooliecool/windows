/************************************* Module Header **************************************\
* Module Name:  FileEnumerator.cs
* Project:      CSListFilesInDirectory
* Copyright (c) Microsoft Corporation.
* 
* The CSListFilesInDirectory project demonstrates how to implement an IEnumerable<string>
* that utilizes the Win32 File Management functions to enable application to get files and
* sub-directories in a specified directory one item a time.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/7/2009 8:00 PM Jie Wang Created
\******************************************************************************************/

#region Using directives
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.InteropServices;
#endregion


namespace CSListFilesInDirectory
{
    public class DirectoryEnumerator : IEnumerable<string>
    {
        #region The Enumerator

        public struct Enumerator : IEnumerator<string>
        {
            #region Private members

            private SafeFindHandle hFindFile;
            private string current;
            private string pattern;
            private Mode mode;

            #endregion

            #region .ctor

            internal Enumerator(string pattern, Mode mode)
            {
                this.pattern = pattern;
                this.current = null;
                this.hFindFile = null;
                this.mode = mode;
            }

            #endregion

            #region IEnumerator<string> Members

            public string Current
            {
                get { return current; }
            }

            #endregion

            #region IDisposable Members

            public void Dispose()
            {
                if (null != hFindFile)
                {
                    hFindFile.Close();
                }
            }

            #endregion

            #region IEnumerator Members

            object IEnumerator.Current
            {
                get { return this.Current; }
            }

            public bool MoveNext()
            {
                if (null == hFindFile)
                {
                    return FindFirst();
                }
                else
                {
                    return FindNext();
                }
            }

            public void Reset()
            {
                if (null != hFindFile)
                {
                    // close the find handle
                    hFindFile.Close();
                    hFindFile = null;
                }
            }

            #endregion

            #region Supporting methods

            /// <summary>
            /// Find the first match.
            /// </summary>
            /// <returns></returns>
            private bool FindFirst()
            {
                WIN32_FIND_DATA fd = new WIN32_FIND_DATA();

                hFindFile = NativeMethods.FindFirstFile(pattern, fd);

                if (hFindFile.IsInvalid)
                {
                    // Got an invalid find handle, get the error code
                    int code = Marshal.GetLastWin32Error();

                    if (code == NativeMethods.ERROR_FILE_NOT_FOUND)
                    {
                        // file not found, just return false
                        return false;
                    }

                    // other errors, throw exception
                    throw new Win32Exception(code);
                }

                if (!AttributesMatchMode(fd.dwFileAttributes))
                {
                    // if the file does not meet the match mode,
                    // go find the next match.
                    return FindNext();
                }

                current = fd.cFileName;
                return true;
            }

            private bool FindNext()
            {
                WIN32_FIND_DATA fd = new WIN32_FIND_DATA();

                while (NativeMethods.FindNextFile(hFindFile, fd))
                {
                    if (!AttributesMatchMode(fd.dwFileAttributes))
                    {
                        // if the file does not meet the match mode,
                        // go find the next match.
                        continue;
                    }

                    // found a match, return.
                    current = fd.cFileName;
                    return true;
                }
                
                int code = Marshal.GetLastWin32Error();

                if (code == NativeMethods.ERROR_NO_MORE_FILES)
                {
                    // no more files, return false.
                    return false;
                }
                
                // other errors, throw exception.
                throw new Win32Exception(code);
            }

            private bool AttributesMatchMode(int fileAttributes)
            {
                bool isDir = (fileAttributes & NativeMethods.FILE_ATTRIBUTE_DIRECTORY) 
                    == NativeMethods.FILE_ATTRIBUTE_DIRECTORY;

                return ((isDir && (mode & Mode.Directory) == Mode.Directory) ||
                    (!isDir && (mode & Mode.File) == Mode.File));
            }

            #endregion
        }

        #endregion

        #region FileEnumeratorMode

        [Flags]
        public enum Mode
        {
            /// <summary>
            /// Enumerate directories.
            /// </summary>
            Directory = 1,
            /// <summary>
            /// Enumerate files.
            /// </summary>
            File = 2
        }

        #endregion

        #region Private members

        private string pattern; // Search pattern
        private Mode mode;      // Enum mode

        #endregion

        #region .ctor

        public DirectoryEnumerator()
            : this("*.*")
        {
        }

        public DirectoryEnumerator(string pattern)
            : this(pattern, Mode.Directory | Mode.File)
        {            
        }

        public DirectoryEnumerator(string pattern, Mode mode)
        {
            this.pattern = pattern;
            this.mode = mode;
        }

        #endregion

        #region IEnumerable<string> Members

        IEnumerator<string> IEnumerable<string>.GetEnumerator()
        {
            return new Enumerator(pattern, mode);
        }

        #endregion

        #region IEnumerable Members

        IEnumerator IEnumerable.GetEnumerator()
        {
            return ((IEnumerable<string>)this).GetEnumerator();
        }

        #endregion
    }
}