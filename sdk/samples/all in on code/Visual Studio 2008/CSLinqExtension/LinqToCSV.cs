/****************************** Module Header ******************************\
* Module Name:    LinqToCSV.cs
* Project:        CSLinqExtension
* Copyright (c) Microsoft Corporation.
*
* It is a simple LINQ to CSV library to use StreamReader to read CSV contents 
* into string arrays line by line. It uses Regular Expression to parse the 
* data based on CSV file standard.
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
using System.IO;
using System.Text.RegularExpressions;
#endregion


namespace CSLinqExtension
{
    // Static class LinqToCSV to make extension methods
    public static class LinqToCSV
    {
        /// <summary>
        /// Extension method of StreamReader to read the CSV file into 
        /// string arrays line by line.
        /// </summary>
        /// <returns>IEnumerable collection of string arrays</returns>
        public static IEnumerable<string[]> Lines(this StreamReader reader)
        {
            if (reader != null)
            {
                string line;

                while ((line = reader.ReadLine()) != null)
                {
                    // Exclude the comment in CSV files
                    if (!line.StartsWith("#"))
                    {
                        // yield return the data of each line
                        yield return LineToArray(line);
                    }
                }
            }
            else
            {
                throw new ArgumentNullException("reader");
            }
        }

        /// <summary>
        /// Use Regular Expression to parse each line of the CSV data.
        /// </summary>
        /// <param name="line">The string of a line of CSV file</param>
        /// <returns>The string array that holds the data</returns>
        private static string[] LineToArray(string line)
        {
            string pattern = ",(?=(?:[^\"]*\"[^\"]*\")*(?![^\"]*\"))";

            Regex regex = new Regex(pattern);

            return regex.Split(line);
        }
    }
}
