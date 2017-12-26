'***************************** Module Header ******************************\
'* Module Name:  MainPage.xaml.vb
'* Project:      VBSL3PlaneProjection
'* Copyright (c) Microsoft Corporation.
'* 
'* This example illustrates how to use the new perspective 3D feature of Silverlight 3. 
'* PlaneProjection is the commonly  used object to achieve perspective 3D effect.
'* 
'* Perspective in the graphic arts, such as drawing, is an approximate representation,
'* on a flat surface (such as paper), of an image as it is perceived by the eye. 
'*
'* Now in Silverlight 3 you can use PlaneProjection to achieve the amazing effect!
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 7/15/2009 01:00 PM Allen Chen Created
'\**************************************************************************

Partial Public Class MainPage
    Inherits UserControl

    Public Sub New()
        InitializeComponent()
    End Sub


    Private Sub Calendar_SelectedDatesChanged(ByVal sender As Object, ByVal e As SelectionChangedEventArgs)
        ' Get the reference of the sender Calendar, then show the selected date on TextBlockShowDate control.
        Dim calendar As Calendar = CType(sender, Calendar)
        Me.TextBlockShowDate.Text = calendar.SelectedDate.Value.ToShortDateString()
    End Sub

    Private Sub SliderRotation_ValueChanged(ByVal sender As Object, ByVal e As RoutedPropertyChangedEventArgs(Of Double))
        ' Change the rotation center position of PlaneProjection and rotate Calendar.
        RefreshPlaneProjection(Me.PlaneProjection, Me.SliderRotationX.Value, Me.SliderRotationY.Value, Me.SliderRotationZ.Value, Double.NaN, Double.NaN, Double.NaN)
        ' Refresh TextBlockPlaneProjectionDetails control to show the current value of 
        ' RotationX, RotationY, RotationZ, CenterOfRotationX, CenterOfRotationY, CenterOfRotationZ 
        ' of PlaneProjection.
        RefreshTextBlockPlaneProjectionDetails()
    End Sub

    Private Sub SliderCenterOfRotation_ValueChanged(ByVal sender As Object, ByVal e As RoutedPropertyChangedEventArgs(Of Double))
        ' Change the rotation center position of PlaneProjection and rotate Calendar.
        RefreshPlaneProjection(Me.PlaneProjection, Double.NaN, Double.NaN, Double.NaN, Me.SliderCenterX.Value, Me.SliderCenterY.Value, Me.SliderCenterZ.Value)
        ' Refresh TextBlockPlaneProjectionDetails control to show the current value of 
        ' RotationX, RotationY, RotationZ, CenterOfRotationX, CenterOfRotationY, CenterOfRotationZ 
        ' of PlaneProjection.
        RefreshTextBlockPlaneProjectionDetails()
    End Sub

    Private Sub RefreshPlaneProjection(ByVal planeProjection As PlaneProjection, ByVal rotationX As Double, ByVal rotationY As Double, ByVal rotationZ As Double, ByVal centerOfRotationX As Double, ByVal centerOfRotationY As Double, ByVal centerOfRotationZ As Double)
        ' Change rotation and rotation center related properties of PlaneProjection.
        If (Not Double.IsNaN(rotationX)) Then
            planeProjection.RotationX = rotationX
        End If
        If (Not Double.IsNaN(rotationY)) Then
            planeProjection.RotationY = rotationY
        End If
        If (Not Double.IsNaN(rotationZ)) Then
            planeProjection.RotationZ = rotationZ
        End If
        If (Not Double.IsNaN(centerOfRotationX)) Then
            planeProjection.CenterOfRotationX = centerOfRotationX
        End If
        If (Not Double.IsNaN(centerOfRotationY)) Then
            planeProjection.CenterOfRotationY = centerOfRotationY
        End If
        If (Not Double.IsNaN(centerOfRotationZ)) Then
            planeProjection.CenterOfRotationZ = centerOfRotationZ
        End If
    End Sub

    Private Sub RefreshTextBlockPlaneProjectionDetails()
        Me.TextBlockPlaneProjectionDetails.Text = String.Format("RotationX:{0}, RotationY:{1}, RotationZ:{2}" & ControlChars.CrLf & "CenterOfRotationX:{3}, CenterOfRotationY:{4}, CenterOfRotationZ:{5}", Me.PlaneProjection.RotationX, Me.PlaneProjection.RotationY, Me.PlaneProjection.RotationZ, Me.PlaneProjection.CenterOfRotationX, Me.PlaneProjection.CenterOfRotationY, Me.PlaneProjection.CenterOfRotationZ)
    End Sub

    Private Sub ButtonResetRotation_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Reset the value of roation related Sliders.
        Me.SliderRotationX.Value = 0
        Me.SliderRotationY.Value = 0
        Me.SliderRotationZ.Value = 0
    End Sub

    Private Sub ButtonResetCenterOfRotation_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Reset the value of roation center related Sliders.
        Me.SliderCenterX.Value = 0
        Me.SliderCenterY.Value = 0
        Me.SliderCenterZ.Value = 0
    End Sub
End Class