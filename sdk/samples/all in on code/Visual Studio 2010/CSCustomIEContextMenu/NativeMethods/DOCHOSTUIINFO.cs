/****************************** Module Header ******************************\
* Module Name:  DOCHOSTUIINFO.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* The class DOCHOSTUIINFO is used by the IDocHostUIHandler::GetHostInfo method 
* to allow MSHTML to retrieve information about the host's UI requirements.
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

using System.Runtime.InteropServices;

namespace CSCustomIEContextMenu.NativeMethods
{
    [StructLayout(LayoutKind.Sequential)]
    [ComVisible(true)]
    public class DOCHOSTUIINFO
    {
        [MarshalAs(UnmanagedType.U4)]
        public int cbSize = Marshal.SizeOf(typeof(NativeMethods.DOCHOSTUIINFO));
        [MarshalAs(UnmanagedType.I4)]
        public int dwFlags;
        [MarshalAs(UnmanagedType.I4)]
        public int dwDoubleClick;
        [MarshalAs(UnmanagedType.I4)]
        public int dwReserved1;
        [MarshalAs(UnmanagedType.I4)]
        public int dwReserved2;
    }
}
