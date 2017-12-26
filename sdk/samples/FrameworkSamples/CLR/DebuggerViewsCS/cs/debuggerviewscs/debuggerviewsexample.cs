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
using System.IO;
using System.Collections.Generic;

namespace Microsoft.Samples
{
	class DebuggerViewsExample
	{
		Dictionary<int, Customer> _customers;

		[STAThread]
		static void Main(string[] args)
		{
			DebuggerViewsExample dve = new DebuggerViewsExample();
			dve._customers = new Dictionary<int, Customer>();
			try
			{
				dve.GetCustomers();
				Console.WriteLine("Customers Successfully Loaded!");
			}
			catch (Exception e)
			{
				Console.WriteLine("There was some problem loading the customer data:");
				Console.WriteLine(e.ToString());
			}
			
		}

		void GetCustomers()
		{
			string[] customerNames = File.ReadAllLines("CustomerNames.txt");
			string[] customerAddresses = File.ReadAllLines("CustomerAddresses.txt");

			foreach (string nextName in customerNames)
			{
				CustomerName cn = CustomerName.GetName(nextName);

				Customer c = new Customer(cn, Address.GetAddressById(customerAddresses, cn.Id));

				_customers.Add(c.Id, c);
			}


		}
	}
}
