'****************************** Module Header ******************************\
' Project Name:   CSAzureServiceBusProtocolBridging
' Module Name:    Service
' File Name:      MyService.vb
' Copyright (c) Microsoft Corporation
'
' This service contains a method that returns the sum of two numbers.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports System.ServiceModel
Imports Common

<ServiceBehavior(InstanceContextMode:=InstanceContextMode.[Single], _
    ConcurrencyMode:=ConcurrencyMode.Multiple)> _
Public Class MyService
    Implements IMyService
    Public Function Add(number1 As Integer, number2 As Integer) As Integer Implements IMyService.Add
        Console.WriteLine("Add({0}, {1}) is called.", number1, number2)
        Return number1 + number2
    End Function
End Class