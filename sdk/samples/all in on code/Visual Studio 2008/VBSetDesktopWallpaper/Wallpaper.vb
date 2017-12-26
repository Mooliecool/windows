'*********************************** Module Header ***********************************'
' Module Name:  Wallpaper.vb
' Project:      VBSetDesktopWallpaper
' Copyright (c) Microsoft Corporation.
' 
' The file defines a wallpaper helper class.
' 
'   Wallpaper.SetDesktopWallpaper(ByVal path As String, ByVal style As WallpaperStyle)
' 
' This is the key method that sets the desktop wallpaper. The method body is composed 
' of configuring the wallpaper style in the registry and setting the wallpaper with 
' SystemParametersInfo.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************************'

Imports Microsoft.Win32
Imports System.Environment
Imports System.Drawing.Imaging
Imports System.ComponentModel
Imports System.Runtime.InteropServices


Public Class Wallpaper

    ''' <summary>
    ''' Determine if .jpg files are supported as wallpaper in the current 
    ''' operating system. The .jpg wallpapers are not supported before 
    ''' Windows Vista.
    ''' </summary>
    Public Shared ReadOnly Property SupportJpgAsWallpaper()
        Get
            Return (Environment.OSVersion.Version >= New Version(6, 0))
        End Get
    End Property


    ''' <summary>
    ''' Determine if the fit and fill wallpaper styles are supported in the 
    ''' current operating system. The styles are not supported before 
    ''' Windows 7.
    ''' </summary>
    Public Shared ReadOnly Property SupportFitFillWallpaperStyles()
        Get
            Return (Environment.OSVersion.Version >= New Version(6, 1))
        End Get
    End Property


    ''' <summary>
    ''' Set the desktop wallpaper.
    ''' </summary>
    ''' <param name="path">Path of the wallpaper</param>
    ''' <param name="style">Wallpaper style</param>
    Public Shared Sub SetDesktopWallpaper(ByVal path As String, ByVal style As WallpaperStyle)

        ' Set the wallpaper style and tile. 
        ' Two registry values are set in the Control Panel\Desktop key.
        ' TileWallpaper
        '  0: The wallpaper picture should not be tiled 
        '  1: The wallpaper picture should be tiled 
        ' WallpaperStyle
        '  0:  The image is centered if TileWallpaper=0 or tiled if TileWallpaper=1
        '  2:  The image is stretched to fill the screen
        '  6:  The image is resized to fit the screen while maintaining the aspect 
        '      ratio. (Windows 7 and later)
        '  10: The image is resized and cropped to fill the screen while 
        '      maintaining the aspect ratio. (Windows 7 and later)
        Dim key As RegistryKey = Registry.CurrentUser.OpenSubKey("Control Panel\Desktop", True)

        Select Case style
            Case WallpaperStyle.Tile
                key.SetValue("WallpaperStyle", "0")
                key.SetValue("TileWallpaper", "1")
                Exit Select
            Case WallpaperStyle.Center
                key.SetValue("WallpaperStyle", "0")
                key.SetValue("TileWallpaper", "0")
                Exit Select
            Case WallpaperStyle.Stretch
                key.SetValue("WallpaperStyle", "2")
                key.SetValue("TileWallpaper", "0")
                Exit Select
            Case WallpaperStyle.Fit ' (Windows 7 and later)
                key.SetValue("WallpaperStyle", "6")
                key.SetValue("TileWallpaper", "0")
                Exit Select
            Case WallpaperStyle.Fill ' (Windows 7 and later)
                key.SetValue("WallpaperStyle", "10")
                key.SetValue("TileWallpaper", "0")
                Exit Select
        End Select

        key.Close()


        ' If the specified image file is neither .bmp nor .jpg, - or -
        ' if the image is a .jpg file but the operating system is Windows Server 
        ' 2003 or Windows XP/2000 that does not support .jpg as the desktop 
        ' wallpaper, convert the image file to .bmp and save it to the 
        '  %appdata%\Microsoft\Windows\Themes folder.
        Dim ext As String = System.IO.Path.GetExtension(path)
        If ((Not ext.Equals(".bmp", StringComparison.OrdinalIgnoreCase) AndAlso _
             Not ext.Equals(".jpg", StringComparison.OrdinalIgnoreCase)) _
            OrElse _
            (ext.Equals(".jpg", StringComparison.OrdinalIgnoreCase) AndAlso _
            (Not SupportJpgAsWallpaper))) Then

            Using image As Image = image.FromFile(path)
                path = String.Format("{0}\Microsoft\Windows\Themes\{1}.bmp", _
                    Environment.GetFolderPath(SpecialFolder.ApplicationData), _
                    System.IO.Path.GetFileNameWithoutExtension(path))
                image.Save(path, ImageFormat.Bmp)
            End Using

        End If

        ' Set the desktop wallpapaer by calling the Win32 API SystemParametersInfo 
        ' with the SPI_SETDESKWALLPAPER desktop parameter. The changes should 
        ' persist, and also be immediately visible.
        If Not Wallpaper.SystemParametersInfo(20, 0, path, 3) Then
            Throw New Win32Exception
        End If
    End Sub


    <DllImport("user32.dll", CharSet:=CharSet.Unicode, SetLastError:=True)> _
    Private Shared Function SystemParametersInfo( _
        ByVal uiAction As UInt32, _
        ByVal uiParam As UInt32, _
        ByVal pvParam As String, _
        ByVal fWinIni As UInt32) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

    Private Const SPI_SETDESKWALLPAPER As UInt32 = 20
    Private Const SPIF_SENDWININICHANGE As UInt32 = 2
    Private Const SPIF_UPDATEINIFILE As UInt32 = 1
End Class


Public Enum WallpaperStyle
    Tile
    Center
    Stretch
    Fit
    Fill
End Enum