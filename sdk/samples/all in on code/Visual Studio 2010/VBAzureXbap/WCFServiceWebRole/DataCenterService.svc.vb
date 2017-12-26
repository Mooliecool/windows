'****************************** Module Header ******************************\
' Module Name:  DataCenterService.cs
' Project:      VBAzureXbap
' Copyright (c) Microsoft Corporation.
' 
' The service implementation.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Public Class DataCenterService
    Implements IDataCenterService

    Public Function GetDataCenters() As List(Of DataCenter) Implements IDataCenterService.GetDataCenters
        Dim dataCenters = New List(Of DataCenter)
        dataCenters.Add(New DataCenter() With {
            .Name = "Chicago",
            .Bound = New Rect(328, 790, 20, 20)
        })
        dataCenters.Add(New DataCenter() With {
            .Name = "San Antonio", _
            .Bound = New Rect(285, 873, 20, 20) _
        })
        dataCenters.Add(New DataCenter() With {
                .Name = "Amsterdam", _
                .Bound = New Rect(856, 711, 20, 20) _
            })
        dataCenters.Add(New DataCenter() With {
                .Name = "Dublin", _
                .Bound = New Rect(796, 703, 20, 20) _
            })
        dataCenters.Add(New DataCenter() With {
                .Name = "Hong Kong", _
                .Bound = New Rect(1454, 923, 20, 20) _
            })
        dataCenters.Add(New DataCenter() With {
                .Name = "Singapore", _
                .Bound = New Rect(1406, 1040, 20, 20) _
            })
        Return dataCenters
    End Function

End Class
