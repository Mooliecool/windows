/****************************** Module Header ******************************\
* Project Name:   CSAzureServiceBusProtocolBridging
* Module Name:    Common
* File Name:      IMyService.cs
* Copyright (c) Microsoft Corporation
*
* A WCF service contract that is shared by both Client project and the Service project.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System.ServiceModel;

namespace Common
{
    [ServiceContract]
    public interface IMyService
    {
        [OperationContract]
        int Add(int number1, int number2);
    }
}
