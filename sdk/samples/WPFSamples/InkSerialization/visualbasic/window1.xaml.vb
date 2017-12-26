Imports Microsoft.Win32
Imports System.Windows.Ink
Imports System.IO

''' <Summary>
''' Interaction logic for Window1.xaml
''' </Summary>
Partial Public Class Window1
    Inherits System.Windows.Window

    ' Create Guids for each of the properties to be added to the 
    ' Strokes collection of the InkCanvas.
    Dim userGuid As New Guid("{ab2503c1-e83c-450c-ae58-3ad2d0819eac}")
    Dim timeStampGuid As New Guid("{a8da70b2-b899-4acd-bb97-1d78e57c24df}")

    Public Sub New()
        InitializeComponent()
        user.Text = Environment.UserName
    End Sub

    ''' <Summary>
    ''' Clear the InkCanvas of all stroke data as well as 
    ''' resetting the user name field and the last saved
    ''' time stamp field.
    ''' </Summary>
    Public Sub Clear(ByVal sender As Object, ByVal args As RoutedEventArgs)
        inkcanvas.Strokes.Clear()
        user.Text = Environment.UserName
        timeStamp.Text = "not saved"
    End Sub

    ''' <Summary>
    ''' Call the .Save method on the InkCanvas' Strokes collection
    ''' to persist the collection of ink as ISF in either compressed 
    ''' or uncompressed format.  In this sample, we also show how to 
    ''' serialize arbitrary data in the ISF by creating extended 
    ''' properties on the Strokes collection of an InkCanvas.
    ''' </Summary>
    Public Sub Save_Click(ByVal sender As Object, ByVal args As RoutedEventArgs)
        Dim dialog As New SaveFileDialog()
        dialog.DefaultExt = "isf"
        dialog.Filter = "ISF Files (*.isf)|*.isf"
        dialog.ShowDialog()
        Try
            If dialog.FileName <> String.Empty Then
                Dim filestream As New FileStream(dialog.FileName, FileMode.Create)
                ' Set the time stamp and update the UI with it...
                Dim theTimeStamp As Date
                theTimeStamp = Now()
                timeStamp.Text = theTimeStamp.ToString()
                ' Add the time stamp and user data as properties of the 
                ' Strokes collection of the InkCanvas for serialization.
                inkcanvas.Strokes.AddPropertyData(userGuid, user.Text)
                inkcanvas.Strokes.AddPropertyData(timeStampGuid, theTimeStamp)
                inkcanvas.Strokes.Save(filestream, compress.IsChecked)
                filestream.Close()
                ' Remove the property data since it's been serialized now.
                inkcanvas.Strokes.RemovePropertyData(userGuid)
                inkcanvas.Strokes.RemovePropertyData(timeStampGuid)
            End If
        Catch exc As Exception
            MessageBox.Show(exc.Message)
        End Try
    End Sub

    ''' <Summary>
    ''' Deserialize ISF previously saved from this or any other 
    ''' application to a temporary stroke collection.  Check the 
    ''' stroke collection's extended properties for data related to
    ''' this application in particular (i.e., user and time stamp)
    ''' and update the relevant UI fields if found.  Finally, set this
    ''' application's inkcanvas.Strokes collection to the temporary
    ''' stroke collection.
    ''' </Summary>
    Public Sub Load_Click(ByVal sender As Object, ByVal args As RoutedEventArgs)
        Dim dialog As New OpenFileDialog()
        dialog.DefaultExt = "isf"
        dialog.Filter = "ISF Files (*.isf)|*.isf"
        dialog.ShowDialog()
        Try
            If dialog.FileName <> String.Empty Then
                Dim filestream As New FileStream(dialog.FileName, FileMode.Open)
                Dim strokes As New StrokeCollection(filestream)
                filestream.Close()
                ' Check for our property data serialized with the Strokes collection.
                If strokes.ContainsPropertyData(userGuid) Then
                    user.Text = strokes.GetPropertyData(userGuid).ToString()
                    strokes.RemovePropertyData(userGuid)
                End If
                If strokes.ContainsPropertyData(timeStampGuid) Then
                    timeStamp.Text = strokes.GetPropertyData(timeStampGuid).ToString()
                    strokes.RemovePropertyData(timeStampGuid)
                End If
                inkcanvas.Strokes = strokes
            End If
        Catch exc As Exception
            MessageBox.Show(exc.Message)
        End Try
    End Sub

End Class
