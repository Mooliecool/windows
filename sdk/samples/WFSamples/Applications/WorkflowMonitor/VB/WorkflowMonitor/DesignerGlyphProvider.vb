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

Imports System.Workflow.ComponentModel.Design

Namespace WorkflowMonitor
    Friend Class WorkflowMonitorDesignerGlyphProvider
        Implements IDesignerGlyphProvider

        Dim activityStatusList As Dictionary(Of String, ActivityStatusInfo)

        Friend Sub New(ByVal activityStatusList As Dictionary(Of String, ActivityStatusInfo))
            Me.activityStatusList = activityStatusList
        End Sub

        Public Function GetGlyphs(ByVal activityDesigner As System.Workflow.ComponentModel.Design.ActivityDesigner) As System.Workflow.ComponentModel.Design.ActivityDesignerGlyphCollection Implements System.Workflow.ComponentModel.Design.IDesignerGlyphProvider.GetGlyphs
            Dim glyphList As ActivityDesignerGlyphCollection = New ActivityDesignerGlyphCollection()

            'Walk all of the activities and use the 'CompletedGlyph' for all activities that are not 'closed'
            For Each activityStatus As ActivityStatusInfo In activityStatusList.Values
                If activityStatus.Name = activityDesigner.Activity.Name Then
                    If activityStatus.Status = "Closed" Then
                        glyphList.Add(New CompletedGlyph())
                    Else
                        glyphList.Add(New ExecutingGlyph())
                    End If
                End If
            Next

            Return glyphList
        End Function
    End Class

    ' Define a glyph to show an activity is executing, i.e. not 'closed'
    Friend Class ExecutingGlyph
        Inherits DesignerGlyph

        Public Overrides Function GetBounds(ByVal designer As System.Workflow.ComponentModel.Design.ActivityDesigner, ByVal activated As Boolean) As System.Drawing.Rectangle
            Dim imageBounds As Rectangle = Rectangle.Empty
            Dim image As Image = AppResources.Executing
            If Not image Is Nothing Then
                Dim glyphSize As Size = WorkflowTheme.CurrentTheme.AmbientTheme.GlyphSize
                imageBounds.Location = New Point(designer.Bounds.Right - glyphSize.Width / 2, designer.Bounds.Top - glyphSize.Height / 2)
                imageBounds.Size = glyphSize
            End If
            Return imageBounds
        End Function

        Protected Overrides Sub OnPaint(ByVal graphics As System.Drawing.Graphics, ByVal activated As Boolean, ByVal ambientTheme As System.Workflow.ComponentModel.Design.AmbientTheme, ByVal designer As System.Workflow.ComponentModel.Design.ActivityDesigner)
            Dim bitmap As Bitmap = AppResources.Executing
            bitmap.MakeTransparent(Color.FromArgb(0, 255, 255))

            If Not bitmap Is Nothing Then
                graphics.DrawImage(bitmap, GetBounds(designer, activated), New Rectangle(Point.Empty, bitmap.Size), GraphicsUnit.Pixel)
            End If
        End Sub
    End Class


    'Define a glyph to show an activity is 'closed'
    Friend Class CompletedGlyph
        Inherits DesignerGlyph

        Public Overrides Function GetBounds(ByVal designer As System.Workflow.ComponentModel.Design.ActivityDesigner, ByVal activated As Boolean) As System.Drawing.Rectangle
            Dim imageBounds As Rectangle = Rectangle.Empty
            Dim image As Image = AppResources.complete
            If Not image Is Nothing Then
                Dim glyphSize As Size = WorkflowTheme.CurrentTheme.AmbientTheme.GlyphSize
                imageBounds.Location = New Point(designer.Bounds.Right - glyphSize.Width / 2, designer.Bounds.Top - glyphSize.Height / 2)
                imageBounds.Size = glyphSize
            End If
            Return imageBounds
        End Function

        Protected Overrides Sub OnPaint(ByVal graphics As System.Drawing.Graphics, ByVal activated As Boolean, ByVal ambientTheme As System.Workflow.ComponentModel.Design.AmbientTheme, ByVal designer As System.Workflow.ComponentModel.Design.ActivityDesigner)
            Dim bitmap As Bitmap = AppResources.complete
            bitmap.MakeTransparent(Color.FromArgb(0, 255, 255))
            If Not bitmap Is Nothing Then
                graphics.DrawImage(bitmap, GetBounds(designer, activated), New Rectangle(Point.Empty, bitmap.Size), GraphicsUnit.Pixel)
            End If
        End Sub
    End Class
End Namespace
