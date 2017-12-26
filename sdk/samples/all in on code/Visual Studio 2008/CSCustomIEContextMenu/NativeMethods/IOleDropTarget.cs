/****************************** Module Header ******************************\
* Module Name:  IOleDropTarget.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* This interface enables objects and their containers to dispatch commands to 
* each other. For example, an object's toolbars may contain buttons for 
* commands such as Print, Print Preview, Save, New, and Zoom.
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
    [Guid("00000122-0000-0000-C000-000000000046")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IOleDropTarget
    {
        [PreserveSig]
        int OleDragEnter(
            [In, MarshalAs(UnmanagedType.Interface)] object pDataObj, 
            [In, MarshalAs(UnmanagedType.U4)] int grfKeyState, 
            [In, MarshalAs(UnmanagedType.U8)] long pt, 
            [In, Out] ref int pdwEffect);

        [PreserveSig]
        int OleDragOver(
            [In, MarshalAs(UnmanagedType.U4)] int grfKeyState, 
            [In, MarshalAs(UnmanagedType.U8)] long pt, 
            [In, Out] ref int pdwEffect);

        [PreserveSig]
        int OleDragLeave();
        [PreserveSig]
        int OleDrop(
            [In, MarshalAs(UnmanagedType.Interface)] object pDataObj, 
            [In, MarshalAs(UnmanagedType.U4)] int grfKeyState, 
            [In, MarshalAs(UnmanagedType.U8)] long pt,
            [In, Out] ref int pdwEffect);
    }

 

 

}
