'****************************** Module Header *******************************
' Module Name:	HighLightTextBlock.vb
' Project:		VBSL3CustomControl
' Copyright (c) Microsoft Corporation.
' 
' Implementation of custom control HighLightTextBlock.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 8/18/2009 8:06 PM Mog Liang Created
'****************************************************************************

Imports System.Windows.Threading
Imports System.ComponentModel

<TemplateVisualState(Name:="NonHighLight", GroupName:="HightLightStates"), TemplateVisualState(Name:="HighLight", GroupName:="HightLightStates")> _
Public Class HighLightTextBlock : Inherits Control

    Public Sub New()
        ' Register custom control's default style
        MyBase.DefaultStyleKey = GetType(HighLightTextBlock)
        Me.InitTimer()
    End Sub

    Private Sub Dehighlight()
        VisualStateManager.GoToState(Me, "NonHighLight", True)
        Me._timer.Stop()
    End Sub

    ' User call this method to highlight text.
    Public Sub Highlight()
        ' Change control state to 'HightLight'
        VisualStateManager.GoToState(Me, "HighLight", True)
        If Me.AutoDehighlight Then
            ' Start timer
            Me._timer.Start()
        End If
    End Sub

    ' Initialize timer
    Private Sub InitTimer()
        Me._timer = New DispatcherTimer
        Me._timer.Interval = TimeSpan.FromSeconds(1)
        ' When time out, change HighLightStates to 'NonHighLight'
        AddHandler Me._timer.Tick, New EventHandler(AddressOf Me.Timer_OnTick)
    End Sub

    Private Sub Timer_OnTick(ByVal sender As Object, ByVal e As EventArgs)
        Dehighlight()
    End Sub

    ' Use this property to (de)highlight the text.
    Private Shared Sub IsHighlightedChanged(ByVal sender As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)
        Dim newvalue As Boolean? = DirectCast(e.NewValue, Boolean?)
        If newvalue.Value Then
            DirectCast(sender, HighLightTextBlock).Highlight()
        Else
            DirectCast(sender, HighLightTextBlock).Dehighlight()
        End If
        DirectCast(sender, HighLightTextBlock).IsHighlighted = DirectCast(e.NewValue, Boolean?)
    End Sub

    Public Overrides Sub OnApplyTemplate()
        MyBase.OnApplyTemplate()
    End Sub


    ' If this property is set to true, the text will automatically be dehilighted after a specific time. Use the LightTime to control the highlight period.
    Private _AutoDehighlight As Boolean
    Public Property AutoDehighlight() As Boolean
        Get
            Return _AutoDehighlight
        End Get
        Set(ByVal value As Boolean)
            _AutoDehighlight = value
        End Set
    End Property

    <TypeConverter(GetType(NullableBoolConverter))> _
    Public Property IsHighlighted() As Boolean?
        Get
            Return DirectCast(MyBase.GetValue(HighLightTextBlock.IsHighlightedProperty), Boolean?)
        End Get
        Set(ByVal value As Boolean?)
            MyBase.SetValue(HighLightTextBlock.IsHighlightedProperty, value)
        End Set
    End Property


    ' Expose LightTime property for highlight time.
    Public Property LightTime() As TimeSpan
        Get
            If (Me._timer Is Nothing) Then
                Me.InitTimer()
            End If
            Return Me._timer.Interval
        End Get
        Set(ByVal value As TimeSpan)
            If (Me._timer Is Nothing) Then
                Me.InitTimer()
            End If
            Me._timer.Interval = value
        End Set
    End Property

    ' In order to use templatebinding on 'Text' property,
    ' it must be dependency property.
    Public Property [Text]() As String
        Get
            Return CStr(MyBase.GetValue(HighLightTextBlock.TextProperty))
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(HighLightTextBlock.TextProperty, value)
        End Set
    End Property


    ' Fields
    Private _timer As DispatcherTimer
    Public Shared ReadOnly IsHighlightedProperty As DependencyProperty = DependencyProperty.Register("DependencyProperty", GetType(Boolean?), GetType(HighLightTextBlock), New PropertyMetadata(False, New PropertyChangedCallback(AddressOf HighLightTextBlock.IsHighlightedChanged)))
    Public Shared ReadOnly TextProperty As DependencyProperty = DependencyProperty.Register("Text", GetType(String), GetType(HighLightTextBlock), Nothing)
End Class

