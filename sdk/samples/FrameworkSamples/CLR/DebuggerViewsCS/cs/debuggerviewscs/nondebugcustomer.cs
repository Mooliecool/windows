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

namespace Microsoft.Samples
{
	public class Customer
	{
		Address _address;
		CustomerName _name;
		int _id;

		public Customer(CustomerName name, Address address)
		{
			if (address == null)
				throw new ArgumentException("A customer's address cannot be set to nothing");
			if (name == null)
				throw new ArgumentException("A customer's name cannot be set to nothing");
			if (name.Id < 0)
				throw new ArgumentException("A customer canoot have a negative id");
			if (name.Id == 0)
			{
				_id = GenerateNewId();
			}
			else
			{
				_id = name.Id;
			}

			_address = address;
			_name = name;
		}

		public CustomerName Name
		{
			get { return _name; }
			set
			{
				if (value == null)
					throw new ArgumentException("A customer's name cannot be set to nothing");
				_name = value;
			}
		}

		public Address Address
		{
			get { return _address; }
			set
			{
				if (value == null)
					throw new ArgumentException("A customer's address cannot be set to nothing");
				_address = value;
			}
		}

		public int Id
		{
			get { return _id; }
		}

		private int GenerateNewId()
		{
			return GetHighestId() + 1;
		}

		private int GetHighestId()
		{
			string[] lines = File.ReadAllLines("CustomerAddresses.txt");
			int highest = 0;
			foreach (string line in lines)
			{
				int nextId = Convert.ToInt32((line.Split(','))[0]);
				if (nextId > highest)
				{
					highest = nextId;
				}
			}
			return highest;
		}
	}
}
