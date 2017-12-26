/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSLoadLibrary
* Copyright (c) Microsoft Corporation.
* 
* CSLoadLibrary in C# mimics the behavior of CppLoadLibrary to dynamically 
* load a native DLL (LoadLibrary) get the address of a function in the export  
* table (GetProcAddress, Marshal.GetDelegateForFunctionPointer), and call it. 
* The technology is called Dynamic P/Invoke. It serves as a supplement for 
* the P/Invoke technique and is useful especially when the target DLL is not 
* in the search path of P/Invoke. If you use P/Invoke, CLR will search the 
* dll in your assembly's directory first, then search the dll in directories 
* listed in PATH environment variable. If the dll is not in any of those 
* directories, you have to use the so called Dynamic PInvoke technique that 
* is demonstrated in this code sample. 
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
using System.Runtime.InteropServices;
using System.Text;
#endregion


namespace CSLoadLibrary
{
    class Program
    {
        #region Function Delegates

        // Function delegate of GetStringLength1 exported from the DLL module.
        // The string parameter must be marshaled as LPWStr, otherwise, the
        // string will be passed into the native as an ANSI string that the
        // Unicode API cannot resolve appropriately.
        delegate int GetStringLength1Delegate(
            [MarshalAs(UnmanagedType.LPWStr)] string str);

        // Function delegate of GetStringLength2 exported from the DLL.
        delegate int GetStringLength2Delegate(
            [MarshalAs(UnmanagedType.LPWStr)] string str);

        // Function delegate of the 'PFN_COMPARE' callback function, and the 
        // Max function that requires the callback as one of the arguments.
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        delegate int CompareCallback(int a, int b);
        delegate int MaxDelegate(int a, int b, CompareCallback cmpFunc);

        #endregion


        static void Main(string[] args)
        {
            bool isLoaded = false;
            const string moduleName = "CppDynamicLinkLibrary";

            // Check whether or not the module is loaded.
            isLoaded = IsModuleLoaded(moduleName);
            Console.WriteLine("Module \"{0}\" is {1}loaded", moduleName, 
                isLoaded ? "" : "not ");

            // Load the DLL module.
            Console.WriteLine("Load the library");
            using (UnmanagedLibrary lib = new UnmanagedLibrary(moduleName))
            {
                // Check whether or not the module is loaded.
                isLoaded = IsModuleLoaded(moduleName);
                Console.WriteLine("Module \"{0}\" is {1}loaded", moduleName, 
                    isLoaded ? "" : "not ");

                //
                // Access the global data exported from the module.
                // 

                // The solution does not allow you to access the global data 
                // exported from a DLL module.

                //
                // Call the functions exported from the module.
                //

                string str = "HelloWorld";
                int length;

                // Call int /*__cdecl*/ GetStringLength1(PWSTR pszString);
                GetStringLength1Delegate GetStringLength1 = 
                    lib.GetUnmanagedFunction<GetStringLength1Delegate>(
                    "GetStringLength1");
                if (GetStringLength1 == null)
                {
                    throw new EntryPointNotFoundException(
                        "Unable to find an entry point named 'GetStringLength1'");
                }
                length = GetStringLength1(str);
                Console.WriteLine("GetStringLength1(\"{0}\") => {1}", str, length);

                // Call int __stdcall GetStringLength2(PWSTR pszString);
                GetStringLength2Delegate GetStringLength2 = 
                    lib.GetUnmanagedFunction<GetStringLength2Delegate>(
                    "_GetStringLength2@4");
                if (GetStringLength2 == null)
                {
                    throw new EntryPointNotFoundException(
                        "Unable to find an entry point named 'GetStringLength2'");
                }
                length = GetStringLength2(str);
                Console.WriteLine("GetStringLength2(\"{0}\") => {1}", str, length);

                // 
                // Call the callback functions exported from the module.
                // 

                CompareCallback cmpFunc = new CompareCallback(CompareInts);
                MaxDelegate Max = lib.GetUnmanagedFunction<MaxDelegate>("Max");
                if (Max == null)
                {
                    throw new EntryPointNotFoundException(
                        "Unable to find an entry point named 'Max'");
                }
                int max = Max(2, 3, cmpFunc);
                Console.WriteLine("Function: Max(2, 3) => {0}", max);

                //
                // Use the class exported from a module.
                //

                // The solution does not allow you to use the class exported 
                // from the DLL.

                // Attempt to free the library on exit.
                Console.WriteLine("Unload the dynamically-loaded DLL");

            } // The DLL module should be unloaded here.

            // Check whether or not the module is loaded.
            isLoaded = IsModuleLoaded(moduleName);
            Console.WriteLine("Module \"{0}\" is {1}loaded", moduleName,
                isLoaded ? "" : "not ");
        }


        /// <summary>
        /// This is the callback function for the method Max exported from 
        /// the DLL CppDynamicLinkLibrary.dll
        /// </summary>
        /// <param name="a">the first integer</param>
        /// <param name="b">the second integer</param>
        /// <returns>
        /// The function returns a positive number if a > b, returns 0 if a 
        /// equals b, and returns a negative number if a < b.
        /// </returns>
        static int CompareInts(int a, int b)
        {
            return (a - b);
        }


        #region IsModuleLoaded

        /// <summary>
        /// Check whether or not the specified module is loaded in the 
        /// current process.
        /// </summary>
        /// <param name="moduleName">the module name</param>
        /// <returns>
        /// The function returns true if the specified module is loaded in 
        /// the current process. If the module is not loaded, the function 
        /// returns false.
        /// </returns>
        static bool IsModuleLoaded(string moduleName)
        {
            // Get the module in the process according to the module name.
            IntPtr hMod = GetModuleHandle(moduleName);
            return (hMod != IntPtr.Zero);
        }

        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        static extern IntPtr GetModuleHandle(string moduleName);

        #endregion
    }
}