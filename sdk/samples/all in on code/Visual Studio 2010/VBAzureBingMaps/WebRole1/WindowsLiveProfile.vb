'***************************** Module Header ******************************\
'* Module Name:	WindowsLiveProfile.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* A class corresponds to the response
'* from Windows Live Messenger Connect profile API.
'* Used by WCF syndication API.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Imports System.Collections.Generic
Imports System.Runtime.Serialization

''' <summary>
''' A class corresponds to the response
''' from Windows Live Messenger Connect profile API.
''' Used by WCF syndication API.
''' </summary>
<DataContract([Namespace]:="http://schemas.microsoft.com/ado/2007/08/dataservices")> _
Public Class WindowsLiveProfile
    <DataMember()> _
    Public Property Addresses As List(Of element)
    <DataMember()> _
    Public Property BirthMonth As Integer
    <DataMember()> _
    Public Property Emails As List(Of element)
    <DataMember()> _
    Public Property FirstName As String
    <DataMember()> _
    Public Property Gender As String
    <DataMember()> _
    Public Property LastName As String
    <DataMember()> _
    Public Property Location As String
    <DataMember()> _
    Public Property PhoneNumbers As List(Of element)
End Class

<DataContract([Namespace]:="http://schemas.microsoft.com/ado/2007/08/dataservices")> _
Public Class element
    <DataMember()> _
    Public Property Address As String
    <DataMember()> _
    Public Property Type As String
    <DataMember()> _
    Public Property City As String
    <DataMember()> _
    Public Property CountryRegion As String
End Class