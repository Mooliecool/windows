/****************************** Module Header ******************************\
 * Module Name:  WebBrowserEx.cs
 * Project:      CSWebBrowserLoadComplete
 * Copyright (c) Microsoft Corporation.
 * 
 * This WebBrowserEx class inherits WebBrowser class and supplies LoadCompleted 
 * event.
 * 
 * In the case of a page with no frames, DocumentComplete is fired once after 
 * everything is done. In case of multiple frames, DocumentComplete gets fired
 * multiple times. So if the DocumentCompleted event is fired, it does not mean
 * that the page is done loading.
 * 
 * So, to check if a page is done loading, you need to check if the 
 * sender is the same as the the WebBrowser control. 
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
using System.Security.Permissions;
using System.Windows.Forms;

namespace CSWebBrowserLoadComplete
{
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    [PermissionSetAttribute(SecurityAction.InheritanceDemand, Name = "FullTrust")]
    public partial class WebBrowserEx : WebBrowser
    {
        AxHost.ConnectionPointCookie cookie;

        DWebBrowserEvents2Helper helper;

        public event EventHandler<WebBrowserNavigatingEventArgs> StartNavigating;

        public event EventHandler<WebBrowserDocumentCompletedEventArgs> LoadCompleted;

        /// <summary>
        /// Associates the underlying ActiveX control with a client that can 
        /// handle control events including NavigateError event.
        /// </summary>
        protected override void CreateSink()
        {

            base.CreateSink();

            helper = new DWebBrowserEvents2Helper(this);
            cookie = new AxHost.ConnectionPointCookie(
                this.ActiveXInstance, helper, typeof(DWebBrowserEvents2));         
        }

        /// <summary>
        /// Releases the event-handling client attached in the CreateSink method
        /// from the underlying ActiveX control
        /// </summary>
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
        /// Raise the LoadCompleted event.
        /// </summary>
        protected virtual void OnLoadCompleted(WebBrowserDocumentCompletedEventArgs e)
        {
            if (LoadCompleted != null)
            {
                this.LoadCompleted(this, e);
            }
        }

        /// <summary>
        /// Raise the StartNavigating event.
        /// </summary>
        protected virtual void OnStartNavigating(WebBrowserNavigatingEventArgs e)
        {
            if (StartNavigating != null)
            {
                this.StartNavigating(this, e);
            }
        }
    }
}
