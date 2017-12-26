//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//
/*=====================================================================
  File:      Invoke.cs

  Summary:   Demonstrates how to use reflection invoke.

  Warning:	This sample shows how to invoke ANY method via reflection on ANY location.
			This can potentially be a security issue.
			When implementing this pattern in your code be sure to take appropriate security measures,
				such as hosting inside an AppDomain with locked down security permission.

=====================================================================*/




using System;
using System.Globalization;
using System.Text;
using System.Threading;
using System.Reflection;
using System.Reflection.Emit;
using System.IO;

namespace Microsoft.Samples
{
	public sealed class App
	{
		private App()
		{
		}

		public static void Main(String[] args)
		{
			if (args.Length < 3)
			{
				Usage();
				return;
			}

			Assembly assembly;
			Type type;

			try
			{
				// Load the requested assembly and get the requested type
				assembly = Assembly.LoadFrom(args[0]);
				type = assembly.GetType(args[1], true);
			}
			catch (FileNotFoundException)
			{
				Console.WriteLine("Could not load Assembly: \"{0}\"", args[0]);
				return;
			}
			catch (TypeLoadException)
			{
				Console.WriteLine("Could not load Type: \"{0}\"\nfrom assembly: \"{1}\"", args[1], args[0]);
				return;
			}

			// Get the methods from the type
			MethodInfo[] methods = type.GetMethods();

			if (methods == null)
			{
				Console.WriteLine("No Matching Types Found");
				return;
			}

			// Make a new array that holds only the args for the call
			String[] newArgs = new String[args.Length - 3];

			if (newArgs.Length != 0)
			{
				Array.Copy(args, 3, newArgs, 0, newArgs.Length);
			}

			// Try each method for a match
			StringBuilder failureExcuses = new StringBuilder();

			foreach (MethodInfo m in methods)
			{
				Object obj = null;

				try
				{
					obj = AttemptMethod(type, m, args[2], newArgs);
				}
				catch (CustomException e)
				{
					failureExcuses.Append(e.Message + "\n");
					continue;
				}

				// If we make it this far without a throw, our job is done!
				Console.WriteLine(obj);
				return;
			}

			Console.WriteLine("Suitable method not found!");
			Console.WriteLine("Here are the reasons:\n" + failureExcuses);
		}

		// Checks a method for a signature match, and invokes it if there is one
		private static Object AttemptMethod(Type type, MethodInfo method, String name, String[] args)
		{
			// Name does not match?
			if (String.Compare(method.Name, name, false, CultureInfo.InvariantCulture) != 0)
			{
				throw new CustomException(method.DeclaringType + "." + method.Name + ": Method Name Doesn't Match!");
			}

			// Wrong number of parameters?
			ParameterInfo[] param = method.GetParameters();

			if (param.Length != args.Length)
			{
				throw new CustomException(method.DeclaringType + "." + method.Name + ": Method Signatures Don't Match!");
			}

			// Ok, can we convert the strings to the right types?
			Object[] newArgs = new Object[args.Length];

			for (int index = 0; index < args.Length; index++)
			{
				try
				{
					newArgs[index] = Convert.ChangeType(args[index], param[index].ParameterType, CultureInfo.InvariantCulture);
				}
				catch (Exception e)
				{
					throw new CustomException(method.DeclaringType + "." + method.Name + ": Argument Conversion Failed", e);
				}				
			}

			// We made it this far, lets see if we need an instance of this type
			Object instance = null;

			if (!method.IsStatic)
			{
				instance = Activator.CreateInstance(type);
			}

			// ok, let's invoke this one!
			return method.Invoke(instance, newArgs);
		}

		// Print usage
		private static void Usage()
		{
			Console.WriteLine("Usage:\n" + "   Invoke [Assembly] [Type] [Method] [Parameters...]");
		}

		class CustomException : Exception
		{
			public CustomException(String m):base(m) { }

			public CustomException(String m, Exception n):base(m,n) { }
		}
	}
}
