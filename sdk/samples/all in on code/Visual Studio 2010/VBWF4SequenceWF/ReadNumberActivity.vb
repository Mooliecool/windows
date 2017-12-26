'****************************** Module Header ******************************\
' Module Name:  ReadNumberActivity.vb
' Project:		VBWF4SequenceWF
' Copyright (c) Microsoft Corporation.
' 
' A Guess Number Game workflow demostrates the usage of WF4 sequence workflow.
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

Namespace VBWF4SequenceWF

    Public NotInheritable Class ReadNumberActivity
        Inherits CodeActivity

        ' Define an activity out argument of type int
        Public Property playerInputNumber() As OutArgument(Of Int32)
            Get
                Return _playerInputNumber
            End Get
            Set(ByVal value As OutArgument(Of Integer))
                _playerInputNumber = value
            End Set
        End Property

        Private _playerInputNumber As OutArgument(Of Integer)

        Protected Overrides Sub Execute(ByVal context As CodeActivityContext)

            playerInputNumber.Set(context, Int32.Parse(Console.ReadLine()))

        End Sub
    End Class

End Namespace