/****************************** Module Header ******************************\
* Module Name:  KeyboardSupport.xaml.cs
* Project:      CSSL3Input
* Copyright (c) Microsoft Corporation.
* 
* The KeyboardSupport UserControl's codeb-heind file, implemented keyboard 
* event register function.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/28/2009 5:50 PM Mog Liang Created
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
#endregion


namespace CSSL3Input
{
    public partial class KeyboardSupport : UserControl
    {
        public KeyboardSupport()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(KeyboardSupport_Loaded);
        }

        void KeyboardSupport_Loaded(object sender, RoutedEventArgs e)
        {
            // When usercontrol loaded, set focus to button, then button could catch
            // key event.
            btn1.Focus();
        }

        private void Button_KeyDown(object sender, KeyEventArgs e)
        {
            var button = sender as Button;

            // Determine moving direction.
            double step=5;
            double x = Canvas.GetLeft(button);
            double y = Canvas.GetTop(button);
            switch (e.Key)
            {
                case Key.Left:
                    x -= step;
                    break;
                case Key.Right:
                    x += step;
                    break;
                case Key.Up:
                    y -= step;
                    break;
                case Key.Down:
                    y += step;
                    break;
            }
            Canvas.SetLeft(button,x);
            Canvas.SetTop(button,y);

            // Check if ctrl+z combination is pressed.
            if ((Keyboard.Modifiers & ModifierKeys.Control) == ModifierKeys.Control &&
                e.Key == Key.Z)
            {
                Canvas.SetLeft(button, 0);
                Canvas.SetTop(button, 0);
            }

            button.Content = "Press Key: "+e.Key;
        }

        private void Button_KeyUp(object sender, KeyEventArgs e)
        {
            // Reset button content when key up.
            ((Button)sender).Content = "Press Key";
        }
    }
}
