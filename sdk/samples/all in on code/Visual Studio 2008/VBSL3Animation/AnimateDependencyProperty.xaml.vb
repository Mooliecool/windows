'***************************** Module Header ******************************\
'* Module Name:  AnimateDependencyProperty.xaml.vb
'* Project:      VBSL3Animation
'* Copyright (c) Microsoft Corporation.
'* 
'* This module shows how to catch custom event of MyEllipse object and in the
'* relevant event handler, it creates animation effect for the Line object by
'* syncronizing its endpoints with the latest mouse click point and current position
'* of MyEllipse object.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 9/8/2009 05:00 PM Allen Chen Created
'\**************************************************************************

Partial Public Class AnimateDependencyProperty
    Inherits UserControl

    Private _currenttargetpoint As Point

    Public Sub New()
        InitializeComponent()

    End Sub

    Private Sub MyStackPanel_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        _currenttargetpoint = e.GetPosition(Me.MyStackPanel)
        Me.MyAnimation.To = _currenttargetpoint
        Me.MyAnimationStoryboard.Begin()

    End Sub

    ''' <summary>
    ''' The following method syncronize MyLine's endpoints with the latest mouse
    ''' click point and current position of MyEllipse object. Therefore, an animation
    ''' is created for MyLine.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub MyAnimatedEllipseGeometry_EllipseCenterChanged(ByVal sender As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)
        Me.MyLine.Visibility = Visibility.Visible
        Me.MyLine.X1 = Me.MyAnimatedEllipseGeometry.EllipseCenter.X
        Me.MyLine.Y1 = Me.MyAnimatedEllipseGeometry.EllipseCenter.Y
        Me.MyLine.X2 = Me._currenttargetpoint.X
        Me.MyLine.Y2 = Me._currenttargetpoint.Y

    End Sub
End Class
