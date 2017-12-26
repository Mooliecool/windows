/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSRegFreeCOMClient
* Copyright (c) Microsoft Corporation.
* 
* Registration-free COM is a mechanism available on the Microsoft Windows XP 
* (SP2 for .NET Framework-based components), Microsoft Windows Server 2003 
* and newer platforms. As the name suggests, the mechanism enables easy (e.g. 
* XCOPY) deployment of COM components to a machine without the need to  
* register them.
* 
* The CSRegFreeCOMClient sample demonstrates how to create a registration-
* free COM from the aspect of .NET Framework based client, so that the client 
* can consume existing COM server as if the COM server is Registration-free.
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
using Microsoft.Win32.SafeHandles;
using System.ComponentModel;

namespace CSRegFreeCOMClient
{
    class Program
    {
        static string manifestPath;

        [STAThread]
        static void Main(string[] args)
        {
            try
            {
                // If the application is run with args, the activation context
                // will be activated by specifying manifest file manually.
                if (args.Length > 0)
                {
                    Console.WriteLine("------ Activate activation context manually ------");

                    Console.WriteLine("Please input the full path of manifest file:");
                    manifestPath = Console.ReadLine();

                    ActivateActivationContext(ConsumeCOMComponent, manifestPath);
                }
                // If the application is run without args, the activation context
                // will be activated by searching the manifest file automatically.
                else
                {
                    Console.WriteLine("------Activate activation context automatically------");

                    ConsumeCOMComponent();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

            Console.WriteLine("Press any key to continue...");
            Console.ReadKey();
        }

        /// <summary>
        /// Consumes the COM component.
        /// </summary>
        static void ConsumeCOMComponent()
        {
            ATLDllCOMServerLib.SimpleObject simpleObj =
                new ATLDllCOMServerLib.SimpleObject();

            try
            {
                // Call the method: HelloWorld, that returns a string.
                {
                    string strResult = simpleObj.HelloWorld();
                    Console.WriteLine("Call HelloWorld => {0}", strResult);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("The server throws the error: {0}", ex.Message);
                if (ex.InnerException != null)
                {
                    Console.WriteLine("Description: {0}", ex.InnerException.Message);
                }
            }

            Marshal.FinalReleaseComObject(simpleObj);
        }

        /// <summary>
        /// Activate activation context according to the given manifest file, 
        /// and the specified action.
        /// </summary>
        /// <param name="actoinToDo">
        /// The method that will be executed if the activation succeed.
        /// </param>
        /// <param name="manifestPath">
        /// The full path of manifest file.
        /// </param>
        static void ActivateActivationContext(Action actoinToDo, string manifestPath)
        {
            ACTCTX ac = new ACTCTX();
            ac.cbSize = Marshal.SizeOf(typeof(ACTCTX));
            ac.lpSource = manifestPath;
            ac.dwFlags = 0;

            IntPtr cookie;
            SafeActCtxHandle hActCtx = NativeMethod.CreateActCtx(ref ac);
            if (!hActCtx.IsInvalid)
            {
                try
                {
                    // Activate the activation context.
                    if (NativeMethod.ActivateActCtx(hActCtx, out cookie))
                    {
                        try
                        {
                            actoinToDo();
                        }
                        finally
                        {
                            // Deactivate the activation context.
                            NativeMethod.DeactivateActCtx(0, cookie);
                        }
                    }
                    else
                    {
                        Console.WriteLine("The ActCtx failed to be activated w/err {0}",
                            Marshal.GetLastWin32Error());
                    }
                }
                finally
                {
                    hActCtx.Dispose();
                }
            }
            else
            {
                Console.WriteLine("The ActCtx failed to be created w/err {0}",
                    Marshal.GetLastWin32Error());
            }
        }
    }

    /// <summary>
    /// The class contains native methods.
    /// </summary>
    class NativeMethod
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        public static extern SafeActCtxHandle CreateActCtx(ref ACTCTX pActCtx);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool ActivateActCtx(SafeActCtxHandle hActCtx, out IntPtr lpCookie);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool DeactivateActCtx(int dwFlags, IntPtr lpCookie);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        public static extern void ReleaseActCtx(IntPtr hActCtx);
    }

    class SafeActCtxHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        private SafeActCtxHandle()
            : base(true)
        {
        }

        public SafeActCtxHandle(IntPtr preexistingHandle, bool ownsHandle)
            : base(ownsHandle)
        {
            base.SetHandle(preexistingHandle);
        }

        protected override bool ReleaseHandle()
        {
            NativeMethod.ReleaseActCtx(base.handle);
            return true;
        }
    }

    /// <summary>
    /// The ACTCTX struct.
    /// </summary>
    [StructLayout(LayoutKind.Sequential, Pack = 4, CharSet = CharSet.Unicode)]
    public struct ACTCTX
    {
        public int cbSize;
        public uint dwFlags;
        public string lpSource;
        public ushort wProcessorArchitecture;
        public Int16 wLangId;
        public string lpAssemblyDirectory;
        public string lpResourceName;
        public string lpApplicationName;
        public IntPtr hModule;
    }
}
