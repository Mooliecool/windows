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
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.WindowsCE.Forms
Imports System.Globalization

Public Class RotatedText
    Private Const CONTROL_HEIGHT As Integer = 22

    Private drawButton As Button
    Private spokesNumericUpDown As NumericUpDown
    Private whirlControl As FontwhirlControl


    Public Sub New()
        ' Suspend control layout for better performance and to suspend
        ' automatic scaling until we've set the AutoScaleDimensions and
        ' AutoScaleMode
        Me.SuspendLayout()

        ' note: controls use the Dock property to automatically
        ' reposition the controls when a device's screen orientation
        ' changes
        Me.whirlControl = New FontwhirlControl()
        Me.whirlControl.BackColor = Color.LawnGreen
        Me.whirlControl.Dock = DockStyle.Fill
        Me.whirlControl.Parent = Me
        Me.whirlControl.Text = "abc ABC 123"

        Dim controlsPanel As New Panel()
        controlsPanel.Height = CONTROL_HEIGHT
        controlsPanel.Dock = DockStyle.Bottom
        controlsPanel.Parent = Me

        Me.drawButton = New Button()
        Me.drawButton.Text = "Draw"
        Me.drawButton.Parent = controlsPanel

        Me.drawButton.Width = 60
        Me.drawButton.Dock = DockStyle.Right
        AddHandler Me.drawButton.Click, AddressOf Me.DrawButton_Click

        Me.spokesNumericUpDown = New NumericUpDown()
        Me.spokesNumericUpDown.Parent = controlsPanel
        Me.spokesNumericUpDown.Width = 50
        Me.spokesNumericUpDown.Maximum = 50
        Me.spokesNumericUpDown.Minimum = 1
        Me.spokesNumericUpDown.Value = 10
        Me.spokesNumericUpDown.Dock = DockStyle.Left

        Dim label As New Label()
        label.Parent = controlsPanel
        label.Width = 115
        label.Text = "Number of Spokes:"
        label.Dock = DockStyle.Left

        ' put (ok) button on form to close it when done with this sample
        Me.MinimizeBox = False
        
        ' dpi settings for the screen the form was initially designed on
        Me.AutoScaleDimensions = New SizeF(96.0F, 96.0F)

        ' automatically scale controls based on the DPI of the screen
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi

        ' show current dpi of screen
        Dim currentHeight() As Object
        ReDim currentHeight(1)
        currentHeight(1) = Me.CurrentAutoScaleDimensions.Height
        Me.Text = String.Format(CultureInfo.InvariantCulture, _
               "RotatedText({0}dpi)", currentHeight(1))


        ' Now that we've set the AutoScaleMode, call ResumeLayout() to
        ' do the actual scaling
        Me.ResumeLayout()

        ' draw default image
        DrawButton_Click(Nothing, EventArgs.Empty)

    End Sub 'New


    '/ <summary>
    '/     When someone clicks the Draw button
    '/        a) set the number of spokes from the numericUpDown control
    '/        b) instruct whirl control to render the rotated text
    '/ </summary>
    Sub DrawButton_Click(ByVal o As Object, ByVal e As EventArgs)
        Me.whirlControl.Spokes = Fix(Me.spokesNumericUpDown.Value)
        Me.whirlControl.DrawText()

    End Sub 'DrawButton_Click


    Shared Sub Main()
        Application.Run(New RotatedText())

    End Sub 'Main
End Class 'RotatedText


'/ <summary>
'/     Control to draw rotated text.  By rendering the rotated text in a
'/     separate control we can take advantage of docking and anchoring
'/     to resize the control and make the application respond to screen
'/     orientation changes.
'/ </summary>

Public Class FontwhirlControl
    Inherits Control
    Private Const POINTSPERINCH As Integer = 96 ' initial dpi when designed
    Private spokesValue As Integer = 10
    Private offScreenBitmap As Bitmap = Nothing


    Public Sub New()

    End Sub 'New

    '/ <summary>
    '/   property specifying number of text spokes to draw
    '/ </summary>

    Public Property Spokes() As Integer
        Get
            Return Me.spokesValue
        End Get
        Set(ByVal value As Integer)
            Me.spokesValue = value
        End Set
    End Property


    '/ <summary>
    '/     when the Resize() event fires, make a new off screen bitmap
    '/     to draw into.
    '/ </summary>
    Sub ResizeDrawingBuffer()
        Dim newBitmap As New Bitmap(Me.ClientSize.Width, Me.ClientSize.Height)
        Dim g As Graphics = Graphics.FromImage(newBitmap)
        Try
            g.Clear(Me.BackColor)
        Finally
            g.Dispose()
        End Try
        ' copy the old offScreenBitmap into the new buffer
        If Not (Me.offScreenBitmap Is Nothing) Then
            Dim newg As Graphics = Graphics.FromImage(newBitmap)
            Try
                newg.DrawImage(Me.offScreenBitmap, 0, 0)
            Finally
                newg.Dispose()
            End Try
            ' Graphics objects are not garbage collected and need to be
            ' explicitly disposed
            Me.offScreenBitmap.Dispose()
        End If

        Me.offScreenBitmap = newBitmap

    End Sub 'ResizeDrawingBuffer


    '/ <summary>
    '/     Create the rotated font using a LOGFONT structure.  Important
    '/     notes:
    '/         a) rotation angle is in 1/10's of a degree
    '/         b) orientation and escapement values are the same on
    '/            mobile devices.
    '/ </summary>
    Function CreateRotatedFont(ByVal angle As Integer, ByVal g As Graphics) _
       As Font
        Dim lf As New LogFont()

        ' scale a 12 point font for current screen DPI
        lf.Height = Fix(-16.0F * g.DpiY / POINTSPERINCH)
        lf.Width = 0

        ' rotation angle in tenths of degrees
        lf.Escapement = angle * 10

        ' Orientation == Escapement for mobile device OS
        lf.Orientation = lf.Escapement
        lf.Weight = 0
        lf.Italic = 0
        lf.Underline = 0
        lf.StrikeOut = 0
        lf.CharSet = LogFontCharSet.Default
        lf.OutPrecision = LogFontPrecision.Default
        lf.ClipPrecision = LogFontClipPrecision.Default
        lf.Quality = LogFontQuality.ClearType
        lf.PitchAndFamily = LogFontPitchAndFamily.Default
        lf.FaceName = "Tahoma"

        Return System.Drawing.Font.FromLogFont(lf)

    End Function 'CreateRotatedFont



    '/ <summary>
    '/     Draw rotated text spokes into an off screen bitmap which will
    '/     be drawn to the screen via the OnPaint() event.
    '/ </summary>
    Public Sub DrawText()
        Dim g As Graphics = Graphics.FromImage(Me.offScreenBitmap)
        Try
            g.Clear(Me.BackColor)

            Dim fontAngle As Single
            Dim rotatedFont As Font
            Dim spoke As Integer
            For spoke = 0 To (Me.spokesValue)
                ' calculate the rotation angle and create a rotated font
                fontAngle = System.Convert.ToSingle(spoke) * _
                        (360.0F / System.Convert.ToSingle(Me.spokesValue))
                rotatedFont = CreateRotatedFont(Fix(fontAngle), g)

                ' Draw the rotated text starting from the center of the
                ' off screen bitmap (essentially, the client area of the
                ' custom control).
                g.DrawString(Me.Text, rotatedFont, _
                   New SolidBrush(Me.ForeColor), Me.offScreenBitmap.Width / 2, _
                   Me.offScreenBitmap.Height / 2, _
                   New StringFormat(StringFormatFlags.NoWrap Or _
                   StringFormatFlags.NoClip))

                ' Graphics objects are not garbage collected and need to be
                ' explicitly disposed
                rotatedFont.Dispose()
            Next spoke
        Finally
            g.Dispose()
        End Try

        ' cause the control to re-paint itself
        Me.Invalidate()

    End Sub 'DrawText


    ' override this to do nothing.  Allowing the background to paint would
    ' cause the control to flicker when it repaints.  And, it's not
    ' necessary since we draw the whole off screen bitmap during a paint
    ' event.
    Protected Overrides Sub OnPaintBackground(ByVal e As PaintEventArgs)

    End Sub 'OnPaintBackground


    '/ <summary>
    '/     Draw the off screen bitmap that contains the rotated text spokes
    '/ </summary>
    Protected Overrides Sub OnPaint(ByVal e As PaintEventArgs)
        If Me.offScreenBitmap Is Nothing Then
            Return
        End If
        e.Graphics.DrawImage(Me.offScreenBitmap, 0, 0)

    End Sub 'OnPaint


    '/ <summary>
    '/     Resize event fires when screen orientation changes.  When this
    '/     happens, resize the off screen bitmap and redraw the rotated
    '/     text spokes.
    '/ </summary>
    Protected Overrides Sub OnResize(ByVal e As EventArgs)
        If Me.ClientSize.Width <= 0 OrElse Me.ClientSize.Height <= 0 Then
            Return
        End If
        ResizeDrawingBuffer()
        DrawText()

    End Sub 'OnResize
End Class 'FontwhirlControl
