'****************************** Module Header ******************************'
' Module Name:              MainPage.xaml.vb
' Project:                  VBSL3Navigation
' Copyright (c) Microsoft Corporation.
' 
' MainPage.xaml code behind file.
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

    ' When Frame navigated, traverse the mainpage's hyperlink
    ' button and highlight current visible page's link button.
    Private Sub Frame_Navigated(ByVal sender As Object, ByVal e As System.Windows.Navigation.NavigationEventArgs)
        For Each ctl In LinkStackPanel.Children
            If TypeOf ctl Is HyperlinkButton Then
                Dim hlb = TryCast(ctl, HyperlinkButton)
                If hlb.NavigateUri.ToString().Equals(e.Uri.ToString()) Then
                    hlb.FontWeight = FontWeights.Bold
                Else
                    hlb.FontWeight = FontWeights.Normal
                End If
            End If
        Next
    End Sub

    ' If frame naviagate failed, popup an error window.
    Private Sub Frame_NavigationFailed(ByVal sender As Object, ByVal e As System.Windows.Navigation.NavigationFailedEventArgs)
        e.Handled = True
        Dim window = New ErrorWindow(e.Exception.Message)
        window.Show()
    End Sub
End Class