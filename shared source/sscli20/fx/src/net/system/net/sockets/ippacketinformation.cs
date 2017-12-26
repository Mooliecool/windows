//------------------------------------------------------------------------------
// <copyright file="IPPacketInformation.cs" company="Microsoft">
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

          
          
namespace System.Net.Sockets {
    using System.Net;

    public struct IPPacketInformation {
        IPAddress address;
        int networkInterface;

        internal IPPacketInformation(IPAddress address, int networkInterface){
            this.address = address;
            this.networkInterface = networkInterface;
        }
        
        public IPAddress Address {
            get{
                return address;
            }
        }
        
        public int Interface {
            get{
                return networkInterface;
            }
        }

        public static bool operator == (IPPacketInformation packetInformation1, 
                                        IPPacketInformation packetInformation2 ) {
            return packetInformation1.Equals(packetInformation2);
        }

        public static bool operator != (IPPacketInformation packetInformation1, 
                                        IPPacketInformation packetInformation2 ) {
            return !packetInformation1.Equals(packetInformation2);
        }

        public override bool Equals(object comparand) {
            if ((object) comparand == null) {
                return false;
            }

            if (!(comparand is IPPacketInformation))
                return false;

            IPPacketInformation obj = (IPPacketInformation) comparand;

            if (address.Equals(obj.address) && networkInterface == obj.networkInterface)
                return (true);

            return false;
        }

        public override int GetHashCode() {
            return address.GetHashCode() + networkInterface.GetHashCode();
        }

    }; // enum SocketFlags
}





