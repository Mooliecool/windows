using System;

namespace Microsoft.SDK.Samples.VistaBridge.Library.Network
{
    /// <summary>
    /// Specifies the level of connectivity for 
    /// networks returned by the 
    /// <see cref="Microsoft.SDK.Samples.VistaBridge.Library.Network.NetworkListManager"/> 
    /// class.
    /// </summary>
    [Flags]
    public enum NetworkConnectivityLevels
    {
        /// <summary>
        /// Networks that the machine is connected to.
        /// </summary>
        Connected = 1,
        /// <summary>
        /// Networks that the machine is not connected to.
        /// </summary>
        Disconnected = 2,
        /// <summary>
        /// All networks.
        /// </summary>
        All = 3,
    }
}