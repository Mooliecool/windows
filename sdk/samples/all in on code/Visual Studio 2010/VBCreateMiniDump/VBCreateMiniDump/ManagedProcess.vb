'*************************** Module Header ******************************'
' Module Name:  ManagedProcess.vb
' Project:      VBCreateMiniDump
' Copyright (c) Microsoft Corporation.
' 
' This class represents a managed process. It contains a MDbgProcess field
' and a System.Diagnostics.Process field. When a new instance is initialized,
' it will attach a debugger to the target process.
' 
' When the target process stops, this class will check the stop reason to 
' continue or quite.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.ComponentModel
Imports System.Runtime.InteropServices
Imports System.Security.Permissions
Imports Microsoft.Samples.Debugging.CorDebug
Imports Microsoft.Samples.Debugging.MdbgEngine

<PermissionSet(SecurityAction.LinkDemand, Name:="FullTrust"),
 PermissionSet(SecurityAction.InheritanceDemand, Name:="FullTrust")>
Public Class ManagedProcess
    Implements IDisposable

    Private _disposed As Boolean = False

    ' The reason why the process that is debugging stopped.
    Private _stopReason As ManagedCallbackType

    ' Specify whether there is an unhandled exception in the process.
    Private _isExceptionUnhandled As Boolean = False

    Private _debugger As MDbgEngine

    Public ReadOnly Property Debugger() As MDbgEngine
        Get
            If _debugger Is Nothing Then
                _debugger = New MDbgEngine()
            End If

            Return _debugger
        End Get
    End Property

    ''' <summary>
    ''' Represents a Process in which code execution context can be controlled.
    ''' </summary>
    Private _mdbgProcess As MDbgProcess
    Public Property MdbgProcess() As MDbgProcess
        Get
            Return _mdbgProcess
        End Get
        Private Set(ByVal value As MDbgProcess)
            _mdbgProcess = value
        End Set
    End Property


    Private _diagnosticsProcess As System.Diagnostics.Process = Nothing

    ''' <summary>
    ''' Get System.Diagnostics.Process using ProcessID.
    ''' </summary>
    Public ReadOnly Property DiagnosticsProcess() As System.Diagnostics.Process
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

    ' This event occurs when the watch dog start to handle the exception. If it is 
    ' cancelled, the watch dog will not continue to handle the exception.
    Public Event StartHandleException As EventHandler(Of CancelEventArgs)

    ' This event occurs when the watch dog has got the information of the 
    ' unhandled exception.
    Public Event UnhandledExceptionOccurred As EventHandler(Of ManagedProcessUnhandledExceptionOccurredEventArgs)

    Public Sub New(ByVal diagnosticsProcess As System.Diagnostics.Process)
        If diagnosticsProcess Is Nothing Then
            Throw New ArgumentNullException("diagnosticsProcess",
                                            "The System.Diagnostics.Process could not be null. ")
        End If
        Me._diagnosticsProcess = diagnosticsProcess

        AttachDebuggerToProcess()
    End Sub

    ''' <summary>
    ''' Attach a debugger to the target process.
    ''' </summary>
    Private Sub AttachDebuggerToProcess()

        Dim version As String = MdbgVersionPolicy.GetDefaultAttachVersion(Me.DiagnosticsProcess.Id)

        If String.IsNullOrEmpty(version) Then
            Throw New ApplicationException("Can't determine what version of the CLR to " _
                                            & "attach to the process.")
        End If

        Me.MdbgProcess = Me.Debugger.Attach(Me.DiagnosticsProcess.Id, Nothing, version)

        Dim result As Boolean = Me.MdbgProcess.Go.WaitOne()

        If Not result Then
            Throw New ApplicationException(String.Format("The process with an ID {0} could not be " _
                                                         & "attached. Operation time out.",
                                                         Me.DiagnosticsProcess.Id))
        End If

        AddHandler MdbgProcess.PostDebugEvent, AddressOf MDbgProcess_PostDebugEvent
    End Sub


    ''' <summary>
    ''' Handle the Debug Event of the MDbgProcess.
    ''' </summary>
    Private Sub MDbgProcess_PostDebugEvent(ByVal sender As Object,
                                           ByVal e As CustomPostCallbackEventArgs)
        Me._stopReason = e.CallbackType

        Select Case Me._stopReason
            Case ManagedCallbackType.OnException
                Dim exceptionEventArgs As CorExceptionEventArgs =
                    TryCast(e.CallbackArgs, CorExceptionEventArgs)
                Me._isExceptionUnhandled = exceptionEventArgs IsNot Nothing _
                    AndAlso exceptionEventArgs.Unhandled
            Case Else
        End Select
    End Sub

    ''' <summary>
    ''' When the target process stops, check the stop reason and determine to 
    ''' continue or quite. 
    ''' </summary>
    Public Sub StartWatch()
        Do
            Dim result As Boolean = Me.MdbgProcess.Go.WaitOne

            If Me._stopReason = ManagedCallbackType.OnException _
                AndAlso Me._isExceptionUnhandled Then
                Me.HandleException()
                Exit Do
            ElseIf Me._stopReason = ManagedCallbackType.OnProcessExit Then
                Exit Do
            End If
        Loop

    End Sub

    ''' <summary>
    ''' Initialize a ManagedProcessUnhandledExceptionOccurredEventArgs object and raise the 
    ''' UnhandledExceptionOccurred event.
    ''' </summary>
    Private Sub HandleException()

        Dim e As CancelEventArgs = New CancelEventArgs()
        Me.OnStartHandleException(e)
        If e.Cancel Then
            Return
        End If

        Dim ex As MDbgValue = Me.MdbgProcess.Threads.Active.CurrentException
        If ex.IsNull Then
            ' No current exception is available.  Perhaps the user switched to a different
            ' thread which was not throwing an exception.
            Return
        End If

        Dim exceptionPointers As IntPtr = IntPtr.Zero

        For Each f As MDbgValue In ex.GetFields()
            If f.Name = "_xptrs" Then
                Dim outputValue As String = f.GetStringValue(0)
                exceptionPointers = CType(Integer.Parse(outputValue), IntPtr)
            End If
        Next f

        If exceptionPointers = IntPtr.Zero Then
            ' Get the Exception Pointer in the target process
            Dim value As MDbgValue = FunctionEval(
                "System.Runtime.InteropServices.Marshal.GetExceptionPointers")
            If value IsNot Nothing Then
                exceptionPointers = CType(Integer.Parse(value.GetStringValue(1)), IntPtr)
            End If
        End If

        Me.OnUnhandledExceptionOccurred(
            New ManagedProcessUnhandledExceptionOccurredEventArgs With _
            {.ProcessID = Me.MdbgProcess.CorProcess.Id,
             .ThreadID = Me.MdbgProcess.Threads.Active.Id,
             .ExceptionPointers = exceptionPointers})
    End Sub

    ''' <summary>
    '''  Eval a function in the target process.
    ''' </summary>
    ''' <param name="functionNameFromScope">The full function name.</param>
    Private Function FunctionEval(ByVal functionNameFromScope As String) As MDbgValue
        Dim corAD As CorAppDomain = Me.MdbgProcess.Threads.Active.CorThread.AppDomain
        Dim func As MDbgFunction = Me.MdbgProcess.ResolveFunctionNameFromScope(functionNameFromScope, corAD)
        Dim eval As CorEval = Me.MdbgProcess.Threads.Active.CorThread.CreateEval()
        eval.CallFunction(func.CorFunction, New CorValue() {})
        Dim waitResult As Boolean = Me.MdbgProcess.Go.WaitOne

        Dim value As MDbgValue = Nothing
        If TypeOf MdbgProcess.StopReason Is EvalCompleteStopReason Then
            Dim result As CorValue = (TryCast(MdbgProcess.StopReason, EvalCompleteStopReason)).Eval.Result
            If result IsNot Nothing Then
                value = New MDbgValue(MdbgProcess, result)
            End If
        End If
        Return value
    End Function

    ''' <summary>
    ''' Raise the UnhandledExceptionOccurred event.
    ''' </summary>
    ''' <param name="e"></param>
    Protected Sub OnUnhandledExceptionOccurred(ByVal e As ManagedProcessUnhandledExceptionOccurredEventArgs)
        RaiseEvent UnhandledExceptionOccurred(Me, e)
    End Sub

    ''' <summary>
    ''' Raise the StartHandleException event.
    ''' </summary>
    ''' <param name="e"></param>
    Protected Sub OnStartHandleException(ByVal e As CancelEventArgs)
        RaiseEvent StartHandleException(Me, e)
    End Sub

    Public Sub Dispose() Implements IDisposable.Dispose

        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub

    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        ' Protect from being called multiple times.
        If _disposed Then
            Return
        End If

        If disposing Then
            ' Clean up all managed resources.
            If Me.MdbgProcess IsNot Nothing Then

                ' Make sure the underlying CorProcess was stopped before 
                ' detached it.  
                If Me.MdbgProcess.IsAlive Then

                    Dim waithandler = Me.MdbgProcess.AsyncStop()
                    waithandler.WaitOne()
                    Me.MdbgProcess.Detach()
                End If

            End If
        End If

        _disposed = True
    End Sub

End Class
