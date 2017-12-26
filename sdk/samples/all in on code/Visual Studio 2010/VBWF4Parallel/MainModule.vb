'***************************** Module Header ******************************\
' Module Name:  MainModule.vb
' Project:		VBWF4Parallel
' Copyright (c) Microsoft Corporation.
' 
' A workflow demonstrates the usage of WF4 Parallel activity, ForEach activity
' and ParallelForEach activity..
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

Namespace VBWF4Parallel

    Module MainModule

        Sub Main()

            WorkflowInvoker.Invoke(New ParallelWF())

        End Sub

    End Module

End Namespace


