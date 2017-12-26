/************************************* Module Header **************************************\
* Module Name:  MainWindow.xaml.cs
* Project:      CSWPFDataBinding
* Copyright (c) Microsoft Corporation.
* 
* The sample demonstrates how to display a series of photos just like a digital
* picuture frame with a "Wipe" effect. 
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 11/23/2009 3:00 PM Linda Liu Created
 * 
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Media.Animation;

namespace CSWPFAnimatedImage
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        int nextImageIndex;
        List<BitmapImage> images = new List<BitmapImage>();

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // initialize the images collection
            images.Add(new BitmapImage(new Uri("Images/image1.jpg", UriKind.Relative)));
            images.Add(new BitmapImage(new Uri("Images/image2.jpg", UriKind.Relative)));
            images.Add(new BitmapImage(new Uri("Images/image3.jpg", UriKind.Relative)));
            images.Add(new BitmapImage(new Uri("Images/image4.jpg", UriKind.Relative)));

            nextImageIndex = 2;
        }
               
        private void VisbleToInvisible_Completed(object sender, EventArgs e)
        {
            // change the source of the myImage1 to the next image to be shown 
            // and increase the nextImageIndex
            this.myImage1.Source = images[nextImageIndex++];

            // if the nextImageIndex exceeds the top bound of the ocllection, 
            // get it to 0 so as to show the first image next time
            if (nextImageIndex == images.Count)
            {
                nextImageIndex = 0;
            }

            // get the InvisibleToVisible storyboard and start it
            Storyboard sb = this.FindResource("InvisibleToVisible") as Storyboard;
            sb.Begin(this);

        }

        private void InvisibleToVisible_Completed(object sender, EventArgs e)
        {
            // change the source of the myImage2 to the next image to be shown
            // and increase the nextImageIndex
            this.myImage2.Source = images[nextImageIndex++];

            // if the nextImageIndex exceeds the top bound of the ocllection, 
            // get it to 0 so as to show the first image next time
            if (nextImageIndex == images.Count)
            {
                nextImageIndex = 0;
            }

            // get the VisibleToInvisible storyboard and start it
            Storyboard sb = this.FindResource("VisibleToInvisible") as Storyboard;
            sb.Begin(this);
        }   

    }
}
