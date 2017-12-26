'************************** Module Header ******************************'
' Module Name:  RunningProcess.vb
' Project:      VBCheckProcessType
' Copyright (c) Microsoft Corporation.
' 
' This class represents a running process, and determines whether this process 
' is a 64bit ptocess, managed process, .NET 4.0 process, WPF process or console
' process.
' 
' To determine whether a process is a managed process, we can check whether the 
' .Net Runtime Execution engine MSCOREE.dll is loaded.
' 
' To determine whether a process is a managed process, we can check whether the 
' CLR.dll is loaded. Before .Net 4.0, the workstation CLR runtime is called 
' MSCORWKS.DLL. In .Net 4.0, this DLL is replaced by CLR.dll. 
' 
' To determine whether a process is a WPF process, we can check whether the 
' PresentationCore.dll is loaded.
' 
' To determine whether a process is a console process, we can check whether
' the target process has a console window.
'
'
'This source is subject to the Microsoft Public License.
'See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'All other rights reserved.
'
'THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.Linq
Imports System.Security.Permissions
Imports System.Text
Imports System.Runtime.InteropServices
Imports System.IO
Imports System.ComponentModel

<PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")>
Public Class RunningProcess

    Public Shared ReadOnly Property IsOSVersionSupported() As Boolean
        Get
            Return Environment.OSVersion.Version.Major >= 6
        End Get
    End Property

    ' The System.Diagnostics.Process instance.
    Private _diagnosticsProcess As Process

    ''' <summary>
    ''' The name of the process.
    ''' </summary>
    Public ReadOnly Property ProcessName() As String
        Get
            Return Me._diagnosticsProcess.ProcessName
        End Get
    End Property

    ''' <summary>
    ''' The ID of the process.
    ''' </summary>
    Public ReadOnly Property Id() As Integer
        Get
            Return Me._diagnosticsProcess.Id
        End Get
    End Property

    ''' <summary>
    ''' Specify whether the process is a managed application.
    ''' </summary>
    Private _isManaged As Boolean
    Public Property IsManaged() As Boolean
        Get
            Return _isManaged
        End Get
        Private Set(ByVal value As Boolean)
            _isManaged = value
        End Set
    End Property

    ''' <summary>
    ''' Specify whether the process is a .Net 4.0 application.
    ''' </summary>
    Private _isDotNet4 As Boolean
    Public Property IsDotNet4() As Boolean
        Get
            Return _isDotNet4
        End Get
        Private Set(ByVal value As Boolean)
            _isDotNet4 = value
        End Set
    End Property

    ''' <summary>
    ''' Specify whether the process is a console application.
    ''' </summary>
    Private _isConsole As Boolean
    Public Property IsConsole() As Boolean
        Get
            Return _isConsole
        End Get
        Private Set(ByVal value As Boolean)
            _isConsole = value
        End Set
    End Property

    ''' <summary>
    ''' Specify whether the process is a WPF application.
    ''' </summary>
    Private _isWPF As Boolean
    Public Property IsWPF() As Boolean
        Get
            Return _isWPF
        End Get
        Private Set(ByVal value As Boolean)
            _isWPF = value
        End Set
    End Property

    ''' <summary>
    ''' Specify whether the process is a 64bit application.
    ''' </summary>
    Private _is64BitProcess As Boolean
    Public Property Is64BitProcess() As Boolean
        Get
            Return _is64BitProcess
        End Get
        Private Set(ByVal value As Boolean)
            _is64BitProcess = value
        End Set
    End Property

    ''' <summary>
    ''' The remarks of this instance. Normally it is the exception message.
    ''' </summary>
    Private _remarks As String
    Public Property Remarks() As String
        Get
            Return _remarks
        End Get
        Private Set(ByVal value As String)
            _remarks = value
        End Set
    End Property

    Public Sub New(ByVal proc As Process)
        Me._diagnosticsProcess = proc

        Try
            CheckProcess()
        Catch ex As Exception
            Me.Remarks = ex.Message
        End Try
    End Sub

    ''' <summary>
    ''' Check the properties of the process.
    ''' </summary>
    Public Sub CheckProcess()
        Dim procID As UInteger = CUInt(Me._diagnosticsProcess.Id)

        Try

            ' Use kernel32.dll attach the process console to the windows form. 
            If NativeMethods.AttachConsole(procID) Then

                ' Use Kernel32.dll get the current process (windows form) std handle,
                ' as we attach the console window before.
                Dim handle As IntPtr = NativeMethods.GetStdHandle(NativeMethods.STD_OUTPUT_HANDLE)
                Dim lp As UInteger = 0
                Me.IsConsole = NativeMethods.GetConsoleMode(handle, lp)
                NativeMethods.FreeConsole()
            End If
        Catch ex As Exception
            Me.Remarks += String.Format("| Check IsConsole: {0}", ex.Message)
        End Try

        Try

            Dim loadedModules As List(Of String) = Me.GetLoadedModules()

            ' Check whether the .Net Runtime Execution engine MSCOREE.dll is loaded.
            Me.IsManaged = loadedModules.Where(
                Function(m) m.Equals("MSCOREE.dll",
                                     StringComparison.OrdinalIgnoreCase)).Count() > 0
            If Me.IsManaged Then

                ' Check whether the CLR.dll is loaded.
                Me.IsDotNet4 = loadedModules.Where(
                    Function(m) m.Equals("CLR.dll",
                                         StringComparison.OrdinalIgnoreCase)).Count() > 0

                ' Check whether the PresentationCore.dll is loaded.
                Me.IsWPF = loadedModules.Where(
                    Function(m) m.Equals("PresentationCore.dll",
                                         StringComparison.OrdinalIgnoreCase) _
                                     OrElse m.Equals("PresentationCore.ni.dll",
                                                     StringComparison.OrdinalIgnoreCase)).Count() > 0
            End If

        Catch ex As Exception
            Me.Remarks += String.Format("| Check IsManaged: {0}", ex.Message)
        End Try

        Try
            Me.Is64BitProcess = Check64BitProcess()
        Catch ex As Exception
            Me.Remarks += String.Format("| Check Is64Bit: {0}", ex.Message)
        End Try

    End Sub

    ''' <summary>
    ''' Get all loaded modules using EnumProcessModulesEx Function.
    ''' The EnumProcessModulesEx function is only available on Vista or later versions.
    ''' </summary>
    ''' <returns></returns>
    Private Function GetLoadedModules() As List(Of String)

        If Environment.OSVersion.Version.Major < 6 Then
            Throw New ApplicationException("This application must run on Windows Vista" _
                                           & " or later versions. ")
        End If

        Dim modulesHandles(1023) As IntPtr
        Dim size As Integer = 0

        Dim success As Boolean = NativeMethods.EnumProcessModulesEx(
            Me._diagnosticsProcess.Handle,
            modulesHandles,
            Marshal.SizeOf(GetType(IntPtr)) * modulesHandles.Length,
            size,
            NativeMethods.ModuleFilterFlags.LIST_MODULES_ALL)

        If Not success Then
            Throw New Win32Exception()
        End If

        Dim moduleNames As New List(Of String)()

        For i As Integer = 0 To modulesHandles.Length - 1
            If modulesHandles(i) = IntPtr.Zero Then
                Exit For
            End If

            Dim moduleName As New StringBuilder(1024)

            Dim length As UInteger = NativeMethods.GetModuleFileNameEx(
                Me._diagnosticsProcess.Handle,
                modulesHandles(i),
                moduleName,
                Convert.ToUInt32(moduleName.Capacity))

            If length <= 0 Then
                Dim code As Integer = Marshal.GetLastWin32Error()
                Marshal.ThrowExceptionForHR(code)
            Else
                Dim fileName = Path.GetFileName(moduleName.ToString())
                moduleNames.Add(fileName)
            End If
        Next i

        Return moduleNames
    End Function

    ''' <summary>
    ''' Determines whether the specified process is a 64-bit process.
    ''' </summary>
    ''' <param name="hProcess">The process handle</param>
    ''' <returns>
    ''' true if the given process is 64-bit; otherwise, false.
    ''' </returns>
    Private Function Check64BitProcess() As Boolean
        Dim flag As Boolean = False

        If Environment.Is64BitOperatingSystem Then
            ' On 64-bit OS, if a process is not running under Wow64 mode, 
            ' the process must be a 64-bit process.
            flag = Not (NativeMethods.IsWow64Process(
                        Me._diagnosticsProcess.Handle,
                        flag) AndAlso flag)
        End If

        Return flag
    End Function

End Class
