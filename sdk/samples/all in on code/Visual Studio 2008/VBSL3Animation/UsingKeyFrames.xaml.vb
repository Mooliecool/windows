'***************************** Module Header ******************************\
'* Module Name:  UsingKeyFrames.xaml.vb
'* Project:      VBSL3Animation
'* Copyright (c) Microsoft Corporation.
'* 
'* This module shows how to create KeyFrames based animation in Silverilght
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 9/10/2009 03:00 PM Allen Chen Created
'\**************************************************************************

Partial Public Class UsingKeyFrames
    Inherits UserControl


    Public Sub New()
        InitializeComponent()
    End Sub

    Private Sub Button_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.MyAnimationStoryboard.Begin()
    End Sub

End Class
