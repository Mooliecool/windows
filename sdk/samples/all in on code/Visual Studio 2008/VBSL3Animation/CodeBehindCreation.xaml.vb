'***************************** Module Header ******************************\
'* Module Name:  CodeBehindCreation.xaml.vb
'* Project:      VBSL3Animation
'* Copyright (c) Microsoft Corporation.
'* 
'* This module shows how to initialize a Storyboard in code behind. The final effect
'* is the same as BasicPointAnimation.xaml, which uses XAML to add Storyboard.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 9/10/2009 03:00 PM Allen Chen Created
'\**************************************************************************

Partial Public Class CodeBehindCreation
    Inherits UserControl

    Private _myAnimation As PointAnimation = New PointAnimation()
    Private _myAnimationStoryboard As Storyboard = New Storyboard()

    ''' <summary>
    ''' In the following constructor, the PointAnimation is added to Storyboard.
    ''' They are initialized for animation.
    ''' </summary>
    Public Sub New()
        InitializeComponent()
        _myAnimation.Duration = New Duration(TimeSpan.FromSeconds(2))
        _myAnimation.SetValue(Storyboard.TargetPropertyProperty, New PropertyPath("Center"))
        Storyboard.SetTarget(_myAnimation, MyAnimatedEllipseGeometry)
        _myAnimationStoryboard.Children.Add(_myAnimation)
    End Sub

    ''' <summary>
    ''' Tbe following event handler change the To property of PointAnimation object,
    ''' then begin the Storyboard to play the animation. Please note we can change
    ''' To property even when the animation is playing.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub MyStackPanel_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)

        Dim targetpoint = e.GetPosition(Me.MyStackPanel)
        Me._myAnimation.To = targetpoint
        Me._myAnimationStoryboard.Begin()

    End Sub

End Class
