// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Shapes;
using System.Windows.Navigation;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Controls.Primitives;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Media3D;
using System.Windows.Media.Imaging;
using System.Windows.Media.TextFormatting;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Windows.Interop;


namespace Microsoft.Samples.MSBuildG
{

    // Summary:
    // Resources class for the "Vista" theme
    
    public partial class Resources_Vista
    {
        public Resources_Vista()
        {
            this.Add("MaxConverter", new MaxBindingConverter());
            
            InitializeComponent();
        }

        private void CloseDialogWindow(object sender, RoutedEventArgs e)
        {
            //Get the button
            Button senderButton = sender as Button;

            //Get the parent window this button belongs to.
            Window parentWindow = senderButton.TemplatedParent as Window;

            //Close the window by sending a "Cancel" result.
            parentWindow.DialogResult = null;
        }

        private void CloseWindow(object sender, RoutedEventArgs e)
        {
            Button senderButton = sender as Button;

            Window parentWindow = senderButton.TemplatedParent as Window;

            parentWindow.Close();
        }

        private void MaximizeWindow(object sender, RoutedEventArgs e)
        {
            Button senderButton = sender as Button;

            Window parentWindow = senderButton.TemplatedParent as Window;

            if (parentWindow.WindowState == WindowState.Normal)
            {
                parentWindow.WindowState = WindowState.Maximized;
            }
            else
            {
                parentWindow.WindowState = WindowState.Normal;
            }
        }

        private void MinimizeWindow(object sender, RoutedEventArgs e)
        {
            Button senderButton = sender as Button;

            Window parentWindow = senderButton.TemplatedParent as Window;

            parentWindow.WindowState = WindowState.Minimized;
        }

        private void NavigateBack(object sender, RoutedEventArgs e)
        {
            Button senderButton = sender as Button;

            WizardWindow parentWindow = senderButton.TemplatedParent as WizardWindow;

            parentWindow.NavigateBack();
        }

        private void NavigateNext(object sender, RoutedEventArgs e)
        {
            Button senderButton = sender as Button;

            WizardWindow parentWindow = senderButton.TemplatedParent as WizardWindow;

            parentWindow.NavigateNext();
        }

        //API functions to move windows
        private const int WM_NCLBUTTONDOWN = 0xA1;
        private const int HTCAPTION = 0x2;
        [DllImportAttribute("user32.dll")]
        private static extern bool ReleaseCapture();
        [DllImportAttribute("user32.dll")]
        private static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);


        private void MouseDown(object sender, RoutedEventArgs e)
        {
            TextBlock senderBlock = sender as TextBlock;

            Window parentWindow = senderBlock.TemplatedParent as Window;

            //PickerWindow has its own move code for demonstration purposes
            if (parentWindow.Name != "PickerWindow")
            {
                WindowInteropHelper wih = new WindowInteropHelper(parentWindow);
                IntPtr handle = wih.Handle;

                ReleaseCapture();
                SendMessage(handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            }
        }
    }

    // Summary:
    // Converter from an IconData to an ImageSource (in the form of a BitmapImage)
    public class IconBindingConverter : IValueConverter
    {
        public IconBindingConverter()
        {
        }

        public object ConvertBack(object value, System.Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return null;
        }

        public object Convert(object value, System.Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;

            Uri iconFile = new System.Uri(@"pack://application:,,,/" + (string)value, UriKind.Absolute);
            IconBitmapDecoder dec = new IconBitmapDecoder(
                    iconFile, 
                    BitmapCreateOptions.None,
                    BitmapCacheOption.Default);
            return dec.Frames[0];
        }
    }


    // Summary:
    // Converter from a ResizeMode to a boolean for the IsEnabled of the Window Maximize button.
    // If the ResizeMode is "NoResize", then the value returned is "false". Otherwise, "true".
    
    public class MaxBindingConverter : IValueConverter
    {
        public MaxBindingConverter()
        {
        }

        public object ConvertBack(object value, System.Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {

            if (value == null)
            {
                throw new ArgumentNullException("value");
            }

            bool oldValue = (bool)value;

            if (oldValue)
            {
                return (ResizeMode.CanResize);
            }

            return (ResizeMode.NoResize);
        }

        public object Convert(object value, System.Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
            {
                throw new ArgumentNullException("value");
            }


            ResizeMode rm = (ResizeMode)value;

            bool rtnValue = true;

            if (rm == ResizeMode.NoResize)
            {
                rtnValue = false;
            }

            return (rtnValue);
        }
    }
}