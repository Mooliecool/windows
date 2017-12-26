'****************************** Module Header ******************************\
' Module Name:  ReadNumberActivity.vb
' Project:		VBWF4FlowChart
' Copyright (c) Microsoft Corporation.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System
Imports System.Activities

Public Class ReadNumberActivity
    Inherits CodeActivity

    Public Property playerInputNumber() As OutArgument(Of Integer)
        Get
            Return _playerInputNumber
        End Get
        Set(ByVal value As OutArgument(Of Integer))
            _playerInputNumber = value
        End Set
    End Property
    Private _playerInputNumber As OutArgument(Of Integer)


    Protected Overrides Sub Execute(ByVal context As System.Activities.CodeActivityContext)

        playerInputNumber.Set(context, Integer.Parse(Console.ReadLine()))

    End Sub
End Class
