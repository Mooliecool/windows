//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.Xml;
    
namespace Microsoft.Samples.NetHttpBinding {

    internal class NetHttpDefaults {
        
        internal const Boolean DefaultBypassProxyOnLocal = true;

        internal const HostNameComparisonMode DefaultHostNameComparisonMode = HostNameComparisonMode.StrongWildcard;

        internal const Int32 DefaultMaxBufferSize = 65536;

        internal const Int64 DefaultMaxBufferPoolSize = 512 * 1024;

        internal const Int64 DefaultMaxReceivedMessageSize = 65536;

        internal const Uri DefaultProxyAddress = null;

        internal const NetHttpSecurityMode DefaultSecurityMode = NetHttpSecurityMode.Transport;

        internal const TransferMode DefaultTransferMode = TransferMode.Buffered;

        internal const Boolean DefaultUseDefaultWebProxy = true;

        internal const XmlDictionaryReaderQuotas DefaultReaderQuotas = null;
    }
}
