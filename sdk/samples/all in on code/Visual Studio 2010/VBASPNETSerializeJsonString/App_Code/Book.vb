'/****************************** Module Header ******************************\
'* Module Name:    Book.vb
'* Project:        VBASPNETSerializeJsonString
'* Copyright (c) Microsoft Corporation
'*
'* This project illustrates how to serialize Json string. we use jQuery at client 
'* side and manipulate XML data at server side.
'* It demonstrates how to use the serializable json data through an autocomplete 
'* example. 
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\*****************************************************************************/

Imports System
Imports System.Web


Public Class Book

    ' autocomplete example needs "id", "value" and the "label" variables to be sent back.
    ' do not change or remove "id", "value" and the "label" variables
    Public Property id() As String
    Public Property label() As String
    Public Property value() As String

    Public Property Author() As String
    Public Property Genre() As String
    Public Property Price() As String
    Public Property Publish_date() As String
    Public Property Description() As String
End Class
