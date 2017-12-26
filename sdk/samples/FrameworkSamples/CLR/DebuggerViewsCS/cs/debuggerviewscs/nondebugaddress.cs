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

namespace Microsoft.Samples
{

	public class Address
	{
		int _customerId;
		string _streetLine1;
		string _streetLine2;
		string _phone;
		string _city;
		string _country;
		int _zipCode;

		public Address(string streetLine1, string city, string country):this(streetLine1, city, country, 0)
		{
		}

		public Address(string streetLine1, string city, string country, int zipCode):this(streetLine1, city, country, zipCode, null, null)
		{
		}

		public Address(string streetLine1, string city, string country, int zipCode, string streetLine2, string phone)
		{
			if (streetLine1 == null || streetLine1.Trim().Length == 0)
				throw new ArgumentException("Street Line 1 of an address must be a non-empty value");
			if (streetLine1.Trim().Length > 100)
				throw new ArgumentException("Street Line 1 of an address cannot exceed 100 characters");

			if (country == null || country.Trim().Length == 0)
				throw new ArgumentException("Country of an address must be a non-empty value");
			if (country.Trim().Length > 50)
				throw new ArgumentException("Country of an address cannot exceed 50 characters");

			if (streetLine2 != null && streetLine2.Trim().Length > 100)
				throw new ArgumentException("Street Line 2 of an address cannot exceed 100 characters");

			if (city == null || city.Trim().Length == 0)
				throw new ArgumentException("City of an address must be a non-empty value");
			if (city.Trim().Length > 35)
				throw new ArgumentException("City of an address cannot exceed 35 characters");

			if (zipCode < 0)
				throw new ArgumentException("ZipCode must be a value great than, or equal to zero");

			_streetLine1 = streetLine1.Trim();
			_streetLine2 = streetLine2 == null ? "" : streetLine2.Trim();
			_city = city.Trim();
			_country = country.Trim();
			_zipCode = zipCode;
			_phone = phone == null ? "" : phone.Trim();
		}

		public string StreetLine1
		{
			get { return _streetLine1; }
			set
			{
				if (value == null || value.Trim().Length == 0)
					throw new ArgumentException("Street Line 1 of an address must be a non-empty value");
				if (value.Trim().Length > 100)
					throw new ArgumentException("Street Line 1 of an address cannot exceed 100 characters");

				_streetLine1 = value.Trim();
			}
		}
		public string StreetLine2
		{
			get { return _streetLine2; }
			set
			{
				if (value != null && value.Trim().Length > 100)
					throw new ArgumentException("Street Line 2 of an address cannot exceed 100 characters");

				_streetLine2 = value == "" ? null : value.Trim();
			}
		}
		public string City
		{
			get { return _city; }
			set
			{
				if (value == null || value.Trim().Length == 0)
					throw new ArgumentException("City of an address must be a non-empty value");
				if (value.Trim().Length > 35)
					throw new ArgumentException("City of an address cannot exceed 35 characters");

				_city = value.Trim();
			}
		}
		public string Country
		{
			get { return _country; }
			set
			{
				if (value == null || value.Trim().Length == 0)
					throw new ArgumentException("Country of an address must be a non-empty value");
				if (value.Trim().Length > 50)
					throw new ArgumentException("Country of an address cannot exceed 50 characters");

				_country = value.Trim();
			}
		}
		public string Phone
		{
			get { return _phone; }
			set
			{
				_phone = value == null ? "" : value.Trim();
			}
		}

		public int ZipCode
		{
			get { return _zipCode; }
			set
			{
				if (value < 0)
					throw new ArgumentException("ZipCode must be a value great than, or equal to zero");

				_zipCode = value;
			}
		}

		public int Id
		{
			get { return _customerId; }
		}

		public static Address GetAddressById(string[] addresses, int id)
		{
			foreach (string nextAddress in addresses)
			{
				string[] values = nextAddress.Split(',');
				if (Convert.ToInt32(values[0]) == id)
				{
					Address a = new Address(values[1], values[3], values[4]);
					a.StreetLine2 = values[2];
					a._customerId = Convert.ToInt32(values[0]);

					if (values.Length > 5)
					{
						a.ZipCode = Convert.ToInt32(values[5]);
					}
					if (values.Length > 6)
					{
						a.Phone = values[6];
					}

					return a;
				}
			}

			return null;
		}
	}
}
