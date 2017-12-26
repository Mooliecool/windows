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
using System.Collections.Generic;

namespace Microsoft.Samples
{
	public class CustomerName
	{
		int _customerId;
		string _lastName;
		string _firstName;
		List<string> _middleNames;

		public CustomerName(string lastName):this(lastName, null)
		{
		}

		public CustomerName(string lastName, string firstName):this(lastName, firstName, null)
		{
		}

		public CustomerName(string lastName, string firstName, params string[] middleNames)
		{
			if (lastName == null || lastName.Trim().Length == 0)
				throw new ArgumentException("A last name must always be entered. If only one name exists for a customer, enter it as the last name");
			if (lastName.Trim().Length > 50)
				throw new ArgumentException("A maximum of 50 characters can be entered for a last name");

			if (firstName != null && firstName.Trim().Length > 50)
				throw new ArgumentException("A maximum of 50 characters can be entered for a first name");

			if (middleNames != null)
			{
				foreach (string s in middleNames)
				{
					if (s != null && s.Trim().Length > 0)
					{
						if (s.Length > 50)
							throw new ArgumentException("A maximum of 50 characters can be entered for a middle name");

						if (_middleNames == null)
						{
							_middleNames = new List<string>();
						}
						_middleNames.Add(s.Trim());
					}
				}
			}
			_lastName = lastName;
			_firstName = firstName == null ? "" : firstName.Trim();
		}

		public string LastName
		{
			get { return _lastName; }
			set
			{
				if (value == null || value.Trim().Length == 0)
					throw new ArgumentException("A last name cannot be empty. If only one name exists for a customer, enter it as the last name");
				if (value.Trim().Length > 50)
					throw new ArgumentException("A maximum of 50 characters can be entered for a last name");

				_lastName = value;
			}
		}

		public string FirstName
		{
			get { return _firstName == null ? "" : _firstName; }
			set
			{
				if (value != null && value.Trim().Length > 50)
					throw new ArgumentException("A maximum of 50 characters can be entered for a first name");

				_firstName = value == null ? null : value.Trim();
			}
		}

		public int Id
		{
			get { return _customerId; }
		}

		public List<string> MiddleNames
		{
			get { return _middleNames == null ? new List<string>() : _middleNames; }
			set
			{
				if (value != null)
				{
					foreach (string s in value)
					{
						if (s != null && s.Trim().Length > 0)
						{
							if (s.Length > 50)
								throw new ArgumentException("A maximum of 50 characters can be entered for a middle name");

							if (_middleNames == null)
							{
								_middleNames = new List<string>();
							}
							_middleNames.Add(s);
						}
					}
				}
				else
				{
					_middleNames = null;
				}
			}
		}

		public static CustomerName GetName(string dataLine)
		{
			string[] name = dataLine.Split(',');
			CustomerName c = new CustomerName(name[1]);
			c._customerId = Convert.ToInt32(name[0]);

			if (name.Length > 2)
			{
				c.FirstName = name[2];
			}

			if (name.Length > 3)
			{
				for (int i = 3; i < name.Length; i++)
				{
					c.MiddleNames.Add(name[i]);
				}
			}
			return c;
		}
	}
}
