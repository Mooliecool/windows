// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==

//
// X509Certificate.cs
//

namespace System.Security.Cryptography.X509Certificates {
    using Microsoft.Win32;
    using System;
    using System.IO;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;
    using System.Runtime.Serialization;
    using System.Security;
    using System.Security.Permissions;
    using System.Security.Util;
    using System.Text;
    using System.Runtime.Versioning;

    [System.Runtime.InteropServices.ComVisible(true)]
    public enum X509ContentType {
        Unknown         = 0x00,
        Cert            = 0x01,
        SerializedCert  = 0x02,
        Pfx             = 0x03,
        Pkcs12          = Pfx,
        SerializedStore = 0x04, 
        Pkcs7           = 0x05,
        Authenticode    = 0x06
    }

    // DefaultKeySet, UserKeySet and MachineKeySet are mutually exclusive
    [Flags, Serializable()]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum X509KeyStorageFlags {
        DefaultKeySet = 0x00,
        UserKeySet    = 0x01,
        MachineKeySet = 0x02,
        Exportable    = 0x04,
        UserProtected = 0x08,
        PersistKeySet = 0x10
    }

}
