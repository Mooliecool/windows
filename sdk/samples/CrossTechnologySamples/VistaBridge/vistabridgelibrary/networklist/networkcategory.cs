using System;

namespace Microsoft.SDK.Samples.VistaBridge.Library.Network
{
    /// <summary>
    /// Specifies the trust level for a 
    /// network.
    /// </summary>
    public enum NetworkCategory
    {
        /// <summary>
        /// The network is a public (untrusted) network. 
        /// </summary>
        Public,
        /// <summary>
        /// The network is a private (trusted) network. 
        /// </summary>
        Private,
        /// <summary>
        /// The network is authenticated against an Active Directory domain.
        /// </summary>
        Authenticated
    }
}