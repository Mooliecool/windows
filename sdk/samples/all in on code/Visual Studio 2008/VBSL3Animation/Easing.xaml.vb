'***************************** Module Header ******************************\
'* Module Name:  Easing.xaml.vb
'* Project:      VBSL3Animation
'* Copyright (c) Microsoft Corporation.
'* 
'* This module shows how to use EasingFunction for PointAnimation. In addition,
'* it demonstrates how to write a custom Ease class.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 9/9/2009 05:00 PM Allen Chen Created
'\**************************************************************************

Partial Public Class Easing
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
    Private Sub MyStackPanel_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        Dim targetpoint = e.GetPosition(Me.MyStackPanel)
        Me.MyAnimation.To = targetpoint
        Me.MyAnimationStoryboard.Begin()
    End Sub

    Private Sub MyEaseRadioButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.MyAnimation.EasingFunction = TryCast(Me.Resources("MyEase"), IEasingFunction)
    End Sub

    Private Sub BackEaseRadioButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.MyAnimation.EasingFunction = TryCast(Me.Resources("BackEase"), IEasingFunction)
    End Sub

End Class

''' <summary>
''' A custom Ease class 
''' </summary>
Public Class MyEase
    Inherits EasingFunctionBase
    Protected Overrides Function EaseInCore(ByVal normalizedTime As Double) As Double
        Return normalizedTime / 5
    End Function

End Class