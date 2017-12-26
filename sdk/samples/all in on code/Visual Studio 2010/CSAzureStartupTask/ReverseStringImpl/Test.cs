/****************************** Module Header ******************************\
Module Name:  Test.cs
Project:      CSAzureStartupTask
Copyright (c) Microsoft Corporation.

Implementation of reverse string WCF service

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using ReverseString;

namespace ReverseString
{
	public class TestWCFService : ITestWCFService
	{
		#region ITest Members

		public string ReverseString(string s)
		{
			char[] arr = s.ToCharArray();
			Array.Reverse(arr);

			return new string(arr);
		}

		#endregion
	}
}
