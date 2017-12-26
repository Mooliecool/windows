'****************************** Module Header ******************************'
' Module Name:  MainPage.xaml.vb
' Project:      VBSL3Text
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to work with text in Silverlight using VB.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'


Partial Public Class MainPage
    Inherits UserControl

    Public Sub New()
        InitializeComponent()
    End Sub

    Private Sub UserControl_Loaded(ByVal sender As System.Object, ByVal e As System.Windows.RoutedEventArgs)
        Me.CreateSimpleTextBlock()
        Me.CreateComplexTextBlock()
        Me.FormatText()
    End Sub

    ''' <summary>
    ''' This method creates a simple TextBlock, and adds it to the StackPanel 
    ''' simpleTBPlaceHolder.
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CreateSimpleTextBlock()
        Dim simpleTextBlock = New TextBlock()
        simpleTextBlock.Text = "Simple TextBlock"
        Me.simpleTBPlaceHolder.Children.Add(simpleTextBlock)
    End Sub

    ''' <summary>
    ''' This method creates a TextBlock with multiple Runs and LineBreaks, 
    ''' and adds it to the StackPanel complexTBPlaceHolder.
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CreateComplexTextBlock()
        Dim complexTextBlock = New TextBlock()
        Dim paragraph1 = New Run()
        paragraph1.Text = "Paragraph1"
        Dim lineBreak = New LineBreak()
        Dim paragraph2 = New Run()
        paragraph2.Text = "Paragraph2"
        complexTextBlock.Inlines.Add(paragraph1)
        complexTextBlock.Inlines.Add(lineBreak)
        complexTextBlock.Inlines.Add(paragraph2)
        Me.complexTBPlaceHolder.Children.Add(complexTextBlock)
    End Sub

    ''' <summary>
    ''' This method creates a TextBlock with advanced format, and adds it to 
    ''' the StackPanel customizeFormatPlaceHolder.
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub FormatText()
        Dim formatTextBlock = New TextBlock()
        Dim paragraph1 = New Run()
        paragraph1.Text = "Paragraph1"
        paragraph1.FontFamily = New FontFamily("Magnetob.ttf#Magneto")
        Dim lineBreak = New LineBreak()
        Dim paragraph2 = New Run()
        paragraph2.Text = "Paragraph2"
        Dim brush = New LinearGradientBrush()
        Dim stop1 = New GradientStop()
        stop1.Color = Colors.Blue
        stop1.Offset = 0D
        brush.GradientStops.Add(stop1)
        Dim stop2 = New GradientStop()
        stop2.Color = Colors.Red
        stop2.Offset = 1D
        brush.GradientStops.Add(stop2)
        paragraph2.Foreground = brush
        formatTextBlock.Inlines.Add(paragraph1)
        formatTextBlock.Inlines.Add(lineBreak)
        formatTextBlock.Inlines.Add(paragraph2)
        Me.customizeFormatPlaceHolder.Children.Add(formatTextBlock)
    End Sub

    ''' <summary>
    ''' This method selects all text in the TextBox targetTextBox. Note you 
    ''' have to focus the TextBox so that the selection will be displayed.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub selectTextButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.targetTextBox.SelectAll()
        Me.targetTextBox.Focus()
    End Sub

End Class