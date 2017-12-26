'****************************** Module Header ******************************\
' Module Name:	Program.vb
' Project:		Client
' Copyright (c) Microsoft Corporation.
' 
' This is the client application that verifies the workflow service works fine.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports Client.WorkflowServiceReference

Module Program

    Sub Main()
        Dim client = New ServiceClient()
        Console.WriteLine("Processing 10...")
        Console.WriteLine("Service returned: " + client.ProcessData(10))
        Console.WriteLine("Processing 30...")
        Console.WriteLine("Service returned: " + client.ProcessData(30))
        Console.ReadLine()
    End Sub

End Module
