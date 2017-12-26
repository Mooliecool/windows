/****************************** Module Header ******************************\ 
Module Name:    Program.cs 
Project:        CSEnumStringConverter
Copyright (c) Microsoft Corporation. 

This sample demonstrates how to convert enum to comma separated string & vice
versa. It also covers the description attribute for enum while conversion. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
\***************************************************************************/

using System;
using System.ComponentModel;


namespace CSEnumStringConverter
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Using EnumConverter to");
            {
                EnumConverter converter = new EnumConverter(typeof(ProgrammingLanguage));

                // Convert string to enum.
                string langStr = "CS, Cpp, XAML";
                Console.WriteLine("Convert the string \"{0}\" to enum...", langStr);
                ProgrammingLanguage lang = (ProgrammingLanguage)converter.ConvertFromString(langStr);
                Console.WriteLine("Done!");

                // Convert enum to string.
                Console.WriteLine("Convert the resulting enum to string...");
                langStr = converter.ConvertToString(lang);
                Console.WriteLine("Done! \"{0}\"", langStr);
            }

            Console.WriteLine("\nUsing EnumDescriptionConverter to");
            {
                EnumDescriptionConverter converter = new EnumDescriptionConverter(
                    typeof(ProgrammingLanguage));

                // Convert string to enum.
                string langStr = "Visual C#, Visual C++, XAML";
                Console.WriteLine("Convert the string \"{0}\" to enum...", langStr);
                ProgrammingLanguage lang = (ProgrammingLanguage)converter.ConvertFromString(langStr);
                Console.WriteLine("Done!");

                // Convert enum to string.
                Console.WriteLine("Convert the resulting enum to string...");
                langStr = converter.ConvertToString(lang);
                Console.WriteLine("Done! \"{0}\"", langStr);
            }

            Console.ReadLine();
        }
    }
}