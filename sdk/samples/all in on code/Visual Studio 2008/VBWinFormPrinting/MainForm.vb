'************************************* Module Header **************************************\
' Module Name:  MainForm.vb
' Project:      VBWinFormPrinting
' Copyright (c) Microsoft Corporation.
' 
' The Printing sample demonstrates how to do standard print job in Windows Forms Application.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'******************************************************************************************/

Public Class MainForm
    Inherits Form

    Public Sub New()
        MyBase.New()
        InitializeComponent()
    End Sub

    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' The example assumes your form has a Button control, 
        ' a PrintDocument component named myDocument, 
        ' and a PrintPreviewDialog control. 

        ' Handle the PrintPage event to write the print logic.
        AddHandler PrintDocument1.PrintPage, AddressOf Me.printDocument1_PrintPage

        ' Specify a PrintDocument instance for the PrintPreviewDialog component.
        Me.PrintPreviewDialog1.Document = Me.PrintDocument1
    End Sub

    Private Sub printDocument1_PrintPage(ByVal sender As Object, ByVal e As System.Drawing.Printing.PrintPageEventArgs)
        ' Specify what to print and how to print in this event handler.
        ' The follow code specify a string and a rectangle to be print 

        Dim f As Font = New Font("Vanada", 12)
        Dim br As SolidBrush = New SolidBrush(Color.Black)
        Dim p As Pen = New Pen(Color.Black)
        e.Graphics.DrawString("This is a text.", f, br, 50, 50)

        e.Graphics.DrawRectangle(p, 50, 100, 300, 150)
    End Sub

    Private Sub btnPrint_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnPrint.Click
        Me.PrintPreviewDialog1.ShowDialog()
    End Sub
    
End Class
