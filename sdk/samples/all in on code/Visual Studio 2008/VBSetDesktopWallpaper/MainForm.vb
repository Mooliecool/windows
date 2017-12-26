'*********************************** Module Header ***********************************'
' Module Name:  MainForm.vb
' Project:      VBSetDesktopWallpaper
' Copyright (c) Microsoft Corporation.
' 
' This code sample application allows you select an image, view a preview (resized 
' smaller to fit if necessary), select a display style among Tile, Center, Stretch, Fit 
' (Windows 7 and later) and Fill (Windows 7 and later), and set the image as the 
' Desktop wallpaper. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************************'

Imports VBSetDesktopWallpaper.My


Public Class MainForm

    Private wallpaperFileName As String = Nothing


    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles MyBase.Load

        ' If the current operating system is not Windows 7 or later, disable the Fit 
        ' and Fill wallpaper styles.
        If (Not Wallpaper.SupportFitFillWallpaperStyles) Then
            Me.radFit.Enabled = False
            Me.radFill.Enabled = False
        End If

        Me.toolTip.SetToolTip(Me.radTile, Resources.TileStyleTooltip)
        Me.toolTip.SetToolTip(Me.radCenter, Resources.CenterStyleTooltip)
        Me.toolTip.SetToolTip(Me.radStretch, Resources.StretchStyleTooltip)
        Me.toolTip.SetToolTip(Me.radFit, Resources.FitStyleTooltip)
        Me.toolTip.SetToolTip(Me.radFill, Resources.FillStyleTooltip)

    End Sub


    Private Sub btnBrowseWallpaper_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnBrowseWallpaper.Click

        If (DialogResult.OK = Me.wallpaperOpenFileDialog.ShowDialog) Then
            Me.wallpaperFileName = Me.wallpaperOpenFileDialog.FileName
            Me.tbWallpaperFileName.Text = Me.wallpaperFileName

            ' Preview the image in a picture box.
            Dim wallpaper As Image = Image.FromFile(Me.wallpaperFileName)

            If ((wallpaper.Width < Me.pctPreview.Width) AndAlso _
                (wallpaper.Height < Me.pctPreview.Height)) Then
                Me.pctPreview.Image = wallpaper
            Else
                Dim wallpaperRatio As Single = (CSng(wallpaper.Width) / CSng(wallpaper.Height))
                Dim pctPreviewRatio As Single = (CSng(Me.pctPreview.Width) / CSng(Me.pctPreview.Height))

                If (wallpaperRatio >= pctPreviewRatio) Then
                    Me.pctPreview.Image = wallpaper.GetThumbnailImage( _
                        Me.pctPreview.Width, _
                        CInt((CSng(Me.pctPreview.Width) / wallpaperRatio)), _
                        Nothing, IntPtr.Zero)
                Else
                    Me.pctPreview.Image = wallpaper.GetThumbnailImage( _
                        CInt((Me.pctPreview.Height * wallpaperRatio)), _
                        Me.pctPreview.Height, _
                        Nothing, IntPtr.Zero)
                End If
            End If
        End If
    End Sub


    Private Sub btnSetWallpaper_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnSetWallpaper.Click
        If Not String.IsNullOrEmpty(Me.wallpaperFileName) Then
            Wallpaper.SetDesktopWallpaper( _
                Me.wallpaperFileName, _
                Me.SelectedWallpaperStyle)
        End If
    End Sub


    Private ReadOnly Property SelectedWallpaperStyle() As WallpaperStyle
        Get
            If Me.radTile.Checked Then
                Return WallpaperStyle.Tile
            End If
            If Me.radCenter.Checked Then
                Return WallpaperStyle.Center
            End If
            If Me.radStretch.Checked Then
                Return WallpaperStyle.Stretch
            End If
            If Me.radFit.Checked Then
                Return WallpaperStyle.Fit
            End If
            Return WallpaperStyle.Fill
        End Get
    End Property

End Class
