' DocViewerXps SDK Sample - Window1.xaml.vb
' Copyright (c) Microsoft Corporation. All rights reserved.

' ----------------------------- Class Window1 ----------------------------
' <summary>
' Demonstrates the opening of an XPS document with DocumentViewer. </summary>

Partial Public Class Window1
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()

        ' Add the Open Command
        AddCommandBindings(ApplicationCommands.Open, AddressOf OpenCommandHandler)
        ' Add the Close Command
        AddCommandBindings(ApplicationCommands.Close, AddressOf CloseCommandHandler)
    End Sub


    ' -------------------------- OpenCommandHandler ----------------------------
    ' <summary>
    '   Opens an existing XPS document and displays
    '   it with a DocumentViewer.</summary>
    ' <param name="sender"></param>
    ' <param name="e"></param>
    '
    Private Sub OpenCommandHandler(ByVal sender As Object, ByVal e As ExecutedRoutedEventArgs)
        ' Display a file open dialog to find and existing document
        Dim dlg As Forms.OpenFileDialog = New Forms.OpenFileDialog()
        dlg.Filter = "Xps Documents (*.xps)|*.xps"
        dlg.FilterIndex = 1
        If dlg.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
            If Not IsNothing(_xpsDocument) Then
                _xpsDocument.Close()
            End If

            Try
                _xpsDocument = New XpsDocument(dlg.FileName, System.IO.FileAccess.Read)
            Catch ex As UnauthorizedAccessException
                System.Windows.MessageBox.Show(String.Format("Unable to access {0}", dlg.FileName))
                Return
            End Try

            docViewer.Document = _xpsDocument.GetFixedDocumentSequence()
            _fileName = dlg.FileName
        End If
    End Sub 'OpenCommandHandler()


    ' ------------------------ CloseCommandHandler -----------------------
    ' <summary>
    '   File|Close handler - Closes current XPS document.</summary>
    ' <param name="sender"></param>
    ' <param name="e"></param>
    '
    Private Sub CloseCommandHandler(ByVal sender As Object, ByVal e As ExecutedRoutedEventArgs)
        Close()
    End Sub 'CloseCommandHandler()


    ' ------------------------ AddCommandBindings ------------------------
    ' <summary>
    '   Registers menu commands (helper method).</summary>
    ' <param name="command"></param>
    ' <param name="handler"></param>
    '
    Private Sub AddCommandBindings(ByVal command As ICommand, ByVal handler As ExecutedRoutedEventHandler)
        Dim cmdBindings As CommandBinding = New CommandBinding(command)
        AddHandler cmdBindings.Executed, handler
        CommandBindings.Add(cmdBindings)
    End Sub 'AddCommandBindings()

    Dim _xpsDocument As XpsDocument
    Dim _fileName As String
    
End Class
