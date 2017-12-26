'****************************** Module Header ******************************\
' Module Name:    BackgroundWorker.vb
' Project:        VBASPNETBackgroundWorker
' Copyright (c) Microsoft Corporation
'
' The BackgroundWorker class calls a method in a separate thread. It allows 
' passing parameters to the method when it is called. And it can let the target 
' method report progress and result.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports System.Threading

''' <summary>
''' This class is used to execute an operation in a separate thread.
''' </summary>
Public Class BackgroundWorker
    ''' <summary>
    ''' This thread is used to run the operation in the background.
    ''' </summary>
    Private _innerThread As Thread = Nothing

    Private _arguments As Object() = Nothing

#Region "Properties"
    ''' <summary>
    ''' A integer that shows the current progress.
    ''' 100 value means the operation is completed.
    ''' </summary>
    Public ReadOnly Property Progress() As Integer
        Get
            Return _progress
        End Get
    End Property
    Private _progress As Integer = 0

    ''' <summary>
    ''' A object that you can use it to save the result of the operation.
    ''' </summary>
    Public ReadOnly Property Result() As Object
        Get
            Return _result
        End Get
    End Property
    Private _result As Object = Nothing

    ''' <summary>
    ''' A boolean variable identifies if current Background Worker is
    ''' working or not.
    ''' </summary>
    Public ReadOnly Property IsRunning() As Boolean
        Get
            If _innerThread IsNot Nothing Then
                Return _innerThread.IsAlive
            End If
            Return False
        End Get
    End Property
#End Region

#Region "Events"
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="progress">
    ''' Report the progress by changing its value
    ''' </param>
    ''' <param name="_result">
    ''' Store value in this varialbe as the result
    ''' </param>
    ''' <param name="arguments">
    ''' The parameters which will be passed to operation method
    ''' </param>
    Public Delegate Sub DoWorkEventHandler(ByRef progress As Integer, ByRef _result As Object, ByVal arguments As Object())

    Public Event DoWork As DoWorkEventHandler
#End Region

    ''' <summary>
    ''' Starts execution of a background operation.
    ''' </summary>
    ''' <param name="arguments">
    ''' The parameters which will be passed to operation method
    ''' </param>
    Public Sub RunWorker(ByVal arguments As Object())
        _arguments = arguments
        _innerThread = New Thread(AddressOf Worker)
        _innerThread.Start()
    End Sub

    Private Sub Worker()
        _progress = 0
        RaiseEvent DoWork(_progress, _result, _arguments)
        _progress = 100
    End Sub
End Class
