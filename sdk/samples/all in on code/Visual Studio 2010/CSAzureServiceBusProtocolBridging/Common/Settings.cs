/****************************** Module Header ******************************\
* Project Name:   CSAzureServiceBusProtocolBridging
* Module Name:    Common
* File Name:      Settings.cs
* Copyright (c) Microsoft Corporation
*
* This class contains all settings related to a Service Bus namespace. You need
* input these settings before running this sample.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;

namespace Common
{
    public class Settings
    {
        /// <summary>
        /// Please input your Service Bus namespace.
        /// </summary>
        public static string ServiceNamespace = "";

        /// <summary>
        /// Please input your Service Bus Isser.
        /// </summary>
        public static string IssuerName = "";

        /// <summary>
        /// Please input your Service Bus Secret.
        /// </summary>
        public static string IssuerSecret = "";

        /// <summary>
        /// https://{0}.servicebus.windows.net/MyService
        /// Please input your Service Bus namespace.
        /// </summary>
        public static string ServiceEndpoint 
        {
            get
            {
                return String.Format("https://{0}.servicebus.windows.net/MyService", ServiceNamespace);
            }
        }
    }
}
