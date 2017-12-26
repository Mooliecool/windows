'******************************** Module Header **************************************'
' Module Name:  MainForm.vb
' Project:      VBHexStringByteArrayConverter
' Copyright (c) Microsoft Corporation.
'
' This sample demonstrates how to convert byte array to hex string and vice 
' versa. For example, 
' 
'    "FF00EE11" <--> { FF, 00, EE, 11 }
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'******************************** Module Header **************************************'

Imports System.Globalization


Partial Public Class MainForm
    Inherits Form
    Public Sub New()
        InitializeComponent()
    End Sub

    ''' <summary>
    ''' Convert hex string into byte array.
    ''' </summary>
    Private Sub btnConvertHexStringToByteArray_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnConvertHexStringToBytes.Click
        If String.IsNullOrWhiteSpace(tbHexStringInput.Text) Then
            MessageBox.Show("Please input hex strings that you want to convert!")
            Return
        End If
        If Not HexStringByteArrayConverter.VerifyHexString(tbHexStringInput.Text) Then
            MessageBox.Show("The hex string is not in the right format.")
            Return
        End If

        ' Convert the hex string to a byte array
        Dim result() As Byte = HexStringByteArrayConverter.HexStringToBytes(tbHexStringInput.Text)
        Me.cmbByteArrayResult.DataSource = result
    End Sub

    ''' <summary>
    ''' Convert bytes array into hex strings.
    ''' </summary>
    Private Sub btnConvertByteArrayToHexString_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnConvertBytesToHexString.Click
        If Me.cmbByteArrayInput.Items.Count = 0 Then
            MessageBox.Show("Please input bytes array that you want to convert!")
            Return
        End If

        Dim bytes(Me.cmbByteArrayInput.Items.Count - 1) As Byte
        For i As Integer = 0 To Me.cmbByteArrayInput.Items.Count - 1
            bytes(i) = CByte(Me.cmbByteArrayInput.Items(i))
        Next i

        tbHexStringResult.Text = HexStringByteArrayConverter.BytesToHexString(bytes)
    End Sub

    ''' <summary>
    ''' Add a byte into the ComboBox control.
    ''' </summary>
    Private Sub btnAddByte_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnAddByte.Click
        If String.IsNullOrWhiteSpace(Me.tbByteToAdd.Text) Then
            MessageBox.Show("Please input the byte.")
            Return
        End If

        Dim byteToAdd As Byte
        If Not Byte.TryParse(Me.tbByteToAdd.Text, NumberStyles.AllowHexSpecifier, Nothing, byteToAdd) Then
            MessageBox.Show("The byte input is not in the right format.")
            Return
        End If

        Me.cmbByteArrayInput.Items.Add(byteToAdd)
        Me.tbByteToAdd.Text = ""
        Me.cmbByteArrayInput.SelectedIndex = 0
    End Sub

    ''' <summary>
    ''' Clear the ComboBox control.
    ''' </summary>
    Private Sub btnClearBytes_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnClearBytes.Click
        Me.cmbByteArrayInput.Items.Clear()
    End Sub

    ''' <summary>
    ''' Copy the data into Clipboard.
    ''' </summary>
    Private Sub btnCopytoClipboard_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnReverseCopytoClipboard.Click
        If cmbByteArrayResult.Items.Count <> 0 Then
            ' Convert the combobox data to a serializable string collection.
            Dim items As New List(Of Byte)()
            For Each b As Byte In Me.cmbByteArrayResult.Items
                items.Add(b)
            Next b

            Dim ido As IDataObject = New DataObject()
            ido.SetData(GetType(ComboBox).FullName, True, items)
            Clipboard.SetDataObject(ido, False)
        End If
    End Sub

    ''' <summary>
    ''' Paste the data that comes from clipboard to Textbox control.
    ''' </summary>
    Private Sub btnPasteFromClipboard_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnPasteFromClipboard.Click
        Dim ido As IDataObject = Clipboard.GetDataObject()
        Dim format As String = GetType(ComboBox).FullName
        If ido.GetDataPresent(format) Then
            Dim items As List(Of Byte) = TryCast(ido.GetData(format), List(Of Byte))
            If items IsNot Nothing Then
                Me.cmbByteArrayInput.Items.Clear()
                For Each b As Byte In items
                    Me.cmbByteArrayInput.Items.Add(b)
                Next b
                Me.cmbByteArrayInput.SelectedIndex = 0
            End If
        End If
    End Sub
End Class

