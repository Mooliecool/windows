' =====================================================================
'  File:      BubbleForm.vb
'
'
'  ---------------------------------------------------------------------
'   Copyright (C) Microsoft Corporation.  All rights reserved.
'
'  This source code is intended only as a supplement to Microsoft
'  Development Tools and/or on-line documentation.  See these other
'  materials for detailed information regarding Microsoft code samples.
'
'  THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
'  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'  PARTICULAR PURPOSE.
' =====================================================================

Imports System.Threading

Namespace Microsoft.Samples.CompactFramework

Public Class BubbleForm
    Inherits System.Windows.Forms.Form

    Private Const MaxBubbles As Integer = 20

    Private bubbles(MaxBubbles) As Bubble
    Private backgroundBrush As Brush = New SolidBrush(Color.SlateBlue)
    Private backgroundPen As New Pen(Color.SlateBlue)
    Private offscreenBitmap As Bitmap
    Private offscreenGraphics As Graphics
    Private thread As Thread
    Private isRunning As Boolean
    Private realGraphics As Graphics
    Private cachedClientSize As Size

#Region " Windows Form Designer generated code "

    Public Sub New()

        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Additional initialization
        Dim bubble As Integer
        For bubble = 0 To MaxBubbles - 1
            Me.bubbles(bubble) = New Bubble()
            Me.bubbles(bubble).Init(ClientSize)
        Next bubble

        Me.offscreenBitmap = New Bitmap(ClientSize.Width, ClientSize.Height)
        Me.offscreenGraphics = Graphics.FromImage(Me.offscreenBitmap)
        Me.offscreenGraphics.FillRectangle(Me.backgroundBrush, 0, 0, ClientSize.Width, ClientSize.Height)

        Me.realGraphics = Me.CreateGraphics()

        Me.isRunning = True

        Me.thread = New Thread(AddressOf Me.RunMe)
        Me.thread.Start()

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)

        MyBase.Dispose(disposing)

    End Sub

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Private Sub InitializeComponent()
        '
        'BubbleForm
        '
        Me.BackColor = System.Drawing.Color.SlateBlue
        Me.ClientSize = New System.Drawing.Size(240, 295)
        Me.Text = "Bubbles"

    End Sub

    Public Shared Sub Main()

        Application.Run(New BubbleForm())

    End Sub

#End Region

    Protected Overloads Overrides Sub OnResize(ByVal e As EventArgs)
        MyBase.OnResize(e)
        Me.cachedClientSize = Me.ClientSize
    End Sub

    Private Sub DoTick()

        EraseAll(Me.offscreenGraphics)

        Dim bubble As Integer
        For bubble = 0 To MaxBubbles - 1
            Me.bubbles(bubble).DoTick(cachedClientSize)
        Next bubble

        RedrawAll(Me.offscreenGraphics)
        RefreshAll(Me.realGraphics)

    End Sub
		
    Private Sub RefreshAll(ByVal graphicsPhys As Graphics)

        Dim rc As Rectangle

        Dim bubble As Integer
        For bubble = 0 To MaxBubbles - 1
            rc = Me.bubbles(bubble).WholeBounds
            graphicsPhys.DrawImage(Me.offscreenBitmap, rc.X, rc.Y, rc, GraphicsUnit.Pixel)
        Next bubble

    End Sub

    Private Sub RedrawAll(ByVal graphics As Graphics)

        Dim bubble As Integer
        For bubble = 0 To MaxBubbles - 1
            Me.bubbles(bubble).Draw(graphics)
        Next bubble

    End Sub

    Private Sub EraseAll(ByVal graphics As Graphics)

        Dim bubble As Integer
        For bubble = 0 To MaxBubbles - 1
            Me.bubbles(bubble).EraseSub(graphics, Me.backgroundBrush)
        Next bubble

    End Sub

    Private Sub RunMe()

        While Me.isRunning
            Me.DoTick()
        End While

    End Sub

    Private Sub BubbleForm_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles MyBase.MouseDown

        Me.isRunning = False
        Me.Close()

    End Sub

    Private Sub BubbleForm_Closed(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Closed

        Me.isRunning = False

    End Sub

End Class

End Namespace

