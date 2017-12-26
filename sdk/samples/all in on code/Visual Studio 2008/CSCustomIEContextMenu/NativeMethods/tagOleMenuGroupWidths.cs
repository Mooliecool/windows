/****************************** Module Header ******************************\
* Module Name:  tagOleMenuGroupWidths.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* The class tagOleMenuGroupWidths is used by IOleInPlaceFrame.
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
    public sealed class tagOleMenuGroupWidths
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
        public int[] widths = new int[6];
    }
}
