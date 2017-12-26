'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Mainform
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    Private components As System.ComponentModel.IContainer = Nothing

#Region "Windows Form Designer generated code"
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.buttonOrderCreated = New System.Windows.Forms.Button()
        Me.label1 = New System.Windows.Forms.Label()
        Me.textBoxOrderID = New System.Windows.Forms.TextBox()
        Me.listViewOrders = New System.Windows.Forms.ListView()
        Me.columnWorkflowInstanceID = New System.Windows.Forms.ColumnHeader()
        Me.columnOrderID = New System.Windows.Forms.ColumnHeader()
        Me.columnOrderState = New System.Windows.Forms.ColumnHeader()
        Me.columnWorkflowStatus = New System.Windows.Forms.ColumnHeader()
        Me.contextMenuOrdersList = New System.Windows.Forms.ContextMenuStrip(Me.components)
        Me.addOnHoldStateToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.buttonOrderShipped = New System.Windows.Forms.Button()
        Me.buttonOrderUpdated = New System.Windows.Forms.Button()
        Me.buttonOrderCanceled = New System.Windows.Forms.Button()
        Me.buttonOrderProcessed = New System.Windows.Forms.Button()
        Me.comboBoxWorkflowStates = New System.Windows.Forms.ComboBox()
        Me.buttonSetState = New System.Windows.Forms.Button()
        Me.label2 = New System.Windows.Forms.Label()
        Me.contextMenuOrdersList.SuspendLayout()
        Me.SuspendLayout()
        ' 
        ' buttonOrderCreated
        ' 
        Me.buttonOrderCreated.Location = New System.Drawing.Point(140, 32)
        Me.buttonOrderCreated.Name = "buttonOrderCreated"
        Me.buttonOrderCreated.Size = New System.Drawing.Size(99, 23)
        Me.buttonOrderCreated.TabIndex = 0
        Me.buttonOrderCreated.Text = "Order Created"
        ' 
        ' label1
        ' 
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(11, 19)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(44, 13)
        Me.label1.TabIndex = 2
        Me.label1.Text = "OrderID"
        ' 
        ' textBoxOrderID
        ' 
        Me.textBoxOrderID.Location = New System.Drawing.Point(11, 34)
        Me.textBoxOrderID.Name = "textBoxOrderID"
        Me.textBoxOrderID.Size = New System.Drawing.Size(122, 20)
        Me.textBoxOrderID.TabIndex = 3
        ' 
        ' listViewOrders
        ' 
        Me.listViewOrders.Columns.AddRange(New System.Windows.Forms.ColumnHeader() { _
            Me.columnWorkflowInstanceID, _
            Me.columnOrderID, _
            Me.columnOrderState, _
            Me.columnWorkflowStatus})
        Me.listViewOrders.ContextMenuStrip = Me.contextMenuOrdersList
        Me.listViewOrders.FullRowSelect = True
        Me.listViewOrders.GridLines = True
        Me.listViewOrders.Location = New System.Drawing.Point(2, 72)
        Me.listViewOrders.MultiSelect = False
        Me.listViewOrders.Name = "listViewOrders"
        Me.listViewOrders.Size = New System.Drawing.Size(537, 109)
        Me.listViewOrders.TabIndex = 4
        Me.listViewOrders.UseCompatibleStateImageBehavior = False
        Me.listViewOrders.View = System.Windows.Forms.View.Details
        ' 
        ' columnlWorkflowInstanceID
        ' 
        Me.columnWorkflowInstanceID.Name = "columnWorkflowInstanceID"
        Me.columnWorkflowInstanceID.Text = "Workflow InstanceID"
        Me.columnWorkflowInstanceID.Width = 186
        ' 
        ' columnOrderID
        ' 
        Me.columnOrderID.Name = "columnOrderID"
        Me.columnOrderID.Text = "OrderID"
        Me.columnOrderID.Width = 100
        ' 
        ' columnOrderState
        ' 
        Me.columnOrderState.Name = "columnOrderState"
        Me.columnOrderState.Text = "Order State"
        Me.columnOrderState.Width = 139
        ' 
        ' columnWorkflowStatus
        ' 
        Me.columnWorkflowStatus.Name = "columnWorkflowStatus"
        Me.columnWorkflowStatus.Text = "Workflow Status"
        Me.columnWorkflowStatus.Width = 106
        ' 
        ' contextMenuOrdersList
        ' 
        Me.contextMenuOrdersList.Items.AddRange(New System.Windows.Forms.ToolStripItem() { _
            Me.addOnHoldStateToolStripMenuItem})
        Me.contextMenuOrdersList.Name = "contextMenuOrdersList"
        Me.contextMenuOrdersList.Size = New System.Drawing.Size(164, 26)
        ' 
        ' addOnHoldStateToolStripMenuItem
        ' 
        Me.addOnHoldStateToolStripMenuItem.Name = "addOnHoldStateToolStripMenuItem"
        Me.addOnHoldStateToolStripMenuItem.Size = New System.Drawing.Size(163, 22)
        Me.addOnHoldStateToolStripMenuItem.Text = "Add On Hold State"
        ' 
        ' buttonOrderShipped
        ' 
        Me.buttonOrderShipped.Enabled = False
        Me.buttonOrderShipped.Location = New System.Drawing.Point(439, 189)
        Me.buttonOrderShipped.Name = "buttonOrderShipped"
        Me.buttonOrderShipped.Size = New System.Drawing.Size(99, 23)
        Me.buttonOrderShipped.TabIndex = 5
        Me.buttonOrderShipped.Text = "Order Shipped"
        ' 
        ' buttonOrderUpdated
        ' 
        Me.buttonOrderUpdated.Enabled = False
        Me.buttonOrderUpdated.Location = New System.Drawing.Point(88, 189)
        Me.buttonOrderUpdated.Name = "buttonOrderUpdated"
        Me.buttonOrderUpdated.Size = New System.Drawing.Size(99, 23)
        Me.buttonOrderUpdated.TabIndex = 7
        Me.buttonOrderUpdated.Text = "Order Updated"
        ' 
        ' buttonOrderCanceled
        ' 
        Me.buttonOrderCanceled.Enabled = False
        Me.buttonOrderCanceled.Location = New System.Drawing.Point(205, 189)
        Me.buttonOrderCanceled.Name = "buttonOrderCanceled"
        Me.buttonOrderCanceled.Size = New System.Drawing.Size(99, 23)
        Me.buttonOrderCanceled.TabIndex = 8
        Me.buttonOrderCanceled.Text = "Order Canceled"
        ' 
        ' buttonOrderProcessed
        ' 
        Me.buttonOrderProcessed.Enabled = False
        Me.buttonOrderProcessed.Location = New System.Drawing.Point(322, 189)
        Me.buttonOrderProcessed.Name = "buttonOrderProcessed"
        Me.buttonOrderProcessed.Size = New System.Drawing.Size(99, 23)
        Me.buttonOrderProcessed.TabIndex = 9
        Me.buttonOrderProcessed.Text = "Order Processed"
        ' 
        ' comboBoxWorkflowStates
        ' 
        Me.comboBoxWorkflowStates.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.comboBoxWorkflowStates.FormattingEnabled = True
        Me.comboBoxWorkflowStates.Location = New System.Drawing.Point(291, 32)
        Me.comboBoxWorkflowStates.Name = "comboBoxWorkflowStates"
        Me.comboBoxWorkflowStates.Size = New System.Drawing.Size(152, 21)
        Me.comboBoxWorkflowStates.TabIndex = 10
        ' 
        ' buttonSetState
        ' 
        Me.buttonSetState.Location = New System.Drawing.Point(449, 32)
        Me.buttonSetState.Name = "buttonSetState"
        Me.buttonSetState.Size = New System.Drawing.Size(36, 23)
        Me.buttonSetState.TabIndex = 11
        Me.buttonSetState.Text = "&Go"
        ' 
        ' label2
        ' 
        Me.label2.AutoSize = True
        Me.label2.Location = New System.Drawing.Point(288, 16)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(54, 13)
        Me.label2.TabIndex = 12
        Me.label2.Text = "Set State:"
        ' 
        ' Form1
        ' 
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(543, 223)
        Me.Controls.Add(Me.buttonSetState)
        Me.Controls.Add(Me.comboBoxWorkflowStates)
        Me.Controls.Add(Me.label2)
        Me.Controls.Add(Me.buttonOrderProcessed)
        Me.Controls.Add(Me.buttonOrderCanceled)
        Me.Controls.Add(Me.buttonOrderUpdated)
        Me.Controls.Add(Me.buttonOrderShipped)
        Me.Controls.Add(Me.listViewOrders)
        Me.Controls.Add(Me.textBoxOrderID)
        Me.Controls.Add(Me.label1)
        Me.Controls.Add(Me.buttonOrderCreated)
        Me.Name = "Form1"
        Me.Text = "Order Application - State Machine Example"
        Me.contextMenuOrdersList.ResumeLayout(False)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub


#End Region

    Dim label1 As System.Windows.Forms.Label
    Dim textBoxOrderID As System.Windows.Forms.TextBox
    Dim WithEvents listViewOrders As System.Windows.Forms.ListView
    Dim columnOrderID As System.Windows.Forms.ColumnHeader
    Dim columnWorkflowInstanceID As System.Windows.Forms.ColumnHeader
    Dim WithEvents buttonOrderShipped As System.Windows.Forms.Button
    Dim WithEvents buttonOrderUpdated As System.Windows.Forms.Button
    Dim WithEvents buttonOrderCanceled As System.Windows.Forms.Button
    Dim WithEvents buttonOrderProcessed As System.Windows.Forms.Button
    Dim columnOrderState As System.Windows.Forms.ColumnHeader
    Dim WithEvents buttonOrderCreated As System.Windows.Forms.Button
    Dim columnWorkflowStatus As System.Windows.Forms.ColumnHeader
    Dim WithEvents buttonSetState As System.Windows.Forms.Button
    Dim comboBoxWorkflowStates As System.Windows.Forms.ComboBox
    Dim label2 As System.Windows.Forms.Label
    Dim WithEvents contextMenuOrdersList As System.Windows.Forms.ContextMenuStrip
    Dim addOnHoldStateToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
End Class
