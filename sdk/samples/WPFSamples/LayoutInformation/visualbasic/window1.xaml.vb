Imports System
Imports System.Windows     
Imports System.Windows.Controls     
Imports System.Windows.Media
Imports System.Windows.Shapes
Imports System.Windows.Controls.Primitives

Namespace SDKSample

    '@ <summary>
    '@ Interaction logic for Window1.xaml
    '@ </summary>

    Partial Public Class Window1
        Inherits Window

        Private Sub getLayoutSlot1(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim myRectangleGeometry As New RectangleGeometry
            myRectangleGeometry.Rect = LayoutInformation.GetLayoutSlot(txt1)
            Dim myGeometryDrawing As New GeometryDrawing
            Dim myPath As New Path
            myPath.Data = myRectangleGeometry
            myPath.Stroke = Brushes.LightGoldenrodYellow
            myPath.StrokeThickness = 5
            Grid.SetColumn(myPath, 0)
            Grid.SetRow(myPath, 0)
            myGrid.Children.Add(myPath)
            txt2.Text = "LayoutSlot is equal to " + LayoutInformation.GetLayoutSlot(txt1).ToString()
        End Sub



    End Class
End Namespace
