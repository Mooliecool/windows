'*************************** Module Header ******************************'
' Module Name:  MiniDumpCreator.vb
' Project:      VBCreateMiniDump
' Copyright (c) Microsoft Corporation.
' 
' This class wraps the extern method MiniDumpWriteDump in dbghelp.dll.
'  
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Runtime.InteropServices
Imports Microsoft.Win32.SafeHandles

Namespace MiniDump

    Friend NotInheritable Class NativeMethods
        ''' <summary>
        ''' 
        ''' </summary>
        ''' <param name="hProcess">
        ''' A handle to the process for which the information is to be generated.
        ''' </param>
        ''' <param name="processId">
        ''' The identifier of the process for which the information is to be generated.
        ''' </param>
        ''' <param name="hFile">
        ''' A handle to the file in which the information is to be written
        ''' </param>
        ''' <param name="dumpType">
        ''' The type of information to be generated. This parameter can be one or more of
        ''' the values from the MINIDUMP_TYPE enumeration.
        ''' </param>
        ''' <param name="exceptionParam">
        ''' A pointer to a MINIDUMP_EXCEPTION_INFORMATION structure describing the client
        ''' exception that caused the minidump to be generated. If the value of this 
        ''' parameter is IntPtr.Zero, no exception information is included in the minidump
        ''' file.
        ''' </param>
        <DllImport("dbghelp.dll", CharSet:=CharSet.Auto, SetLastError:=True)>
        Public Shared Function MiniDumpWriteDump(ByVal hProcess As IntPtr,
                                                 ByVal processId As Integer,
                                                 ByVal hFile As SafeFileHandle,
                                                 ByVal dumpType As MINIDUMP_TYPE,
                                                 ByVal exceptionParam As IntPtr,
                                                 ByVal userStreamParam As IntPtr,
                                                 ByVal callbackParam As IntPtr) As Boolean
        End Function
    End Class

End Namespace
