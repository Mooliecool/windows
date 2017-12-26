/****************************** Module Header ******************************\
Module Name:  Program.cs
Project:      CSCallNativeDllWrapper
Copyright (c) Microsoft Corporation.

The code sample demonstrates calling the functions and classes exported by a 
native C++ DLL from Visual C# code through C++/CLI wrapper classes.

  CSCallNativeDllWrapper (this .NET application)
          -->
      CppCLINativeDllWrapper (the C++/CLI wrapper)
              -->
          CppDynamicLinkLibrary (a native C++ DLL module)

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Runtime.InteropServices;
using CppCLINativeDllWrapper;


namespace CSCallNativeDllWrapper
{
    class Program
    {
        static void Main(string[] args)
        {
            bool isLoaded = false;
            const string moduleName = "CppDynamicLinkLibrary";

            // Check whether or not the module is loaded.
            isLoaded = IsModuleLoaded(moduleName);
            Console.WriteLine("Module \"{0}\" is {1}loaded", moduleName, isLoaded ? "" : "not ");

            // Call the functions exported from the module.
            {
                string str = "HelloWorld";
                int length;

                length = NativeMethods.GetStringLength1(str);
                Console.WriteLine("GetStringLength1(\"{0}\") => {1}", str, length);

                length = NativeMethods.GetStringLength2(str);
                Console.WriteLine("GetStringLength2(\"{0}\") => {1}", str, length);
            }

            // Call the callback functions exported from the module.
            {
                CompareCallback cmpFunc = new CompareCallback(CompareInts);
                int max = NativeMethods.Max(2, 3, cmpFunc);

                // Make sure the lifetime of the delegate instance covers the 
                // lifetime of the unmanaged code; otherwise, the delegate 
                // will not be available after it is garbage-collected, and 
                // you may get the Access Violation or Illegal Instruction 
                // error.
                GC.KeepAlive(cmpFunc);
                Console.WriteLine("Max(2, 3) => {0}", max);
            }

            // Use the class exported from the module.
            {
                CSimpleObjectWrapper obj = new CSimpleObjectWrapper();
                obj.FloatProperty = 1.2F;
                float fProp = obj.FloatProperty;
                Console.WriteLine("Class: CSimpleObject::FloatProperty = {0:F2}", fProp);
            }

            // You cannot unload the C++ DLL CppDynamicLinkLibrary by calling 
            // GetModuleHandle and FreeLibrary.

            // Check whether or not the module is loaded.
            isLoaded = IsModuleLoaded(moduleName);
            Console.WriteLine("Module \"{0}\" is {1}loaded", moduleName, isLoaded ? "" : "not ");
        }


        /// <summary>
        /// This is the callback function for the method Max exported from 
        /// the DLL CppDynamicLinkLibrary.dll.
        /// </summary>
        /// <param name="a">the first integer</param>
        /// <param name="b">the second integer</param>
        /// <returns>
        /// The function returns a positive number if a > b, returns 0 if a 
        /// equals b, and returns a negative number if a &lt b.
        /// </returns>
        static int CompareInts(int a, int b)
        {
            return (a - b);
        }


        #region Module Related Helper Functions

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

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static extern IntPtr GetModuleHandle(string moduleName);

        #endregion
    }
}