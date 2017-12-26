/****************************** Module Header ******************************\
* Module Name:  NativeMethod.cs
* Project:      CSCOMClient
* Copyright (c) Microsoft Corporation.
* 
* The P/Invoke signatures of some native APIs.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
#endregion


namespace CSCOMClient
{
    internal class NativeMethod
    {
        /// <summary>
        /// Get current thread ID.
        /// </summary>
        [DllImport("kernel32.dll")]
        public static extern uint GetCurrentThreadId();

        /// <summary>
        /// Get current process ID.
        /// </summary>
        [DllImport("kernel32.dll")]
        public static extern uint GetCurrentProcessId();
    }
}