//------------------------------------------------------------------------------
// <copyright file="IPEndPoint.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.Net {
    using System.Net.Sockets;
    using System.Globalization;

    /// <devdoc>
    ///    <para>
    ///       Provides an IP address.
    ///    </para>
    /// </devdoc>
    [Serializable]
    public class IPEndPoint : EndPoint {
        /// <devdoc>
        ///    <para>
        ///       Specifies the minimum acceptable value for the <see cref='System.Net.IPEndPoint.Port'/>
        ///       property.
        ///    </para>
        /// </devdoc>
        public const int MinPort = 0x00000000;
        /// <devdoc>
        ///    <para>
        ///       Specifies the maximum acceptable value for the <see cref='System.Net.IPEndPoint.Port'/>
        ///       property.
        ///    </para>
        /// </devdoc>
        public const int MaxPort = 0x0000FFFF;

        private IPAddress m_Address;
        private int m_Port;

        internal const int AnyPort = MinPort;

        internal static IPEndPoint Any     = new IPEndPoint(IPAddress.Any, AnyPort);
        internal static IPEndPoint IPv6Any = new IPEndPoint(IPAddress.IPv6Any,AnyPort);


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public override AddressFamily AddressFamily {
            get {
                //
                // IPv6 Changes: Always delegate this to the address we are
                //               wrapping.
                //
                return m_Address.AddressFamily;
            }
        }

        /// <devdoc>
        ///    <para>Creates a new instance of the IPEndPoint class with the specified address and
        ///       port.</para>
        /// </devdoc>
        public IPEndPoint(long address, int port) {
            if (!ValidationHelper.ValidateTcpPort(port)) {
                throw new ArgumentOutOfRangeException("port");
            }
            m_Port = port;
            m_Address = new IPAddress(address);
        }

        /// <devdoc>
        ///    <para>Creates a new instance of the IPEndPoint class with the specified address and port.</para>
        /// </devdoc>
        public IPEndPoint(IPAddress address, int port) {
            if (address==null) {
                throw new ArgumentNullException("address");
            }
            if (!ValidationHelper.ValidateTcpPort(port)) {
                throw new ArgumentOutOfRangeException("port");
            }
            m_Port = port;
            m_Address = address;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the IP address.
        ///    </para>
        /// </devdoc>
        public IPAddress Address {
            get {
                return m_Address;
            }
            set {
                m_Address = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the port.
        ///    </para>
        /// </devdoc>
        public int Port {
            get {
                return m_Port;
            }
            set {
                if (!ValidationHelper.ValidateTcpPort(value)) {
                    throw new ArgumentOutOfRangeException("value");
                }
                m_Port = value;
            }
        }


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public override string ToString() {
            return Address.ToString() + ":" + Port.ToString(NumberFormatInfo.InvariantInfo);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public override SocketAddress Serialize() {
            if ( m_Address.AddressFamily == AddressFamily.InterNetworkV6 ) {
                //
                // IPv6 Changes: create a new SocketAddress that is large enough for an
                //               IPv6 address and then pack the address into it.
                //
                SocketAddress socketAddress = new SocketAddress(this.AddressFamily, SocketAddress.IPv6AddressSize);
                //
                // populate it
                //
                int port = this.Port;
                socketAddress[2] = (byte)(port>> 8);
                socketAddress[3] = (byte)port;
                //
                // Note: No handling for Flow Information
                //
                socketAddress[4]  = (byte)0;
                socketAddress[5]  = (byte)0;
                socketAddress[6]  = (byte)0;
                socketAddress[7]  = (byte)0;
                //
                // Scope serialization
                //
                long scope = this.Address.ScopeId;

                socketAddress[24] = (byte)scope;
                socketAddress[25] = (byte)(scope >> 8);
                socketAddress[26] = (byte)(scope >> 16);
                socketAddress[27] = (byte)(scope >> 24);
                //
                // Address serialization
                //
                byte[] addressBytes = this.Address.GetAddressBytes();

                for ( int i = 0; i < addressBytes.Length; i++ ) {
                    socketAddress[8 + i] = addressBytes[i];
                }

                GlobalLog.Print("IPEndPoint::Serialize(IPv6): " + this.ToString() );

                //
                // return it
                //
                return socketAddress;
            }
            else
            {
                //
                // create a new SocketAddress
                //
                SocketAddress socketAddress = new SocketAddress(m_Address.AddressFamily, SocketAddress.IPv4AddressSize);
                //
                // populate it
                //
                socketAddress[2] = unchecked((byte)(this.Port>> 8));
                socketAddress[3] = unchecked((byte)(this.Port    ));
    
                socketAddress[4] = unchecked((byte)(this.Address.m_Address    ));
                socketAddress[5] = unchecked((byte)(this.Address.m_Address>> 8));
                socketAddress[6] = unchecked((byte)(this.Address.m_Address>>16));
                socketAddress[7] = unchecked((byte)(this.Address.m_Address>>24));
    
                GlobalLog.Print("IPEndPoint::Serialize: " + this.ToString() );
    
                //
                // return it
                //
                return socketAddress;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public override EndPoint Create(SocketAddress socketAddress) {
            //
            // validate SocketAddress
            //
            if (socketAddress.Family != this.AddressFamily) {
                throw new ArgumentException(SR.GetString(SR.net_InvalidAddressFamily, socketAddress.Family.ToString(), this.GetType().FullName, this.AddressFamily.ToString()), "socketAddress");
            }
            if (socketAddress.Size<8) {
                throw new ArgumentException(SR.GetString(SR.net_InvalidSocketAddressSize, socketAddress.GetType().FullName, this.GetType().FullName), "socketAddress");
            }

            if ( this.AddressFamily == AddressFamily.InterNetworkV6 ) {
                //
                // IPv6 Changes: Extract the IPv6 Address information from the socket address
                //
                byte[] addr = new byte[IPAddress.IPv6AddressBytes];
                for (int i = 0; i < addr.Length; i++)
                {
                    addr[i] = socketAddress[i + 8];
                }
                //
                // Port
                //
                int port = (int)((socketAddress[2]<<8 & 0xFF00) | (socketAddress[3]));
                //
                // Scope
                //
                long scope = (long)((socketAddress[27] << 24) +
                                    (socketAddress[26] << 16) +
                                    (socketAddress[25] << 8 ) +
                                    (socketAddress[24]));

                IPEndPoint created = new IPEndPoint(new IPAddress(addr,scope),port);

                GlobalLog.Print("IPEndPoint::Create IPv6: " + this.ToString() + " -> " + created.ToString() );

                return created;
            }
            else
            {

                //
                // strip out of SocketAddress information on the EndPoint
                //
                int port = (int)(
                        (socketAddress[2]<<8 & 0xFF00) |
                        (socketAddress[3])
                        );
    
                long address = (long)(
                        (socketAddress[4]     & 0x000000FF) |
                        (socketAddress[5]<<8  & 0x0000FF00) |
                        (socketAddress[6]<<16 & 0x00FF0000) |
                        (socketAddress[7]<<24)
                        ) & 0x00000000FFFFFFFF;
    
                IPEndPoint created = new IPEndPoint(address, port);
    
                GlobalLog.Print("IPEndPoint::Create: " + this.ToString() + " -> " + created.ToString() );
    
                //
                // return it
                //
                return created;
            }
        }


        //UEUE
        public override bool Equals(object comparand) {
            if (!(comparand is IPEndPoint)) {
                return false;
            }
            return ((IPEndPoint)comparand).m_Address.Equals(m_Address) && ((IPEndPoint)comparand).m_Port==m_Port;
        }

        //UEUE
        public override int GetHashCode() {
            return m_Address.GetHashCode() ^ m_Port;
        }

        // For security, we need to be able to take an IPEndPoint and make a copy that's immutable and not derived.
        internal IPEndPoint Snapshot()
        {
            return new IPEndPoint(Address.Snapshot(), Port);
        }
    } // class IPEndPoint
} // namespace System.Net
