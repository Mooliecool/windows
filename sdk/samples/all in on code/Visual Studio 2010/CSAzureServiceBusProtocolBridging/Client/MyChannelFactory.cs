/****************************** Module Header ******************************\
* Project Name:   CSAzureServiceBusProtocolBridging
* Module Name:    Client
* File Name:      MyChannelFactory.cs
* Copyright (c) Microsoft Corporation
*
* This is a wrapper for calling WCF service.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using Common;
using System.ServiceModel;

namespace Client
{
    public class MyChannelFactory : IDisposable
    {
        IMyService channel = null;
        ChannelFactory<IMyService> channelFactory = null;

        public IMyService MyService { get { return channel; } }
        public string EndpointAddress { get { return channelFactory.Endpoint.Address.ToString(); } }

        public MyChannelFactory(string endpointName, string endpointAddress)
        {
            channelFactory = new ChannelFactory<IMyService>(endpointName, new EndpointAddress(endpointAddress));
            channel = channelFactory.CreateChannel();
            ((ICommunicationObject)channel).Open();
        }

        public void Dispose()
        {
            if (channel != null)
            {
                ((ICommunicationObject)channel).Close();
            }
            if (channelFactory != null)
            {
                channelFactory.Close();
            }
        }
    }
}
