'***************************** Module Header ******************************\
'* Module Name:	RetryPolicy.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* A SQL Azure retry policy.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Imports System.Data.SqlClient
Imports System.Threading

''' <summary>
''' A SQL Azure retry policy.
''' </summary>
Public Class RetryPolicy
    Public Property RetryNumber As Integer
    Public Property WaitTime As TimeSpan
    Public Property Task As Action

    Public Sub New()
        Me.RetryNumber = 3
    End Sub

    Public Sub New(ByVal retryNumber As Integer)
        Me.RetryNumber = retryNumber
        Me.WaitTime = TimeSpan.FromSeconds(5.0)
    End Sub

    ''' <summary>
    ''' If the execution fails, retry it after a specific time,
    ''' until the maximum retry policy exceeds.
    ''' </summary>
    Public Sub Execute()
        For i As Integer = 0 To Me.RetryNumber - 1
            Try
                Me.Task.Invoke()
                Exit Try
            Catch ex As SqlException
                If i = Me.RetryNumber - 1 Then
                    Throw New SqlExceptionWithRetry("Maximum retry reached. Still unable to process the request. See inner exception for more details.", ex)
                End If
                Thread.Sleep(Me.WaitTime)
            End Try
        Next
    End Sub
End Class

Public Class SqlExceptionWithRetry
    Inherits Exception
    Public Sub New(ByVal message As String, ByVal innerException As SqlException)
        MyBase.New(message, innerException)
    End Sub
End Class