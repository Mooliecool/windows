/****************************** Module Header ******************************\
* Module Name:  Page.cs
* Project:      DeepZoomProjectSite
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to generate the deep zoom content programmatically in Silverlight using C#. It wraps the functionality in a WCF service.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 8/27/2009 15:40 Yilun Luo Created
\***************************************************************************/
/****************************** Module Header ******************************\
* Module Name:  IGenerateDeepZoomService.cs
* Project:      DeepZoomProjectSite
* Copyright (c) Microsoft Corporation.
* 
* The interface for the WCF service.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 8/27/2009 15:40 Yilun Luo Created
\***************************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.Text;

// NOTE: If you change the interface name "IGenerateDeepZoomService" here, you must also update the reference to "IGenerateDeepZoomService" in Web.config.
[ServiceContract(Namespace = "http://code.fx/")]
public interface IGenerateDeepZoomService
{
	[OperationContract]
	bool PrepareDeepZoom(bool forceGenerateDeepZoom);
}
