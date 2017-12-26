using System;
using System.Windows;
using System.Windows.Media;

namespace TextDecorationExample
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : Window
    {
         // To use Loaded event put Loaded="WindowLoaded" attribute in root element of .xaml file.
         private void WindowLoaded(object sender, EventArgs e)
         {
             SetUnderline();
             SetStrikeThrough();
             SetOverline();
             SetBaseline();
         }

         private void SetUnderline()
         {
             // Fill the underline decoration with a solid color brush.
             TextDecorationCollection myCollection = new TextDecorationCollection();
             TextDecoration myUnderline = new TextDecoration();
             myUnderline.Location = TextDecorationLocation.Underline;

             // Set the solid color brush.
             myUnderline.Pen = new Pen(Brushes.Red, 1);
             myUnderline.PenThicknessUnit = TextDecorationUnit.FontRecommended;

             // Set the underline decoration to the text block.
             myCollection.Add(myUnderline);
             underlineTextBlock.TextDecorations = myCollection;
         }

         private void SetStrikeThrough()
         {
             // Fill the overline decoration with a solid color brush.
             TextDecorationCollection myCollection = new TextDecorationCollection();
             TextDecoration myStrikeThrough = new TextDecoration();
             myStrikeThrough.Location = TextDecorationLocation.Strikethrough;

             // Set the solid color brush.
             myStrikeThrough.Pen = new Pen(Brushes.Blue, 1);
             myStrikeThrough.PenThicknessUnit = TextDecorationUnit.FontRecommended;

             // Set the underline decoration to the text block.
             myCollection.Add(myStrikeThrough);
             strikethroughTextBlock.TextDecorations = myCollection;
         }

         private void SetOverline()
         {
             // Fill the overline decoration with a linear gradient brush.
             TextDecorationCollection myCollection = new TextDecorationCollection();
             TextDecoration myOverline = new TextDecoration();
             myOverline.Location = TextDecorationLocation.OverLine;

             // Set the linear gradient brush.
             Pen myPen = new Pen();
             myPen.Brush = new LinearGradientBrush(Colors.LimeGreen, Colors.Yellow, 0);
             myPen.Thickness = 3;
             myOverline.Pen = myPen;
             myOverline.PenThicknessUnit = TextDecorationUnit.FontRecommended;

             // Set the overline decoration to the text block.
             myCollection.Add(myOverline);
             overlineTextBlock.TextDecorations = myCollection;
         }

         private void SetBaseline()
         {
             // Fill the baseline decoration with a linear gradient brush.
             TextDecorationCollection myCollection = new TextDecorationCollection();
             TextDecoration myBaseline = new TextDecoration();
             myBaseline.Location = TextDecorationLocation.Baseline;

             // Set the linear gradient brush.
             Pen myPen = new Pen();
             myPen.Brush = new LinearGradientBrush(Colors.Orange, Colors.Red, 0);
             myPen.Thickness = 3;
             myBaseline.Pen = myPen;
             myBaseline.PenThicknessUnit = TextDecorationUnit.FontRecommended;

             // Set the baseline decoration to the text block.
             myCollection.Add(myBaseline);
             baselineTextBlock.TextDecorations = myCollection;
         }

    }
}