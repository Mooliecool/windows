'*************************** Module Header ******************************\
' Module Name:  RegistryKeyChangeEventArgs.vb
' Project:	    VBMonitorRegistryChange
' Copyright (c) Microsoft Corporation.
' 
' This class derived from EventArgs. It is used to wrap the ManagementBaseObject of
' EventArrivedEventArgs.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************

Imports System.Management

Friend Class RegistryKeyChangeEventArgs
    Inherits EventArgs

    Public Property Hive() As String
    Public Property KeyPath() As String
    Public Property SECURITY_DESCRIPTOR() As UInteger()
    Public Property TIME_CREATED() As Date

    Public Sub New(ByVal arrivedEvent As ManagementBaseObject)

        ' Class RegistryKeyChangeEvent has following properties: Hive, KeyPath, 
        ' SECURITY_DESCRIPTOR and TIME_CREATED. These properties could get from
        ' arrivedEvent.Properties.
        Me.Hive = TryCast(arrivedEvent.Properties("Hive").Value, String)
        Me.KeyPath = TryCast(arrivedEvent.Properties("KeyPath").Value, String)

        ' The property TIME_CREATED is a unique value that indicates the time 
        ' when an event is generated. 
        ' This is a 64-bit FILETIME value that represents the number of 
        ' 100-nanosecond intervals after January 1, 1601. The information is in
        ' the Coordinated Universal Time (UTC) format. 
        Me.TIME_CREATED = New Date(
            CLng(Fix(CULng(arrivedEvent.Properties("TIME_CREATED").Value))),
            DateTimeKind.Utc).AddYears(1600)
    End Sub
End Class
