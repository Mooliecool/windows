//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//  This source code is intended only as a supplement to Microsoft
//  Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
// 
//  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.IO;
using System.CodeDom;
using System.CodeDom.Compiler;
using Microsoft.CSharp;
using Microsoft.VisualBasic;

namespace Microsoft.VisualStudio.Samples.CodeDOM.CodeDOMSample
{
	/// <summary>
	/// Generate generic code using CodeDOM API
	/// </summary>
	class GenerateGenericList
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		public static void Main(string[] args)
		{

		//Static variables
		CodeDomProvider provider = null;
		//Target location of generated files
		string path = string.Empty;

			//Check if number of arguments are right
			if (args.Length < 1 || args.Length > 2)
			{
				WriteUsage();
				return;
			}

			//Check for language
			if (args[0] == "VB")
			{
				//Instantiate a Visual Basic Code Provider
				provider = new VBCodeProvider();
			}
			else
				if (args[0] == "C#")
			{
				//Instantiate a C# Code Provider
				provider = new CSharpCodeProvider();
			}
			else
			{
				WriteUsage();
				return;
			}

			//Extract path from command line arguments
			if(args.Length > 1)
			{
				try
				{
					path = Path.GetFullPath(args[1]);
					if (!Directory.Exists(path))
					{
						Console.WriteLine("Path does not exist!");
						return;
					}
				}
				catch (System.Security.SecurityException)
				{
					Console.WriteLine("Write permission to specified path is denied!");
					return;
				}
				catch (System.IO.PathTooLongException)
				{
					Console.WriteLine("Specified path too long!");
					return;
				}
				catch (System.IO.DirectoryNotFoundException)
				{
					Console.WriteLine("Specified path does not exist!");
					return;
				}
			}

			if (path == null || path.Trim().Length == 0)
			{
				path = Environment.CurrentDirectory;
			}

			try
			{
				GenerateList genList = new GenerateList(provider, path);
				genList.GenerateCode();
			}
			catch
			{
				Console.WriteLine("Error generating files");
				return;
			}

			Console.WriteLine("Code generated at " + path);
		}

		//Help message
		private static void WriteUsage()
		{
			Console.WriteLine("---Usage---");
			Console.WriteLine("CodeDomSample [lang] [path (optional)]");
			Console.WriteLine("lang - VB/C# - language in which files need to be generated");
			Console.WriteLine("path - path where files generated need to reside (optional parameter)");
			Console.WriteLine();
		}

		
	}
}