'****************************** Module Header ******************************'
' Module Name:                MainPage.xaml.vb
' Project:                    VBSL4COMInterop
' Copyright (c) Microsoft Corporation.
' 
' Silverlight COM interoperate sample codebehind file.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Runtime.InteropServices.Automation
Imports System.Threading

Partial Public Class MainPage
    Inherits UserControl

    Public Sub New()
        InitializeComponent()
        AddHandler Me.Loaded, AddressOf MainPage_Loaded
    End Sub

    Private Sub MainPage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Create entity list
        Dim list = New List(Of PersonEntity)()
        Dim rand = New Random()
        For i As Integer = 0 To 8
            Dim newentity = New PersonEntity()
            newentity.Name = "Person:" + i.ToString()
            newentity.Age = rand.Next(100)
            If i Mod 2 = 0 Then
                newentity.Gender = "Male"
            Else
                newentity.Gender = "Female"
            End If
            list.Add(newentity)
        Next

        ' Bind entity list to datagrid.
        dataGrid1.ItemsSource = list
    End Sub

    Private _isprint As Boolean
    ' Update "print directly" state.
    Private Sub CheckBox_StateChanged(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim state = DirectCast(sender, CheckBox).IsChecked
        If state.HasValue AndAlso state.Value Then
            _isprint = True
        Else
            _isprint = False
        End If
    End Sub

    ' Export data to notepad.
    Private Sub TextExport_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Check if using AutomationFactory is allowed.
        If Not AutomationFactory.IsAvailable Then
            MessageBox.Show("This function need the silverlight application running at evaluated OOB mode.")
        Else
            ' Use shell to open notepad application.
            Dim shell = AutomationFactory.CreateObject("WScript.Shell")
            shell.Run("%windir%\notepad", 5)
            Thread.Sleep(100)

            shell.SendKeys("Name{Tab}Age{Tab}Gender{Enter}")
            For Each item As PersonEntity In TryCast(dataGrid1.ItemsSource, List(Of PersonEntity))
                shell.SendKeys(item.Name & "{Tab}" & item.Age & "{Tab}" & item.Gender & "{Enter}")
            Next
        End If
    End Sub

    ' Export data to word.
    Private Sub WordExport_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Check if using AutomationFactory is allowed.
        If Not AutomationFactory.IsAvailable Then
            MessageBox.Show("This function need the silverlight application running at evaluated OOB mode.")
        Else
            ' Create Word automation object.
            Dim word = AutomationFactory.CreateObject("Word.Application")
            word.Visible = True

            ' Create a new word document.
            Dim doc = word.Documents.Add()

            ' Write title
            Dim range1 = doc.Paragraphs(1).Range
            range1.Text = "Silverlight4 Word Automation Sample" & vbLf
            range1.Font.Size = 24
            range1.Font.Bold = True

            Dim list = TryCast(dataGrid1.ItemsSource, List(Of PersonEntity))

            Dim range2 = doc.Paragraphs(2).Range
            range2.Font.Size = 12
            range2.Font.Bold = False

            ' Create table
            doc.Tables.Add(range2, list.Count + 1, 3, Nothing, Nothing)

            Dim cell = doc.Tables(1).Cell(1, 1)
            cell.Range.Text = "Name"
            cell.Range.Font.Bold = True

            cell = doc.Tables(1).Cell(1, 2)
            cell.Range.Text = "Age"
            cell.Range.Font.Bold = True

            cell = doc.Tables(1).Cell(1, 3)
            cell.Range.Text = "Gender"
            cell.Range.Font.Bold = True

            ' Fill data to table cells
            For i As Integer = 0 To list.Count - 1
                cell = doc.Tables(1).Cell(i + 2, 1)
                cell.Range.Text = list(i).Name

                cell = doc.Tables(1).Cell(i + 2, 2)
                cell.Range.Text = list(i).Age

                cell = doc.Tables(1).Cell(i + 2, 3)
                cell.Range.Text = list(i).Gender
            Next

            If _isprint Then
                ' Print the word directly without preview.
                doc.PrintOut()

            End If
        End If
    End Sub
End Class