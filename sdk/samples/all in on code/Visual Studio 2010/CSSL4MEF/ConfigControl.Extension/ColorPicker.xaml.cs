/****************************** Module Header ******************************\
* Module Name:                ColorPicker.xaml.cs
* Project:                    ConfigControl.Extension
* Copyright (c) Microsoft Corporation.
* 
* ColorPicker's code behind file.
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
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace ConfigControl.Extension
{
    public partial class ColorPicker : UserControl
    {
        /// <summary>
        /// Expose dependencyproperty, represent control's selected color.
        /// </summary>
        public Color SelectedColor
        {
            get { return (Color)GetValue(SelectedColorProperty); }
            set { SetValue(SelectedColorProperty, value); }
        }

        public static readonly DependencyProperty SelectedColorProperty =
            DependencyProperty.Register("SelectedColor", typeof(Color),
            typeof(ColorPicker), null);

        

        public ColorPicker()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(ColorPicker_Loaded);
            LayoutRoot.DataContext = this;
        }

        /// <summary>
        /// Initialize color palette.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void ColorPicker_Loaded(object sender, RoutedEventArgs e)
        {
            var colors = new List<Color>();
            int granularity = 51;
            double colorcellsize = 5;
            double positon = 0;
            for (int r = 0; r <= 255; r += (granularity/2))
            {
                for (int g = 0; g <= 255; g += granularity)
                {
                    for (int b = 0; b <= 255; b += granularity)
                    {
                        var rect = new Rectangle();
                        rect.Width = colorcellsize;
                        rect.Height = colorcellsize;
                        rect.Fill = new SolidColorBrush(
                            Color.FromArgb(255, (byte)r, (byte)g, (byte)b));

                        int vpos = (int)(positon / colorspanel2.Width) * (int)colorcellsize;
                        int hpos = (int)positon%(int)colorspanel2.Width;
                        Canvas.SetLeft(rect, hpos);
                        Canvas.SetTop(rect, vpos);

                        colorspanel2.Children.Add(rect);

                        positon += colorcellsize;
                    }
                }
            }
            colorspanel2.Height = Canvas.GetTop(colorspanel2.Children.Last()) + colorcellsize;
        }

        /// <summary>
        /// Update selected color when user clicked one of the
        /// color rectangle
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void colorspanel2_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            var rect = VisualTreeHelper.FindElementsInHostCoordinates(
                e.GetPosition(Application.Current.RootVisual),
                colorspanel2).FirstOrDefault();

            if (rect != null)
                SelectedColor = ((SolidColorBrush)((Rectangle)rect).Fill).Color;
        }

        /// <summary>
        /// Update current color when mouse move
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void colorspanel2_MouseMove(object sender, MouseEventArgs e)
        {
            var rect = VisualTreeHelper.FindElementsInHostCoordinates(
                e.GetPosition(Application.Current.RootVisual),
                colorspanel2).FirstOrDefault();

            if (rect != null)
                ((SolidColorBrush)curcolorRect.Fill).Color =
                    ((SolidColorBrush)((Rectangle)rect).Fill).Color;
        }

        private void colorspanel2_MouseLeave(object sender, MouseEventArgs e)
        {
            ((SolidColorBrush)curcolorRect.Fill).Color = Colors.Transparent;
        }
    }
}
