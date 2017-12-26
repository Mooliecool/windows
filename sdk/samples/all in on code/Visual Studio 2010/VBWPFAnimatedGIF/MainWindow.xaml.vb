'***************************** Module Header ******************************\
' Module Name:	MainWindow.xaml.vb
' Project:	    VBWPFAnimatedGIF
' Copyright (c) Microsoft Corporation.
'
' The VBWPFAnimatedGIF demonstrates how to implement 
' an animated GIF image in WPF.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Imports System.Windows

Partial Public Class MainWindow
    Inherits Window
    Public Sub New()
        InitializeComponent()
    End Sub

    ''' <summary>
    ''' "Start" button click event handler, start to play the animation
    ''' </summary>
    Private Sub btnStart_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        GIFCtrl.StartAnimate()
    End Sub

    ''' <summary>
    ''' "Stop" button click event handler, stop to play the animation
    ''' </summary>
    Private Sub btnStop_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        GIFCtrl.StopAnimate()
    End Sub
End Class