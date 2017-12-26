'*************************** Module Header ******************************'
' Module Name:  BlockSites.vb
' Project:	    VBWebBrowserAutomation
' Copyright (c) Microsoft Corporation.
' 
' This BlockSites class includes a list of block sites. The static method GetBlockSites
' deserializes the BlockList.xml to a BlockSites instance.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Public Class BlockSites

    Public Property Hosts() As List(Of String)

    Private Shared ReadOnly _instance As BlockSites = GetBlockSites()

    Public Shared ReadOnly Property Instance() As BlockSites
        Get
            Return _instance
        End Get
    End Property

    ''' <summary>
    ''' Deserialize the BlockList.xml to a BlockSites instance.
    ''' </summary>
    Private Shared Function GetBlockSites() As BlockSites
        Dim path As String = String.Format("{0}\Resources\BlockList.xml",
                                           Environment.CurrentDirectory)
        Return XMLSerialization(Of BlockSites).DeserializeFromXMLToObject(path)
    End Function

End Class
