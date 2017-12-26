/****************************** Module Header ******************************\
* Module Name:  IOleInPlaceFrame.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* This interface is used by object applications to control the display and 
* placement of composite menus, keystroke accelerator translation, 
* context-sensitive help mode, and modeless dialog boxes.
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
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("00000116-0000-0000-C000-000000000046")]
    public interface IOleInPlaceFrame
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

        [PreserveSig]
        int SetActiveObject(
            [In, MarshalAs(UnmanagedType.Interface)] IOleInPlaceActiveObject pActiveObject, 
            [In, MarshalAs(UnmanagedType.LPWStr)] string pszObjName);

        [PreserveSig]
        int InsertMenus(
            [In] IntPtr hmenuShared, 
            [In, Out] NativeMethods.tagOleMenuGroupWidths lpMenuWidths);

        [PreserveSig]
        int SetMenu(
            [In] IntPtr hmenuShared, 
            [In] IntPtr holemenu, 
            [In] IntPtr hwndActiveObject);

        [PreserveSig]
        int RemoveMenus([In] IntPtr hmenuShared);

        [PreserveSig]
        int SetStatusText([In, MarshalAs(UnmanagedType.LPWStr)] string pszStatusText);

        [PreserveSig]
        int EnableModeless(bool fEnable);

        [PreserveSig]
        int TranslateAccelerator(
            [In] ref NativeMethods.MSG lpmsg, 
            [In, MarshalAs(UnmanagedType.U2)] short wID);
    }





}
