using System;

namespace Microsoft.SDK.Samples.VistaBridge.Library.Network
{
    /// <summary>
    /// Specifies the domain type of a network.
    /// </summary>
    public enum DomainType
    {
        /// <summary>
        /// The network is not an Active Directory network.
        /// </summary>
        NonDomainNetwork = 0,
        /// <summary>
        /// The network is an Active Directory network, but this machine is not authenticated against it.
        /// </summary>
        DomainNetwork = 1,
        /// <summary>
        /// The network is an Active Directory network, and this machine is authenticated against it.
        /// </summary>
        DomainAuthenticated = 2,
    }
}