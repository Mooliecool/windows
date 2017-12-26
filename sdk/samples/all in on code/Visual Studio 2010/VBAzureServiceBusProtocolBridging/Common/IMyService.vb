'****************************** Module Header ******************************\
' Project Name:   CSAzureServiceBusProtocolBridging
' Module Name:    Common
' File Name:      IMyService.vb
' Copyright (c) Microsoft Corporation
'
' A WCF service contract that is shared by both Client project and the Service project.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports System.ServiceModel

<ServiceContract()> _
Public Interface IMyService
    <OperationContract()> _
    Function Add(number1 As Integer, number2 As Integer) As Integer
End Interface