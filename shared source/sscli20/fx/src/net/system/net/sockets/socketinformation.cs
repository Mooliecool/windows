//------------------------------------------------------------------------------
// <copyright file="SocketInformation.cs" company="Microsoft">
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

    [Serializable]
    public struct SocketInformation{
        byte[] protocolInformation;
        SocketInformationOptions options;

        public byte[] ProtocolInformation{
            get{
               return protocolInformation;
            }
            set{
                protocolInformation = value;
            }
        }

        
        public SocketInformationOptions Options{
            get{
               return options;
            }
            set{
                options = value;
            }
        }
        
        internal bool IsNonBlocking{
            get{
               return ((options&SocketInformationOptions.NonBlocking)!=0);
            }
            set{
                if(value){
                    options |= SocketInformationOptions.NonBlocking;
                }
                else{
                    options &= ~SocketInformationOptions.NonBlocking;
                }
            }
        }

        internal bool IsConnected{
            get{
                return ((options&SocketInformationOptions.Connected)!=0);
            }
            set{
                if(value){
                    options |= SocketInformationOptions.Connected;
                }
                else{
                    options &= ~SocketInformationOptions.Connected;
                }
            }
        }

        internal bool IsListening{
            get{
                return ((options&SocketInformationOptions.Listening)!=0);
            }
            set{
                if(value){
                    options |= SocketInformationOptions.Listening;
                }
                else{
                    options &= ~SocketInformationOptions.Listening;
                }
            }
        }

        internal bool UseOnlyOverlappedIO{
            get{
                return ((options&SocketInformationOptions.UseOnlyOverlappedIO)!=0);
            }
            set{
                if(value){
                    options |= SocketInformationOptions.UseOnlyOverlappedIO;
                }
                else{
                    options &= ~SocketInformationOptions.UseOnlyOverlappedIO;
                }
            }
        }
    }
}
