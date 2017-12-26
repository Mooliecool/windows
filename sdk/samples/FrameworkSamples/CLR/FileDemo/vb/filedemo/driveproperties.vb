'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
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
'-----------------------------------------------------------------------

Imports System.IO
Imports System.Globalization

Namespace Microsoft.Samples.FileDemo
    Public NotInheritable Class DriveProperties
        Inherits System.Windows.Forms.Form

        ' the declaration for the object we will manipulate...
        ' simply declare d here
        Dim d As DriveInfo


        Dim sweepAngle As Single

#Region " Windows Form Designer generated code "

        Public Sub New()
            MyBase.New()

            'This call is required by the Windows Form Designer.
            InitializeComponent()

            'Add any initialization after the InitializeComponent() call

        End Sub

        'Form overrides dispose to clean up the component list.
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub

        'Required by the Windows Form Designer
        Private components As System.ComponentModel.IContainer

        'NOTE: The following procedure is required by the Windows Form Designer
        'It can be modified using the Windows Form Designer.  
        'Do not modify it using the code editor.
        Friend WithEvents labelTypeValue As System.Windows.Forms.Label
        Friend WithEvents buttonOK As System.Windows.Forms.Button
        Friend WithEvents line1 As System.Windows.Forms.GroupBox
        Friend WithEvents labelType As System.Windows.Forms.Label
        Friend WithEvents labelCapacityValue As System.Windows.Forms.Label
        Friend WithEvents labelFileSystemValue As System.Windows.Forms.Label
        Friend WithEvents labelCapacity As System.Windows.Forms.Label
        Friend WithEvents labelFreeSpace As System.Windows.Forms.Label
        Friend WithEvents labelUsedSpace As System.Windows.Forms.Label
        Friend WithEvents labelFileSystem As System.Windows.Forms.Label
        Friend WithEvents labelFreeSpaceValue As System.Windows.Forms.Label
        Friend WithEvents labelUsedSpaceValue As System.Windows.Forms.Label
        Friend WithEvents labelUsedShort As System.Windows.Forms.Label
        Friend WithEvents labelCapacityShort As System.Windows.Forms.Label
        Friend WithEvents labelFreeShort As System.Windows.Forms.Label
        Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
        Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
        Friend WithEvents textUsedSpace As System.Windows.Forms.TextBox
        Friend WithEvents textFreeSpace As System.Windows.Forms.TextBox
        Friend WithEvents labelDrive As System.Windows.Forms.Label

        <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
            Me.labelCapacityValue = New System.Windows.Forms.Label
            Me.labelFreeSpaceValue = New System.Windows.Forms.Label
            Me.labelFileSystemValue = New System.Windows.Forms.Label
            Me.labelTypeValue = New System.Windows.Forms.Label
            Me.buttonOK = New System.Windows.Forms.Button
            Me.line1 = New System.Windows.Forms.GroupBox
            Me.labelCapacity = New System.Windows.Forms.Label
            Me.labelFreeSpace = New System.Windows.Forms.Label
            Me.labelUsedSpace = New System.Windows.Forms.Label
            Me.labelFileSystem = New System.Windows.Forms.Label
            Me.labelType = New System.Windows.Forms.Label
            Me.labelUsedSpaceValue = New System.Windows.Forms.Label
            Me.labelUsedShort = New System.Windows.Forms.Label
            Me.labelCapacityShort = New System.Windows.Forms.Label
            Me.labelFreeShort = New System.Windows.Forms.Label
            Me.GroupBox1 = New System.Windows.Forms.GroupBox
            Me.GroupBox2 = New System.Windows.Forms.GroupBox
            Me.textUsedSpace = New System.Windows.Forms.TextBox
            Me.textFreeSpace = New System.Windows.Forms.TextBox
            Me.labelDrive = New System.Windows.Forms.Label
            Me.SuspendLayout()
            '
            'labelCapacityValue
            '
            Me.labelCapacityValue.Location = New System.Drawing.Point(144, 167)
            Me.labelCapacityValue.Name = "labelCapacityValue"
            Me.labelCapacityValue.Size = New System.Drawing.Size(272, 25)
            Me.labelCapacityValue.TabIndex = 0
            Me.labelCapacityValue.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'labelFreeSpaceValue
            '
            Me.labelFreeSpaceValue.Location = New System.Drawing.Point(144, 117)
            Me.labelFreeSpaceValue.Name = "labelFreeSpaceValue"
            Me.labelFreeSpaceValue.Size = New System.Drawing.Size(272, 25)
            Me.labelFreeSpaceValue.TabIndex = 1
            Me.labelFreeSpaceValue.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'labelFileSystemValue
            '
            Me.labelFileSystemValue.Location = New System.Drawing.Point(144, 42)
            Me.labelFileSystemValue.Name = "labelFileSystemValue"
            Me.labelFileSystemValue.Size = New System.Drawing.Size(320, 25)
            Me.labelFileSystemValue.TabIndex = 3
            '
            'labelTypeValue
            '
            Me.labelTypeValue.Location = New System.Drawing.Point(144, 8)
            Me.labelTypeValue.Name = "labelTypeValue"
            Me.labelTypeValue.Size = New System.Drawing.Size(320, 25)
            Me.labelTypeValue.TabIndex = 25
            '
            'buttonOK
            '
            Me.buttonOK.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.buttonOK.Location = New System.Drawing.Point(432, 376)
            Me.buttonOK.Name = "buttonOK"
            Me.buttonOK.Size = New System.Drawing.Size(88, 34)
            Me.buttonOK.TabIndex = 23
            Me.buttonOK.Text = "OK"
            '
            'line1
            '
            Me.line1.AllowDrop = True
            Me.line1.Location = New System.Drawing.Point(16, 151)
            Me.line1.Name = "line1"
            Me.line1.Size = New System.Drawing.Size(504, 8)
            Me.line1.TabIndex = 20
            Me.line1.TabStop = False
            '
            'labelCapacity
            '
            Me.labelCapacity.Location = New System.Drawing.Point(16, 167)
            Me.labelCapacity.Name = "labelCapacity"
            Me.labelCapacity.Size = New System.Drawing.Size(104, 25)
            Me.labelCapacity.TabIndex = 19
            Me.labelCapacity.Text = "Capacity:"
            '
            'labelFreeSpace
            '
            Me.labelFreeSpace.Location = New System.Drawing.Point(32, 117)
            Me.labelFreeSpace.Name = "labelFreeSpace"
            Me.labelFreeSpace.Size = New System.Drawing.Size(112, 25)
            Me.labelFreeSpace.TabIndex = 18
            Me.labelFreeSpace.Text = "Free space:"
            '
            'labelUsedSpace
            '
            Me.labelUsedSpace.Location = New System.Drawing.Point(32, 84)
            Me.labelUsedSpace.Name = "labelUsedSpace"
            Me.labelUsedSpace.Size = New System.Drawing.Size(120, 25)
            Me.labelUsedSpace.TabIndex = 17
            Me.labelUsedSpace.Text = "Used space:"
            '
            'labelFileSystem
            '
            Me.labelFileSystem.Location = New System.Drawing.Point(16, 42)
            Me.labelFileSystem.Name = "labelFileSystem"
            Me.labelFileSystem.Size = New System.Drawing.Size(120, 25)
            Me.labelFileSystem.TabIndex = 16
            Me.labelFileSystem.Text = "File System:"
            '
            'labelType
            '
            Me.labelType.Location = New System.Drawing.Point(16, 8)
            Me.labelType.Name = "labelType"
            Me.labelType.Size = New System.Drawing.Size(120, 25)
            Me.labelType.TabIndex = 15
            Me.labelType.Text = "Type:"
            '
            'labelUsedSpaceValue
            '
            Me.labelUsedSpaceValue.Location = New System.Drawing.Point(144, 84)
            Me.labelUsedSpaceValue.Name = "labelUsedSpaceValue"
            Me.labelUsedSpaceValue.Size = New System.Drawing.Size(272, 25)
            Me.labelUsedSpaceValue.TabIndex = 2
            Me.labelUsedSpaceValue.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'labelUsedShort
            '
            Me.labelUsedShort.Location = New System.Drawing.Point(424, 84)
            Me.labelUsedShort.Name = "labelUsedShort"
            Me.labelUsedShort.Size = New System.Drawing.Size(96, 25)
            Me.labelUsedShort.TabIndex = 26
            Me.labelUsedShort.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'labelCapacityShort
            '
            Me.labelCapacityShort.Location = New System.Drawing.Point(424, 167)
            Me.labelCapacityShort.Name = "labelCapacityShort"
            Me.labelCapacityShort.Size = New System.Drawing.Size(96, 25)
            Me.labelCapacityShort.TabIndex = 27
            Me.labelCapacityShort.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'labelFreeShort
            '
            Me.labelFreeShort.Location = New System.Drawing.Point(424, 117)
            Me.labelFreeShort.Name = "labelFreeShort"
            Me.labelFreeShort.Size = New System.Drawing.Size(96, 25)
            Me.labelFreeShort.TabIndex = 28
            Me.labelFreeShort.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'GroupBox1
            '
            Me.GroupBox1.AllowDrop = True
            Me.GroupBox1.Location = New System.Drawing.Point(16, 67)
            Me.GroupBox1.Name = "GroupBox1"
            Me.GroupBox1.Size = New System.Drawing.Size(504, 8)
            Me.GroupBox1.TabIndex = 29
            Me.GroupBox1.TabStop = False
            '
            'GroupBox2
            '
            Me.GroupBox2.AllowDrop = True
            Me.GroupBox2.Location = New System.Drawing.Point(8, 360)
            Me.GroupBox2.Name = "GroupBox2"
            Me.GroupBox2.Size = New System.Drawing.Size(512, 8)
            Me.GroupBox2.TabIndex = 30
            Me.GroupBox2.TabStop = False
            '
            'textUsedSpace
            '
            Me.textUsedSpace.BackColor = System.Drawing.Color.Blue
            Me.textUsedSpace.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.textUsedSpace.ForeColor = System.Drawing.Color.Blue
            Me.textUsedSpace.Location = New System.Drawing.Point(8, 84)
            Me.textUsedSpace.Name = "textUsedSpace"
            Me.textUsedSpace.Size = New System.Drawing.Size(20, 20)
            Me.textUsedSpace.TabIndex = 31
            '
            'textFreeSpace
            '
            Me.textFreeSpace.BackColor = System.Drawing.Color.Magenta
            Me.textFreeSpace.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.textFreeSpace.ForeColor = System.Drawing.Color.Blue
            Me.textFreeSpace.Location = New System.Drawing.Point(8, 117)
            Me.textFreeSpace.Name = "textFreeSpace"
            Me.textFreeSpace.Size = New System.Drawing.Size(20, 20)
            Me.textFreeSpace.TabIndex = 32
            '
            'labelDrive
            '
            Me.labelDrive.Location = New System.Drawing.Point(242, 335)
            Me.labelDrive.Name = "labelDrive"
            Me.labelDrive.Size = New System.Drawing.Size(112, 25)
            Me.labelDrive.TabIndex = 33
            Me.labelDrive.Text = "Drive "
            '
            'DriveProperties
            '
            Me.AllowDrop = True
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.BackColor = System.Drawing.SystemColors.Control
            Me.CancelButton = Me.buttonOK
            Me.ClientSize = New System.Drawing.Size(530, 417)
            Me.Controls.Add(Me.labelDrive)
            Me.Controls.Add(Me.textFreeSpace)
            Me.Controls.Add(Me.textUsedSpace)
            Me.Controls.Add(Me.GroupBox2)
            Me.Controls.Add(Me.GroupBox1)
            Me.Controls.Add(Me.labelFreeShort)
            Me.Controls.Add(Me.labelCapacityShort)
            Me.Controls.Add(Me.labelUsedShort)
            Me.Controls.Add(Me.labelCapacityValue)
            Me.Controls.Add(Me.labelFreeSpaceValue)
            Me.Controls.Add(Me.labelUsedSpaceValue)
            Me.Controls.Add(Me.labelFileSystemValue)
            Me.Controls.Add(Me.labelTypeValue)
            Me.Controls.Add(Me.buttonOK)
            Me.Controls.Add(Me.line1)
            Me.Controls.Add(Me.labelCapacity)
            Me.Controls.Add(Me.labelFreeSpace)
            Me.Controls.Add(Me.labelUsedSpace)
            Me.Controls.Add(Me.labelFileSystem)
            Me.Controls.Add(Me.labelType)
            Me.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
            Me.Location = New System.Drawing.Point(13, 13)
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "DriveProperties"
            Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
            Me.ResumeLayout(False)
            Me.PerformLayout()

        End Sub

        'Default Instance - temporary plumbing (twhitney)
        Friend Shared ReadOnly Property GetInstance() As DriveProperties
            Get
                If m_DefaultInstance Is Nothing OrElse m_DefaultInstance.IsDisposed() Then
                    SyncLock GetType(DriveProperties)
                        If m_DefaultInstance Is Nothing OrElse m_DefaultInstance.IsDisposed() Then
                            m_DefaultInstance = New DriveProperties
                        End If
                    End SyncLock
                End If
                Return m_DefaultInstance
            End Get
        End Property

        Private Shared m_DefaultInstance As DriveProperties

#End Region

        Private Sub GetDriveData()

            ' have to write the line of instantiation code for d here:
            ' use curDir to access the current Directory...
            d = New DriveInfo(curDir)


            '''' what's the total freespace?
            labelFreeSpaceValue.Text = String.Format(CultureInfo.CurrentCulture, "{0:N0} bytes", d.TotalFreeSpace)
            '''' whats the total size?
            labelCapacityValue.Text = String.Format(CultureInfo.CurrentCulture, "{0:N0} bytes", d.TotalSize)
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            '
            labelUsedSpaceValue.Text = String.Format(CultureInfo.CurrentCulture, "{0:N0} bytes", d.TotalSize - d.TotalFreeSpace)
            labelFileSystemValue.Text = d.DriveType.ToString("f")
            labelTypeValue.Text = GetDriveType(d, False)
            Me.Text = String.Format(CultureInfo.CurrentCulture, "{0} ({1}) Properties", labelTypeValue.Text, d.Name)
            labelCapacityShort.Text = GetShortVal(d.TotalSize)
            labelFreeShort.Text = GetShortVal(d.TotalFreeSpace)
            labelUsedShort.Text = GetShortVal(d.TotalSize - d.TotalFreeSpace)
            labelDrive.Text = labelDrive.Text & d.Name.Substring(0, 1)
            sweepAngle = CSng(360 * CSng((d.TotalSize - d.TotalFreeSpace)) / CSng(d.TotalSize))
        End Sub

        Private Sub DriveProperties_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

            Try
                GetDriveData()
            Catch ex As IOException
                MsgBox("An exception occurred attempting to access the drive: it is inaccessible")
            End Try

        End Sub

        Private Function GetShortVal(ByVal value As Long) As String

            If value > 1000000000 Then
                Return String.Format(CultureInfo.CurrentCulture, "{0:N2} GB", value / 1000000000)
            Else
                Return String.Format(CultureInfo.CurrentCulture, "{0:N0} MB", value / 1000000)
            End If
        End Function

        Private Sub buttonOK_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonOK.Click
            Me.Close()
        End Sub

        Private Sub DriveProperties_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles MyBase.Paint
            ' Create pen.
            Dim blackPen As New Pen(Color.Black, 1)
            Dim blueBrush As New SolidBrush(Color.Blue)
            Dim magentaBrush As New SolidBrush(Color.Magenta)
            ' Create rectangle for ellipse.
            Dim rect As New Rectangle(CInt((Me.Width / 2) - 150), 210, 300, 100)
            ' Create start and sweep angles.
            Dim startAngle As Single = 180.0F

            ' Draw pie to screen
            With e.Graphics
                .DrawPie(blackPen, rect, startAngle, sweepAngle)
                .FillPie(blueBrush, rect, startAngle, sweepAngle)
                .DrawPie(blackPen, rect, 180 + sweepAngle, 360 - sweepAngle)
                .FillPie(magentaBrush, rect, 180 + sweepAngle, 360 - sweepAngle)
            End With


        End Sub
    End Class
End Namespace