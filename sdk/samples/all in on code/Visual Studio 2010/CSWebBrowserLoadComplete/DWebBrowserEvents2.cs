/****************************** Module Header ******************************\
 * Module Name:  DWebBrowserEvents2.cs
 * Project:      CSWebBrowserLoadComplete
 * Copyright (c) Microsoft Corporation.
 * 
 * The interface DWebBrowserEvents2 designates an event sink interface that an
 * application must implement to receive event notifications from a WebBrowser 
 * control or from the Windows Internet Explorer application. The event 
 * notifications include DocumentComplete event that will be used in this 
 * application.
 * 
 * To get the full event list of DWebBrowserEvents2, see
 * http://msdn.microsoft.com/en-us/library/aa768283(VS.85).aspx
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

namespace CSWebBrowserLoadComplete
{
    [ComImport, TypeLibType(TypeLibTypeFlags.FHidden), 
    InterfaceType(ComInterfaceType.InterfaceIsIDispatch),
    Guid("34A715A0-6587-11D0-924A-0020AFC7AC4D")]
    public interface DWebBrowserEvents2
    {
        /// <summary>
        /// Fires when a document is completely loaded and initialized.
        /// </summary>
        [DispId(259)]
        void DocumentComplete(
            [In, MarshalAs(UnmanagedType.IDispatch)] object pDisp, 
            [In] ref object URL);


        [DispId(250)]
        void BeforeNavigate2(
            [In, MarshalAs(UnmanagedType.IDispatch)] object pDisp,
            [In] ref object URL, 
            [In] ref object flags, 
            [In] ref object targetFrameName, 
            [In] ref object postData, 
            [In] ref object headers,
            [In, Out] ref bool cancel);
    }
}
