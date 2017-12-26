/****************************** Module Header ******************************\
* Module Name:  IDENTITY_ATTRIBUTE.cs
* Project:	    CSCheckEXEType
* Copyright (c) Microsoft Corporation.
* 
* Assembly attributes. Contains data about an IReferenceIdentity. 
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

namespace CSCheckEXEType.Fusion
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct IDENTITY_ATTRIBUTE
    {
        [MarshalAs(UnmanagedType.LPWStr)]
        public string Namespace;

        [MarshalAs(UnmanagedType.LPWStr)]
        public string Name;

        [MarshalAs(UnmanagedType.LPWStr)]
        public string Value;

    }
}
