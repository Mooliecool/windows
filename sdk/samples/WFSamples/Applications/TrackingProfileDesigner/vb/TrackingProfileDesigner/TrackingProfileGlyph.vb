'---------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'   Copyright (C) Microsoft Corporation.  All rights reserved.
' 
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
' 
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Workflow
Imports System.Text
Imports System.Collections.ObjectModel
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports Microsoft.Samples.Workflow.TrackingProfileDesigner.My.Resources

' Provides the glyphs for mapping tracking profile data onto individual activity designers
Friend Class TrackingGlyphProvider
    Implements IDesignerGlyphProvider

    ' Keep static references to our resource bitmaps
    Shared redPin As Bitmap = Resources.redpin
    Shared fadedRedPin As Bitmap = Resources.fadedredpin

    Dim profileManager As TrackingProfileManager

    Friend Sub New(ByVal context As TrackingProfileManager)
        profileManager = context
    End Sub

#Region "IDesignerGlyphProvider Members"
    Public Function GetGlyphs(ByVal activityDesigner As ActivityDesigner) As ActivityDesignerGlyphCollection _
        Implements IDesignerGlyphProvider.GetGlyphs

        Dim glyphs As New ActivityDesignerGlyphCollection()
        ' The glyph position indicates how far down the glyph is drawn
        Dim glyphPosition As Integer = -1
        Dim validationError = String.Empty
        If (profileManager.IsActivityValid(activityDesigner.Activity, validationError)) Then
            ' Add an error glyph if the selected activity is not configured correctly
            glyphPosition += 1
            glyphs.Add(New ErrorActivityGlyph(validationError))
        End If
        If (profileManager.IsTracked(activityDesigner.Activity)) Then
            'Add the glyph for the trackpoint
            glyphPosition += 1
            glyphs.Add(New TrackedActivityGlyph(glyphPosition, redPin))
        End If
        If profileManager.IsMatchedByDerivedTrackPoint(activityDesigner.Activity) Then
            ' Add faded derive match glyph
            glyphPosition += 1
            glyphs.Add(New TrackedActivityGlyph(glyphPosition, fadedRedPin))
        End If
        Dim annotation As String = profileManager.GetAnnotation(activityDesigner.Activity)
        If annotation IsNot Nothing Then
            ' If an annotation exists, use the tooltip via the description.
            activityDesigner.Activity.Description = annotation
        End If
        Return glyphs
    End Function
#End Region

End Class


' This glyph shows that the activity is tracked
Friend NotInheritable Class TrackedActivityGlyph
    Inherits DesignerGlyph
    Dim image As Bitmap = Nothing
    Dim position As Integer = 0


    Friend Sub New(ByVal glyphPosition As Integer, ByVal icon As Bitmap)
        position = glyphPosition
        image = icon
    End Sub

    'Sets the bounds for the tracked activity glyph; the position is used to determine how far down to draw the glyph
    Public Overrides Function GetBounds(ByVal designer As ActivityDesigner, ByVal activated As Boolean) As Rectangle
        Dim imageBounds As New Rectangle()
        If image IsNot Nothing Then
            Dim glyphSize As New Size(CType((image.Width * 0.75), Integer), CType((image.Height * 0.75), Integer))
            imageBounds.Location = New Point( _
              designer.Bounds.Right - glyphSize.Width / 4, _
              designer.Bounds.Top - glyphSize.Height / 2 + (position * (CType((0.8 * glyphSize.Height), Integer))))
            imageBounds.Size = glyphSize
        End If
        Return imageBounds
    End Function

    Protected Overrides Sub OnPaint(ByVal graphics As Graphics, ByVal activated As Boolean, ByVal ambientTheme As AmbientTheme, ByVal designer As ActivityDesigner)
        image.MakeTransparent(Color.FromArgb(255, 255, 255))
        If image IsNot Nothing Then
            graphics.DrawImage(image, GetBounds(designer, activated), New Rectangle(Point.Empty, image.Size), GraphicsUnit.Pixel)
        End If
    End Sub
End Class


'This glyph shows that the activity's track point is not correctly configured
Friend NotInheritable Class ErrorActivityGlyph
    Inherits DesignerGlyph

    Shared image As Bitmap = Resources.errorIcon
    Dim errorMessage As String

    Friend Sub New(ByVal errorMessage As String)
        Me.errorMessage = errorMessage
    End Sub

    Public Overrides ReadOnly Property CanBeActivated() As Boolean
        Get
            Return True
        End Get
    End Property

    ' Display an error message when this glyph is clicked
    Protected Overrides Sub OnActivate(ByVal designer As ActivityDesigner)
        MessageBox.Show(errorMessage)
    End Sub

    Public Overrides Function GetBounds(ByVal designer As ActivityDesigner, ByVal activated As Boolean) As Rectangle
        Dim imageBounds As New Rectangle()
        If image IsNot Nothing Then
            imageBounds.Size = image.Size
            imageBounds.Location = New Point(designer.Bounds.Right - imageBounds.Size.Width / 4, designer.Bounds.Top - imageBounds.Size.Height / 2)
        End If
        Return imageBounds
    End Function


    Protected Overrides Sub OnPaint(ByVal graphics As Graphics, ByVal activated As Boolean, ByVal ambientTheme As AmbientTheme, ByVal designer As ActivityDesigner)
        image.MakeTransparent(Color.FromArgb(255, 255, 255))
        If image IsNot Nothing Then
            graphics.DrawImage(image, GetBounds(designer, activated), New Rectangle(Point.Empty, image.Size), GraphicsUnit.Pixel)
        End If
    End Sub
End Class
