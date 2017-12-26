/****************************** Module Header ******************************\
* Module Name:  NativeMethods.cs
* Project:	    CSCheckEXEType
* Copyright (c) Microsoft Corporation.
* 
* This class wraps the CLRCreateInstance method in mscoree.dll. 
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

namespace CSCheckEXEType.Hosting
{
    internal static class NativeMethods
    {
        [DllImport("mscoree.dll", CharSet = CharSet.Auto, SetLastError=true, PreserveSig = false)]
        public static extern void CLRCreateInstance(
            ref Guid clsid, 
            ref Guid riid, 
            [MarshalAs(UnmanagedType.Interface)] out object metahostInterface);

        public static Guid CLSID_CLRMetaHost = 
            new Guid("9280188D-0E8E-4867-B30C-7FA83884E8DE");

        public static Guid IID_ICLRMetaHost = 
            new Guid("D332DB9E-B9B3-4125-8207-A14884F53216");

    }
}
