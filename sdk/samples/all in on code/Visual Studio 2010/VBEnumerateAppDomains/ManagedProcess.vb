'*************************** Module Header ******************************\
' Module Name:  ManagedProcess.vb
' Project:	    VBEnumerateAppDomains
' Copyright (c) Microsoft Corporation.
' 
' This class represents a managed process. It contains a MDbgProcess field
' and a System.Diagnostics.Process field. This class also supplies three 
' static methods: GetManagedProcesses, GetManagedProcessByID and
' GetAppDomainsInCurrentProcess.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************

Imports System.Collections
Imports System.ComponentModel
Imports System.Runtime.InteropServices
Imports System.Security.Permissions
Imports Microsoft.Samples.Debugging.CorDebug
Imports Microsoft.Samples.Debugging.MdbgEngine
Imports mscoree


<PermissionSet(SecurityAction.LinkDemand, Name:="FullTrust"),
PermissionSet(SecurityAction.InheritanceDemand, Name:="FullTrust")>
Public Class ManagedProcess
    Implements IDisposable
    Private disposed As Boolean = False

    ' Represents a Process in which code execution context can be controlled.
    Private _mdbgProcess As MDbgProcess

    ''' <summary>
    ''' Do not intialize the _mdbgProcess until it is called.
    ''' </summary>
    Private ReadOnly Property MDbgProcess() As MDbgProcess
        Get
            If _mdbgProcess Is Nothing Then
                Try

                    ' Intialize a MDbgEngine instance. 
                    Dim engine As New MDbgEngine()

                    'Attach a debugger to the specified process, and 
                    'return a MDbgProcess instance that represents the Process.
                    If Me.LoadedRuntimes IsNot Nothing _
                        AndAlso Me.LoadedRuntimes.Count() = 1 Then
                        _mdbgProcess = engine.Attach(
                            ProcessID, LoadedRuntimes.First().GetVersionString())
                    Else
                        _mdbgProcess = engine.Attach(
                            ProcessID, CorDebugger.GetDefaultDebuggerVersion())
                    End If

                    _mdbgProcess.Go()

                Catch _COMException As COMException
                    Throw New ApplicationException(
                        String.Format(
                            "The process with an ID {0} could not be attached. " _
                            & "Access is denied or it has already been attached.",
                            ProcessID))
                Catch
                    Throw
                End Try

            End If

            Return _mdbgProcess
        End Get
    End Property

    Private _diagnosticsProcess As Process = Nothing

    ''' <summary>
    ''' Get System.Diagnostics.Process using ProcessID.
    ''' </summary>
    Public ReadOnly Property DiagnosticsProcess() As Process
        Get
            Return _diagnosticsProcess
        End Get
    End Property

    ''' <summary>
    ''' The ID of the process. 
    ''' </summary>
    Public ReadOnly Property ProcessID() As Integer
        Get
            Return DiagnosticsProcess.Id
        End Get
    End Property

    ''' <summary>
    ''' The name of the process. 
    ''' </summary>
    Public ReadOnly Property ProcessName() As String
        Get
            Return DiagnosticsProcess.ProcessName
        End Get
    End Property

    ''' <summary>
    ''' Get all Runtimes loaded in the process.
    ''' </summary>
    Public ReadOnly Property LoadedRuntimes() As IEnumerable(Of CLRRuntimeInfo)
        Get
            Try
                Dim host As New CLRMetaHost()
                Return host.EnumerateLoadedRuntimes(ProcessID)
            Catch e1 As EntryPointNotFoundException
                Return Nothing
            End Try
        End Get
    End Property

    ''' <summary>
    ''' Gets all CorAppDomains in the processes. 
    ''' </summary>
    Public ReadOnly Property AppDomains() As IEnumerable
        Get
            Dim _appDomains = MDbgProcess.CorProcess.AppDomains
            Return _appDomains
        End Get
    End Property


    Private Sub New(ByVal processID As Integer)
        Dim diagnosticsProcess As Process = Process.GetProcessById(processID)
        Me._diagnosticsProcess = diagnosticsProcess

        ' Make sure that the specified process is a managed process.
        If Me.LoadedRuntimes Is Nothing OrElse Me.LoadedRuntimes.Count() = 0 Then
            Throw New ArgumentException("This process is not a managed process. ")
        End If
    End Sub


    Private Sub New(ByVal diagnosticsProcess As Process)
        If diagnosticsProcess Is Nothing Then
            Throw New ArgumentNullException(
                "diagnosticsProcess", "The System.Diagnostics.Process could not be null. ")
        End If
        Me._diagnosticsProcess = diagnosticsProcess
        If Me.LoadedRuntimes Is Nothing OrElse Me.LoadedRuntimes.Count() = 0 Then
            Throw New ArgumentException("This process is not a managed process. ")
        End If
    End Sub



    Public Sub Dispose() Implements IDisposable.Dispose
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub


    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        ' Protect from being called multiple times.
        If disposed Then
            Return
        End If

        If disposing Then
            ' Clean up all managed resources.
            If _mdbgProcess IsNot Nothing Then

                ' Make sure the underlying CorProcess was stopped before 
                ' detached it.                     
                Dim waithandler = _mdbgProcess.AsyncStop()
                waithandler.WaitOne()
                _mdbgProcess.Detach()

            End If
        End If

        disposed = True
    End Sub


    ''' <summary>
    ''' Gets all managed processes. 
    ''' </summary>
    Public Shared Function GetManagedProcesses() As List(Of ManagedProcess)
        Dim managedProcesses As New List(Of ManagedProcess)()

        ' CLRMetaHost implements ICLRMetaHost Interface which provides a method that
        ' return list all runtimes that are loaded in a specified process.
        Dim host As New CLRMetaHost()

        Dim processes = Process.GetProcesses()

        For Each diagnosticsProcess As Process In processes
            Try

                ' Lists all runtimes that are loaded in a specified process.
                Dim runtimes = host.EnumerateLoadedRuntimes(diagnosticsProcess.Id)

                ' If the process loads CLRs, it could be considered as a managed process.
                If runtimes IsNot Nothing AndAlso runtimes.Count() > 0 Then
                    managedProcesses.Add(New ManagedProcess(diagnosticsProcess))
                End If


                ' The method EnumerateLoadedRuntimes will throw Win32Exception when the 
                ' file cannot be found or access is denied. For example, the 
                ' diagnosticsProcess is System or System Idle Process.
            Catch _Win32Exception As Win32Exception

                ' The method EnumerateLoadedRuntimes will throw COMException when it tries
                ' to access a 64bit process on 64bit OS if this application is built on 
                ' platform x86.
            Catch _COMException As COMException

            End Try
        Next diagnosticsProcess
        Return managedProcesses
    End Function

    ''' <summary>
    ''' Gets a managed process by ID. This method is used to get AppDomains in other
    ''' process. If you want to get AppDomains in current process, please use the
    ''' static method GetAppDomainsInCurrentProcess.
    ''' </summary>
    ''' <exception cref="ArgumentException">
    ''' If there is no managed process with this ID, an ArgumentException will be thrown.
    ''' </exception>
    Public Shared Function GetManagedProcessByID(ByVal processID As Integer) As ManagedProcess
        If processID = Process.GetCurrentProcess().Id Then
            Throw New ArgumentException("Cannot debug current process.")
        End If
        Return New ManagedProcess(processID)
    End Function

    ''' <summary>
    ''' Gets all AppDomains in current process.
    ''' This method uses ICorRuntimeHost interface to get an enumerator for the domains
    ''' in the current process.
    ''' </summary>
    Public Shared Function GetAppDomainsInCurrentProcess() As List(Of AppDomain)
        Dim appDomains = New List(Of AppDomain)()
        Dim hEnum = IntPtr.Zero

        ' The class CorRuntimeHostClass is a coclass, which implements the
        ' ICorRuntimeHost interface. 
        Dim host = New CorRuntimeHost()

        Try
            ' Gets an enumerator for the domains in the current process.
            host.EnumDomains(hEnum)
            Do
                Dim domain = New Object
                host.NextDomain(hEnum, domain)
                If domain Is Nothing Then
                    Exit Do
                End If
                appDomains.Add(TryCast(domain, AppDomain))
            Loop
        Finally
            host.CloseEnum(hEnum)
            Marshal.ReleaseComObject(host)
        End Try
        Return appDomains
    End Function

End Class

