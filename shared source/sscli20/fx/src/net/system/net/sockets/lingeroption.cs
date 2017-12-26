//------------------------------------------------------------------------------
// <copyright file="LingerOption.cs" company="Microsoft">
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
    using System;
    
    
    /// <devdoc>
    ///    <para>Contains information for a socket's linger time, the amount of time it will
    ///       remain after closing if data remains to be sent.</para>
    /// </devdoc>
    public class LingerOption {
        bool enabled;
        int lingerTime;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='Sockets.LingerOption'/> class.
        ///    </para>
        /// </devdoc>
        public LingerOption(bool enable, int seconds) {
            Enabled = enable;
            LingerTime = seconds;
        }
        
        /// <devdoc>
        ///    <para>
        ///       Enables or disables lingering after
        ///       close.
        ///    </para>
        /// </devdoc>
        public bool Enabled {
            get {
                return enabled;
            }
            set {
                enabled = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       The amount of time, in seconds, to remain connected after a close.
        ///    </para>
        /// </devdoc>
        public int LingerTime {
            get {
                return lingerTime;
            }
            set {
                lingerTime = value;
            }
        }

    } // class LingerOption
} // namespace System.Net.Sockets
