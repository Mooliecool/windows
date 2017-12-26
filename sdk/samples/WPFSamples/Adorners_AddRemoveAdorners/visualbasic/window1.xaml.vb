Imports System     
Imports System.Windows     
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Media

Namespace SDKSample

    '@ <summary>
    '@ Interaction logic for Window1.xaml
    '@ </summary>

    Partial Public Class Window1
        Inherits Window

        Dim alSingle As System.Windows.Documents.AdornerLayer
        Dim alChildren As System.Windows.Documents.AdornerLayer


        Public Sub WindowLoaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
            alSingle = AdornerLayer.GetAdornerLayer(loneTextBox)
            alChildren = AdornerLayer.GetAdornerLayer(elementsGrid)

        End Sub

        Public Sub ClickAdornMe(ByVal sender As Object, ByVal e As RoutedEventArgs)
            alSingle.Add(New SimpleCircleAdorner(loneTextBox))
            btUnAdornMe.IsEnabled = True
        End Sub
        Public Sub ClickUnAdornMe(ByVal sender As Object, ByVal e As RoutedEventArgs)
            'GetAdorners returns an array of adorners in the adorner layer.
            'In this case, since we know we've only added a single adorner to the
            'layer, call the Remove method on the first (and only) adorner in
            'the array returned by GetAdorners.
            'alSingle.Remove((alSingle.GetAdorners(loneTextBox))[0])
            alSingle.Remove((alSingle.GetAdorners(loneTextBox))(0))
            btUnAdornMe.IsEnabled = False

        End Sub
        Public Sub ClickAdornUs(ByVal sender As Object, ByVal e As RoutedEventArgs)
            For Each toAdorn As UIElement In elementsGrid.Children
                alChildren.Add(New SimpleCircleAdorner(toAdorn))
            Next
            btUnAdornUs.IsEnabled = True

        End Sub

        Public Sub ClickUnAdornUs(ByVal sender As Object, ByVal e As RoutedEventArgs)
            For Each toUnAdorn As UIElement In elementsGrid.Children
                alChildren.Remove((alChildren.GetAdorners(toUnAdorn))(0))
            Next
            btUnAdornUs.IsEnabled = False
        End Sub

    End Class

    Public Class SimpleCircleAdorner
        Inherits Adorner
        Sub New(ByVal adornedElement As UIElement)
            MyBase.New(adornedElement)
        End Sub
        'A common way to implement an adorner's rendering behavior is to override the OnRender
        'method, which is called by the layout subsystem as part of a rendering pass.
        'adorned element.
        Protected Overrides Sub OnRender(ByVal drawingContext As System.Windows.Media.DrawingContext)
            MyBase.OnRender(drawingContext)
            Dim adornedElementRect As New Rect(AdornedElement.DesiredSize)
            Dim renderBrush As New SolidColorBrush(Colors.Green)
            renderBrush.Opacity = 0.2
            Dim renderPen As New Pen(New SolidColorBrush(Colors.Navy), 1.5)
            Dim renderRadius As Double
            renderRadius = 5.0

            'Just draw a circle at each corner.
            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.TopLeft, renderRadius, renderRadius)
            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.TopRight, renderRadius, renderRadius)
            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.BottomLeft, renderRadius, renderRadius)
            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.BottomRight, renderRadius, renderRadius)
        End Sub
    End Class

End Namespace
