/****************************** Module Header ******************************\
 * Module Name:  MiniDumpCreator.cs
 * Project:      CSCreateMiniDump
 * Copyright (c) Microsoft Corporation.
 * 
 * This supplies a static method  CreateMiniDump to create a MiniDump with the specified
 * parameters.
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
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Security.Permissions;
using System.ComponentModel;

namespace CSCreateMiniDump.MiniDump
{
    public class MiniDumpCreator
    {

        /// <summary>
        /// Create a MiniDump with the specified parameters.
        /// The MiniDump file is stored in the same folder as this application.
        /// </summary>
        [PermissionSet(SecurityAction.LinkDemand, Name = "FullTrust")]
        public static string CreateMiniDump(int targetProcessID, int threadID,
            IntPtr exceptionPointers)
        {

            Process targetProcess = Process.GetProcessById(targetProcessID);

            if (targetProcess == null)
            {
                throw new ArgumentException("The specified process does not exist!");
            }

            // Construct this MiniDump file path.
            string dumpFilePath = string.Format("{0}\\{1}_{2}.dmp",
                Environment.CurrentDirectory,
                targetProcess.ProcessName,
                DateTime.Now.ToString("yyyy_MM_dd_HH_mm_ss"));

            using (FileStream fs = new FileStream(dumpFilePath, FileMode.CreateNew))
            {
                IntPtr pExceptionParam = IntPtr.Zero;

                // If exceptionPointers is not null(IntPtr.Zero), then initialize an
                // instance of MINIDUMP_EXCEPTION_INFORMATION.
                if (exceptionPointers != IntPtr.Zero)
                {
                    MINIDUMP_EXCEPTION_INFORMATION mei =
                        new MINIDUMP_EXCEPTION_INFORMATION();
                    mei.ExceptionPointers = exceptionPointers;
                    mei.ThreadId = threadID;
                    mei.ClientPointers = true;

                    // Allocate a block of memory of the MINIDUMP_EXCEPTION_INFORMATION
                    // instance, and then marshal data from a managed object to an 
                    // unmanaged block of memory.
                    pExceptionParam = Marshal.AllocCoTaskMem(Marshal.SizeOf(mei));
                    Marshal.StructureToPtr(mei, pExceptionParam, true);
                }

                // Create the MiniDump with the types MiniDumpWithIndirectlyReferencedMemory 
                // and MiniDumpScanMemory.
                var result = NativeMethods.MiniDumpWriteDump(
                     targetProcess.Handle,
                     targetProcess.Id,
                     fs.SafeFileHandle,
                     MINIDUMP_TYPE.MiniDumpWithIndirectlyReferencedMemory
                     | MINIDUMP_TYPE.MiniDumpScanMemory,
                     pExceptionParam,
                     IntPtr.Zero,
                     IntPtr.Zero
                     );
                if (!result)
                {
                    throw new Win32Exception();
                }
                else
                {
                    return dumpFilePath;
                }
            }
        }

    }
}
