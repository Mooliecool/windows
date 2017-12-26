/****************************** Module Header ******************************\
* Module Name:	IProcessDataWorkflowService.cs
* Project:		WFServiceLibrary
* Copyright (c) Microsoft Corporation.
* 
* This is the contract of the workflow service.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace WFServiceLibrary
{
	// This simple workflow service doesn't need sesion.
	[ServiceContract(SessionMode = SessionMode.NotAllowed)]
	public interface IProcessDataWorkflow
	{
		[OperationContract]
		string ProcessData(int value);
	}
}
