/****************************** Module Header ******************************\
Module Name:  MainPage.xaml.cs
Project:      CSSL3HTMLBridge
Copyright (c) Microsoft Corporation.

HTML bridge samples code behind file.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Browser;
using System.Runtime.Serialization;
using System.ComponentModel;

namespace CSSL3HTMLBridge
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();

            // Register Scriptable Object.
            HtmlPage.RegisterScriptableObject("silverlightPage", this);

            // Attach Html Element event.
            HtmlPage.Document.GetElementById("Text2").AttachEvent("onkeyup", new EventHandler(HtmlKeyUp));

            tb1.TextChanged += new TextChangedEventHandler(tb1_TextChanged);
            tb4.TextChanged += tb4_TextChanged;
        }

        // Call javascript when first textbox text changed.
        private void tb1_TextChanged(object sender, TextChangedEventArgs e)
        {
            HtmlPage.Window.Invoke("changetext", tb1.Text);
        }

        // Handle html textbox keyup event.
        private void HtmlKeyUp(object sender, EventArgs e)
        {
            var value = ((HtmlElement)sender).GetProperty("value");
            tb2.Text = value.ToString();
        }

        // Create method for javascript.
        [ScriptableMember]
        public void ChangeTB3Text(string text)
        {
            tb3.Text = text;
        }

        // Create event allow registering by javascript.
        [ScriptableMember]
        public event EventHandler<TextEventArgs> TextChanged;

        void tb4_TextChanged(object sender, TextChangedEventArgs e)
        {
            var myargs = new TextEventArgs();
            myargs.Text = tb4.Text;
            if (TextChanged != null)
            {
                TextChanged(this, new TextEventArgs { Text = tb4.Text });
            }
        }
    }

    // Create custom Eventargs containing text property.
    public class TextEventArgs : EventArgs
    {
        [ScriptableMember]
        public string Text { set; get; }
    }
}