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
/*============================================================
**
** Class:  SatelliteContractVersionAttribute
**
**
** Purpose: Specifies which version of a satellite assembly 
**          the ResourceManager should ask for.
**
**
===========================================================*/

using System;

namespace System.Resources {
    
    [AttributeUsage(AttributeTargets.Assembly, AllowMultiple=false)]  
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class SatelliteContractVersionAttribute : Attribute 
    {
        private String _version;

        public SatelliteContractVersionAttribute(String version)
        {
            if (version == null)
                throw new ArgumentNullException("version");
            _version = version;
        }

        public String Version {
            get { return _version; }
        }
    }
}
