'***************************** Module Header ******************************\
'* Module Name:  MyEllipse.xaml.vb
'* Project:      VBSL3Animation
'* Copyright (c) Microsoft Corporation.
'* 
'* This module shows how to write a UserControl that basically wraps EllipseGeometry
'* to "expose" its Center property.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 9/8/2009 05:00 PM Allen Chen Created
'\**************************************************************************

Public Delegate Sub EllipseCenterChangedEventHandler(ByVal sender As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)

Partial Public Class MyEllipse
    Inherits UserControl

    Public Shared ReadOnly EllipseCenterProperty As DependencyProperty = DependencyProperty.Register("EllipseCenter", GetType(Point), GetType(MyEllipse), New PropertyMetadata(New PropertyChangedCallback(AddressOf MyHandler)))
    Private _mydelegate As EllipseCenterChangedEventHandler

    Public Custom Event EllipseCenterChanged As EllipseCenterChangedEventHandler
        AddHandler(ByVal value As EllipseCenterChangedEventHandler)
            If _mydelegate Is Nothing Then
                _mydelegate = value
            Else
                System.Delegate.Combine(_mydelegate, value)
            End If
        End AddHandler
        RemoveHandler(ByVal value As EllipseCenterChangedEventHandler)

            System.Delegate.Remove(_mydelegate, value)
        End RemoveHandler
        RaiseEvent(ByVal sender As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)
        End RaiseEvent
    End Event


    Public Property EllipseCenter() As Point
        Get
            Return CType(GetValue(EllipseCenterProperty), Point)
        End Get
        Set(ByVal value As Point)
            SetValue(EllipseCenterProperty, value)
        End Set

    End Property


    Public Sub New()

        InitializeComponent()
        EllipseCenter = Me.MyAnimatedEllipseGeometry.Center
    End Sub

    ''' <summary>
    ''' This is a callback method that triggers OnEllipseCenterChanged method of
    ''' the MyEllipse object whose EllipseCenter property got changed.
    ''' </summary>
    ''' <param name="obj"></param>
    ''' <param name="e"></param>
    Shared Sub MyHandler(ByVal obj As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)

        Dim ellipse As MyEllipse = TryCast(obj, MyEllipse)
        If Not ellipse Is Nothing Then
            ellipse.OnEllipseCenterChanged(obj, e)
        End If
    End Sub

    ''' <summary>
    ''' This method is triggered by the callback method of EllipseCenterProperty
    ''' property. It updates the Center property of the EllipseGeometry object to
    ''' update UI,then it fires EllipseCenterChanged event.
    ''' </summary>
    ''' <param name="obj"></param>
    ''' <param name="e"></param>
    Private Sub OnEllipseCenterChanged(ByVal obj As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)
        Dim ellipse As MyEllipse = TryCast(obj, MyEllipse)
        If Not ellipse Is Nothing Then
            ellipse.MyAnimatedEllipseGeometry.Center = ellipse.EllipseCenter
        End If
        If Not _mydelegate Is Nothing Then
            _mydelegate(obj, e)
        End If
    End Sub
End Class
