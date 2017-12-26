using System;
using System.Windows;
using System.Windows.Controls;

namespace SDKSample
{
    public partial class Window1 : Window
    {
        static Double myDouble1;
        static Double myDouble2;

        public void initVal(object Sender, RoutedEventArgs e)
        {
            myDouble1 = 1;
            myDouble2 = 1;
        }

        public void setCol(object sender, RoutedEventArgs e)
        {
            ListBoxItem lbi = ((sender as ListBox).SelectedItem as ListBoxItem);
            txt3.Text = "Column currently selected is " + lbi.Content.ToString();
            if (lbi.Content.ToString() == "colDef1") {myDouble1 = 1;}
            else if (lbi.Content.ToString() == "colDef2") {myDouble1 = 2;}
            else if (lbi.Content.ToString() == "colDef3") {myDouble1 = 3;}
        }

        public void setRow(object sender, RoutedEventArgs e)
        {
            ListBoxItem lbi2 = ((sender as ListBox).SelectedItem as ListBoxItem);
            txt4.Text = "Row currently selected is " + lbi2.Content.ToString();
            if (lbi2.Content.ToString() == "rowDef1") {myDouble2 = 1;}
            else if (lbi2.Content.ToString() == "rowDef2") {myDouble2 = 2;}
        }

        public void colOneStar(object sender, RoutedEventArgs e) 
		{
            if (myDouble1 == 1)
            {
                colDef1.Width = new GridLength(1, GridUnitType.Star);
                txt1.Text = "colDef1.Width is currently set to " + colDef1.Width.ToString();
            }
            if (myDouble1 == 2)
            {
                colDef2.Width = new GridLength(1, GridUnitType.Star);
                txt1.Text = "colDef2.Width is currently set to " + colDef2.Width.ToString();
            }
            if (myDouble1 == 3)
            {
                colDef3.Width = new GridLength(1, GridUnitType.Star);
                txt1.Text = "colDef3.Width is currently set to " + colDef3.Width.ToString();
            }
        }
        public void colTwoStar(object sender, RoutedEventArgs e)
        {
            if (myDouble1 == 1)
            {
                colDef1.Width = new GridLength(2, GridUnitType.Star);
                txt1.Text = "colDef1.Width is currently set to " + colDef1.Width.ToString();
            }
            if (myDouble1 == 2)
            {
                colDef2.Width = new GridLength(2, GridUnitType.Star);
                txt1.Text = "colDef2.Width is currently set to " + colDef2.Width.ToString();
            }
            if (myDouble1 == 3)
            {
                colDef3.Width = new GridLength(2, GridUnitType.Star);
                txt1.Text = "colDef3.Width is currently set to " + colDef3.Width.ToString();
            }
        }
        public void colThreeStar(object sender, RoutedEventArgs e)
        {
            if (myDouble1 == 1)
            {
                colDef1.Width = new GridLength(3, GridUnitType.Star);
                txt1.Text = "colDef1.Width is currently set to " + colDef1.Width.ToString();
            }
            if (myDouble1 == 2)
            {
                colDef2.Width = new GridLength(3, GridUnitType.Star);
                txt1.Text = "colDef2.Width is currently set to " + colDef2.Width.ToString();
            }
            if (myDouble1 == 3)
            {
                colDef3.Width = new GridLength(3, GridUnitType.Star);
                txt1.Text = "colDef3.Width is currently set to " + colDef3.Width.ToString();
            }
        }
        public void rowOneStar(object sender, RoutedEventArgs e)
        {
            if (myDouble2 == 1)
            {
                rowDef1.Height = new GridLength(1, GridUnitType.Star);
                txt2.Text = "rowDef1.Height is currently set to " + rowDef1.Height.ToString();
            }
            if (myDouble2 == 2)
            {
                rowDef2.Height = new GridLength(1, GridUnitType.Star);
                txt2.Text = "rowDef2.Height is currently set to " + rowDef2.Height.ToString();
            }
        }
        public void rowTwoStar(object sender, RoutedEventArgs e)
        {
            if (myDouble2 == 1)
            {
                rowDef1.Height = new GridLength(2, GridUnitType.Star);
                txt2.Text = "rowDef1.Height is currently set to " + rowDef1.Height.ToString();
            }
            if (myDouble2 == 2)
            {
                rowDef2.Height = new GridLength(2, GridUnitType.Star);
                txt2.Text = "rowDef2.Height is currently set to " + rowDef2.Height.ToString();
            }
        }
        public void rowThreeStar(object sender, RoutedEventArgs e)
        {
            if (myDouble2 == 1)
            {
                rowDef1.Height = new GridLength(3, GridUnitType.Star);
                txt2.Text = "rowDef1.Height is currently set to " + rowDef1.Height.ToString();
            }
            if (myDouble2 == 2)
            {
                rowDef2.Height = new GridLength(3, GridUnitType.Star);
                txt2.Text = "rowDef2.Height is currently set to " + rowDef2.Height.ToString();
            }
        }
        public void resetSample(object sender, RoutedEventArgs e)
        {
            myDouble1 = 1;
            myDouble2 = 1;
            rowDef1.Height = new GridLength(1, GridUnitType.Auto);
            rowDef2.Height = new GridLength(1, GridUnitType.Auto);
            colDef1.Width = new GridLength(1, GridUnitType.Auto);
            colDef2.Width = new GridLength(1, GridUnitType.Auto);
            colDef3.Width = new GridLength(1, GridUnitType.Auto);
            txt1.Text = null;
            txt2.Text = null;
            txt3.Text = null;
            txt4.Text = null;
        }
	}
}