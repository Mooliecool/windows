'****************************** Module Header ******************************\
' Project Name:   CSAzureWebRoleBackendProcessing
' Module Name:    Processor
' File Name:      Program.vb
' Copyright (c) Microsoft Corporation
'
' This console application instantiates a BackendProcessor object and start it up.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/


Imports VBAzureWebRoleBackendProcessing.Common

Class Program
    Public Shared Sub Main()
        ' Trace to the console window.
        Trace.Listeners.Add(New ConsoleTraceListener())

        ' Start-up the backend processor.
        Dim backendProcessor = New BackendProcessor()
        backendProcessor.Start()

        ' Pause.
        Console.ReadKey()
    End Sub
End Class