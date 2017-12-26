'*************************** Module Header ******************************'
' Module Name:  MiniDumpCreator.vb
' Project:      VBCreateMiniDump
' Copyright (c) Microsoft Corporation.
' 
' This supplies a static method  CreateMiniDump to create a MiniDump with the specified
' parameters.
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

Imports System.IO
Imports System.Runtime.InteropServices
Imports System.Security.Permissions
Imports System.ComponentModel

Namespace MiniDump

    Public Class MiniDumpCreator

        ''' <summary>
        ''' Create a MiniDump with the specified parameters.
        ''' The MiniDump file is stored in the same folder as this application.
        ''' </summary>
        <PermissionSet(SecurityAction.LinkDemand, Name:="FullTrust")>
        Public Shared Function CreateMiniDump(ByVal targetProcessID As Integer,
                                              ByVal threadID As Integer,
                                              ByVal exceptionPointers As IntPtr) As String

            Dim targetProcess As Process = Process.GetProcessById(targetProcessID)

            If targetProcess Is Nothing Then
                Throw New ArgumentException("The specified process does not exist!")
            End If

            ' Construct this MiniDump file path.
            Dim dumpFilePath As String = String.Format("{0}\{1}_{2}.dmp",
                                                       Environment.CurrentDirectory,
                                                       targetProcess.ProcessName,
                                                       Date.Now.ToString("yyyy_MM_dd_HH_mm_ss"))

            Using fs As New FileStream(dumpFilePath, FileMode.CreateNew)
                Dim pExceptionParam As IntPtr = IntPtr.Zero

                ' If exceptionPointers is not null(IntPtr.Zero), then initialize an
                ' instance of MINIDUMP_EXCEPTION_INFORMATION.
                If exceptionPointers <> IntPtr.Zero Then
                    Dim mei As New MINIDUMP_EXCEPTION_INFORMATION()
                    mei.ExceptionPointers = exceptionPointers
                    mei.ThreadId = threadID
                    mei.ClientPointers = True

                    ' Allocate a block of memory of the MINIDUMP_EXCEPTION_INFORMATION
                    ' instance, and then marshal data from a managed object to an 
                    ' unmanaged block of memory.
                    pExceptionParam = Marshal.AllocCoTaskMem(Marshal.SizeOf(mei))
                    Marshal.StructureToPtr(mei, pExceptionParam, True)
                End If

                ' Create the MiniDump with the types MiniDumpWithIndirectlyReferencedMemory 
                ' and MiniDumpScanMemory.
                Dim result = NativeMethods.MiniDumpWriteDump(
                    targetProcess.Handle,
                    targetProcess.Id,
                    fs.SafeFileHandle,
                    MINIDUMP_TYPE.MiniDumpWithIndirectlyReferencedMemory Or MINIDUMP_TYPE.MiniDumpScanMemory,
                    pExceptionParam,
                    IntPtr.Zero,
                    IntPtr.Zero)

                If result Then
                    Return dumpFilePath
                Else
                    Throw New Win32Exception()
                End If
            End Using
        End Function

    End Class
End Namespace
