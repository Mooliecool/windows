/****************************** Module Header ******************************\
* Module Name:  IOleInPlaceActiveObject.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* This interface is implemented by object applications in order to provide
* support for their objects while they are active in-place.
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
using System.Security;

namespace CSCustomIEContextMenu.NativeMethods
{
    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [SuppressUnmanagedCodeSecurity]
    [Guid("00000117-0000-0000-C000-000000000046")]
    public interface IOleInPlaceActiveObject
    {
        [PreserveSig]
        int GetWindow(out IntPtr hwnd);
        void ContextSensitiveHelp(int fEnterMode);

        [PreserveSig]
        int TranslateAccelerator([In] ref NativeMethods.MSG lpmsg);

        void OnFrameWindowActivate(bool fActivate);

        void OnDocWindowActivate(int fActivate);

        void ResizeBorder(
            [In] NativeMethods.COMRECT prcBorder,
            [In] NativeMethods.IOleInPlaceUIWindow pUIWindow,
            bool fFrameWindow);

        void EnableModeless(int fEnable);
    }
}
