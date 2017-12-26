/****************************** Module Header ******************************\
* Module Name:  NativeMethods.cs
* Project:	    CSCheckEXEType
* Copyright (c) Microsoft Corporation.
* 
* This class wraps the methods in clr.dll. 
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
using System.Security;

namespace CSCheckEXEType.Fusion
{
    internal static class NativeMethods
    {
        [return: MarshalAs(UnmanagedType.IUnknown)]
        [DllImport("clr.dll", CharSet = CharSet.Auto, SetLastError = true,
            ExactSpelling = true, PreserveSig = false)]
        internal static extern object GetAssemblyIdentityFromFile(
            [In, MarshalAs(UnmanagedType.LPWStr)] string filePath,
            [In] ref Guid riid);

        [return: MarshalAs(UnmanagedType.Interface)]
        [SecurityCritical, DllImport("clr.dll", CharSet = CharSet.Auto,
            SetLastError = true, PreserveSig = false)]
        internal static extern IIdentityAuthority GetIdentityAuthority();

        internal static Guid ReferenceIdentityGuid =
            new Guid("6eaf5ace-7917-4f3c-b129-e046a9704766");

        internal static Guid DefinitionIdentityGuid =
            new Guid("587bf538-4d90-4a3c-9ef1-58a200a8a9e7");

        internal static Guid IdentityAuthorityGuid =
            new Guid("261a6983-c35d-4d0d-aa5b-7867259e77bc");

    }
}
