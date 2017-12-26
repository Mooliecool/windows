'***************************** Module Header ******************************\
'* Module Name:  MyPointAnimation.xaml.vb
'* Project:      VBSL3Animation
'* Copyright (c) Microsoft Corporation.
'* 
'* This module shows how to write baisc PointAnimation for an EllipseGeometry
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 9/8/2009 05:00 PM Allen Chen Created
'\**************************************************************************

Partial Public Class BasicPointAnimation
    Inherits UserControl

    Public Sub New()
        InitializeComponent()
    End Sub
    ''' <summary>
    ''' Tbe following event handler change the To property of PointAnimation object,
    ''' then begin the Storyboard to play the animation. Please note we can change
    ''' To property even when the animation is playing.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub StackPanel_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)

        Dim targetpoint = e.GetPosition(Me.MyStackPanel)
        Me.MyAnimation.To = targetpoint
        Me.MyAnimationStoryboard.Begin()

    End Sub
End Class
