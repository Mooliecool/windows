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
** Class:  MissingSatelliteAssemblyException
**
**
** Purpose: Exception for a missing satellite assembly needed
**          for ultimate resource fallback.  This usually
**          indicates a setup and/or deployment problem.
**
**
===========================================================*/

using System;
using System.Runtime.Serialization;

namespace System.Resources {
    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public class MissingSatelliteAssemblyException : SystemException
    {
        private String _cultureName;

        public MissingSatelliteAssemblyException() 
            : base(Environment.GetResourceString("MissingSatelliteAssembly_Default")) {
    		SetErrorCode(__HResults.COR_E_MISSINGSATELLITEASSEMBLY);
        }
        
        public MissingSatelliteAssemblyException(String message) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_MISSINGSATELLITEASSEMBLY);
        }
    	
        public MissingSatelliteAssemblyException(String message, String cultureName) 
            : base(message) {
    		SetErrorCode(__HResults.COR_E_MISSINGSATELLITEASSEMBLY);
            _cultureName = cultureName;
        }

        public MissingSatelliteAssemblyException(String message, Exception inner) 
            : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_MISSINGSATELLITEASSEMBLY);
        }

        protected MissingSatelliteAssemblyException(SerializationInfo info, StreamingContext context) : base (info, context) {
        }

        public String CultureName {
            get { return _cultureName; }
        }
    }
}
