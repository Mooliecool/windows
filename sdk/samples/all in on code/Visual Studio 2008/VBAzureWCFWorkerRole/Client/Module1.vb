'***************************** Module Header ******************************\
'* Module Name:	Program.cs
'* Project:		Client
'* Copyright (c) Microsoft Corporation.
'* 
'* This class demonstrates how to consume the WCF services that is hosted in Worker Role.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Module Module1

    Sub Main()
        Dim proxy As ServiceReference1.MyServiceClient = New ServiceReference1.MyServiceClient()
        Dim result = proxy.DoWork()
        Console.WriteLine(String.Format("Server Returned: {0}", result))
        Console.ReadLine()
    End Sub

End Module
