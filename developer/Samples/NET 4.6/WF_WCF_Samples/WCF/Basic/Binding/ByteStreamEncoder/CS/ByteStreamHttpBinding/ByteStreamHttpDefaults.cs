//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.Xml;

namespace Microsoft.Samples.ByteStreamEncoder
{

    static class ByteStreamHttpDefaults
    {
        internal const Boolean DefaultBypassProxyOnLocal = true;
        internal const HostNameComparisonMode DefaultHostNameComparisonMode = HostNameComparisonMode.StrongWildcard;
        internal const Int64 DefaultMaxBufferPoolSize = 512 * 1024;
        internal const Int32 DefaultMaxBufferSize = 65536;
        internal const Int64 DefaultMaxReceivedMessageSize = 65536;
        internal const Uri DefaultProxyAddress = null;
        internal const XmlDictionaryReaderQuotas DefaultReaderQuotas = null;
        internal const TransferMode DefaultTransferMode = TransferMode.Streamed;
        internal const Boolean DefaultUseDefaultWebProxy = true;
    }
}
