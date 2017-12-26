'****************************** Module Header ******************************\
' Module Name:  MainPage.cs
' Project:      VBAzureXbap
' Copyright (c) Microsoft Corporation.
' 
' The main page. All touching events are handled in this class.
' It also invokes a WCF service to obtain the data center list.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports XbapClient.DataCenterServiceReference
Imports System.Windows.Media.Media3D
Imports System.Windows.Media.Animation
Imports System.Windows.Interop
Imports System.ServiceModel

Class MainPage
    Inherits Page
    Private _angleBuffer As Double = 0.0R
    ' Modify the image's size if you want to change the image.
    Private Shared _imageWidth As Integer = 1995
    Private Shared _imageHeight As Integer = 2051
    Private _dataCenters As List(Of DataCenter)

    ' The following variables are for mouse simulation.
    Private _isMouseDown As Boolean
    Private _startPoint As Point

    Public Sub New()
        InitializeComponent()
        ' You can obtain the current XBAP's URI using BrowserInteropHelper.Source.
        Dim serviceUri = New Uri(BrowserInteropHelper.Source, "../DataCenterService.svc")
        Dim client = New DataCenterServiceClient(New BasicHttpBinding(), New EndpointAddress(serviceUri))
        AddHandler client.GetDataCentersCompleted, New EventHandler(Of GetDataCentersCompletedEventArgs)(AddressOf Me.client_GetDataCentersCompleted)
        client.GetDataCentersAsync()
        AddHandler Touch.FrameReported, AddressOf Touch_FrameReported
    End Sub

    Private Sub client_GetDataCentersCompleted(ByVal sender As Object, ByVal e As GetDataCentersCompletedEventArgs)
        Me._dataCenters = e.Result.ToList()
    End Sub


    ''' <summary>
    ''' This is the event handler for the multi-touch ManipulationDelta event.
    ''' </summary>
    ''' <param name="e">
    ''' Provides 2D manipulation information such as scale, translate, and rotate.
    ''' The scale and translate information can be used in 3D scenarios as well. But the rotate information can't.
    ''' For this sample, however, we do not translate the 3D scene using the translate gesture. 
    ''' Instead, we allow the user to rotate the ball follow the finger.
    ''' </param>
    Private Sub OnManipulationDelta(ByVal sender As Object, ByVal e As ManipulationDeltaEventArgs)
        Me.scaleTransform.ScaleX *= e.DeltaManipulation.Scale.X
        Me.scaleTransform.ScaleY *= e.DeltaManipulation.Scale.Y
        Me.scaleTransform.ScaleZ *= e.DeltaManipulation.Scale.X

        Me._angleBuffer += 1
        ' To avoid screen slash and to save a few CPU resource, do not rotate the scene whenever a maniputation event occurs.
        ' Only rotate the scene if the angle cumulated enough.
        If _angleBuffer >= 0 Then
            Dim delta As Vector = e.DeltaManipulation.Translation
            Me.RotateEarth(delta)
        End If
        e.Handled = True
    End Sub

    ''' <summary>
    ''' Common rotate method that invoked by both manipulation event handler and mouse event handler.
    ''' </summary>
    ''' <param name="delta">How long has the finger/mouse moved.</param>
    Private Sub RotateEarth(ByVal delta As Vector)
        If delta.X <> 0 OrElse delta.Y <> 0 Then
            ' Convert delta to a 3D vector.
            Dim vOriginal As New Vector3D(-delta.X, delta.Y, 0.0R)
            Dim vZ As New Vector3D(0, 0, 1)
            ' Find a vector that is perpendicular with the delta vector on the XY surface. This will be the rotation axis.
            Dim perpendicular As Vector3D = Vector3D.CrossProduct(vOriginal, vZ)
            Dim rotate As New RotateTransform3D()
            ' The QuaternionRotation3D allows you to easily specify a rotation axis.
            Dim quatenion As New QuaternionRotation3D()
            quatenion.Quaternion = New Quaternion(perpendicular, 3)
            rotate.Rotation = quatenion
            Me.transformGroup.Children.Add(rotate)
            Me._angleBuffer = 0
        End If
    End Sub

    ''' <summary>
    ''' TouchUp event handler. Similar to MouseUp event handler
    ''' </summary>
    Private Sub OnTouchUp(ByVal sender As Object, ByVal e As TouchEventArgs)
        DoHitTest(e.GetTouchPoint(Me.viewport).Position)
    End Sub

    ''' <summary>
    ''' Common hittest method that invoked by both touch event handler and mouse event handler.
    ''' </summary>
    ''' <param name="point">The touched/clicked point</param>
    Private Sub DoHitTest(ByVal point As Point)
        If (Not (Me._dataCenters Is Nothing)) Then
            VisualTreeHelper.HitTest(Me.viewport, Nothing, New HitTestResultCallback(Function(target)
                                                                                         Dim result As RayMeshGeometry3DHitTestResult = TryCast(target, RayMeshGeometry3DHitTestResult)
                                                                                         If result IsNot Nothing Then
                                                                                             ' Calculate the hit point using barycentric coordinates formula:
                                                                                             ' p = p1 * w1 + p2 * w2 + p3 * w3.
                                                                                             ' For more information, please refer to http://en.wikipedia.org/wiki/Barycentric_coordinates_%28mathematics%29.
                                                                                             Dim p1 As Point = result.MeshHit.TextureCoordinates(result.VertexIndex1)
                                                                                             Dim p2 As Point = result.MeshHit.TextureCoordinates(result.VertexIndex2)
                                                                                             Dim p3 As Point = result.MeshHit.TextureCoordinates(result.VertexIndex3)
                                                                                             Dim hitX As Double = p1.X * result.VertexWeight1 + p2.X * result.VertexWeight2 + p3.X * result.VertexWeight3
                                                                                             Dim hitY As Double = p1.Y * result.VertexWeight1 + p2.Y * result.VertexWeight2 + p3.Y * result.VertexWeight3
                                                                                             Dim pointHit As New Point(hitX * _imageWidth, hitY * _imageHeight)
                                                                                             ' If a data center circle is hit, display the information.
                                                                                             For Each dc As DataCenter In Me._dataCenters
                                                                                                 Dim bound = New System.Windows.Rect(dc.Bound.X, dc.Bound.Y, dc.Bound.Width, dc.Bound.Height)
                                                                                                 If bound.Contains(pointHit) Then
                                                                                                     Me.InfoTextBox.Text = "You've just touched the " & dc.Name & " data center!"
                                                                                                     Dim sb As Storyboard = TryCast(Me.Resources("sb"), Storyboard)
                                                                                                     If sb IsNot Nothing Then
                                                                                                         sb.Begin()
                                                                                                     End If
                                                                                                     Return HitTestResultBehavior.Stop
                                                                                                 End If
                                                                                             Next
                                                                                         End If
                                                                                         Return HitTestResultBehavior.Continue
                                                                                     End Function), New PointHitTestParameters(point))
        End If
    End Sub

    ''' <summary>
    ''' Handle raw touch events.
    ''' In this sample, if you touch the screen with two fingers and release the first finger, 
    ''' a line will be drawn to connect the two touch points.
    ''' </summary>
    Private Sub Touch_FrameReported(ByVal sender As Object, ByVal e As TouchFrameEventArgs)
        Dim touchPoints = e.GetTouchPoints(Me.viewport)
        If touchPoints.Count >= 2 AndAlso touchPoints(0).Action = TouchAction.Up Then
            Me.TouchLine.X1 = touchPoints(0).Position.X
            Me.TouchLine.X2 = touchPoints(1).Position.X
            Me.TouchLine.Y1 = touchPoints(0).Position.Y
            Me.TouchLine.Y2 = touchPoints(1).Position.Y
        End If
    End Sub

    ' The following are event handlers for mouse simulation.
    Private Sub Grid_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        Me._isMouseDown = True
        Me._startPoint = e.GetPosition(Me.viewport)
    End Sub

    Private Sub Grid_MouseMove(ByVal sender As Object, ByVal e As MouseEventArgs)
        If Me._isMouseDown AndAlso Me.MouseSimulationCheckBox.IsChecked.Value Then
            Me._angleBuffer += 1
            If _angleBuffer >= 0 Then
                Dim currentPoint As Point = e.GetPosition(Me.viewport)
                Dim delta As New Vector(currentPoint.X - Me._startPoint.X, currentPoint.Y - Me._startPoint.Y)
                RotateEarth(delta)
            End If
        End If
    End Sub

    Private Sub Grid_MouseLeftButtonUp(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        Me._isMouseDown = False
        If Me.MouseSimulationCheckBox.IsChecked.Value Then
            Me.DoHitTest(e.GetPosition(Me.viewport))
        End If
    End Sub

    Private Sub Grid_MouseWheel(ByVal sender As Object, ByVal e As MouseWheelEventArgs)
        If Me.MouseSimulationCheckBox.IsChecked.Value Then
            Dim delta As Double = If(e.Delta > 0, 1.2, 0.8)
            Me.scaleTransform.ScaleX *= delta
            Me.scaleTransform.ScaleY *= delta
            Me.scaleTransform.ScaleZ *= delta
        End If
    End Sub
End Class
