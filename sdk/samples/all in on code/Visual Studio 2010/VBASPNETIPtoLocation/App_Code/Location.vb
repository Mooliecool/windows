'*************************** Module Header ********************************\
' Module Name:    Location.vb
' Project:        VBASPNETIPtoLocation
' Copyright (c) Microsoft Corporation
'
' This project illustrates how to get the geographical location from a db file.
' You need install Sqlserver Express for run the web applicaiton. The code-sample
' only support Internet Protocol version 4.
' 
' This class is IP location information entity class. 
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************/


Public Class Location
    Public Property BeginIP As String
    Public Property EndIP As String
    Public Property CountryTwoCode As String
    Public Property CountryThreeCode As String
    Public Property CountryName As String
End Class
