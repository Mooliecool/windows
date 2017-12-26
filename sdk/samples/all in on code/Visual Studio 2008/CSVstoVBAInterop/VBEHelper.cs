/************************************* Module Header **************************************\
* Module Name:  VBEHelper.cs
* Project:      CSVstoVBAInterop
* Copyright (c) Microsoft Corporation.
* 
* The CSVstoVBAInterop project demonstrates how to interop with VBA project object model in 
* VSTO projects. Including how to programmatically add Macros (or VBA UDF in Excel) into an
* Office document; how to call Macros / VBA UDFs from VSTO code; and how to call VSTO code
* from VBA code. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

#region Using directives
using System;
using Microsoft.Win32;
#endregion


namespace CSVstoVBAInterop
{
    /// <summary>
    /// This static class provides helper methods of VBE related functions to the project.
    /// </summary>
    internal static class VBEHelper
    {
        /// <summary>
        /// Gets or sets whether access to Excel VBA project object model is allowed.
        /// </summary>
        public static bool AccessVBOM
        {
            get
            {
                RegistryKey key = null;
                bool val = false;

                try
                {
                    key = Registry.CurrentUser.OpenSubKey(@"Software\Microsoft\Office\12.0\Excel\Security", false);
                    val = (int)key.GetValue("AccessVBOM", 0) != 0;
                }
                finally
                {
                    if (key != null)
                    {
                        key.Close();
                    }
                }

                return val;
            }

            set
            {
                RegistryKey key = null;

                try
                {
                    key = Registry.CurrentUser.OpenSubKey(@"Software\Microsoft\Office\12.0\Excel\Security", true);
                    key.SetValue("AccessVBOM", value ? 1 : 0);
                }
                finally
                {
                    if (key != null)
                    {
                        key.Close();
                    }
                }
            }
        }
    }
}
