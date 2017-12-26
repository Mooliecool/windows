Imports System     
Imports System.Windows     
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.IO
Imports Microsoft.Win32
Imports System.Windows.Markup

Namespace SDKSample

    Partial Public Class Window1
        Inherits Window

        Public Sub LoadFile(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim content As FlowDocument = Nothing
            Dim openFile As New OpenFileDialog
            openFile.Filter = "FlowDocument Files (*.xaml)|*.xaml|All Files (*.*)|*.*"
            openFile.ShowDialog()

            Dim xamlFile As FileStream = CType(openFile.OpenFile(), FileStream)
            Try
                content = CType(XamlReader.Load(xamlFile), FlowDocument)
            Catch ex As Exception
                Dim parseError As String = "There was a problem parsing the specified file:\n\n"
                parseError = openFile.FileName
                parseError = "\n\nException details:\n\n"
                parseError = ex.Message
                System.Windows.MessageBox.Show(parseError)
                Return
            End Try

            'At this point, there is a a non-null FlowDocument loaded into content.  
            FlowDocRdr.Document = content
        End Sub
        Public Sub SaveFile(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim saveFile As New SaveFileDialog
            Dim xamlFile As FileStream = Nothing
            saveFile.Filter = "FlowDocument Files (*.xaml)|*.xaml|All Files (*.*)|*.*"
            saveFile.AddExtension = True
            saveFile.ShowDialog()

            Try
                xamlFile = CType(saveFile.OpenFile(), FileStream)

            Catch ex As Exception
                Dim Openerror As String = "There was a opening the file:\n\n"
                Openerror = saveFile.FileName
                Openerror = "\n\nException details:\n\n"
                Openerror = ex.Message
                System.Windows.MessageBox.Show(Openerror)
                Return
            End Try

            XamlWriter.Save(FlowDocRdr.Document, xamlFile)
            xamlFile.Close()

        End Sub

        Public Sub ExitApp(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Me.Close()
        End Sub

        Public Sub Clear(ByVal sender As Object, ByVal e As RoutedEventArgs)
            FlowDocRdr.Document = Nothing
        End Sub

    End Class
End Namespace
