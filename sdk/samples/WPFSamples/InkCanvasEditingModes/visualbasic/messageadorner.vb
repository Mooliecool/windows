Imports System, _
        System.Globalization, _
        System.Windows, _
        System.Windows.Controls, _
        System.Windows.Data, _
        System.Windows.Documents, _
        System.Windows.Input, _
        System.Windows.Media, _
        System.Windows.Media.Animation
''' <summary>
''' An Adorner which displays animated text message. 
''' </summary>
''' <remarks></remarks>
Public Class MessageAdorner
    Inherits Adorner

    Private _message As FormattedText
    Private _location As Point
    Private ReadOnly AnimationTimeSpan As TimeSpan = New TimeSpan(0, 0, 1)
    Private ReadOnly MessageTypeface As Typeface = New Typeface(SystemFonts.MessageFontFamily, SystemFonts.MessageFontStyle, _
                                                    SystemFonts.MessageFontWeight, FontStretches.Normal)

    ''' <summary>
    ''' Construtor
    ''' </summary>
    ''' <param name="adornedElement"></param>
    ''' <remarks></remarks>
    Public Sub New(ByVal adornedElement As UIElement)
        MyBase.New(adornedElement)
    End Sub

    ''' <summary>
    ''' Show a text message at the specified location
    ''' </summary>
    ''' <param name="message"></param>
    ''' <param name="location"></param>
    ''' <remarks></remarks>
    Public Sub ShowMessage(ByVal message As String, ByVal location As Point)
        ' Format the message text
        _message = New FormattedText(message, CultureInfo.InvariantCulture, _
                   Windows.FlowDirection.LeftToRight, MessageTypeface, 32, Brushes.DarkGray)

        _location = location

        ' Setup an opacity animation
        Dim storyboard As Storyboard = New Storyboard()
        Dim opacityPropertyPath As PropertyPath = New PropertyPath(UIElement.OpacityProperty)

        Dim animationOpacity As DoubleAnimation = New DoubleAnimation(1D, 0D, AnimationTimeSpan)
        animationOpacity.SetValue(storyboard.TargetPropertyProperty, opacityPropertyPath)
        storyboard.Children.Add(animationOpacity)
        BeginStoryboard(storyboard)

        ' Invalidate rendering
        InvalidateVisual()
    End Sub

    ''' <summary>
    ''' The overriden rendering Method
    ''' </summary>
    ''' <param name="drawingContext"></param>
    ''' <remarks></remarks>
    Protected Overrides Sub OnRender(ByVal drawingContext As System.Windows.Media.DrawingContext)
        If _message Is Nothing Then
            ' Nothing to render
            Return
        End If

        ' Draw the text message
        drawingContext.DrawText(_message, _location)
    End Sub
End Class
