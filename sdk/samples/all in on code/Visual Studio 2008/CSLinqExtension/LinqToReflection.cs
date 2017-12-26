/****************************** Module Header ******************************\
* Module Name:    LinqToReflection.cs
* Project:        CSLinqExtension
* Copyright (c) Microsoft Corporation.
*
* It is a simple LINQ to Reflection library to get the methods of the 
* exported class in an assembly grouped by return value type.
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
using System.Reflection;
using System.Windows.Forms;
using System.IO;
#endregion


namespace CSLinqExtension
{
    // Static class LinqToCSV to make extension methods
    public static class LinqToReflection
    {
        /// <summary>
        /// Get the methods of the exported class in an assembly grouped by 
        /// return value type.
        /// </summary>
        /// <param name="assemblyPath">The path of the assembly file</param>
        /// <returns>IEnumerable collection methods grouped by return value type</returns> 
        public static IEnumerable<IGrouping<string, MethodInfo>> 
            GroupMethodsByReturnValue(string assemblyPath)
        {
            // Check if the assembly file exists
            if (File.Exists(assemblyPath))
            {
                // Load the assembly
                Assembly assembly = Assembly.LoadFrom(assemblyPath);

                // BindingFlags to load all the static and instance methods
                BindingFlags staticInstanceAll = BindingFlags.Static | BindingFlags.Instance | BindingFlags.NonPublic | BindingFlags.Public;

                // Get the methods of the exported class grouped by return 
                // value type. 
                var query = from type in assembly.GetExportedTypes()
                            from method in type.GetMethods(staticInstanceAll)
                            where method.DeclaringType == type
                            group method by method.ReturnType.FullName;

                return query;
            }
            else 
            {
                throw new FileNotFoundException();
            }
        }
    }
}
