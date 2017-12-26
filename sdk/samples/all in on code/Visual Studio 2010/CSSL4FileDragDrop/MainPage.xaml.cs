/****************************** Module Header ******************************\
* Module Name:                MainPage.xaml.cs
* Project:                    CSSL4FileDragDrop
* Copyright (c) Microsoft Corporation.
* 
* MainPage's code behind file.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.IO;

namespace CSSL4FileDragDrop
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();
        }

        int itemindex = 0;

        // Handle drop event here.
        private void LayoutRoot_Drop(object sender, DragEventArgs e)
        {
            if (e.Data != null)
            {
                // Get fileinfos.
                var files = e.Data.GetData(DataFormats.FileDrop) as FileInfo[];

                foreach (var file in files)
                {
                    var image = new Image();
                    if(IsImage(file.Extension))
                    {
                        try
                        {
                            // Read file stream.
                            var stream = file.OpenRead();
                            var bitmapimage = new BitmapImage();
                            bitmapimage.SetSource(stream);
                            image.Source = bitmapimage;
                        }
                        catch
                        {
                            // If read failed, use default icon .
                            image.Source = new BitmapImage(new Uri("/images/file.png", UriKind.Relative));
                        }
                    }
                    else 
                        // If not image type file, use default icon image.
                        image.Source = new BitmapImage(new Uri("/images/file.png", UriKind.Relative));

                    image.DataContext = file;
                    image.Height = cellsize;
                    image.Width = cellsize;
                    
                    var positon = e.GetPosition(LayoutRoot);
                    LayoutRoot.Children.Add(image);
                    Canvas.SetLeft(image, positon.X);
                    Canvas.SetTop(image, positon.Y);

                    // Init icon drag&drop behavior
                    InitIconDragDrop(image);

                    // Snap to grid
                    FlyToGrid(image, itemindex);

                    itemindex++;
                }
            }
        }

        // Determine if is silverlight supported image format
        bool IsImage(string extension)
        {
            extension = extension.ToLower();
            if (extension == ".jpg" ||
                extension== ".jpeg" ||
                extension == ".png")
                return true;
            else
                return false;
        }

        int cellsize = 100;

        // Snap to grid animation
        void FlyToGrid(UIElement element,int index)
        {
            int cellcolcount = (int)LayoutRoot.ActualWidth/cellsize;
            int row = index / cellcolcount;
            int col = index % cellcolcount;

            int endposx = col * cellsize;
            int endposy = row * cellsize;

            Storyboard sb = new Storyboard();

            DoubleAnimation dax = new DoubleAnimation();
            Storyboard.SetTarget(dax, element);
            Storyboard.SetTargetProperty(dax, new PropertyPath(Canvas.LeftProperty));
            dax.To = endposx;
            dax.Duration = new Duration(TimeSpan.FromSeconds(0.5));
            dax.EasingFunction = new CubicEase { EasingMode = EasingMode.EaseOut };
            sb.Children.Add(dax);

            DoubleAnimation day = new DoubleAnimation();
            Storyboard.SetTarget(day, element);
            Storyboard.SetTargetProperty(day, new PropertyPath(Canvas.TopProperty));
            day.To = endposy;
            day.Duration = new Duration(TimeSpan.FromSeconds(0.5));
            day.EasingFunction = new CubicEase { EasingMode = EasingMode.EaseOut };
            sb.Children.Add(day);

            sb.Begin();
        }

        bool isdrag = false;
        Point relativepos;

        // Initialize element drag&drop behavior
        void InitIconDragDrop(UIElement element)
        {
            element.MouseLeftButtonDown += (s, e) =>
                {
                    isdrag = true;
                    element.CaptureMouse();
                    relativepos = e.GetPosition(element);
                };

            element.MouseLeftButtonUp += (s, e) =>
                {
                    var pos = e.GetPosition(LayoutRoot);
                    var colcount = (int)LayoutRoot.ActualWidth / cellsize;
                    var col = Math.Max(0, Math.Min((int)pos.X / cellsize, colcount - 1));
                    var rowcount = (int)LayoutRoot.ActualHeight/cellsize;
                    var row = Math.Max(0, Math.Min((int)pos.Y / cellsize, rowcount - 1));

                    var index = row * colcount + col;

                    FlyToGrid(element, index);
                    
                    element.ReleaseMouseCapture();
                    isdrag = false;
                };

            element.MouseMove += (s, e) =>
                {
                    if (isdrag)
                    {
                        var pos = e.GetPosition(LayoutRoot);
                        Canvas.SetLeft(element, pos.X - relativepos.X);
                        Canvas.SetTop(element, pos.Y - relativepos.Y);
                    }
                };
        }
    }
}
