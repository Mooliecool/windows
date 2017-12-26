//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET SDK Code Samples.
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
//-----------------------------------------------------------------------
using System;
using Microsoft.Samples.SharedInterface;

namespace Microsoft.Samples.Client
{
	/// <summary>
	/// This is a sample IPC client.
	/// </summary>
	public static class ClientApp
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		public static void Main(string[] args)
		{
			object remoteObject = Activator.GetObject(typeof(ISharedInterface<string>), "ipc://test/stringRemoteObject.rem");
			ISharedInterface<string> stringRemoteObject = remoteObject as ISharedInterface<string>;
			Console.WriteLine(stringRemoteObject.GetType().ToString());
			Console.WriteLine(stringRemoteObject.GetType().IsAssignableFrom(typeof(ISharedInterface<string>)));
			Console.WriteLine(typeof(ISharedInterface<string>).IsAssignableFrom(stringRemoteObject.GetType()));
			Console.WriteLine("String Remote Object responds: " + stringRemoteObject.HelloWorld("Hi Server"));

			Console.WriteLine();

			ISharedInterface<int> intRemoteObject = (ISharedInterface<int>)Activator.GetObject(typeof(ISharedInterface<int>), "ipc://test/intRemoteObject.rem");
			Console.WriteLine("Int Remote Object responds: " + intRemoteObject.HelloWorld(42));

		}
	}
}
