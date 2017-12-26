/****************************** Module Header ******************************\
 * Module Name:  DWebBrowserEvents2Helper.cs
 * Project:      CSWebBrowserLoadComplete
 * Copyright (c) Microsoft Corporation.
 * 
 * The class DWebBrowserEvents2Helper is used to handles the BeforeNavigate2 and 
 * DocumentComplete events from the underlying ActiveX control by raising the 
 * StartNavigating and LoadCompleted events defined in class WebBrowserEx. 
 * 
 * If the WebBrowser control is hosting a normal html page without frame, the 
 * DocumentComplete event is fired once after everything is done.
 *
 * If the WebBrowser control is hosting a frameset. DocumentComplete gets 
 * fired multiple times. The DocumentComplete event has a pDisp parameter, which
 * is the IDispatch of the frame (shdocvw) for which DocumentComplete is fired. 
 *
 * Then we could check if the pDisp parameter of the DocumentComplete is the same
 * as the ActiveXInstance of the WebBrowser.
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
using System.Collections.Generic;
using System.Windows.Forms;

namespace CSWebBrowserLoadComplete
{
    public partial class WebBrowserEx
    {
        private class DWebBrowserEvents2Helper : StandardOleMarshalObject, DWebBrowserEvents2
        {
            private WebBrowserEx parent;

            public DWebBrowserEvents2Helper(WebBrowserEx parent)
            {
                this.parent = parent;
            }

            /// <summary>
            /// Fires when a document is completely loaded and initialized.
            /// If the frame is the top-level window element, then the page is
            /// done loading.
            /// 
            /// Then reset the glpDisp to null after the WebBrowser is done loading.
            /// </summary>
            public void DocumentComplete(object pDisp, ref object URL)
            {
                string url = URL as string;

                if (string.IsNullOrEmpty(url)
                    || url.Equals("about:blank", StringComparison.OrdinalIgnoreCase))
                {
                    return;
                }

                if (pDisp != null && pDisp.Equals(parent.ActiveXInstance))
                {
                    var e = new WebBrowserDocumentCompletedEventArgs(new Uri(url)); 

                    parent.OnLoadCompleted(e);
                }
            }

            /// <summary>
            /// Fires before navigation occurs in the given object 
            /// (on either a window element or a frameset element).
            /// 
            /// </summary>
            public void BeforeNavigate2(object pDisp, ref object URL, ref object flags,
                ref object targetFrameName, ref object postData, ref object headers,
                ref bool cancel)
            {
                string url = URL as string;

                if (string.IsNullOrEmpty(url)
                    || url.Equals("about:blank", StringComparison.OrdinalIgnoreCase))
                {
                    return;
                }

                if (pDisp != null && pDisp.Equals(parent.ActiveXInstance))
                {
                    WebBrowserNavigatingEventArgs e = new WebBrowserNavigatingEventArgs(
                        new Uri(url), targetFrameName as string);

                    parent.OnStartNavigating(e);
                }
            }
        }
    }
}
