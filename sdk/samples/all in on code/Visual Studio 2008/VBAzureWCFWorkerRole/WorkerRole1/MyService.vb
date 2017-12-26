'***************************** Module Header ******************************\
'* Module Name:	MyService.cs
'* Project:		CSWorkerRoleHostingWCF
'* Copyright (c) Microsoft Corporation.
'* 
'* This module includes the service contract of MyService.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************

Imports System.ServiceModel

<ServiceContract()> _
Public Interface IMyService

    <OperationContract()> _
    Function DoWork() As String
End Interface


Public Class MyService
    Implements IMyService

    Public Function DoWork() As String Implements IMyService.DoWork
        Return "Hello World"
    End Function

End Class
