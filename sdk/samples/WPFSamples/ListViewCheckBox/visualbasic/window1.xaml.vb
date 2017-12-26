Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Media
Imports System.Windows.Shapes
Imports System.Windows.Controls.Primitives
Imports System.Collections.ObjectModel
Imports System.Xml


    '@ <summary>
    '@ Interaction logic for Window1.xaml
    '@ </summary>

    public partial class Window1 

    Private Sub mySelectionChanged(ByVal Sender As Object, _
                           ByVal e As SelectionChangedEventArgs)
        Dim mySelectedElement As XmlElement = _
                  CType(myPlaylist.SelectedItem, XmlElement)
        NowPlaying.Text = mySelectedElement.GetAttribute("Name").ToString() + _
          " by " + mySelectedElement.GetAttribute("Artist").ToString()

    End Sub
End Class
    
