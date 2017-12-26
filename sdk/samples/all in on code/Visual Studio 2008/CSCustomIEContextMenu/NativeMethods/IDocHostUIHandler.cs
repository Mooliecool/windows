/****************************** Module Header ******************************\
* Module Name:  IDocHostUIHandler.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* The interface IDocHostUIHandler enables an application that is hosting the 
* WebBrowser Control or automating Windows Internet Explorer to replace the 
* menus, toolbars, and context menus used by MSHTML.
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
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

namespace CSCustomIEContextMenu.NativeMethods
{
    [ComImport]
    [Guid("BD3F23C0-D43E-11CF-893B-00AA00BDCE1A")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [ComVisible(true)]
    public interface IDocHostUIHandler
    {
        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int ShowContextMenu(
            [In, MarshalAs(UnmanagedType.U4)] int dwID, 
            [In] NativeMethods.POINT pt, 
            [In, MarshalAs(UnmanagedType.Interface)] object pcmdtReserved,
            [In, MarshalAs(UnmanagedType.Interface)] object pdispReserved);
        
        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int GetHostInfo([In, Out] NativeMethods.DOCHOSTUIINFO info);
        
        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int ShowUI(
            [In, MarshalAs(UnmanagedType.I4)] int dwID, 
            [In] NativeMethods.IOleInPlaceActiveObject activeObject, 
            [In] NativeMethods.IOleCommandTarget commandTarget,
            [In] NativeMethods.IOleInPlaceFrame frame, 
            [In] NativeMethods.IOleInPlaceUIWindow doc);
        
        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int HideUI();
       
        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int UpdateUI();
       
        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int EnableModeless([In, MarshalAs(UnmanagedType.Bool)] bool fEnable);
        
        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int OnDocWindowActivate([In, MarshalAs(UnmanagedType.Bool)] bool fActivate);
        
        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int OnFrameWindowActivate([In, MarshalAs(UnmanagedType.Bool)] bool fActivate);
       
        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int ResizeBorder(
            [In] NativeMethods.COMRECT rect, 
            [In] NativeMethods.IOleInPlaceUIWindow doc, 
            bool fFrameWindow);

        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int TranslateAccelerator(
            [In] ref NativeMethods.MSG msg, 
            [In] ref Guid group, 
            [In, MarshalAs(UnmanagedType.I4)] int nCmdID);

        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int GetOptionKeyPath(
            [Out, MarshalAs(UnmanagedType.LPArray)] string[] pbstrKey, 
            [In, MarshalAs(UnmanagedType.U4)] int dw);

        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int GetDropTarget(
            [In, MarshalAs(UnmanagedType.Interface)] IOleDropTarget pDropTarget, 
            [MarshalAs(UnmanagedType.Interface)] out IOleDropTarget ppDropTarget);

        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int GetExternal([MarshalAs(UnmanagedType.Interface)] out object ppDispatch);

        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int TranslateUrl(
            [In, MarshalAs(UnmanagedType.U4)] int dwTranslate, 
            [In, MarshalAs(UnmanagedType.LPWStr)] string strURLIn,
            [MarshalAs(UnmanagedType.LPWStr)] out string pstrURLOut);

        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int FilterDataObject(IDataObject pDO, out IDataObject ppDORet);
    }
}
