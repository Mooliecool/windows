'***************************** Module Header ******************************\
'* Module Name:  MainPage.xaml.vb
'* Project:      VBSL3FullScreen
'* Copyright (c) Microsoft Corporation.
'* 
'* This example illustrates how to use the full screen feature of Silverlight 3 and
'* what the keyboard limitation is in full screen mode.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 7/14/2009 02:00 PM Allen Chen Created
'\**************************************************************************


Partial Public Class MainPage
    Inherits UserControl
    Public Sub New()
        InitializeComponent()

        ' Attach the Loaded event to hook up events on load stage.    
        AddHandler Loaded, AddressOf MainPage_Loaded

    End Sub

    Private Sub MainPage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Attach events of SilverlightHost to subscribe the 
        ' FullScreenChanged and Resized event.
        AddHandler App.Current.Host.Content.FullScreenChanged, AddressOf Content_FullScreenChanged
        AddHandler App.Current.Host.Content.Resized, AddressOf Content_Resized
    End Sub

    Private Sub Content_Resized(ByVal sender As Object, ByVal e As EventArgs)
        ' When content get resized, refresh TextBlockShowSize control to
        ' show the size of the Silverlight plug-in.           
        RefreshTextBlockShowSize()
    End Sub

    Private Sub Content_FullScreenChanged(ByVal sender As Object, ByVal e As EventArgs)
        ' When full screen mode changed, refresh TextBlockShowSize 
        ' control to show the size of the Silverlight plug-in.
        RefreshTextBlockShowSize()
    End Sub
    Private Sub RefreshTextBlockShowSize()
        ' Show the size of the Silverlight plug-in on TextBlockShowSize 
        ' control.
        Me.TextBlockShowSize.Text = String.Format("{0}*{1}", App.Current.Host.Content.ActualWidth, App.Current.Host.Content.ActualHeight)
    End Sub
    Private Sub UserControl_KeyDown(ByVal sender As Object, ByVal e As KeyEventArgs)
        ' Show the input key on TextBlockShowKeyboardInput control.
        Me.TextBlockShowKeyboardInput.Text = e.Key.ToString()
    End Sub

    Private Sub Button_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Switch to full screen mode or embeded mode.
        App.Current.Host.Content.IsFullScreen = Not App.Current.Host.Content.IsFullScreen
    End Sub
End Class