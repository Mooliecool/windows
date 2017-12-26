/****************************** Module Header ******************************\
* Module Name:  WebBrowser2EventHelper.cs
* Project:	    CSWebBrowserSuppressError
* Copyright (c) Microsoft Corporation.
* 
* The class WebBrowser2EventHelper is used to handles the NavigateError event 
* from the underlying ActiveX control by raising the NavigateError event 
* defined in class WebBrowserEx. 
* 
* Because of the protected method WebBrowserEx.OnNavigateError, this
* class is defined inside the class WebBrowserEx.
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
using System.Runtime.InteropServices;

namespace CSWebBrowserSuppressError
{
    public partial class WebBrowserEx
    {
        private class WebBrowser2EventHelper : StandardOleMarshalObject, DWebBrowserEvents2
        {
            private WebBrowserEx parent;

            public WebBrowser2EventHelper(WebBrowserEx parent)
            {
                this.parent = parent;
            }

            /// <summary>
            /// Raise the NavigateError event.
            /// If a instance of WebBrowser2EventHelper is associated with the underlying
            /// ActiveX control, this method will be called When NavigateError event was
            /// fired in the ActiveX control.
            /// </summary>
            public void NavigateError(object pDisp, ref object url,
                ref object frame, ref object statusCode, ref bool cancel)
            {
                
                // Raise the NavigateError event in WebBrowserEx class.
                this.parent.OnNavigateError(new WebBrowserNavigateErrorEventArgs(
                    (String)url, (String)frame, (Int32)statusCode, cancel));

            }          
        }
    }
}
