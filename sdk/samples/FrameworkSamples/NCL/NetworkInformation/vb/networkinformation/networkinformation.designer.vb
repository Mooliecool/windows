'---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
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

Partial Public Class NetworkInformation
    Inherits System.Windows.Forms.Form

    <System.Diagnostics.DebuggerNonUserCode()> _
    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()
    End Sub

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Me.networkAvailabilityLabel = New System.Windows.Forms.Label
        Me.networkAvailabilityTextLabel = New System.Windows.Forms.Label
        Me.interfacesLabel = New System.Windows.Forms.Label
        Me.dnsSuffixLabel = New System.Windows.Forms.Label
        Me.dnsSuffixTextLabel = New System.Windows.Forms.Label
        Me.bytesSentLabel = New System.Windows.Forms.Label
        Me.networkInterfacesComboBox = New System.Windows.Forms.ComboBox
        Me.bytesSentTextLabel = New System.Windows.Forms.Label
        Me.supportsMulticastLabel = New System.Windows.Forms.Label
        Me.supportsMulticastTextLabel = New System.Windows.Forms.Label
        Me.operationalStatusLabel = New System.Windows.Forms.Label
        Me.operationalStatusTextLabel = New System.Windows.Forms.Label
        Me.bytesReceivedLabel = New System.Windows.Forms.Label
        Me.speedLabel = New System.Windows.Forms.Label
        Me.speedTextLabel = New System.Windows.Forms.Label
        Me.addressListView = New System.Windows.Forms.ListView
        Me.addressColumnHeader = New System.Windows.Forms.ColumnHeader("")
        Me.addressTypeColumnHeader = New System.Windows.Forms.ColumnHeader("")
        Me.bytesReceivedTextLabel = New System.Windows.Forms.Label
        Me.updateInfoTimer = New System.Windows.Forms.Timer(Me.components)
        Me.SuspendLayout()
        '
        'networkAvailabilityLabel
        '
        Me.networkAvailabilityLabel.Location = New System.Drawing.Point(11, 12)
        Me.networkAvailabilityLabel.Margin = New System.Windows.Forms.Padding(3, 3, 3, 1)
        Me.networkAvailabilityLabel.Name = "networkAvailabilityLabel"
        Me.networkAvailabilityLabel.Size = New System.Drawing.Size(106, 19)
        Me.networkAvailabilityLabel.TabIndex = 3
        Me.networkAvailabilityLabel.Text = "Network Availability:"
        '
        'networkAvailabilityTextLabel
        '
        Me.networkAvailabilityTextLabel.Location = New System.Drawing.Point(122, 12)
        Me.networkAvailabilityTextLabel.Name = "networkAvailabilityTextLabel"
        Me.networkAvailabilityTextLabel.Size = New System.Drawing.Size(380, 20)
        Me.networkAvailabilityTextLabel.TabIndex = 4
        Me.networkAvailabilityTextLabel.Text = "At least one network interface is up"
        '
        'interfacesLabel
        '
        Me.interfacesLabel.Location = New System.Drawing.Point(11, 37)
        Me.interfacesLabel.Name = "interfacesLabel"
        Me.interfacesLabel.Size = New System.Drawing.Size(72, 20)
        Me.interfacesLabel.TabIndex = 1
        Me.interfacesLabel.Text = "Interfaces:"
        '
        'dnsSuffixLabel
        '
        Me.dnsSuffixLabel.Location = New System.Drawing.Point(11, 63)
        Me.dnsSuffixLabel.Name = "dnsSuffixLabel"
        Me.dnsSuffixLabel.Size = New System.Drawing.Size(68, 18)
        Me.dnsSuffixLabel.TabIndex = 15
        Me.dnsSuffixLabel.Text = "DNS Suffix:"
        '
        'dnsSuffixTextLabel
        '
        Me.dnsSuffixTextLabel.Location = New System.Drawing.Point(121, 63)
        Me.dnsSuffixTextLabel.Margin = New System.Windows.Forms.Padding(3, 3, 3, 1)
        Me.dnsSuffixTextLabel.Name = "dnsSuffixTextLabel"
        Me.dnsSuffixTextLabel.Size = New System.Drawing.Size(159, 23)
        Me.dnsSuffixTextLabel.TabIndex = 16
        Me.dnsSuffixTextLabel.Text = "n/a"
        '
        'bytesSentLabel
        '
        Me.bytesSentLabel.Location = New System.Drawing.Point(11, 90)
        Me.bytesSentLabel.Name = "bytesSentLabel"
        Me.bytesSentLabel.Size = New System.Drawing.Size(89, 18)
        Me.bytesSentLabel.TabIndex = 9
        Me.bytesSentLabel.Text = "Bytes Sent:"
        '
        'networkInterfacesComboBox
        '
        Me.networkInterfacesComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.networkInterfacesComboBox.DropDownWidth = 431
        Me.networkInterfacesComboBox.FormattingEnabled = True
        Me.networkInterfacesComboBox.Location = New System.Drawing.Point(85, 36)
        Me.networkInterfacesComboBox.Margin = New System.Windows.Forms.Padding(1, 3, 3, 3)
        Me.networkInterfacesComboBox.Name = "networkInterfacesComboBox"
        Me.networkInterfacesComboBox.Size = New System.Drawing.Size(431, 21)
        Me.networkInterfacesComboBox.TabIndex = 0
        '
        'bytesSentTextLabel
        '
        Me.bytesSentTextLabel.Location = New System.Drawing.Point(122, 91)
        Me.bytesSentTextLabel.Name = "bytesSentTextLabel"
        Me.bytesSentTextLabel.Size = New System.Drawing.Size(144, 17)
        Me.bytesSentTextLabel.TabIndex = 10
        Me.bytesSentTextLabel.Text = "0"
        '
        'supportsMulticastLabel
        '
        Me.supportsMulticastLabel.Location = New System.Drawing.Point(286, 63)
        Me.supportsMulticastLabel.Name = "supportsMulticastLabel"
        Me.supportsMulticastLabel.Size = New System.Drawing.Size(104, 21)
        Me.supportsMulticastLabel.TabIndex = 7
        Me.supportsMulticastLabel.Text = "Supports Multicast:"
        '
        'supportsMulticastTextLabel
        '
        Me.supportsMulticastTextLabel.Location = New System.Drawing.Point(418, 63)
        Me.supportsMulticastTextLabel.Margin = New System.Windows.Forms.Padding(3, 3, 3, 1)
        Me.supportsMulticastTextLabel.Name = "supportsMulticastTextLabel"
        Me.supportsMulticastTextLabel.Size = New System.Drawing.Size(73, 20)
        Me.supportsMulticastTextLabel.TabIndex = 8
        Me.supportsMulticastTextLabel.Text = "Yes"
        '
        'operationalStatusLabel
        '
        Me.operationalStatusLabel.Location = New System.Drawing.Point(286, 88)
        Me.operationalStatusLabel.Margin = New System.Windows.Forms.Padding(3, 0, 3, 3)
        Me.operationalStatusLabel.Name = "operationalStatusLabel"
        Me.operationalStatusLabel.Size = New System.Drawing.Size(104, 15)
        Me.operationalStatusLabel.TabIndex = 5
        Me.operationalStatusLabel.Text = "Operational Status:"
        '
        'operationalStatusTextLabel
        '
        Me.operationalStatusTextLabel.Location = New System.Drawing.Point(418, 86)
        Me.operationalStatusTextLabel.Margin = New System.Windows.Forms.Padding(3, 2, 3, 3)
        Me.operationalStatusTextLabel.Name = "operationalStatusTextLabel"
        Me.operationalStatusTextLabel.Size = New System.Drawing.Size(113, 16)
        Me.operationalStatusTextLabel.TabIndex = 6
        Me.operationalStatusTextLabel.Text = "Up"
        '
        'bytesReceivedLabel
        '
        Me.bytesReceivedLabel.Location = New System.Drawing.Point(11, 115)
        Me.bytesReceivedLabel.Name = "bytesReceivedLabel"
        Me.bytesReceivedLabel.Size = New System.Drawing.Size(88, 17)
        Me.bytesReceivedLabel.TabIndex = 11
        Me.bytesReceivedLabel.Text = "Bytes Received:"
        '
        'speedLabel
        '
        Me.speedLabel.Location = New System.Drawing.Point(286, 113)
        Me.speedLabel.Name = "speedLabel"
        Me.speedLabel.Size = New System.Drawing.Size(88, 18)
        Me.speedLabel.TabIndex = 13
        Me.speedLabel.Text = "Speed:"
        '
        'speedTextLabel
        '
        Me.speedTextLabel.Location = New System.Drawing.Point(418, 114)
        Me.speedTextLabel.Margin = New System.Windows.Forms.Padding(3, 1, 3, 3)
        Me.speedTextLabel.Name = "speedTextLabel"
        Me.speedTextLabel.Size = New System.Drawing.Size(119, 22)
        Me.speedTextLabel.TabIndex = 14
        Me.speedTextLabel.Text = "100"
        '
        'addressListView
        '
        Me.addressListView.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.addressColumnHeader, Me.addressTypeColumnHeader})
        Me.addressListView.FullRowSelect = True
        Me.addressListView.GridLines = True
        Me.addressListView.BackgroundImageTiled = False
        Me.addressListView.Location = New System.Drawing.Point(24, 147)
        Me.addressListView.Name = "addressListView"
        Me.addressListView.Size = New System.Drawing.Size(492, 272)
        Me.addressListView.TabIndex = 2
        Me.addressListView.View = System.Windows.Forms.View.Details
        '
        'addressColumnHeader
        '
        Me.addressColumnHeader.Text = "Address"
        Me.addressColumnHeader.Width = 236
        '
        'addressTypeColumnHeader
        '
        Me.addressTypeColumnHeader.Text = "Type"
        Me.addressTypeColumnHeader.Width = 248
        '
        'bytesReceivedTextLabel
        '
        Me.bytesReceivedTextLabel.Location = New System.Drawing.Point(122, 116)
        Me.bytesReceivedTextLabel.Name = "bytesReceivedTextLabel"
        Me.bytesReceivedTextLabel.Size = New System.Drawing.Size(158, 16)
        Me.bytesReceivedTextLabel.TabIndex = 12
        Me.bytesReceivedTextLabel.Text = "0"
        '
        'updateInfoTimer
        '
        Me.updateInfoTimer.Enabled = True
        '
        'NetworkInformation
        '
        Me.AutoSize = True
        Me.ClientSize = New System.Drawing.Size(558, 440)
        Me.Controls.Add(Me.bytesReceivedTextLabel)
        Me.Controls.Add(Me.addressListView)
        Me.Controls.Add(Me.speedTextLabel)
        Me.Controls.Add(Me.speedLabel)
        Me.Controls.Add(Me.bytesReceivedLabel)
        Me.Controls.Add(Me.operationalStatusTextLabel)
        Me.Controls.Add(Me.operationalStatusLabel)
        Me.Controls.Add(Me.supportsMulticastTextLabel)
        Me.Controls.Add(Me.supportsMulticastLabel)
        Me.Controls.Add(Me.bytesSentTextLabel)
        Me.Controls.Add(Me.networkInterfacesComboBox)
        Me.Controls.Add(Me.bytesSentLabel)
        Me.Controls.Add(Me.dnsSuffixTextLabel)
        Me.Controls.Add(Me.dnsSuffixLabel)
        Me.Controls.Add(Me.interfacesLabel)
        Me.Controls.Add(Me.networkAvailabilityTextLabel)
        Me.Controls.Add(Me.networkAvailabilityLabel)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D
        Me.MaximizeBox = False
        Me.Name = "NetworkInformation"
        Me.Padding = New System.Windows.Forms.Padding(9)
        Me.Text = "Network Information"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dnsSuffixLabel As System.Windows.Forms.Label
    Friend WithEvents dnsSuffixTextLabel As System.Windows.Forms.Label
    Friend WithEvents bytesSentLabel As System.Windows.Forms.Label
    Friend WithEvents networkInterfacesComboBox As System.Windows.Forms.ComboBox
    Friend WithEvents networkAvailabilityLabel As System.Windows.Forms.Label
    Friend WithEvents networkAvailabilityTextLabel As System.Windows.Forms.Label
    Friend WithEvents interfacesLabel As System.Windows.Forms.Label
    Friend WithEvents bytesSentTextLabel As System.Windows.Forms.Label
    Friend WithEvents supportsMulticastLabel As System.Windows.Forms.Label
    Friend WithEvents supportsMulticastTextLabel As System.Windows.Forms.Label
    Friend WithEvents operationalStatusLabel As System.Windows.Forms.Label
    Friend WithEvents operationalStatusTextLabel As System.Windows.Forms.Label
    Friend WithEvents bytesReceivedLabel As System.Windows.Forms.Label
    Friend WithEvents speedLabel As System.Windows.Forms.Label
    Friend WithEvents speedTextLabel As System.Windows.Forms.Label
    Friend WithEvents addressListView As System.Windows.Forms.ListView
    Friend WithEvents bytesReceivedTextLabel As System.Windows.Forms.Label
    Friend WithEvents addressColumnHeader As System.Windows.Forms.ColumnHeader
    Friend WithEvents addressTypeColumnHeader As System.Windows.Forms.ColumnHeader
    Friend WithEvents updateInfoTimer As System.Windows.Forms.Timer

End Class
