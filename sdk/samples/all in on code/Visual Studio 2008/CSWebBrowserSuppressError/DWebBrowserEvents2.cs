/****************************** Module Header ******************************\
* Module Name:  DWebBrowserEvents2.cs
* Project:	    CSWebBrowserSuppressError
* Copyright (c) Microsoft Corporation.
* 
* The interface DWebBrowserEvents2 designates an event sink interface that an
* application must implement to receive event notifications from a WebBrowser 
* control or from the Windows Internet Explorer application. The event 
* notifications include NavigateError event that will be used in this 
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

namespace CSWebBrowserSuppressError
{
    [ComImport, TypeLibType(TypeLibTypeFlags.FHidden), 
    InterfaceType(ComInterfaceType.InterfaceIsIDispatch),
    Guid("34A715A0-6587-11D0-924A-0020AFC7AC4D")]
    public interface DWebBrowserEvents2
    {      
        [DispId(271)]
        void NavigateError(
            [In, MarshalAs(UnmanagedType.IDispatch)] object pDisp,
            [In] ref object url, 
            [In] ref object frame,
            [In] ref object statusCode,
            [In, Out] ref bool cancel);        
    }
}
