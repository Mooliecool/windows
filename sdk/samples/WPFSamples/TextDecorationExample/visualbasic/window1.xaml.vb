Imports System 
Imports System.Windows 
Imports System.Windows.Media 

namespace TextDecorationExample

    '@ <summary>
    '@ Interaction logic for Window1.xaml
    '@ </summary>

    Partial Class Window1

        Inherits Window

        ' To use Loaded event put Loaded="WindowLoaded" attribute in root element of .xaml file.
        Private Sub WindowLoaded(ByVal sender As Object, ByVal args As RoutedEventArgs)

            setUnderline()
            setStrikeThrough()
            setOverline()
            setBaseline()
        End Sub

        Private Sub SetUnderline()

            ' Fill the underline decoration with a solid color brush.
            Dim myCollection As TextDecorationCollection = New TextDecorationCollection()
            Dim myUnderline As TextDecoration = New TextDecoration()
            myUnderline.Location = TextDecorationLocation.Underline

            ' Set the solid color brush.
            myUnderline.Pen = New Pen(Brushes.Red, 1)
            myUnderline.PenThicknessUnit = TextDecorationUnit.FontRecommended

            ' Set the underline decoration to the text block.
            myCollection.Add(myUnderline)
            underlineTextBlock.TextDecorations = myCollection
        End Sub

        Private Sub SetStrikeThrough()

            ' Fill the overline decoration with a solid color brush.
            Dim myCollection As TextDecorationCollection = New TextDecorationCollection()
            Dim myStrikeThrough As TextDecoration = New TextDecoration()
            myStrikeThrough.Location = TextDecorationLocation.Strikethrough

            ' Set the solid color brush.
            myStrikeThrough.Pen = New Pen(Brushes.Blue, 1)
            myStrikeThrough.PenThicknessUnit = TextDecorationUnit.FontRecommended

            ' Set the underline decoration to the text block.
            myCollection.Add(myStrikeThrough)
            strikethroughTextBlock.TextDecorations = myCollection
        End Sub

        Private Sub SetOverline()

            ' Fill the overline decoration with a linear gradient brush.
            Dim myCollection As TextDecorationCollection = New TextDecorationCollection()
            Dim myOverline As TextDecoration = New TextDecoration()
            myOverline.Location = TextDecorationLocation.OverLine

            ' Set the linear gradient brush.
            Dim myPen As Pen = New Pen()
            myPen.Brush = New LinearGradientBrush(Colors.LimeGreen, Colors.Yellow, 0)
            myPen.Thickness = 3
            myOverline.Pen = myPen
            myOverline.PenThicknessUnit = TextDecorationUnit.FontRecommended

            ' Set the overline decoration to the text block.
            myCollection.Add(myOverline)
            overlineTextBlock.TextDecorations = myCollection
        End Sub

        Private Sub SetBaseline()

            ' Fill the baseline decoration with a linear gradient brush.
            Dim myCollection As TextDecorationCollection = New TextDecorationCollection()
            Dim myBaseline As TextDecoration = New TextDecoration()
            myBaseline.Location = TextDecorationLocation.Baseline

            ' Set the linear gradient brush.
            Dim myPen As Pen = New Pen()
            myPen.Brush = New LinearGradientBrush(Colors.Orange, Colors.Red, 0)
            myPen.Thickness = 3
            myBaseline.Pen = myPen
            myBaseline.PenThicknessUnit = TextDecorationUnit.FontRecommended

            ' Set the baseline decoration to the text block.
            myCollection.Add(myBaseline)
            baselineTextBlock.TextDecorations = myCollection
        End Sub

    End Class
End Namespace
