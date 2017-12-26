/****************************** Module Header ******************************\
* Module Name:  IEnumIDENTITY_ATTRIBUTE.cs
* Project:	    CSCheckEXEType
* Copyright (c) Microsoft Corporation.
* 
* Serves as an enumerator for the attributes of the code object in the current 
* scope.
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
    [Guid("9cdaae75-246e-4b00-a26d-b9aec137a3eb")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IEnumIDENTITY_ATTRIBUTE
    {
        /// <summary>
        /// Gets the next attributes.
        /// </summary>
        /// <param name="celt">Count of elements.</param>
        /// <param name="attributes">Array of attributes being returned.</param>
        /// <returns>The next attribute.</returns>
        [SecurityCritical]
        uint Next(
            [In] uint celt, 
            [Out, MarshalAs(UnmanagedType.LPArray)] IDENTITY_ATTRIBUTE[] rgAttributes);


        /// <summary>
        /// Copy the current attribute into a buffer.
        /// </summary>
        /// <param name="available">Number of available bytes.</param>
        /// <param name="data">Buffer into which attribute should be written.</param>
        /// <returns>Pointer to buffer containing the attribute.</returns>
        [SecurityCritical]
        IntPtr CurrentIntoBuffer(
            [In] IntPtr Available, 
            [Out, MarshalAs(UnmanagedType.LPArray)] byte[] Data);


        /// <summary>
        /// Skip past a number of elements.
        /// </summary>
        /// <param name="celt">Count of elements to skip.</param>
        [SecurityCritical]
        void Skip([In] uint celt);


        /// <summary>
        /// Reset the enumeration to the beginning.
        /// </summary>
        [SecurityCritical]
        void Reset();


        /// <summary>
        /// Clone this attribute enumeration.
        /// </summary>
        /// <returns>Clone of a IEnumIDENTITY_ATTRIBUTE.</returns>
        [SecurityCritical]
        IEnumIDENTITY_ATTRIBUTE Clone();

    }
}
