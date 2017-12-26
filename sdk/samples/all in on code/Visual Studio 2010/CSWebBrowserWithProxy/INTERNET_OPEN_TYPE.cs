/****************************** Module Header ******************************\
 Module Name:  INTERNET_OPEN_TYPE.cs
 Project:      CSWebBrowserWithProxy
 Copyright (c) Microsoft Corporation.
 
 This enum contains 4 WinINet constants used in InternetOpen function.
 Visit http://msdn.microsoft.com/en-us/library/aa385096(VS.85).aspx to get the 
 whole constants list.
 
 This source is subject to the Microsoft Public License.
 See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 All other rights reserved.
 
 THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


namespace CSWebBrowserWithProxy
{
    public enum INTERNET_OPEN_TYPE
    {
        /// <summary>
        /// Retrieves the proxy or direct configuration from the registry.
        /// </summary>
        INTERNET_OPEN_TYPE_PRECONFIG = 0,

        /// <summary>
        /// Resolves all host names locally.
        /// </summary>
        INTERNET_OPEN_TYPE_DIRECT = 1,

        /// <summary>
        /// Passes requests to the proxy unless a proxy bypass list is supplied and the name to be resolved bypasses the proxy.
        /// </summary>
        INTERNET_OPEN_TYPE_PROXY=3,

        /// <summary>
        /// Retrieves the proxy or direct configuration from the registry and prevents
        /// the use of a startup Microsoft JScript or Internet Setup (INS) file.
        /// </summary>
        INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY =4
    }
}
