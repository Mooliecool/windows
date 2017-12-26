//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Configuration;
using System.Text;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Xml;

namespace Microsoft.ServiceModel.Samples
{

	[ServiceContract]
	public interface ISampleServer
	{
		[OperationContract]
		string Echo(string input);

		[OperationContract]
		string[] BigEcho(string[] input);
	}

	[ServiceBehavior(InstanceContextMode = InstanceContextMode.PerCall)]
	public class SampleServer : ISampleServer
	{
		public string Echo(string input)
		{
			Console.WriteLine("\n\tServer Echo(string input) called:", input);
			Console.WriteLine("\tClient message:\t{0}\n", input);
			return input + " " + input;
		}

		public string[] BigEcho(string[] input)
		{
			Console.WriteLine("\n\tServer BigEcho(string[] input) called:", input);
			Console.WriteLine("\t{0} client messages", input.Length);
			return input;
		}
	}

	static class Program
	{
		static void Main()
		{
			using (ServiceHost sampleServer = new ServiceHost(typeof(SampleServer)))
			{
				sampleServer.Open();
				Console.WriteLine("\nPress Enter key to Exit.");
				Console.ReadLine();

				sampleServer.Close();
			}
		}
	}
}
