//------------------------------------------------------------------------------
// <copyright file="ConfigSystem.cs" company="Microsoft">
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

namespace System.Configuration.Internal {
    using System;
    using System.Configuration;

    // The runtime config system
    internal class ConfigSystem : IConfigSystem {
        IInternalConfigRoot _configRoot;
        IInternalConfigHost _configHost;

        void IConfigSystem.Init(Type typeConfigHost, params object[] hostInitParams) {
            _configRoot = new InternalConfigRoot();
            _configHost = (IInternalConfigHost) TypeUtil.CreateInstanceWithReflectionPermission(typeConfigHost);

            _configRoot.Init(_configHost, false);
            _configHost.Init(_configRoot, hostInitParams);
        }

        IInternalConfigHost IConfigSystem.Host {
            get {return _configHost;}
        }

        IInternalConfigRoot IConfigSystem.Root {
            get {return _configRoot;}
        }
    }
}
