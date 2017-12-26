'****************************** Module Header ******************************'
' Module Name:  MainWindow.xaml.vb
' Project:      VBWPFClipboardViewer
' Copyright (c) Microsoft Corporation.
' 
' The VBWPFClipboardViewer project provides the sample on how to monitor
' Windows clipboard changes in a WPF application.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
'\***************************************************************************'

Class MainWindow

#Region "Private fields"

    ''' <summary>
    ''' Next clipboard viewer window
    ''' </summary>
    Private hWndNextViewer As IntPtr

    ''' <summary>
    ''' The <see cref="HwndSource"/> for this window.
    ''' </summary>
    Private hWndSource As HwndSource

    Private isViewing As Boolean

#End Region

#Region "Clipboard viewer related methods"

    Private Sub InitCBViewer()
        Dim wih As New WindowInteropHelper(Me)
        hWndSource = HwndSource.FromHwnd(wih.Handle)

        hWndSource.AddHook(AddressOf WinProc)   ' start processing window messages
        hWndNextViewer = Win32.SetClipboardViewer(hWndSource.Handle)    ' set this window as a viewer
        isViewing = True
    End Sub

    Private Sub CloseCBViewer()
        ' remove this window from the clipboard viewer chain
        Win32.ChangeClipboardChain(hWndSource.Handle, hWndNextViewer)

        hWndNextViewer = IntPtr.Zero
        hWndSource.RemoveHook(AddressOf WinProc)
        pnlContent.Children.Clear()
        isViewing = False
    End Sub

    Private Sub DrawContent()
        pnlContent.Children.Clear()

        If Clipboard.ContainsText() Then
            ' we have some text in the clipboard
            Dim tb As New TextBox
            tb.HorizontalScrollBarVisibility = ScrollBarVisibility.Auto
            tb.VerticalScrollBarVisibility = ScrollBarVisibility.Auto
            tb.Text = Clipboard.GetText()
            tb.IsReadOnly = True
            tb.TextWrapping = TextWrapping.NoWrap
            pnlContent.Children.Add(tb)

        ElseIf Clipboard.ContainsFileDropList() Then
            ' we have a file drop list in the clipboard
            Dim lb As New ListBox
            lb.ItemsSource = Clipboard.GetFileDropList()
            pnlContent.Children.Add(lb)

        ElseIf Clipboard.ContainsImage() Then
            ' Because of a known issue in WPF,
            ' we have to use a workaround to get correct
            ' image that can be displayed.
            ' The image have to be saved to a stream and then 
            ' read out to workaround the issue.
            Dim ms As New MemoryStream
            Dim enc As New BmpBitmapEncoder
            enc.Frames.Add(BitmapFrame.Create(Clipboard.GetImage()))
            enc.Save(ms)
            ms.Seek(0, SeekOrigin.Begin)

            Dim dec As New BmpBitmapDecoder(ms, BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.Default)
            Dim img As New Image
            img.Stretch = Stretch.Uniform
            img.Source = dec.Frames(0)
            pnlContent.Children.Add(img)

        Else
            Dim lb As New Label
            lb.Content = "The type of the data in the clipboard is not supported by this sample."
            pnlContent.Children.Add(lb)
        End If
    End Sub

    Private Function WinProc(ByVal hwnd As IntPtr, ByVal msg As Integer, ByVal wParam As IntPtr, ByVal lParam As IntPtr, ByRef handled As Boolean) As IntPtr
        Select Case msg
            Case Win32.WM_CHANGECBCHAIN
                If wParam = hWndNextViewer Then
                    ' clipboard viewer chain changed, need to fix it.
                    hWndNextViewer = lParam

                ElseIf hWndNextViewer <> IntPtr.Zero Then
                    ' pass the message to the next viewer
                    Win32.SendMessage(hWndNextViewer, msg, wParam, lParam)
                End If

            Case Win32.WM_DRAWCLIPBOARD
                ' clipboard content changed
                Me.DrawContent()
                ' pass the message to the next viewer
                Win32.SendMessage(hWndNextViewer, msg, wParam, lParam)

        End Select

        Return IntPtr.Zero
    End Function

#End Region

#Region "Control event handlers"

    Private Sub btnSwitch_Click(ByVal sender As System.Object, ByVal e As System.Windows.RoutedEventArgs)
        ' switching between start/stop viewing state
        If Not isViewing Then
            Me.InitCBViewer()
            btnSwitch.Content = "Stop viewer"

        Else
            Me.CloseCBViewer()
            btnSwitch.Content = "Start viewer"
        End If
    End Sub

    Private Sub btnClose_Click(ByVal sender As System.Object, ByVal e As System.Windows.RoutedEventArgs)
        Me.Close()
    End Sub

    Private Sub Window_Closed(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Me.CloseCBViewer()
    End Sub

#End Region

End Class
