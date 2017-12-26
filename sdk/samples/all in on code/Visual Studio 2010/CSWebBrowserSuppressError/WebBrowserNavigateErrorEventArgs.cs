/****************************** Module Header ******************************\
* Module Name:  WebBrowserNavigateErrorEventArgs.cs
* Project:	    CSWebBrowserSuppressError
* Copyright (c) Microsoft Corporation.
* 
* The class WebBrowserNavigateErrorEventArgs defines the event arguments used
* by WebBrowserEx.NavigateError event.
* 
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

namespace CSWebBrowserSuppressError
{

    public class WebBrowserNavigateErrorEventArgs : EventArgs
    {
        public String Url { get; set; }

        public String Frame { get; set; }

        public Int32 StatusCode { get; set; }

        public Boolean Cancel { get; set; }

        public WebBrowserNavigateErrorEventArgs(String url, String frame,
            Int32 statusCode, Boolean cancel)
        {
            this.Url = url;
            this.Frame = frame;
            this.StatusCode = statusCode;
            this.Cancel = cancel;
        }

    }
}
