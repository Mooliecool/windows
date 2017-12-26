'****************************** Module Header ******************************'
' Module Name:  ExeCOMServer.vb
' Project:      VBExeCOMServer
' Copyright (c) Microsoft Corporation.
' 
' ExeCOMServer encapsulates the skeleton of an out-of-process COM server in  
' VB.NET. The class implements the singleton design pattern and it's 
' thread-safe. To start the server, call CSExeCOMServer.Instance.Run(). If 
' the server is running, the function returns directly. Inside the Run method, 
' it registers the class factories for the COM classes to be exposed from the 
' COM server, and starts the message loop to wait for the drop of lock count 
' to zero. When lock count equals zero, it revokes the registrations and 
' quits the server.
' 
' The lock count of the server is incremented when a COM object is created, 
' and it's decremented when the object is released (GC-ed). In order that the 
' COM objects can be GC-ed in time, ExeCOMServer triggers GC every 5 seconds 
' by running a Timer after the server is started.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.Threading

#End Region


Friend NotInheritable Class ExeCOMServer

#Region "Singleton Pattern"

    Private Sub New()
    End Sub

    Private Shared _instance As ExeCOMServer = New ExeCOMServer
    Public Shared ReadOnly Property Instance() As ExeCOMServer
        Get
            Return ExeCOMServer._instance
        End Get
    End Property

#End Region

  
    Private syncRoot As Object = New Object  ' For thread-sync in lock
    Private _bRunning As Boolean = False  ' Whether the server is running

    ' The ID of the thread that runs the message loop
    Private _nMainThreadID As UInt32 = 0

    ' The lock count (the number of active COM objects) in the server
    Private _nLockCnt As Integer = 0

    ' The timer to trigger GC every 5 seconds
    Private _gcTimer As Timer

    ''' <summary>
    ''' The method is call every 5 seconds to GC the managed heap after 
    ''' the COM server is started.
    ''' </summary>
    ''' <param name="stateInfo"></param>
    Private Shared Sub GarbageCollect(ByVal stateInfo As Object)
        GC.Collect()  ' GC
    End Sub

    Private _cookieSimpleObj As UInt32


    ''' <summary>
    ''' PreMessageLoop is responsible for registering the COM class 
    ''' factories for the COM classes to be exposed from the server, and 
    ''' initializing the key member variables of the COM server (e.g. 
    ''' _nMainThreadID and _nLockCnt).
    ''' </summary>
    Private Sub PreMessageLoop()

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Register the COM class factories.
        ' 

        Dim clsidSimpleObj As New Guid(SimpleObject.ClassId)

        ' Register the SimpleObject class object
        Dim hResult As Integer = COMNative.CoRegisterClassObject( _
        clsidSimpleObj, New SimpleObjectClassFactory, CLSCTX.LOCAL_SERVER, _
        REGCLS.SUSPENDED Or REGCLS.MULTIPLEUSE, Me._cookieSimpleObj)
        If (hResult <> 0) Then
            Throw New ApplicationException( _
            "CoRegisterClassObject failed w/err 0x" & hResult.ToString("X"))
        End If

        ' Register other class objects 
        ' ...

        ' Inform the SCM about all the registered classes, and begins 
        ' letting activation requests into the server process.
        hResult = COMNative.CoResumeClassObjects
        If (hResult <> 0) Then
            ' Revoke the registration of SimpleObject on failure
            If (Me._cookieSimpleObj <> 0) Then
                COMNative.CoRevokeClassObject(Me._cookieSimpleObj)
            End If

            ' Revoke the registration of other classes
            ' ...

            Throw New ApplicationException( _
            "CoResumeClassObjects failed w/err 0x" & hResult.ToString("X"))
        End If


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Initialize member variables.
        ' 

        ' Records the ID of the thread that runs the COM server so that 
        ' the server knows where to post the WM_QUIT message to exit the 
        ' message loop.
        Me._nMainThreadID = NativeMethod.GetCurrentThreadId

        ' Records the count of the active COM objects in the server. 
        ' When _nLockCnt drops to zero, the server can be shut down.
        Me._nLockCnt = 0

        ' Start the GC timer to trigger GC every 5 seconds.
        Me._gcTimer = New Timer( _
        New TimerCallback(AddressOf ExeCOMServer.GarbageCollect), Nothing, _
        5000, 5000)

    End Sub


    ''' <summary>
    ''' RunMessageLoop runs the standard message loop. The message loop 
    ''' quits when it receives the WM_QUIT message.
    ''' </summary>
    Private Sub RunMessageLoop()
        Dim msg As MSG
        Do While NativeMethod.GetMessage(msg, IntPtr.Zero, 0, 0)
            NativeMethod.TranslateMessage((msg))
            NativeMethod.DispatchMessage((msg))
        Loop
    End Sub


    ''' <summary>
    ''' PostMessageLoop is called to revoke the registration of the COM 
    ''' classes exposed from the server, and perform the cleanups.
    ''' </summary>
    Private Sub PostMessageLoop()

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Revoke the registration of the COM classes.
        ' 

        ' Revoke the registration of SimpleObject
        If (Me._cookieSimpleObj <> 0) Then
            COMNative.CoRevokeClassObject(Me._cookieSimpleObj)
        End If

        ' Revoke the registration of other classes
        ' ...


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Perform the cleanup.
        ' 

        ' Dispose the GC timer.
        If (Not Me._gcTimer Is Nothing) Then
            Me._gcTimer.Dispose()
        End If

        ' Wait for any threads to finish.
        Thread.Sleep(1000)

    End Sub


    ''' <summary>
    ''' Run the COM server. If the server is running, the function 
    ''' returns directly.
    ''' </summary>
    ''' <remarks>The method is thread-safe.</remarks>
    Public Sub Run()
        SyncLock Me.syncRoot  ' Ensure thread-safe
            ' If the server is running, return directly
            If Me._bRunning Then
                Return
            End If
            ' Indicate that the server is running now
            Me._bRunning = True
        End SyncLock

        Try
            ' Call PreMessageLoop to initialize the member variables 
            ' and register the class factories.
            Me.PreMessageLoop()

            ' Run the message loop.
            Me.RunMessageLoop()

            ' Call PostMessageLoop to revoke the registration.
            Me.PostMessageLoop()
        Finally
            Me._bRunning = False
        End Try
    End Sub


    ''' <summary>
    ''' Increase the lock count
    ''' </summary>
    ''' <returns>The new lock count after the increment</returns>
    ''' <remarks>The method is thread-safe.</remarks>
    Public Function Lock() As Integer
        Return Interlocked.Increment(Me._nLockCnt)
    End Function


    ''' <summary>
    ''' Decrease the lock count. When the lock count drops to zero, post 
    ''' the WM_QUIT message to the message loop in the main thread to 
    ''' shut down the COM server.
    ''' </summary>
    ''' <returns>The new lock count after the increment</returns>
    Public Function Unlock() As Integer
        Dim nRet As Integer = Interlocked.Decrement(Me._nLockCnt)

        ' If lock drops to zero, attempt to terminate the server.
        If (nRet = 0) Then
            ' Post the WM_QUIT message to the main thread
            NativeMethod.PostThreadMessage( _
            _nMainThreadID, NativeMethod.WM_QUIT, UIntPtr.Zero, IntPtr.Zero)
        End If
        Return nRet
    End Function


    ''' <summary>
    ''' Get the current lock count.
    ''' </summary>
    ''' <returns></returns>
    Public Function GetLockCount() As Integer
        Return Me._nLockCnt
    End Function

End Class