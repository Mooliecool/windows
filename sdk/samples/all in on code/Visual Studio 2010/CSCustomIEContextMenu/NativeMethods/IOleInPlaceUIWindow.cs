/****************************** Module Header ******************************\
* Module Name:  IOleInPlaceUIWindow.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* This interface is used by object applications to negotiate border space on
* the document or frame window when one of its objects is being activated,
* or to renegotiate border space if the size of the object changes.
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

namespace CSCustomIEContextMenu.NativeMethods
{
    [ComImport]
    [Guid("00000115-0000-0000-C000-000000000046")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IOleInPlaceUIWindow
    {
        IntPtr GetWindow();

        [PreserveSig]
        int ContextSensitiveHelp(int fEnterMode);

        [PreserveSig]
        int GetBorder([Out] NativeMethods.COMRECT lprectBorder);

        [PreserveSig]
        int RequestBorderSpace([In] NativeMethods.COMRECT pborderwidths);

        [PreserveSig]
        int SetBorderSpace([In] NativeMethods.COMRECT pborderwidths);

        void SetActiveObject(
            [In, MarshalAs(UnmanagedType.Interface)] IOleInPlaceActiveObject pActiveObject,
            [In, MarshalAs(UnmanagedType.LPWStr)] string pszObjName);
    }

 

 

}
