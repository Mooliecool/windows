
Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Controls.Primitives
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Imaging
Imports System.Windows.Shapes



'/ <summary>
'/ Interaction logic for Window1.xaml
'/ </summary>

Partial Class Window1
    Inherits System.Windows.Window '

    Private berlinDesc As EditablePopup
    Private budapestDesc As EditablePopup
    Private dresdenDesc As EditablePopup
    Private pragueDesc As EditablePopup


    Public Sub New()
        InitializeComponent()
        AddHandler Me.LocationChanged, AddressOf Window1_LocationChanged

        CreateDescriptions()

    End Sub 'New


    Sub Window1_LocationChanged(ByVal sender As Object, ByVal e As EventArgs)
        If CBool(showDesc.IsChecked) Then
            TogglePopups(False)
            TogglePopups(True)
        End If

    End Sub 'Window1_LocationChanged


    'Create an EditablePopup for each image.
    Sub CreateDescriptions()
        berlinDesc = New EditablePopup()
        berlinDesc.Text = "Title:" & vbTab & "Berlin, Germany" & vbLf _
                        & "Description:" & vbLf & vbTab & "Brandenburg Gate."
        berlinDesc.PlacementTarget = berlinPic

        budapestDesc = New EditablePopup()
        budapestDesc.Text = "Title:" & vbTab & "Budapest, Hungary" & vbLf _
                          & "Description:" & vbLf & vbTab _
                          & "Hungarian Parliament Building and the Danube River"
        budapestDesc.PlacementTarget = budapestPic

        dresdenDesc = New EditablePopup()
        dresdenDesc.Text = "Title:" & vbTab & "Dresden, Germany" & vbLf _
                         & "Description:" & vbLf & vbTab & "Zwinger Palace"
        dresdenDesc.PlacementTarget = dresdenPic

        pragueDesc = New EditablePopup()
        pragueDesc.Text = "Title:" & vbTab & "Prague, Czech Republic" & vbLf _
                        & "Description:" & vbLf & vbTab & "View from the Prague Castle."
        pragueDesc.PlacementTarget = praguePic

    End Sub 'CreateDescriptions



    Sub TogglePopups(ByVal showPopup As Boolean)
        berlinDesc.IsOpen = showPopup
        budapestDesc.IsOpen = showPopup
        dresdenDesc.IsOpen = showPopup
        pragueDesc.IsOpen = showPopup

    End Sub 'TogglePopups


    ' Hide the EditablePopup controls.
    Sub showDesc_Unchecked(ByVal sender As Object, ByVal e As RoutedEventArgs)
        TogglePopups(False)

    End Sub 'showDesc_Unchecked


    ' Show the EditablePopup controls.
    Sub showDesc_Checked(ByVal sender As Object, ByVal e As RoutedEventArgs)
        TogglePopups(True)

    End Sub 'showDesc_Checked
End Class 'Window1 
