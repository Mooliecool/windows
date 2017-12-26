'*************************** Module Header ******************************'
' Module Name:  XMLSerialization.vb
' Project:	    VBWebBrowserAutomation
' Copyright (c) Microsoft Corporation.
' 
' This class is used to serialize an object to an XML file or deserialize an XML
' file to an object.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.IO
Imports System.Xml.Serialization

Public Class XMLSerialization(Of T)

    ''' <summary>
    ''' Serialize an object to an XML file. 
    ''' </summary>
    Public Shared Function SerializeFromObjectToXML(ByVal obj As T, _
                                                    ByVal filepath As String) As Boolean
        If obj Is Nothing Then
            Throw New ArgumentException("The object to serialize could not be null!")
        End If

        Dim successed As Boolean = False
        Dim objType As Type = obj.GetType()
        Using fs As New FileStream(filepath, FileMode.Create, FileAccess.ReadWrite)
            Dim xs As New XmlSerializer(objType)
            xs.Serialize(fs, obj)
            successed = True
        End Using

        Return successed
    End Function

    ''' <summary>
    ''' Deserialize an XML file to an object.
    ''' </summary>
    Public Shared Function DeserializeFromXMLToObject(ByVal filepath As String) As T
        If Not File.Exists(filepath) Then
            Throw New ArgumentException("The file does not exist!")
        End If

        Dim obj As T
        Using fs As New FileStream(filepath, FileMode.Open)
            Dim xs As New XmlSerializer(GetType(T))
            obj = CType(xs.Deserialize(fs), T)
        End Using
        Return obj
    End Function

End Class
