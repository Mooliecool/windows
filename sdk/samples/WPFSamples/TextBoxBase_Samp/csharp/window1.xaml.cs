using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;

namespace SDKSample
{
    public partial class Window1 : Window
    {
        private void initValues(object sender, EventArgs e)
        {
            myTB1.Text= "ExtentHeight is currently " + myTextBox.ExtentHeight.ToString();
            myTB2.Text= "ExtentWidth is currently " + myTextBox.ExtentWidth.ToString();
            myTB3.Text= "HorizontalOffset is currently " + myTextBox.HorizontalOffset.ToString();
            myTB4.Text= "VerticalOffset is currently " + myTextBox.VerticalOffset.ToString();
            myTB5.Text = "ViewportHeight is currently " + myTextBox.ViewportHeight.ToString();
            myTB6.Text = "ViewportWidth is currently " + myTextBox.ViewportWidth.ToString();
            radiobtn1.IsChecked = true;
        }
        private void copyText(object sender, RoutedEventArgs e)
        {
            myTextBox.Copy();
        }
        private void cutText(object sender, RoutedEventArgs e)
        {
            myTextBox.Cut();
        }
        private void pasteSelection(object sender, RoutedEventArgs e)
        {
            myTextBox.Paste();
        }
        private void selectAll(object sender, RoutedEventArgs e)
        {
            myTextBox.SelectAll();
        }
        private void undoAction(object sender, RoutedEventArgs e)
        {
            if (myTextBox.CanUndo == true)
            {
                myTextBox.Undo();
            }
        }
        private void redoAction(object sender, RoutedEventArgs e)
        {
            if (myTextBox.CanRedo == true)
            {
                myTextBox.Redo();
            }
        }
        private void selectChanged(object sender, RoutedEventArgs e)
        {
            myTextBox.AppendText("Selection Changed event in myTextBox2 has just occurred.");
        }
        private void tChanged(object sender, RoutedEventArgs e)
        {
            myTextBox.AppendText("Text content of myTextBox2 has changed.");
        }
        private void wrapOff(object sender, RoutedEventArgs e)
        {
            myTextBox.TextWrapping = TextWrapping.NoWrap;
        }
        private void wrapOn(object sender, RoutedEventArgs e)
        {
            myTextBox.TextWrapping = TextWrapping.Wrap;
        }   
        private void clearTB1(object sender, RoutedEventArgs e)
        {
            myTextBox.Clear();
        }
        private void clearTB2(object sender, RoutedEventArgs e)
        {
            myTextBox2.Clear();
        }
        private void lineDown(object sender, RoutedEventArgs e)
        {
            myTextBox.LineDown();
        }
        private void lineLeft(object sender, RoutedEventArgs e)
        {
            myTextBox.LineLeft();
        }
        private void lineRight(object sender, RoutedEventArgs e)
        {
            myTextBox.LineRight();
        }
        private void lineUp(object sender, RoutedEventArgs e)
        {
            myTextBox.LineUp();
        }
        private void pageDown(object sender, RoutedEventArgs e)
        {
            myTextBox.PageDown();
        }
        private void pageLeft(object sender, RoutedEventArgs e)
        {
            myTextBox.PageLeft();
        }
        private void pageRight(object sender, RoutedEventArgs e)
        {
            myTextBox.PageRight();
        }
        private void pageUp(object sender, RoutedEventArgs e)
        {
            myTextBox.PageUp();
        }
        private void scrollHome(object sender, RoutedEventArgs e)
        {
            myTextBox.ScrollToHome();
        }
        private void scrollEnd(object sender, RoutedEventArgs e)
        {
            myTextBox.ScrollToEnd();
        }
    }
}