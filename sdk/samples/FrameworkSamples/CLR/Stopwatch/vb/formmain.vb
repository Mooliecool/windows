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
Imports System.Threading
Imports System.Globalization

' This is a simple demo that demonstrates the new StopWatch API
' You'll notice in this demo that most of the code
' is simply to draw the form! The StopWatch code itself
' is a cinch
Namespace Microsoft.Samples.StopWatchDemo
    Public Class FormMain
        Inherits System.Windows.Forms.Form

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
        Private WithEvents labelExit As System.Windows.Forms.Label
        Private WithEvents labelTime As System.Windows.Forms.Label
        Private WithEvents timerMain As System.Windows.Forms.Timer
        Private WithEvents buttonLap As System.Windows.Forms.Button
        Private WithEvents buttonStart As System.Windows.Forms.Button
        Private WithEvents labelLapPrompt As System.Windows.Forms.Label
        Private WithEvents LabelTimePrompt As System.Windows.Forms.Label
        Private WithEvents labelLap As System.Windows.Forms.Label
        Private WithEvents labelBottomLeft As System.Windows.Forms.Label
        Private WithEvents labelBottomRight As System.Windows.Forms.Label
        Private WithEvents labelTopLeft As System.Windows.Forms.Label
        Private WithEvents labelTopRight As System.Windows.Forms.Label
        <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
            Me.labelExit = New System.Windows.Forms.Label
            Me.labelTime = New System.Windows.Forms.Label
            Me.buttonLap = New System.Windows.Forms.Button
            Me.components = New System.ComponentModel.Container
            Me.timerMain = New System.Windows.Forms.Timer(Me.components)
            Me.labelLap = New System.Windows.Forms.Label
            Me.buttonStart = New System.Windows.Forms.Button
            Me.labelLapPrompt = New System.Windows.Forms.Label
            Me.LabelTimePrompt = New System.Windows.Forms.Label
            Me.labelBottomLeft = New System.Windows.Forms.Label
            Me.labelBottomRight = New System.Windows.Forms.Label
            Me.labelTopLeft = New System.Windows.Forms.Label
            Me.labelTopRight = New System.Windows.Forms.Label
            Me.SuspendLayout()
            '
            'labelExit
            '
            Me.labelExit.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
            Me.labelExit.Location = New System.Drawing.Point(88, 4)
            Me.labelExit.Margin = New System.Windows.Forms.Padding(3, 3, 3, 1)
            Me.labelExit.Name = "labelExit"
            Me.labelExit.Size = New System.Drawing.Size(20, 21)
            Me.labelExit.TabIndex = 0
            Me.labelExit.Text = "x"
            Me.labelExit.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            '
            'labelTime
            '
            Me.labelTime.Font = New System.Drawing.Font("Microsoft Sans Serif", 20.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.labelTime.ForeColor = System.Drawing.Color.Yellow
            Me.labelTime.Location = New System.Drawing.Point(8, 52)
            Me.labelTime.Margin = New System.Windows.Forms.Padding(3, 3, 3, 1)
            Me.labelTime.Name = "labelTime"
            Me.labelTime.Size = New System.Drawing.Size(188, 33)
            Me.labelTime.TabIndex = 1
            Me.labelTime.Text = "00:00:00.00"
            '
            'buttonLap
            '
            Me.buttonLap.Font = New System.Drawing.Font("Comic Sans MS", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.buttonLap.ForeColor = System.Drawing.Color.Yellow
            Me.buttonLap.Location = New System.Drawing.Point(102, 154)
            Me.buttonLap.Margin = New System.Windows.Forms.Padding(2, 3, 1, 3)
            Me.buttonLap.Name = "buttonLap"
            Me.buttonLap.Size = New System.Drawing.Size(72, 34)
            Me.buttonLap.TabIndex = 4
            Me.buttonLap.Text = "Lap"
            '
            'timerMain
            '
            Me.timerMain.Enabled = True
            Me.timerMain.Interval = 50
            '
            'labelLap
            '
            Me.labelLap.Font = New System.Drawing.Font("Microsoft Sans Serif", 20.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.labelLap.ForeColor = System.Drawing.Color.Yellow
            Me.labelLap.Location = New System.Drawing.Point(8, 112)
            Me.labelLap.Name = "labelLap"
            Me.labelLap.Size = New System.Drawing.Size(191, 34)
            Me.labelLap.TabIndex = 5
            Me.labelLap.Visible = False
            '
            'buttonStart
            '
            Me.buttonStart.FlatStyle = System.Windows.Forms.FlatStyle.Flat
            Me.buttonStart.Font = New System.Drawing.Font("Comic Sans MS", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.buttonStart.ForeColor = System.Drawing.Color.Yellow
            Me.buttonStart.Location = New System.Drawing.Point(26, 154)
            Me.buttonStart.Margin = New System.Windows.Forms.Padding(1, 3, 2, 3)
            Me.buttonStart.Name = "buttonStart"
            Me.buttonStart.Size = New System.Drawing.Size(72, 34)
            Me.buttonStart.TabIndex = 2
            Me.buttonStart.Text = "Start"
            '
            'labelLapPrompt
            '
            Me.labelLapPrompt.Font = New System.Drawing.Font("Comic Sans MS", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.labelLapPrompt.ForeColor = System.Drawing.Color.Yellow
            Me.labelLapPrompt.Location = New System.Drawing.Point(24, 87)
            Me.labelLapPrompt.Margin = New System.Windows.Forms.Padding(3, 1, 3, 3)
            Me.labelLapPrompt.Name = "labelLapPrompt"
            Me.labelLapPrompt.Size = New System.Drawing.Size(56, 25)
            Me.labelLapPrompt.TabIndex = 6
            Me.labelLapPrompt.Text = "Lap"
            Me.labelLapPrompt.Visible = False
            '
            'LabelTimePrompt
            '
            Me.LabelTimePrompt.Font = New System.Drawing.Font("Comic Sans MS", 15.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.LabelTimePrompt.ForeColor = System.Drawing.Color.Yellow
            Me.LabelTimePrompt.Location = New System.Drawing.Point(16, 26)
            Me.LabelTimePrompt.Margin = New System.Windows.Forms.Padding(3, 0, 3, 3)
            Me.LabelTimePrompt.Name = "LabelTimePrompt"
            Me.LabelTimePrompt.Size = New System.Drawing.Size(88, 26)
            Me.LabelTimePrompt.TabIndex = 7
            Me.LabelTimePrompt.Text = "Time"
            '
            'labelBottomLeft
            '
            Me.labelBottomLeft.BackColor = System.Drawing.Color.Black
            Me.labelBottomLeft.ForeColor = System.Drawing.Color.Black
            Me.labelBottomLeft.Location = New System.Drawing.Point(8, 177)
            Me.labelBottomLeft.Margin = New System.Windows.Forms.Padding(3, 3, 1, 3)
            Me.labelBottomLeft.Name = "labelBottomLeft"
            Me.labelBottomLeft.Size = New System.Drawing.Size(16, 17)
            Me.labelBottomLeft.TabIndex = 8
            '
            'labelBottomRight
            '
            Me.labelBottomRight.BackColor = System.Drawing.Color.Black
            Me.labelBottomRight.ForeColor = System.Drawing.Color.Black
            Me.labelBottomRight.Location = New System.Drawing.Point(176, 177)
            Me.labelBottomRight.Margin = New System.Windows.Forms.Padding(1, 3, 3, 3)
            Me.labelBottomRight.Name = "labelBottomRight"
            Me.labelBottomRight.Size = New System.Drawing.Size(16, 17)
            Me.labelBottomRight.TabIndex = 9
            '
            'labelTopLeft
            '
            Me.labelTopLeft.BackColor = System.Drawing.Color.Black
            Me.labelTopLeft.ForeColor = System.Drawing.Color.Black
            Me.labelTopLeft.Location = New System.Drawing.Point(8, 8)
            Me.labelTopLeft.Margin = New System.Windows.Forms.Padding(3, 3, 3, 0)
            Me.labelTopLeft.Name = "labelTopLeft"
            Me.labelTopLeft.Size = New System.Drawing.Size(16, 17)
            Me.labelTopLeft.TabIndex = 10
            '
            'labelTopRight
            '
            Me.labelTopRight.BackColor = System.Drawing.Color.Black
            Me.labelTopRight.ForeColor = System.Drawing.Color.Black
            Me.labelTopRight.Location = New System.Drawing.Point(176, 8)
            Me.labelTopRight.Name = "labelTopRight"
            Me.labelTopRight.Size = New System.Drawing.Size(16, 17)
            Me.labelTopRight.TabIndex = 11
            '
            'FormMain
            '
            Me.AutoScaleDimensions = New System.Drawing.Size(9, 23)
            Me.BackColor = System.Drawing.Color.Navy
            Me.ClientSize = New System.Drawing.Size(200, 200)
            Me.Controls.Add(Me.labelTopRight)
            Me.Controls.Add(Me.labelTopLeft)
            Me.Controls.Add(Me.labelBottomRight)
            Me.Controls.Add(Me.labelBottomLeft)
            Me.Controls.Add(Me.LabelTimePrompt)
            Me.Controls.Add(Me.labelLapPrompt)
            Me.Controls.Add(Me.labelLap)
            Me.Controls.Add(Me.buttonLap)
            Me.Controls.Add(Me.buttonStart)
            Me.Controls.Add(Me.labelTime)
            Me.Controls.Add(Me.labelExit)
            Me.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None
            Me.Name = "FormMain"
            Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
            Me.Text = "StopWatch Demo"
            Me.TopMost = True
            Me.ResumeLayout(False)

        End Sub

#End Region

        Dim sw As New Stopwatch
        Dim paused As Boolean = False

        Private Sub formMain_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
            DrawForm()
        End Sub

        Private Sub DrawForm()
            'Shape the viewer form to look like a windshield
            Dim GraphicsPath As New Drawing2D.GraphicsPath

            GraphicsPath.AddArc(0, 0, 100, 100, 180, 90)
            GraphicsPath.AddArc(100, 0, 100, 100, 270, 90)
            GraphicsPath.AddArc(100, 100, 100, 100, 0, 90)
            GraphicsPath.AddArc(0, 100, 100, 100, 90, 90)

            Me.Region = New Region(GraphicsPath)
        End Sub

        Private Sub labelExit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles labelExit.Click
            Me.Close()
        End Sub

        ' StopWatch code, easy as pie
        Private Sub buttonStart_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonStart.Click
            If sw.IsRunning Then
                sw.Stop()
                buttonStart.Text = "Start"
                buttonLap.Text = "Reset"
            Else
                sw.Start()
                buttonStart.Text = "Stop"
                buttonLap.Text = "Lap"
                labelLap.Visible = False
                labelLapPrompt.Visible = False
            End If

        End Sub

        ' StopWatch code, easy as pie
        Private Sub buttonLap_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonLap.Click
            If buttonLap.Text = "Lap" Then
                If sw.IsRunning Then
                    paused = True
                    labelLap.Visible = True
                    labelLapPrompt.Visible = True
                End If
            Else
                labelLap.Visible = False
                labelLapPrompt.Visible = False
                labelTime.Text = "00:00:00.00"
                buttonLap.Text = "Lap"
                sw.Reset()
            End If
        End Sub

        Private Sub timerMain_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timerMain.Tick
            If sw.IsRunning Then
                Dim ts As TimeSpan = TimeSpan.FromMilliseconds(sw.ElapsedMilliseconds)
                labelTime.Text = String.Format(CultureInfo.CurrentCulture, "{0:00}:{1:00}:{2:00}.{3:00}", ts.Hours, ts.Minutes, ts.Seconds, ts.Milliseconds / 10)

                If paused Then
                    labelLap.Text = labelTime.Text
                    labelLapPrompt.Text = "Lap"
                    paused = False
                End If
            End If
        End Sub

        Private Sub labelTopLeft_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles labelTopLeft.Click
            Me.Location = New Point(0, 0)
        End Sub

        Private Sub labelBottomLeft_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles labelBottomLeft.Click
            Me.Location = New Point(0, Screen.PrimaryScreen.WorkingArea.Height - Me.Height)
        End Sub

        Private Sub labelTopRight_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles labelTopRight.Click
            Me.Location = New Point(Screen.PrimaryScreen.WorkingArea.Width - Me.Width, 0)
        End Sub

        Private Sub labelBottomRight_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles labelBottomRight.Click
            Me.Location = New Point(Screen.PrimaryScreen.WorkingArea.Width - Me.Width, Screen.PrimaryScreen.WorkingArea.Height - Me.Height)
        End Sub

        Private Sub formMain_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Click
            Me.Location = New Point(CInt((Screen.PrimaryScreen.WorkingArea.Width - Me.Width) / 2), CInt((Screen.PrimaryScreen.WorkingArea.Height - Me.Height) / 2))
        End Sub

        Private Sub LabelTimePrompt_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles LabelTimePrompt.Click
            Me.Location = New Point(CInt((Screen.PrimaryScreen.WorkingArea.Width - Me.Width) / 2), CInt((Screen.PrimaryScreen.WorkingArea.Height - Me.Height) / 2))
        End Sub

        Private Sub labelLap_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles labelLap.Click
            Me.Location = New Point(CInt((Screen.PrimaryScreen.WorkingArea.Width - Me.Width) / 2), CInt((Screen.PrimaryScreen.WorkingArea.Height - Me.Height) / 2))
        End Sub

        Private Sub labelLapPrompt_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles labelLapPrompt.Click
            Me.Location = New Point(CInt((Screen.PrimaryScreen.WorkingArea.Width - Me.Width) / 2), CInt((Screen.PrimaryScreen.WorkingArea.Height - Me.Height) / 2))
        End Sub

        Private Sub labelTime_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles labelTime.Click
            Me.Location = New Point(CInt((Screen.PrimaryScreen.WorkingArea.Width - Me.Width) / 2), CInt((Screen.PrimaryScreen.WorkingArea.Height - Me.Height) / 2))
        End Sub
    End Class
End Namespace