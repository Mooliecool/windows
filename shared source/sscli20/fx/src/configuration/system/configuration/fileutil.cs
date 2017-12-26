//------------------------------------------------------------------------------
// <copyright file="FileUtil.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.Configuration {
    using System;
    using System.IO;
    using Microsoft.Win32;
    using System.Runtime.InteropServices;

    static internal class FileUtil {
        const int HRESULT_WIN32_FILE_NOT_FOUND = unchecked((int)0x80070002);
        const int HRESULT_WIN32_PATH_NOT_FOUND = unchecked((int)0x80070003);

        //
        // Use to avoid the perf hit of a Demand when the Demand is not necessary for security.
        // 
        // If trueOnError is set, then return true if we cannot confirm that the file does NOT exist.
        //
        internal static bool FileExists(string filename, bool trueOnError) {
            UnsafeNativeMethods.WIN32_FILE_ATTRIBUTE_DATA data;
            bool ok = UnsafeNativeMethods.GetFileAttributesEx(filename, UnsafeNativeMethods.GetFileExInfoStandard, out data);
            if (ok) {
                // The path exists. Return true if it is a file, false if a directory.
                return (data.fileAttributes & (int) FileAttributes.Directory) != (int) FileAttributes.Directory;
            }
            else {
                if (!trueOnError) {
                    return false;
                }
                else {
                    // Return true if we cannot confirm that the file does NOT exist.
                    int hr = Marshal.GetHRForLastWin32Error();
                    if (hr == HRESULT_WIN32_FILE_NOT_FOUND || hr == HRESULT_WIN32_PATH_NOT_FOUND) {
                        return false;
                    }
                    else {
                        return true;
                    }
                }
            }
        }
    }
}

