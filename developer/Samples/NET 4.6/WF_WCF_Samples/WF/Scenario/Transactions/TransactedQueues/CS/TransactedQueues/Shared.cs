//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Channels;

namespace Microsoft.Samples.TransactedQueue
{
    static class Shared
    {
        private const string queueLocation = "net.msmq://localhost/private/";
        private const string queueName = "rewardspointservice";
        private const string queuePrefix = ".\\Private$\\";

        public const string Namespace = "http://tempuri.org/";
        public const string Contract = "IRewardsPointService";

        public static Binding Binding
        {
            get
            {
                return new NetMsmqBinding(NetMsmqSecurityMode.None);
            }
        }

        public static Uri Address
        {
            get
            {
                return new Uri(queueLocation + queueName);
            }
        }

        public static string QueuePath
        {
            get
            {
                return queuePrefix + queueName;
            }
        }
    }
}