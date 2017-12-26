'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.IO
Imports System.ServiceModel.Channels
Imports System.Xml

Public Class ByteStreamBodyWriter
    Inherits BodyWriter
    Private testFileName As String

    Public Sub New(ByVal testFileName As String)
        MyBase.New(False)
        Me.testFileName = testFileName
    End Sub

    Protected Overrides Sub OnWriteBodyContents(ByVal writer As XmlDictionaryWriter)
        writer.WriteStartElement("Binary")

        Dim fs As New FileStream(Me.testFileName, FileMode.Open)

        If fs.Length > Integer.MaxValue Then
            Throw New ApplicationException(String.Format("{0} is too large.", Me.testFileName))
        End If

        Dim tmp(CInt(fs.Length - 1)) As Byte

        fs.Read(tmp, 0, tmp.Length)
        writer.WriteBase64(tmp, 0, CInt(Fix(tmp.Length)))

        writer.WriteEndElement()
        fs.Close()
    End Sub

End Class
