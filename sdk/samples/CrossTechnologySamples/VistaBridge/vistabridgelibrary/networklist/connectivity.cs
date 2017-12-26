using System;

namespace Microsoft.SDK.Samples.VistaBridge.Library.Network
{
    /// <summary>
    /// Specifies types of network connectivity.
    /// </summary>
    [Flags]
    public enum Connectivity
    {
        /// <summary>
        /// The underlying network interfaces have no 
        /// connectivity to any network.
        /// </summary>
        Disconnected = 0,
        /// <summary>
        /// There is connectivity to the Internet 
        /// using the IPv4 protocol.
        /// </summary>
        IPv4Internet = 0x40,
        /// <summary>
        /// There is connectivity to a routed network
        /// using the IPv4 protocol.
        /// </summary>
        IPv4LocalNetwork = 0x20,
        /// <summary>
        /// There is connectivity to a network, but 
        /// the service cannot detect any IPv4 
        /// network traffic.
        /// </summary>
        IPv4NoTraffic = 1,
        /// <summary>
        /// There is connectivity to the local 
        /// subnet using the IPv4 protocol.
        /// </summary>
        IPv4Subnet = 0x10,
        /// <summary>
        /// There is connectivity to the Internet 
        /// using the IPv4 protocol.
        /// </summary>
        IPv6Internet = 0x400,
        /// <summary>
        /// There is connectivity to a local 
        /// network using the IPv6 protocol.
        /// </summary>
        IPv6LocalNetwork = 0x200,
        /// <summary>
        /// There is connectivity to a network, 
        /// but the service cannot detect any 
        /// IPv6 network traffic
        /// </summary>
        IPv6NoTraffic = 2,
        /// <summary>
        /// There is connectivity to the local 
        /// subnet using the IPv6 protocol.
        /// </summary>
        IPv6Subnet = 0x100
    }
}