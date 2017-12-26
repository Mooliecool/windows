/****************************** Module Header ******************************\
* Module Name:  IReferenceIdentity.cs
* Project:	    CSCheckEXEType
* Copyright (c) Microsoft Corporation.
* 
* Represents a reference to the unique signature of a code object.
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
    [ComImport]
    [Guid("6eaf5ace-7917-4f3c-b129-e046a9704766")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IReferenceIdentity
    {
        /// <summary>
        /// Get an assembly attribute.
        /// </summary>
        /// <param name="attributeNamespace">Attribute namespace.</param>
        /// <param name="attributeName">Attribute name.</param>
        /// <returns>The assembly attribute.</returns>
        [return: MarshalAs(UnmanagedType.LPWStr)]
        [SecurityCritical]
        string GetAttribute(
            [In, MarshalAs(UnmanagedType.LPWStr)] string attributeNamespace,
            [In, MarshalAs(UnmanagedType.LPWStr)] string attributeName);

        /// <summary>
        /// Set an assembly attribute.
        /// </summary>
        /// <param name="attributeNamespace">Attribute namespace.</param>
        /// <param name="attributeName">Attribute name.</param>
        /// <param name="attributeValue">Attribute value.</param>
        [SecurityCritical]

        void SetAttribute(
            [In, MarshalAs(UnmanagedType.LPWStr)] string attributeNamespace,
            [In, MarshalAs(UnmanagedType.LPWStr)] string attributeName,
            [In, MarshalAs(UnmanagedType.LPWStr)] string attributeValue);

        /// <summary>
        /// Get an iterator for the assembly's attributes.
        /// </summary>
        /// <returns>Assembly attribute enumerator.</returns>
        [SecurityCritical]
        IEnumIDENTITY_ATTRIBUTE EnumAttributes();

        /// <summary>
        /// Clone an IReferenceIdentity.
        /// </summary>
        /// <param name="countOfDeltas">Count of deltas.</param>
        /// <param name="deltas">The deltas.</param>
        /// <returns>Cloned IReferenceIdentity.</returns>
        [SecurityCritical]
        IReferenceIdentity Clone(
            [In] IntPtr countOfDeltas,
            [In, MarshalAs(UnmanagedType.LPArray)] IDENTITY_ATTRIBUTE[] deltas);
    }
}
