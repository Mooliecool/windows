/****************************** Module Header ******************************\
* Module Name:  MouseSupport.xaml.cs
* Project:      CSSL3Input
* Copyright (c) Microsoft Corporation.
* 
* The MouseSupport UserControl's codebheind file, implemented functions: 
* 
*   Register Mouse event
*   Mouse drag and drop
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/28/2009 5:51 PM Mog Liang Created
* * 7/29/2009 6:30 PM Jialiang Ge Reviewed
\***************************************************************************/

#region Using directives
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
using System.Collections.ObjectModel;
#endregion


namespace CSSL3Input
{
    public partial class MouseSupport : UserControl
    {
        // Use string list for recording mouse event.
        ObservableCollection<string> _eventlist;
        public MouseSupport()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(MouseSupport_Loaded);
        }

        void MouseSupport_Loaded(object sender, RoutedEventArgs e)
        {
            _eventlist = new ObservableCollection<string>();

            // Set string list to listbox as itemssource.
            eventlb1.ItemsSource = _eventlist;
        }

        private void Canvas_MouseEnter(object sender, MouseEventArgs e)
        {
            // ObservableCollection notify any collection changes to listbox, 
            // then listbox will update the view.
            _eventlist.Insert(0, DateTime.Now.ToLongTimeString() +
                ": Mouse entered");
        }

        private void Canvas_MouseLeave(object sender, MouseEventArgs e)
        {
            _eventlist.Insert(0, DateTime.Now.ToLongTimeString() +
                ": Mouse left");
        }

        private void Canvas_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            _eventlist.Insert(0, DateTime.Now.ToLongTimeString() +
                ": Mouse left button down");
        }

        private void Canvas_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _eventlist.Insert(0, DateTime.Now.ToLongTimeString() +
                ": Mouse left button up");
        }

        private void Canvas_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            _eventlist.Insert(0, DateTime.Now.ToLongTimeString() +
                ": Mouse wheel rolled");
        }

        private void Canvas_MouseMove(object sender, MouseEventArgs e)
        {
            positionTB.Text = "Mouse Position:" + e.GetPosition(sender as UIElement);
        }

        // Global variables used to keep track of the mouse position and 
        // whether the object is captured by the mouse.
        bool isMouseCaptured;
        double mouseVerticalPosition;
        double mouseHorizontalPosition;

        private void Button_Loaded(object sender, RoutedEventArgs e)
        {
            ((Button)sender).AddHandler(Button.MouseLeftButtonDownEvent,
                new MouseButtonEventHandler(Button_MouseLeftButtonDown), true);

            ((Button)sender).AddHandler(Button.MouseLeftButtonUpEvent,
                new MouseButtonEventHandler(Button_MouseLeftButtonUp), true);

            ((Button)sender).MouseMove += new MouseEventHandler(Button_MouseMove);
        }

        void Button_MouseMove(object sender, MouseEventArgs e)
        {
            if (isMouseCaptured)
            {
                var item = ((Button)sender);

                // Calculate the current position of the object.
                double deltaV = e.GetPosition(null).Y - mouseVerticalPosition;
                double deltaH = e.GetPosition(null).X - mouseHorizontalPosition;
                double newTop = deltaV + (double)item.GetValue(Canvas.TopProperty);
                double newLeft = deltaH + (double)item.GetValue(Canvas.LeftProperty);

                // Set new position of object.
                item.SetValue(Canvas.TopProperty, newTop);
                item.SetValue(Canvas.LeftProperty, newLeft);

                // Update position global variables.
                mouseVerticalPosition = e.GetPosition(null).Y;
                mouseHorizontalPosition = e.GetPosition(null).X;
            }
        }

        private void Button_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            isMouseCaptured = true;

            // Record mouse start draging position.
            mouseVerticalPosition = e.GetPosition(null).Y;
            mouseHorizontalPosition = e.GetPosition(null).X;

            ((Button)sender).CaptureMouse();
            _eventlist.Insert(0, DateTime.Now.ToLongTimeString() + 
                ": (Button)Mouse left button down");
        }

        private void Button_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            isMouseCaptured = false;
            ((Button)sender).ReleaseMouseCapture();
            _eventlist.Insert(0, DateTime.Now.ToLongTimeString() + 
                ": (Button)Mouse left button up");
        }
    }
}
