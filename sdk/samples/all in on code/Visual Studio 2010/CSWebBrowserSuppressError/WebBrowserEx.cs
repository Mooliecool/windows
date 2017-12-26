/****************************** Module Header ******************************\
* Module Name:  WebBrowserEx.cs
* Project:	    CSWebBrowserSuppressError
* Copyright (c) Microsoft Corporation.
* 
* This WebBrowserEx class inherits WebBrowser class and supplies following 
* features.
* 1. Disable JIT Debugger.
* 2. Suppress html element errors of document loaded in this browser.
* 3. Handle navigation error.
* 
* The class WebBrowser itself also has a Property ScriptErrorsSuppressed to hides
* all its dialog boxes that originate from the underlying ActiveX control, not 
* just script errors.  
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
using System.Security.Permissions;
using System.Windows.Forms;
using Microsoft.Win32;


namespace CSWebBrowserSuppressError
{
    public partial class WebBrowserEx : WebBrowser
    {
        /// <summary>
        /// Get or Set whether JIT debugger needs to be disabled. You have to restart the 
        /// browser to take effect.
        /// </summary>
        public static bool JITDebuggerDisabled
        {
            get
            {
                using (RegistryKey ieMainKey = Registry.CurrentUser.OpenSubKey(
                    @"Software\Microsoft\Internet Explorer\Main"))
                {
                    string keyvalue = ieMainKey.GetValue("Disable Script Debugger") as string;
                    return string.Equals(keyvalue, "yes", StringComparison.OrdinalIgnoreCase);
                }
            }
            set
            {
                var newValue = value ? "yes" : "no";

                using (RegistryKey ieMainKey = Registry.CurrentUser.OpenSubKey(
                    @"Software\Microsoft\Internet Explorer\Main", true))
                {
                    string keyvalue = ieMainKey.GetValue("Disable Script Debugger") as string;
                    if (!keyvalue.Equals(newValue, StringComparison.OrdinalIgnoreCase))
                    {
                        ieMainKey.SetValue("Disable Script Debugger", newValue);
                    }
                }
            }
        }

        // Suppress html element errors.
        public bool HtmlElementErrorsSuppressed { get; set; }

        AxHost.ConnectionPointCookie cookie;

        WebBrowser2EventHelper helper;

        public event EventHandler<WebBrowserNavigateErrorEventArgs> NavigateError;

        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        public WebBrowserEx()
        {
        }

        /// <summary>
        /// Register the Document.Window.Error event.
        /// </summary>
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        protected override void OnDocumentCompleted(WebBrowserDocumentCompletedEventArgs e)
        {
            base.OnDocumentCompleted(e);

            // Occurs when script running inside of the window encounters a run-time error.
            this.Document.Window.Error += new HtmlElementErrorEventHandler(Window_Error);
        }


        /// <summary>
        /// Handle html element errors of document loaded in this browser. 
        /// If HtmlElementErrorsSuppressed is set to true, then set the 
        /// Handled flag to true sothat browser will not display this error.
        /// </summary>
        protected void Window_Error(object sender, HtmlElementErrorEventArgs e)
        {
            if (HtmlElementErrorsSuppressed)
            {
                e.Handled = true;
            }
        }


        /// <summary>
        /// Associates the underlying ActiveX control with a client that can 
        /// handle control events including NavigateError event.
        /// </summary>
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        protected override void CreateSink()
        {
            base.CreateSink();

            helper = new WebBrowser2EventHelper(this);
            cookie = new AxHost.ConnectionPointCookie(
                this.ActiveXInstance, helper, typeof(DWebBrowserEvents2));
        }


        /// <summary>
        /// Releases the event-handling client attached in the CreateSink method
        /// from the underlying ActiveX control
        /// </summary>
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        protected override void DetachSink()
        {
            if (cookie != null)
            {
                cookie.Disconnect();
                cookie = null;
            }
            base.DetachSink();
        }


        /// <summary>
        ///  Raises the NavigateError event.
        /// </summary>
        protected virtual void OnNavigateError(WebBrowserNavigateErrorEventArgs e)
        {
            if (this.NavigateError != null)
            {
                this.NavigateError(this, e);
            }
        }
    }
}
