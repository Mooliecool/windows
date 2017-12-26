
Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Imaging
Imports System.Windows.Shapes
Imports System.Windows.Controls.Primitives


'/ <summary>
'/ Interaction logic for Window1.xaml
'/ </summary>

Class Window1
    Inherits System.Windows.Window '
    Private placementRect As New Rect(50, 150, 60, 90)
    
    
    Public Sub New() 
        InitializeComponent()
        
        popup1.CustomPopupPlacementCallback = New CustomPopupPlacementCallback(AddressOf placePopup)
    
    End Sub 'New
    
    
    
    ' Provide to possible places for the Popup when Placement
    ' is set to Custom.
    Public Function placePopup(ByVal popupSize As Size, ByVal targetSize As Size, ByVal offset As Point) As CustomPopupPlacement() 
        Dim placement1 As New CustomPopupPlacement(New Point(- 50, 100), PopupPrimaryAxis.Vertical)
        
        Dim placement2 As New CustomPopupPlacement(New Point(10, 20), PopupPrimaryAxis.Horizontal)
        
        Dim ttplaces() As CustomPopupPlacement = {placement1, placement2}
        Return ttplaces
    
    End Function 'placePopup
    
    
    ' Set PlacementRectangle and show a Rectangle with the same
    ' dimensions.
    Sub showPlacementRectangle(ByVal sender As Object, ByVal e As RoutedEventArgs) 
        placementRectArea.Visibility = Visibility.Visible
        popup1.PlacementRectangle = placementRect
    
    End Sub 'showPlacementRectangle
    
    
    ' Clear PlacementRectangle and hide the Rectangle
    Sub hidePlacementRectangle(ByVal sender As Object, ByVal e As RoutedEventArgs) 
        placementRectArea.Visibility = Visibility.Hidden
        popup1.PlacementRectangle = Rect.Empty
    
    End Sub 'hidePlacementRectangle
    
    
    ' Set the Placement property of the Popup.
    Sub setPlacement(ByVal sender As Object, ByVal e As RoutedEventArgs) 

        Dim placementChoice As RadioButton

        Try
            placementchoice = CType(e.Source, RadioButton)
        Catch ex As Exception
            Return
        End Try


        Select Case placementChoice.Name
            Case "placementAbsolute"
                popup1.Placement = PlacementMode.Absolute
            Case "placementAbsolutePoint"
                popup1.Placement = PlacementMode.AbsolutePoint
            Case "placementBottom"
                popup1.Placement = PlacementMode.Bottom
            Case "placementCenter"
                popup1.Placement = PlacementMode.Center
            Case "placementCustom"
                popup1.Placement = PlacementMode.Custom
            Case "placementLeft"
                popup1.Placement = PlacementMode.Left
            Case "placementMouse"
                popup1.Placement = PlacementMode.Mouse
            Case "placementMousePoint"
                popup1.Placement = PlacementMode.MousePoint
            Case "placementRelative"
                popup1.Placement = PlacementMode.Relative
            Case "placementRelativePoint"
                popup1.Placement = PlacementMode.RelativePoint
            Case "placementRight"
                popup1.Placement = PlacementMode.Right
            Case "placementTop"
                popup1.Placement = PlacementMode.Top
        End Select
    
    End Sub 'setPlacement
    
    
    ' Reset the offsets of the Popup.
    Sub resetOffsets_Click(ByVal sender As Object, ByVal e As RoutedEventArgs) 
        popup1.HorizontalOffset = 0
        popup1.VerticalOffset = 0
    
    End Sub 'resetOffsets_Click
End Class 'Window1 
