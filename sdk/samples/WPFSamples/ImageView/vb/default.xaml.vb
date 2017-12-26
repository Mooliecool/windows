 'This is a list of commonly used namespaces for a window.
Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Media.Imaging
Imports System.IO
Imports System.Collections

Namespace ImageView

    '/ <summary>
    '/ Interaction logic for Window1.xaml
    '/ </summary>

    Partial Class ImageViewExample
        Inherits Window '
        Private imageFiles As ArrayList

        Public Sub New()
            InitializeComponent()

        End Sub 'New


        Private Sub WindowLoaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
            imageFiles = GetImageFileInfo()
            imageListBox.DataContext = imageFiles

        End Sub 'WindowLoaded


        Private Sub showImage(ByVal sender As Object, ByVal args As SelectionChangedEventArgs)
            Dim list As ListBox = CType(sender, ListBox)
            If Not (list Is Nothing) Then
                Dim index As Integer = list.SelectedIndex 'Save the selected index 
                If index >= 0 Then
                    Dim selection As String = list.SelectedItem.ToString()

                    If Not (selection Is Nothing) AndAlso selection.Length <> 0 Then
                        'Set currentImage to selected Image
                        Dim selLoc As New Uri(selection)
                        Dim id As New BitmapImage(selLoc)
                        Dim currFileInfo As New FileInfo(selection)
                        currentImage.Source = id

                        'Setup Info Text
                        imageSize.Text = id.PixelWidth.ToString() + " x " + id.PixelHeight.ToString()
                        imageFormat.Text = id.Format.ToString()
                        fileSize.Text = ((currFileInfo.Length + 512) / 1024).ToString() + "k"
                    End If
                End If
            End If

        End Sub 'showImage


        Private Function GetImageFileInfo() As ArrayList
            Dim imageFiles As New ArrayList()
            Dim files() As String

            'Get directory path of myData (down two directory levels)
            Dim currDir As String = Directory.GetCurrentDirectory()
            Dim temp As String = currDir + "\..\..\myData"
            files = Directory.GetFiles(temp, "*.jpg")

            Dim image As String
            For Each image In files
                Dim info As New FileInfo(image)
                imageFiles.Add(info)
            Next image

            'imageFiles.Sort();
            Return imageFiles

        End Function 'GetImageFileInfo
    End Class 'ImageViewExample
End Namespace 'ImageView
