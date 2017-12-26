'****************************** Module Header ******************************'
' Module Name:	MainPage.xaml.vb
' Project:		VBSL3PixelShader
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to use new pixel shader feature in 
' Silverlight 3. It mainly covers two parts:
' 
' 1. How to use built-in Effect such as DropShadowEffect.
' 2. How to create a custom ShaderEffect and use it in the application.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.Windows.Threading
Imports System.Windows.Media.Effects

#End Region


Partial Public Class MainPage
    Inherits UserControl

    ' A timer used to reduce the value of _amplitude gradually.
    Private _timer As DispatcherTimer = New DispatcherTimer()

    ' Init custom effect
    Private _effect As OvalWateryEffect = New OvalWateryEffect(New Uri("/VBSL3PixelShader;component/ovalwatery.ps", UriKind.Relative))

    Public Sub New()
        InitializeComponent()
        Me.ImageWithPixelShader.Effect = _effect
        ' Initialize timer and hook Tick event.
        _timer.Interval = TimeSpan.FromMilliseconds(50)
        AddHandler _timer.Tick, AddressOf _timer_Tick
    End Sub

    ''' <summary>
    ''' This event handler reduce the amplitude on each time tick
    ''' </summary>
    Private Sub _timer_Tick(ByVal sender As Object, ByVal e As EventArgs)
        If Me._effect.Amplitude > 0.0 Then
            Me._effect.Amplitude -= 0.05
        Else
            Me._timer.Stop()
        End If

    End Sub

    ''' <summary>
    ''' This event handler get the current mouse position, assign it to a 
    ''' private field and start the timer to apply new OvalWateryEffect.
    ''' </summary>
    Private Sub UserControl_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        ' Convert the mouse position from control coordinates to texture coordinates as required by the PixelShader
        Me._effect.Center = New Point(e.GetPosition(Me.ImageWithPixelShader).X / Me.ImageWithPixelShader.ActualWidth, e.GetPosition(Me.ImageWithPixelShader).Y / Me.ImageWithPixelShader.ActualHeight)
        Me._effect.Amplitude = 0.5
        _timer.Start()
    End Sub
End Class


''' <summary>
''' OvalWateryEffect class is a custom ShaderEffect class.
''' </summary>
Public Class OvalWateryEffect
    Inherits ShaderEffect

    ''' <summary>
    ''' The following two DependencyProperties are the keys of this custom ShaderEffect.
    ''' They create a bridge between managed code and HLSL(High Level Shader Language).
    ''' The PixelShaderConstantCallback will be triggered when the propery get changed.
    ''' The parameter of the callback represents the register.
    ''' For instance, here the 1 in PixelShaderConstantCallback(1) represents C1 of the
    ''' following HLSL code. In another word, by changing
    ''' the Amplitude property we assign the changed value to the amplitude variable of the
    ''' following HLSL code:
    ''' 
    ''' sampler2D input : register(S0);
    ''' float2 center:register(C0);
    ''' float amplitude:register(C1);
    ''' float4 main(float2 uv : TEXCOORD) : COLOR
    ''' {
    ''' if(pow((uv.x-center.x),2)+pow((uv.y-center.y),2)<0.15)
    ''' {
    ''' uv.y = uv.y  + (sin(uv.y*100)*0.1*amplitude);
    ''' }
    ''' return tex2D( input , uv.xy);
    ''' }
    ''' </summary>
    Public Shared ReadOnly AmplitudeProperty As DependencyProperty = _
    DependencyProperty.Register("Amplitude", GetType(Double), GetType(OvalWateryEffect), New PropertyMetadata(0.1, ShaderEffect.PixelShaderConstantCallback(1)))

    Public Shared ReadOnly CenterProperty As DependencyProperty = _
    DependencyProperty.Register("Center", GetType(Point), GetType(OvalWateryEffect), New PropertyMetadata(New Point(0.5, 0.5), ShaderEffect.PixelShaderConstantCallback(0)))


    Public Sub New(ByVal uri As Uri)
        Dim u As Uri = uri
        Dim psCustom As PixelShader = New PixelShader()
        psCustom.UriSource = u
        PixelShader = psCustom

        MyBase.UpdateShaderValue(CenterProperty)
        MyBase.UpdateShaderValue(AmplitudeProperty)
    End Sub

    Public Property Amplitude() As Double
        Get
            Return CDbl(MyBase.GetValue(AmplitudeProperty))
        End Get
        Set(ByVal value As Double)
            MyBase.SetValue(AmplitudeProperty, value)
        End Set
    End Property

    Public Property Center() As Point
        Get
            Return CType(MyBase.GetValue(CenterProperty), Point)
        End Get
        Set(ByVal value As Point)
            MyBase.SetValue(CenterProperty, value)
        End Set
    End Property

End Class