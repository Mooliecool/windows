/******************************** Module Header ********************************\
* Module Name:  MainPage.xaml.cs
* Project:      CSSL4SessionCookie
* Copyright (c) Microsoft Corporation.
* 
* This is the primary .cs file defining the behavior for the sample in C# code.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

using System;
using System.Net;
using System.Text;
using System.Windows;
using System.Windows.Controls;


namespace CSSL4SessionCookie
{
    public partial class MainPage : UserControl
    {
        private HttpWebRequest wr;
        private string output;

        public MainPage()
        {
            InitializeComponent();

            string url = Application.Current.Host.Source.OriginalString;

            // In this case, we simply pass one parameter into Silverlight with 
            // no other text, so we retrieve the whole query string after the '?' 
            // to obtain the session ID passed in.
            string session = url.Substring(url.IndexOf("?") + 1);

            // This sample just calls a simple .aspx page that returns only a 
            // message with the session ID. In order to avoid webpage caching, 
            // a time ticks string is appended to the URL query string.
            // The same technique can be used for any web service call, though.
            wr = WebRequest.CreateHttp("http://localhost:7777/VerifySessionMaintained.aspx" + 
                "?" + DateTime.Now.Ticks);

            // Set up a cookie container we will use to store the session cookie.
            wr.CookieContainer = new CookieContainer();

            ///////////////////////////////////////////////////////////////////////
            // Commenting out the following line will demonstrate the session ID //
            //   being lost in the resulting web client request.                 //
            ///////////////////////////////////////////////////////////////////////
            wr.CookieContainer.Add(new Uri("http://localhost:7777"), 
                new Cookie("ASP.NET_SessionID", session));


            wr.BeginGetResponse(new AsyncCallback(Callback), null);          
        }

        private void Callback(IAsyncResult res)
        {
            // Retrieve the response from the server.
            WebResponse wres = wr.EndGetResponse(res);
            byte[] buffer = new byte[wres.ContentLength];
            wres.GetResponseStream().Read(buffer, 0, buffer.Length);
            output = UTF8Encoding.UTF8.GetString(buffer, 0, buffer.Length);

            // Dispatch a call to the UI thread to update the textbox content with our response.
            Dispatcher.BeginInvoke(new Action(UpdateTextbox));
        }

        private void UpdateTextbox()
        {
            SampleTextBlock.Text = output;
        }
    }
}
