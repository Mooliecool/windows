'*************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:	    VBTFSEventListener
' Copyright (c) Microsoft Corporation.
' 
' Create a ServiceHost to listener the TFS notification.
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Linq
Imports System.ServiceModel

Module MainModule
    Sub Main(ByVal args() As String)
        Using host As New ServiceHost(GetType(CheckinEventService))
            host.Open()
            Console.WriteLine(host.BaseAddresses.First())

            Console.WriteLine("Press <Enter> to exit...")
            Console.ReadLine()
        End Using
    End Sub
End Module
