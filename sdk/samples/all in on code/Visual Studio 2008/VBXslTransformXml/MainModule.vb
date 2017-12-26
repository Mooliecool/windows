'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBXslTransformXml
' Copyright (c) Microsoft Corporation.
' 
' This sample project shows how to use XslCompiledTransform to transform an 
' XML data file to .csv file using an XSLT style sheet.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.Xml.Xsl

#End Region


Module MainModule

    Sub Main()
        Dim transform As New XslCompiledTransform()
        transform.Load("Books.xslt")
        transform.Transform("Books.xml", "Books.csv")
    End Sub

End Module
