/****************************** Module Header ******************************\
 Module Name:  WebBrowserWithProxy.cs
 Project:      CSWebBrowserWithProxy
 Copyright (c) Microsoft Corporation.
 
 This WebBrowserWithProxy class inherits WebBrowser class and has a feature 
 to set proxy server. 

 
 This source is subject to the Microsoft Public License.
 See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 All other rights reserved.
 
 THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;
using System.Security.Permissions;
using System.ComponentModel;

namespace CSWebBrowserWithProxy
{
    public class WebBrowserWithProxy : WebBrowser
    {

        InternetProxy proxy = InternetProxy.NoProxy;

        // The proxy server to connect.
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Browsable(false)]
        public InternetProxy Proxy
        {
            get
            {
                return proxy;
            }
            set
            {
                var newProxy =  InternetProxy.NoProxy;
                if (value != null)
                {
                    newProxy = value;
                }

                if (!proxy.Equals(newProxy))
                {
                    proxy = newProxy;

                    if (proxy != null && !string.IsNullOrEmpty(proxy.Address))
                    {
                        WinINet.SetConnectionProxy(false, proxy.Address);
                    }
                    else
                    {
                        WinINet.RestoreSystemProxy();
                    }
                }
            }
        }

        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        public WebBrowserWithProxy()
        {          
        }      

        /// <summary>
        /// Wrap the method Navigate and set the Proxy-Authorization header if needed.
        /// </summary>
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        public void Goto(string url)
        {
            System.Uri uri = null;
            bool result = System.Uri.TryCreate(url, UriKind.RelativeOrAbsolute, out uri);
            if (!result)
            {
                throw new ArgumentException("The url is not valid. ");
            }

            // If the proxy contains user name and password, then set the Proxy-Authorization
            // header of the request.
            if (Proxy != null && !string.IsNullOrEmpty(Proxy.UserName)
                && !string.IsNullOrEmpty(Proxy.Password))
            {

                // This header uses Base64String to store the credential.
                var credentialStringValue = string.Format("{0}:{1}",
                    Proxy.UserName, Proxy.Password);
                var credentialByteArray = ASCIIEncoding.ASCII.GetBytes(credentialStringValue);
                var credentialBase64String = Convert.ToBase64String(credentialByteArray);
                string authHeader = string.Format("Proxy-Authorization: Basic {0}",
                    credentialBase64String);

                Navigate(uri, string.Empty, null, authHeader);
            }
            else
            {
                Navigate(uri);
            }
        }
    }
}
