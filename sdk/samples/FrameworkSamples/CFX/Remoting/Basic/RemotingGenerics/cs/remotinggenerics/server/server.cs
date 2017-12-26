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
using System.Runtime.Remoting;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Ipc;
using Microsoft.Samples.Implementation;

namespace Microsoft.Samples.Server
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	class Class1
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main(string[] args)
		{
			RemotingConfiguration.RegisterWellKnownServiceType(typeof(ImplementationClass<string>), "StringRemoteObject.rem", WellKnownObjectMode.SingleCall);
			RemotingConfiguration.RegisterWellKnownServiceType(typeof(ImplementationClass<int>), "IntRemoteObject.rem", WellKnownObjectMode.SingleCall);

			IpcChannel channel = new IpcChannel("test");
			ChannelServices.RegisterChannel(channel, false /*ensureSecurity*/);

			Console.WriteLine("Waiting for connections...");
			Console.WriteLine("Press enter to exit.");
			Console.ReadLine();
		}
	}
}
