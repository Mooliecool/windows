/****************************** Module Header ******************************\
Module Name:  ITest.cs
Project:      CSAzureStartupTask
Copyright (c) Microsoft Corporation.

Definition of reverse string WCF service contract

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


using System.ServiceModel;

namespace ReverseString
{
	[ServiceContract]
	public interface ITestWCFService
	{
		[OperationContract]
		string ReverseString(string s);
	}
}
