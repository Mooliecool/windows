/****************************** Module Header ******************************\
* Module Name:  MyFunctions.cs
* Project:      CSExcelAutomationAddIn
* Copyright (c) Microsoft Corporation.
* 
* The CSExcelAutomationAddIn project is a class library project written in C#.
* It illustrates how to write a managed COM component which can be used as an
* Automation AddIn in Excel. The Automation AddIn can provide user defined 
* functions for Excel. 
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
using System.Linq;
using System.Text;
using Microsoft.Win32;
using System.Runtime.InteropServices;
using Excel = Microsoft.Office.Interop.Excel;
#endregion


namespace CSExcelAutomationAddIn
{
    [Guid("7127696E-AB87-427a-BC85-AB3CBA301CF3")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    [ComVisible(true)]
    public class MyFunctions
    {
        #region User Defined Functions

        public double AddNumbers(double num1, [Optional] object num2, 
            [Optional] object num3)
        {
            double result = num1;

            if (!(num2 is System.Reflection.Missing))
            {
                Excel.Range r2 = num2 as Excel.Range;
                result += Convert.ToDouble(r2.Value2);
            }

            if (!(num3 is System.Reflection.Missing))
            {
                Excel.Range r3 = num3 as Excel.Range;
                result += Convert.ToDouble(r3.Value2);
            }

            return result;
        }

        public double NumberOfCells(object range)
        {
            Excel.Range r = range as Excel.Range;
            return r.Cells.Count;
        }

        #endregion


        #region Registration of Automation Add-in

        [ComRegisterFunction]
        public static void RegisterFunction(Type type)
        {
            // Add the "Programmable" registry key under CLSID
            Registry.ClassesRoot.CreateSubKey(
                GetCLSIDSubKeyName(type, "Programmable"));

            // Register the full path to mscoree.dll which makes Excel happier.
            RegistryKey key = Registry.ClassesRoot.OpenSubKey(
                GetCLSIDSubKeyName(type, "InprocServer32"), true);
            key.SetValue("", 
                System.Environment.SystemDirectory + @"\mscoree.dll",
                RegistryValueKind.String);
        }

        [ComUnregisterFunction]
        public static void UnregisterFunction(Type type)
        {
            // Remove the "Programmable" registry key under CLSID
            Registry.ClassesRoot.DeleteSubKey(
                GetCLSIDSubKeyName(type, "Programmable"), false);
        }

        private static string GetCLSIDSubKeyName(Type type, string subKeyName)
        {
            System.Text.StringBuilder s = new System.Text.StringBuilder();
            s.Append(@"CLSID\{");
            s.Append(type.GUID.ToString().ToUpper());
            s.Append(@"}\");
            s.Append(subKeyName);
            return s.ToString();
        }

        #endregion
    }
}
