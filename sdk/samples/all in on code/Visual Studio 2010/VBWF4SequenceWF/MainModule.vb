'***************************** Module Header ******************************\
' Module Name:  Program.vb
' Project:		VBWF4SequenceWF
' Copyright (c) Microsoft Corporation.
' 
' A Guess Number Game workflow demostrates the useage of WF4 sequence worKflow.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/
Imports System.Activities

Namespace VBWF4SequenceWF

    Module MainModule

        Sub Main()

            WorkflowInvoker.Invoke(New GuessNumberGameSequenceWF())

        End Sub

    End Module

End Namespace