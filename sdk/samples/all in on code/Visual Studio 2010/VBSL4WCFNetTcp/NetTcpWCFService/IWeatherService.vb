'****************************** Module Header ******************************'
' Module Name:                IWeatherService.vb
' Project:                    NetTcpWCFService
' Copyright (c) Microsoft Corporation.
' 
' Weather Service ServiceContract.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.ServiceModel

<ServiceContract(CallbackContract := GetType(IWeatherServiceCallback))> _
Public Interface IWeatherService
    <OperationContract(IsOneWay:=True)> _
    Sub Subscribe()

    <OperationContract(IsOneWay:=True)> _
    Sub UnSubscribe()
End Interface

Public Interface IWeatherServiceCallback
    <OperationContract(IsOneWay:=True)> _
    Sub WeatherReport(ByVal report As String)
End Interface

