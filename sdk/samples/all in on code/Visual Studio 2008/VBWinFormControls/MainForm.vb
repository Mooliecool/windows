'************************************* Module Header **************************************'
' Module Name:  MainForm.vb
' Project:      VBWinFormControls
' Copyright (c) Microsoft Corporation.
' 
' The Control Customization sample demonstrates how to customize the Windows Forms controls.
' 
' In this sample, there're 4 examples:
' 
' 1. Multiple Column ComboBox.
'    Demonstrates how to display multiple columns of data in the dropdown of a ComboBox.
' 2. ListBox Items With Different ToolTips.
'    Demonstrates how to display different tooltips on each items of the ListBox.
' 3. Numeric-only TextBox.
'    Demonstrates how to make a TextBox only accepts numbers.
' 4. A Round Button.
'    Demonstrates how to create a Button with irregular shape.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 1/9/2010 3:00 PM Kira Qian Created
'******************************************************************************************'

Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
Imports System.Drawing.Drawing2D

Public Class MainForm

    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        MultipleColumnComboBox()
        ListBoxWithDiffToolTips()
    End Sub

#Region "Example 1 -- Multiple Column ComboBox"
    Private Sub MultipleColumnComboBox()
        Dim dtTest As DataTable = New DataTable()
        dtTest.Columns.Add("ID", GetType(Integer))
        dtTest.Columns.Add("Name", GetType(String))

        dtTest.Rows.Add(1, "John")
        dtTest.Rows.Add(2, "Amy")
        dtTest.Rows.Add(3, "Tony")
        dtTest.Rows.Add(4, "Bruce")
        dtTest.Rows.Add(5, "Allen")

        ' Bind the ComboBox to the DataTable
        Me.comboBox1.DataSource = dtTest
        Me.comboBox1.DisplayMember = "Name"
        Me.comboBox1.ValueMember = "ID"

        ' Enable the owner draw on the ComboBox.
        Me.comboBox1.DrawMode = DrawMode.OwnerDrawFixed
        ' Handle the DrawItem event to draw the items.
    End Sub

    Private Sub comboBox1_DrawItem(ByVal sender As System.Object, _
                                   ByVal e As System.Windows.Forms.DrawItemEventArgs) _
                                   Handles comboBox1.DrawItem
        ' Draw the default background
        e.DrawBackground()

        ' The ComboBox is bound to a DataTable,
        ' so the items are DataRowView objects.
        Dim drv As DataRowView = CType(comboBox1.Items(e.Index), DataRowView)

        ' Retrieve the value of each column.
        Dim id As Integer = drv("ID").ToString()
        Dim name As String = drv("Name").ToString()

        ' Get the bounds for the first column
        Dim r1 As Rectangle = e.Bounds
        r1.Width = r1.Width / 2

        ' Draw the text on the first column
        Using sb As SolidBrush = New SolidBrush(e.ForeColor)
            e.Graphics.DrawString(id, e.Font, sb, r1)
        End Using

        ' Draw a line to isolate the columns 
        Using p As Pen = New Pen(Color.Black)
            e.Graphics.DrawLine(p, r1.Right, 0, r1.Right, r1.Bottom)
        End Using

        ' Get the bounds for the second column
        Dim r2 As Rectangle = e.Bounds
        r2.X = e.Bounds.Width / 2
        r2.Width = r2.Width / 2

        ' Draw the text on the second column
        Using sb As SolidBrush = New SolidBrush(e.ForeColor)
            e.Graphics.DrawString(name, e.Font, sb, r2)
        End Using
    End Sub
#End Region

#Region "Example 2 -- ListBox Items With Different ToolTips"
    Private Sub ListBoxWithDiffToolTips()
        ' Setup the ListBox items
        Me.listBox1.Items.Add("Item1")
        Me.listBox1.Items.Add("Item2")
        Me.listBox1.Items.Add("Item3")
        Me.listBox1.Items.Add("Item4")
        Me.listBox1.Items.Add("Item5")
    End Sub

    Private Sub listBox1_MouseMove(ByVal sender As System.Object, _
                    ByVal e As System.Windows.Forms.MouseEventArgs) _
                                   Handles listBox1.MouseMove
        Dim hoverIndex As Integer = Me.listBox1.IndexFromPoint(e.Location)

        If hoverIndex >= 0 And hoverIndex < listBox1.Items.Count Then
            Me.toolTip1.SetToolTip(listBox1, listBox1.Items(hoverIndex).ToString())
        End If
    End Sub
#End Region

#Region "Example 3 -- Numeric-only TextBox"
    Private Sub textBox1_KeyPress(ByVal sender As System.Object, _
                    ByVal e As System.Windows.Forms.KeyPressEventArgs) _
                                  Handles textBox1.KeyPress
        If Not (Char.IsNumber(e.KeyChar) Or e.KeyChar = vbBack) Then
            e.Handled = True
        End If
    End Sub
#End Region

#Region "Example 4 -- A Round Button"
    Private Sub roundButton1_Click(ByVal sender As System.Object, _
                                   ByVal e As System.EventArgs) _
                                   Handles roundButton1.Click
        MessageBox.Show("Clicked!")
    End Sub
#End Region

End Class

#Region "RoundButton Class"
Public Class RoundButton
    Inherits Button

    Protected Overrides Sub OnPaint(ByVal pevent As System.Windows.Forms.PaintEventArgs)
        Dim path As GraphicsPath = New GraphicsPath()
        path.AddEllipse(0, 0, ClientSize.Width, ClientSize.Height)
        Me.Region = New Region(path)
        MyBase.OnPaint(pevent)
    End Sub
End Class

#End Region
