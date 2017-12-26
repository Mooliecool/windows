/****************************** Module Header ******************************\
 * Module Name:  ManagedProcessUnhandledExceptionOccurredEventArgs.cs
 * Project:      CSCreateMiniDump
 * Copyright (c) Microsoft Corporation.
 * 
 * This ManagedProcessUnhandledExceptionOccurredEventArgs class is used in 
 * ManagedProcess.UnhandledExceptionOccurred event.
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

namespace CSCreateMiniDump
{
    public class ManagedProcessUnhandledExceptionOccurredEventArgs:EventArgs
    {
        public int ProcessID { get; set; }
        public int ThreadID { get; set; }
        public IntPtr ExceptionPointers { get; set; }
    }
}
