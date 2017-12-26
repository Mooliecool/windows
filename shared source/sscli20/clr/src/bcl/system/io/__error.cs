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
** Class:  __Error
**
**
** Purpose: Centralized error methods for the IO package.  
** Mostly useful for translating Win32 HRESULTs into meaningful
** error strings & exceptions.
**
**
===========================================================*/

using System;
using System.Runtime.InteropServices;
using Win32Native = Microsoft.Win32.Win32Native;
using System.Text;
using System.Globalization;
using System.Security;
using System.Security.Permissions;

namespace System.IO {
    // Only static data no need to serialize
    internal static class __Error
    {
        internal static void EndOfFile() {
            throw new EndOfStreamException(Environment.GetResourceString("IO.EOF_ReadBeyondEOF"));
        }

        internal static void FileNotOpen() {
            throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_FileClosed"));
        }

        internal static void StreamIsClosed() {
            throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_StreamClosed"));
        }
    
        internal static void MemoryStreamNotExpandable() {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_MemStreamNotExpandable"));
        }
    
        internal static void ReaderClosed() {
            throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_ReaderClosed"));
        }

        internal static void ReadNotSupported() {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_UnreadableStream"));
        }
    
        internal static void SeekNotSupported() {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_UnseekableStream"));
        }

        internal static void WrongAsyncResult() {
            throw new ArgumentException(Environment.GetResourceString("Arg_WrongAsyncResult"));
        }

        internal static void EndReadCalledTwice() {
            // Should ideally be InvalidOperationExc but we can't maitain parity with Stream and FileStream without some work
            throw new ArgumentException(Environment.GetResourceString("InvalidOperation_EndReadCalledMultiple"));
        }

        internal static void EndWriteCalledTwice() {
            // Should ideally be InvalidOperationExc but we can't maintain parity with Stream and FileStream without some work
            throw new ArgumentException(Environment.GetResourceString("InvalidOperation_EndWriteCalledMultiple"));
        }

        // Given a possible fully qualified path, ensure that we have path
        // discovery permission to that path.  If we do not, return just the 
        // file name.  If we know it is a directory, then don't return the 
        // directory name.
        internal static String GetDisplayablePath(String path, bool isInvalidPath)
        {
            if (String.IsNullOrEmpty(path))
                return path;

            // Is it a fully qualified path?
            bool isFullyQualified = false;
            if (path.Length < 2)
                return path;
            if (Path.IsDirectorySeparator(path[0]) && Path.IsDirectorySeparator(path[1]))
                isFullyQualified = true;
            else if (path[1] == Path.VolumeSeparatorChar) {
                isFullyQualified = true;
            }

            if (!isFullyQualified && !isInvalidPath)
                return path;

            bool safeToReturn = false;
            try {
                if (!isInvalidPath) {
                    new FileIOPermission(FileIOPermissionAccess.PathDiscovery, new String[] { path }, false, false).Demand();
                    safeToReturn = true;
                }
            }
            catch(ArgumentException) {
                // Needed for Win9x
            }
            catch(NotSupportedException) {
                // Needed for Win9x, which will sometimes return 
                // ERROR_PATH_NOT_FOUND for invalid path names, but also 
                // will leak the current working directory.
            }
            catch(SecurityException) {
            }
            
            if (!safeToReturn) {
                if (Path.IsDirectorySeparator(path[path.Length - 1]))
                    path = Environment.GetResourceString("IO.IO_NoPermissionToDirectoryName");
                else
                    path = Path.GetFileName(path);
            }

            return path;
        }

    	internal static void WinIOError() {
            int errorCode = Marshal.GetLastWin32Error();
            WinIOError(errorCode, String.Empty);
        }
    
        // After calling GetLastWin32Error(), it clears the last error field,
        // so you must save the HResult and pass it to this method.  This method
        // will determine the appropriate exception to throw dependent on your 
        // error, and depending on the error, insert a string into the message 
        // gotten from the ResourceManager.
        internal static void WinIOError(int errorCode, String maybeFullPath) {
            // This doesn't have to be perfect, but is a perf optimization.
            bool isInvalidPath = errorCode == Win32Native.ERROR_INVALID_NAME || errorCode == Win32Native.ERROR_BAD_PATHNAME;
            String str = GetDisplayablePath(maybeFullPath, isInvalidPath);

            switch (errorCode) {
            case Win32Native.ERROR_FILE_NOT_FOUND:
                if (str.Length == 0)
                    throw new FileNotFoundException(Environment.GetResourceString("IO.FileNotFound"));
                else
                    throw new FileNotFoundException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("IO.FileNotFound_FileName"), str), str);
                
            case Win32Native.ERROR_PATH_NOT_FOUND:
                if (str.Length == 0)
                    throw new DirectoryNotFoundException(Environment.GetResourceString("IO.PathNotFound_NoPathName"));
                else
                    throw new DirectoryNotFoundException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("IO.PathNotFound_Path"), str));

            case Win32Native.ERROR_ACCESS_DENIED:
                if (str.Length == 0)
                    throw new UnauthorizedAccessException(Environment.GetResourceString("UnauthorizedAccess_IODenied_NoPathName"));
                else
                    throw new UnauthorizedAccessException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("UnauthorizedAccess_IODenied_Path"), str));

            case Win32Native.ERROR_ALREADY_EXISTS:
                if (str.Length == 0)
                    goto default;
                throw new IOException(Environment.GetResourceString("IO.IO_AlreadyExists_Name", str), Win32Native.MakeHRFromErrorCode(errorCode), maybeFullPath);

            case Win32Native.ERROR_FILENAME_EXCED_RANGE:
                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));

            case Win32Native.ERROR_INVALID_DRIVE:
                throw new DriveNotFoundException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("IO.DriveNotFound_Drive"), str));

            case Win32Native.ERROR_INVALID_PARAMETER:
                throw new IOException(Win32Native.GetMessage(errorCode), Win32Native.MakeHRFromErrorCode(errorCode), maybeFullPath);

            case Win32Native.ERROR_SHARING_VIOLATION:
                if (str.Length == 0)
                    throw new IOException(Environment.GetResourceString("IO.IO_SharingViolation_NoFileName"), Win32Native.MakeHRFromErrorCode(errorCode), maybeFullPath);
                else
                    throw new IOException(Environment.GetResourceString("IO.IO_SharingViolation_File", str), Win32Native.MakeHRFromErrorCode(errorCode), maybeFullPath);

            case Win32Native.ERROR_FILE_EXISTS:
                if (str.Length == 0)
                    goto default;
                throw new IOException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("IO.IO_FileExists_Name"), str), Win32Native.MakeHRFromErrorCode(errorCode), maybeFullPath);

            case Win32Native.ERROR_OPERATION_ABORTED:
                throw new OperationCanceledException();

            default:
                throw new IOException(Win32Native.GetMessage(errorCode), Win32Native.MakeHRFromErrorCode(errorCode), maybeFullPath);
            }
        }

        // An alternative to WinIOError with friendlier messages for drives
        internal static void WinIODriveError(String driveName) {
            int errorCode = Marshal.GetLastWin32Error();
            WinIODriveError(driveName, errorCode);
        }

        internal static void WinIODriveError(String driveName, int errorCode)
        {
            switch (errorCode) {
            case Win32Native.ERROR_PATH_NOT_FOUND:
            case Win32Native.ERROR_INVALID_DRIVE:
                throw new DriveNotFoundException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("IO.DriveNotFound_Drive"), driveName));

            default: 
                WinIOError(errorCode, driveName);                
                break;
            }
        }
    
        internal static void WriteNotSupported() {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_UnwritableStream"));
        }

        internal static void WriterClosed() {
            throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_WriterClosed"));
        }

        // From WinError.h
        internal const int ERROR_FILE_NOT_FOUND = Win32Native.ERROR_FILE_NOT_FOUND;
        internal const int ERROR_PATH_NOT_FOUND = Win32Native.ERROR_PATH_NOT_FOUND;
        internal const int ERROR_ACCESS_DENIED  = Win32Native.ERROR_ACCESS_DENIED;
        internal const int ERROR_INVALID_PARAMETER = Win32Native.ERROR_INVALID_PARAMETER;
    }
}
