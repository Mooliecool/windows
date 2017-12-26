' =====================================================================
'  File:      Bubble.vb
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

Namespace Microsoft.Samples.CompactFramework

Public Class Bubble

    Private x, y As Integer
    Private xOld, yOld As Integer
    Private velocityH, velocityV As Integer
    Private width, height As Integer
    Private Shared Rand As New Random()

    Public Sub Init(ByVal windowSize As Size)

        Dim rnd As Random = Bubble.Rand

        Me.x = rnd.Next() Mod windowSize.Width
        Me.y = rnd.Next() Mod windowSize.Height

        Me.ResetDx()
        Me.ResetDy()

        Dim bubbleSize As Integer = rnd.Next(30) + 10

        Me.width = bubbleSize
        Me.height = bubbleSize

    End Sub

    Private Sub ResetDx()

        Me.velocityH = Bubble.Rand.Next(10) - 5
        If Me.velocityH = 0 Then
            Me.velocityH = 3
        End If

    End Sub

    Private Sub ResetDy()

        Me.velocityV = Bubble.Rand.Next(10) - 5
        If Me.velocityV = 0 Then
            Me.velocityV = 1
        End If

    End Sub

    Public Sub DoTick(ByVal windowSize As Size)

        Me.xOld = Me.x
        Me.yOld = Me.y

        Me.x = Me.x + Me.velocityH
        Me.y = Me.y + Me.velocityV

        If Me.x < 0 Then
            Me.x = 0
            Me.ResetDx()
        End If

        If Me.x >= windowSize.Width - Me.width Then
            Me.x = windowSize.Width - Me.width - 1
            Me.ResetDx()
        End If

        If Me.y < 0 Then
            Me.y = 0
            Me.ResetDy()
        End If

        If Me.y >= windowSize.Height - Me.height Then
            Me.y = windowSize.Height - Me.height - 1
            Me.ResetDy()
        End If

    End Sub

    Private Sub FillInnerReflection(ByVal graphics As Graphics, ByRef x As Integer, ByRef y As Integer, ByRef width As Integer, ByRef height As Integer)

        x = x + CInt(width / 8)
        y = y + CInt(height / 8)

        width = CInt(width / 2)
        height = CInt(height / 2)

        graphics.FillEllipse(New SolidBrush(Color.White), x, y, width, height)

    End Sub

    Public Sub Draw(ByVal graphics As Graphics)

        Dim x, y, width, height As Integer

        graphics.DrawEllipse(New Pen(Color.White), Me.x, Me.y, Me.width - 1, Me.height - 1)

        x = Me.x
        y = Me.y
        width = Me.width
        height = Me.height

        FillInnerReflection(graphics, x, y, width, height)

    End Sub

    Public Sub EraseSub(ByVal graphics As Graphics, ByVal brush As Brush)

        graphics.FillRectangle(brush, Me.x, Me.y, Me.width, Me.height)

    End Sub

    Public ReadOnly Property WholeBounds() As Rectangle

        Get
            Return (Rectangle.Union(New Rectangle(Me.x, Me.y, Me.width + 1, Me.height + 1), New Rectangle(Me.xOld, Me.yOld, Me.width + 1, Me.height + 1)))
        End Get

    End Property

End Class

End Namespace

