Imports System.Drawing.Drawing2D

'*************************** Module Header ******************************\
' Module Name:  MainForm.vb
' Project:	    VBGDIPlusManipulateImage
' Copyright (c) Microsoft Corporation.
' 
' This is the main form of this application. It is used to initialize the UI and 
' handle the events.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************

Partial Public Class MainForm
    Inherits Form
    Private _pen As Pen = Nothing

    Private _imgManipulator As ImageManipulator = Nothing

    Private _adjustment As Point = Point.Empty

    Public Sub New()
        InitializeComponent()

        ' Load a bitmap from a local file.
        Dim img As New Bitmap("OneCodeIcon.png")

        ' Initialize the bmpEx.
        _imgManipulator = New ImageManipulator(img)

        ' Add all the InterpolationMode to the combobox.
        For i As Integer = 0 To 7
            cmbInterpolationMode.Items.Add(CType(i, InterpolationMode))
        Next i

        cmbInterpolationMode.SelectedIndex = 0
    End Sub

    ''' <summary>
    ''' Handle the click event of the buttons btnRotateLeft, btnRotateRight,
    ''' btnFlipVertical and btnFlipHorizontal.
    ''' </summary>
    Private Sub btnRotateFlip_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnFlipVertical.Click,
        btnRotateRight.Click,
        btnFlipHorizontal.Click,
        btnRotateLeft.Click

        Dim rotateFlipButton As Button = TryCast(sender, Button)

        If rotateFlipButton Is Nothing Then
            Return
        End If

        Dim rotateFlipType_Renamed As RotateFlipType = RotateFlipType.RotateNoneFlipNone

        Select Case rotateFlipButton.Name
            Case "btnRotateLeft"
                rotateFlipType_Renamed = RotateFlipType.Rotate270FlipNone
            Case "btnRotateRight"
                rotateFlipType_Renamed = RotateFlipType.Rotate90FlipNone
            Case "btnFlipVertical"
                rotateFlipType_Renamed = RotateFlipType.RotateNoneFlipY
            Case "btnFlipHorizontal"
                rotateFlipType_Renamed = RotateFlipType.RotateNoneFlipX
        End Select

        ' Rotate or flip the image.
        _imgManipulator.RotateFlip(rotateFlipType_Renamed)

        ' Redraw the pnlImage.
        _imgManipulator.DrawControl(Me.pnlImage, _adjustment, _pen)
    End Sub

    ''' <summary>
    ''' Handle the click event of the button btnRotateAngle.
    ''' </summary>
    Private Sub btnRotateAngle_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnRotateAngle.Click

        Dim angle As Single = 0

        ' Verify the input value.
        Single.TryParse(tbRotateAngle.Text, angle)

        If angle > 0 AndAlso angle < 360 Then
            ' Rotate or flip the image.
            _imgManipulator.RotateImg(angle)

            ' Redraw the pnlImage.
            _imgManipulator.DrawControl(Me.pnlImage, _adjustment, _pen)

        End If
    End Sub

    ''' <summary>
    ''' Handle the click event of the buttons btnMoveUp, btnMoveDown,
    ''' btnMoveLeft and btnMoveRight.
    ''' </summary>
    Private Sub btnMove_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnMoveLeft.Click,
        btnMoveUp.Click,
        btnMoveDown.Click,
        btnMoveRight.Click

        Dim moveButton As Button = TryCast(sender, Button)
        If moveButton Is Nothing Then
            Return
        End If

        Dim x As Integer = 0
        Dim y As Integer = 0

        Select Case moveButton.Name
            Case "btnMoveUp"
                y = -10
            Case "btnMoveDown"
                y = 10
            Case "btnMoveLeft"
                x = -10
            Case "btnMoveRight"
                x = 10
        End Select
        _adjustment = New Point(_adjustment.X + x, _adjustment.Y + y)

        ' Redraw the pnlImage.
        _imgManipulator.DrawControl(Me.pnlImage, _adjustment, _pen)

    End Sub

    ''' <summary>
    ''' Draw the image on the pnlImage when it is painted.
    ''' </summary>
    Private Sub pnlImage_Paint(ByVal sender As Object, ByVal e As PaintEventArgs) _
        Handles pnlImage.Paint

        ' Draw the pnlImage for the first time..
        _imgManipulator.DrawControl(Me.pnlImage, _adjustment, _pen)

    End Sub

    ''' <summary>
    ''' Handle the click event of the buttons btnAmplify and btnMicrify.
    ''' </summary>
    Private Sub btnAmplify_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnAmplify.Click, btnMicrify.Click

        Dim btnScale As Button = TryCast(sender, Button)
        If btnScale.Name = "btnAmplify" Then
            _imgManipulator.Scale(2, 2)
        Else
            _imgManipulator.Scale(0.5F, 0.5F)
        End If

        ' Redraw the pnlImage.
        _imgManipulator.DrawControl(Me.pnlImage, _adjustment, _pen)
    End Sub

    ''' <summary>
    ''' andle the click event of the buttons btnSkewLeft and btnSkewRight.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub btnSkew_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnSkewRight.Click, btnSkewLeft.Click
        Dim btnSkew As Button = TryCast(sender, Button)

        Select Case btnSkew.Name
            Case "btnSkewLeft"
                _imgManipulator.Skew(-10)
            Case "btnSkewRight"
                _imgManipulator.Skew(10)
        End Select

        ' Redraw the pnlImage.
        _imgManipulator.DrawControl(Me.pnlImage, _adjustment, _pen)
    End Sub


    ''' <summary>
    ''' Reset the bmpEx.
    ''' </summary>
    Private Sub btnReset_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnReset.Click

        ' Dispose the bmpEx.
        _imgManipulator.Dispose()

        ' Load a bitmap from a local file.
        Dim img As New Bitmap("OneCodeIcon.png")

        ' Initialize the bmpEx.
        _imgManipulator = New ImageManipulator(img)

        ' Redraw the pnlImage.
        _imgManipulator.DrawControl(Me.pnlImage, _adjustment, _pen)

    End Sub

    ''' <summary>
    ''' Handle the CheckedChanged event of the checkbox chkDrawBounds.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub chkDrawBounds_CheckedChanged(ByVal sender As Object, ByVal e As EventArgs) _
        Handles chkDrawBounds.CheckedChanged

        ' If the pen is not null, draw the bounds of the image.
        If chkDrawBounds.Checked Then
            _pen = Pens.Blue
        Else
            _pen = Nothing
        End If

        ' Redraw the pnlImage.
        _imgManipulator.DrawControl(Me.pnlImage, _adjustment, _pen)
    End Sub
End Class
