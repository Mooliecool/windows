'*************************** Module Header ******************************'
' Module Name:  ImageManipulator.vb
' Project:	    VBGDIPlusManipulateImage
' Copyright (c) Microsoft Corporation.
' 
' This class ImageManipulator is used to manipulate an image using GDI+. It supplies 
' methods to resize, scale, rotate and flip the image. The rotate method can 
' support any degree. This class also supplies a method to draw the image on
' an object that inherits System.Windows.Forms.Control.
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Drawing.Drawing2D

Public Class ImageManipulator
    Implements IDisposable

    ' Specify whether the object is disposed.
    Private _disposed As Boolean = False

    ''' <summary>
    ''' The image to handle.
    ''' </summary>
    Private _image As Bitmap
    Public Property Image() As Bitmap
        Get
            Return _image
        End Get
        Private Set(ByVal value As Bitmap)
            _image = value
        End Set
    End Property

    ''' <summary>
    ''' The real size of the image. If only rotate or flip the image, the real
    ''' size will not change. 
    ''' This property is used to calculate the necessary size while rotate the
    ''' image.
    ''' </summary>
    Private _contentSize As Size
    Public Property ContentSize() As Size
        Get
            Return _contentSize
        End Get
        Private Set(ByVal value As Size)
            _contentSize = value
        End Set
    End Property

    ''' <summary>
    ''' The rotated angle. Its value is between 0 and 360.
    ''' </summary>
    Private _rotatedAngle As Single
    Public Property RotatedAngle() As Single
        Get
            Return _rotatedAngle
        End Get
        Private Set(ByVal value As Single)
            _rotatedAngle = value
        End Set
    End Property

    ''' <summary>
    ''' The skewed unit. Its value may be less than 0.
    ''' </summary>
    Private _skewedUnit As Integer
    Public Property SkewedUnit() As Integer
        Get
            Return _skewedUnit
        End Get
        Private Set(ByVal value As Integer)
            _skewedUnit = value
        End Set
    End Property

    Public Sub New(ByVal img As Bitmap)
        If img Is Nothing Then
            Throw New ArgumentNullException("Image could not be null")
        End If

        ' Initialize the properties.
        Me.Image = img
        Me.ContentSize = img.Size
        RotatedAngle = 0
    End Sub

    ''' <summary>
    ''' Resize the image with a specified size. 
    ''' This method will create a new Bitmap from the original image with the
    ''' specified size, then set this.Image to the new bitmap.
    ''' </summary>
    ''' <param name="size_Renamed"></param>
    Public Sub ResizeImage(ByVal size_Renamed As Size)
        Dim newImg As Bitmap = Nothing
        Try

            ' Create a new bitmap from the original image with the specified size.
            newImg = New Bitmap(Image, size_Renamed)

            ' Dispose the original image.
            Me.Image.Dispose()

            ' Set this.Image to the new bitmap.
            Me.Image = newImg

            Me.ContentSize = Me.Image.Size
        Catch

            ' If there is any exception, dispose the new bitmap.
            If newImg IsNot Nothing Then
                newImg.Dispose()
            End If

            Throw
        End Try
    End Sub

    ''' <summary>
    ''' Scale the image with the specified factors.
    ''' The steps are
    ''' 1 Calculate the new size.
    ''' 2 Initializes a new instance of the Matrix class with the specified 
    '''   elements. This Matrix is used in Transform. 
    ''' 3 Draw the original image to the new bitmap, then the Transform will
    '''   take effect.
    ''' </summary>
    Public Sub Scale(ByVal xFactor As Single, ByVal yFactor As Single)
        Dim newImg As Bitmap = Nothing
        Dim g As Graphics = Nothing
        Dim mtrx As Matrix = Nothing

        Try
            ' Create a new bitmap with the specified size.
            newImg = New Bitmap(Convert.ToInt32(Me.Image.Size.Width * xFactor),
                                Convert.ToInt32(Me.Image.Size.Height * yFactor))

            ' Create a new Graphics from the bitmap.
            g = Graphics.FromImage(newImg)

            ' Set the Interpolation Mode. 
            g.InterpolationMode = InterpolationMode.HighQualityBilinear

            ' Initializes a new instance of the Matrix class with the specified 
            ' elements. This Matrix is used in Transform. 
            mtrx = New Matrix(xFactor, 0, 0, yFactor, 0, 0)

            ' Multiplies the world transformation of this Graphics and specified the Matrix.
            g.MultiplyTransform(mtrx, MatrixOrder.Append)

            ' Draw the original image to the new bitmap.
            g.DrawImage(Me.Image, 0, 0)

            ' Dispose the original image.
            Me.Image.Dispose()

            ' Set this.Image to the new bitmap.
            Me.Image = newImg

            Me.ContentSize = Me.Image.Size
        Catch

            ' If there is any exception, dispose the new bitmap.
            If newImg IsNot Nothing Then
                newImg.Dispose()
            End If

            Throw
        Finally

            ' Dispose the Graphics and Matrix.
            If g IsNot Nothing Then
                g.Dispose()
            End If

            If mtrx IsNot Nothing Then
                mtrx.Dispose()
            End If
        End Try
    End Sub

    ''' <summary>
    ''' Rotates, flips, or rotates and flips the image.
    ''' </summary>
    ''' <param name="type">
    ''' A System.Drawing.RotateFlipType member that specifies the type of rotation
    '  and flip to apply to the image.
    ''' </param>
    Public Sub RotateFlip(ByVal type As RotateFlipType)
        ' Rotates, flips, or rotates and flips the image.
        Me.Image.RotateFlip(type)

        ' Calculate the rotated angle. 
        Select Case type
            ' Rotate180FlipXY just means RotateNoneFlipNone;
            Case RotateFlipType.RotateNoneFlipNone

                ' Rotate270FlipXY just means Rotate90FlipNone;
            Case RotateFlipType.Rotate90FlipNone
                Me.RotatedAngle += 90

                ' RotateNoneFlipXY just means Rotate180FlipNone;
            Case RotateFlipType.Rotate180FlipNone
                Me.RotatedAngle += 180

                ' Rotate90FlipXY just means Rotate270FlipNone;
            Case RotateFlipType.Rotate270FlipNone
                Me.RotatedAngle += 270

                ' Rotate180FlipY just means RotateNoneFlipX;
            Case RotateFlipType.RotateNoneFlipX
                Me.RotatedAngle = 180 - Me.RotatedAngle

                ' Rotate270FlipY just means Rotate90FlipX;
            Case RotateFlipType.Rotate90FlipX
                Me.RotatedAngle = 90 - Me.RotatedAngle

                ' Rotate180FlipX just means RotateNoneFlipY;
            Case RotateFlipType.RotateNoneFlipY
                Me.RotatedAngle = 360 - Me.RotatedAngle

                ' Rotate270FlipX just means Rotate90FlipY;
            Case RotateFlipType.Rotate90FlipY
                Me.RotatedAngle = 270 - Me.RotatedAngle

            Case Else
        End Select

        ' The value of RotatedAngle is between 0 and 360.
        If RotatedAngle >= 360 Then
            RotatedAngle -= 360
        End If
        If RotatedAngle < 0 Then
            RotatedAngle += 360
        End If
    End Sub

    ''' <summary>
    ''' Rotate the image to any degree.
    ''' The steps are
    ''' 1 Calculate the necessary size based on the ContentSize and RotatedAngle.
    ''' 2 Move the image center to the point (0,0) of the new bitmap.
    ''' 3 Rotate the image to a specified angle.
    ''' 4 Move the rotated image center to the center of the new bitmap.
    ''' </summary>
    Public Sub RotateImg(ByVal angle As Single)
        Dim necessaryRectangle As RectangleF = RectangleF.Empty

        ' Calculate the necessary size while rotate the image based on the 
        ' ContentSize and RotatedAngle.
        Using path As New GraphicsPath()
            path.AddRectangle(
                New RectangleF(0.0F, 0.0F, ContentSize.Width, ContentSize.Height))
            Using mtrx As New Matrix()
                Dim totalAngle As Single = angle + Me.RotatedAngle
                Do While totalAngle >= 360
                    totalAngle -= 360
                Loop
                Me.RotatedAngle = totalAngle
                mtrx.Rotate(RotatedAngle)

                '  the necessary Rectangle
                necessaryRectangle = path.GetBounds(mtrx)
            End Using
        End Using

        Dim newImg As Bitmap = Nothing
        Dim g As Graphics = Nothing

        Try

            ' Create a new bitmap with the specified size.
            newImg = New Bitmap(Convert.ToInt32(necessaryRectangle.Width),
                                Convert.ToInt32(necessaryRectangle.Height))

            ' Create a new Graphics from the bitmap.
            g = Graphics.FromImage(newImg)

            ' Move the image center to the point (0,0) of the new bitmap.
            g.TranslateTransform(-Me.Image.Width \ 2, -Me.Image.Height \ 2)

            ' Rotate the image to a specified angle.
            g.RotateTransform(angle, MatrixOrder.Append)

            ' Move the rotated image center to the center of the new bitmap.
            g.TranslateTransform(newImg.Width \ 2, newImg.Height \ 2, MatrixOrder.Append)

            g.InterpolationMode = InterpolationMode.HighQualityBicubic

            g.DrawImage(Me.Image, 0, 0)

            ' Dispose the original image.
            Me.Image.Dispose()

            ' Set this.Image to the new bitmap.
            Me.Image = newImg

        Catch

            ' If there is any exception, dispose the new bitmap.
            If newImg IsNot Nothing Then
                newImg.Dispose()
            End If

            Throw
        Finally

            ' Dispose the Graphics
            If g IsNot Nothing Then
                g.Dispose()
            End If

        End Try
    End Sub


    ''' <summary>
    ''' You can skew an image by specifying 
    ''' destination points for the upper-left, upper-right, and lower-left corners
    ''' of the original image. The three destination points determine an affine 
    ''' transformation that maps the original rectangular image to a parallelogram.
    ''' 
    ''' For example, the orignal image rectangle is {[0,0], [100,0], [100,50],[50, 0]},
    ''' and unit is -10, the result is {[-10,0], [90,0], [100,50],[50, 0]}. Since the 
    ''' X value of these points may be less than 0, so we have to move the image to 
    ''' make sure that all the values are not less than 0.
    ''' </summary>
    ''' <param name="unit">
    ''' The unit to skew. 
    ''' </param>
    Public Sub Skew(ByVal unit As Integer)
        Dim necessaryRectangle As RectangleF = RectangleF.Empty
        Dim totalUnit As Integer = 0

        ' Calculate the necessary size while rotate the image based on the 
        ' ContentSize and SkewUnit.
        Using path As New GraphicsPath()
            Dim newPoints() As Point = Nothing

            totalUnit = SkewedUnit + unit

            newPoints = New Point() {New Point(totalUnit, 0), New Point(totalUnit + Me.ContentSize.Width, 0), New Point(Me.ContentSize.Width, Me.ContentSize.Height), New Point(0, Me.ContentSize.Height)}
            path.AddLines(newPoints)
            necessaryRectangle = path.GetBounds()
        End Using


        Dim newImg As Bitmap = Nothing
        Dim g As Graphics = Nothing

        Try

            ' Create a new bitmap with the specified size.
            newImg = New Bitmap(Convert.ToInt32(necessaryRectangle.Width), Convert.ToInt32(necessaryRectangle.Height))

            ' Create a new Graphics from the bitmap.
            g = Graphics.FromImage(newImg)

            ' Move the image.
            If totalUnit <= 0 AndAlso SkewedUnit <= 0 Then
                g.TranslateTransform(-unit, 0, MatrixOrder.Append)
            End If

            g.InterpolationMode = InterpolationMode.HighQualityBilinear

            Dim destinationPoints() As Point = {New Point(unit, 0), New Point(unit + Me.Image.Width, 0), New Point(0, Me.Image.Height)}

            g.DrawImage(Me.Image, destinationPoints)

            ' Dispose the original image.
            Me.Image.Dispose()

            ' Set this.Image to the new bitmap.
            Me.Image = newImg

            SkewedUnit = totalUnit
        Catch

            ' If there is any exception, dispose the new bitmap.
            If newImg IsNot Nothing Then
                newImg.Dispose()
            End If

            Throw
        Finally

            ' Dispose the Graphics
            If g IsNot Nothing Then
                g.Dispose()
            End If

        End Try

    End Sub

    ''' <summary>
    ''' Draw the image on a control.
    ''' </summary>
    Public Sub DrawControl(ByVal control_Renamed As Control,
                           ByVal adjust As Point, ByVal penToDrawBounds As Pen)

        '  Creates the System.Drawing.Graphics for the control.
        Dim graphicsForPanel As Graphics = control_Renamed.CreateGraphics()

        ' Clears the entire drawing surface and fills it with the specified 
        ' background color.
        graphicsForPanel.Clear(SystemColors.ControlDark)

        ' Draw the image on the center of the control.
        Dim p As New Point(Convert.ToInt32((control_Renamed.Width - Me.Image.Size.Width) / 2),
                           Convert.ToInt32((control_Renamed.Height - Me.Image.Size.Height) / 2))

        ' Adjust the position.
        graphicsForPanel.TranslateTransform(adjust.X, adjust.Y)

        graphicsForPanel.DrawImage(Me.Image, p)

        ' Draw the bounds if the penToDrawBounds is not null.
        If penToDrawBounds IsNot Nothing Then

            Dim unit = GraphicsUnit.Pixel
            Dim rec = Me.Image.GetBounds(unit)

            graphicsForPanel.DrawRectangle(penToDrawBounds,
                                           rec.X + p.X,
                                           rec.Y + p.Y,
                                           rec.Width,
                                           rec.Height)
            graphicsForPanel.DrawLine(penToDrawBounds,
                                      rec.X + p.X,
                                      rec.Y + p.Y,
                                      rec.X + p.X + rec.Width,
                                      rec.Y + p.Y + rec.Height)

            graphicsForPanel.DrawLine(penToDrawBounds,
                                      rec.X + p.X + rec.Width,
                                      rec.Y + p.Y,
                                      rec.X + p.X,
                                      rec.Y + p.Y + rec.Height)
        End If
    End Sub

    Public Sub Dispose() Implements IDisposable.Dispose
        Me.Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub

    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        ' Protect from being called multiple times.
        If _disposed Then
            Return
        End If

        If disposing Then
            ' Clean up all managed resources.
            If Me.Image IsNot Nothing Then
                Me.Image.Dispose()
            End If

        End If
        _disposed = True
    End Sub
End Class
