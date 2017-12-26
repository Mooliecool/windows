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
/*=============================================================================
**
** Class: ManifestResourceInfo
**
**
** Purpose: For info regarding a manifest resource's topology.
**
**
=============================================================================*/

namespace System.Reflection {
    using System;
    
[System.Runtime.InteropServices.ComVisible(true)]
    public class ManifestResourceInfo {
        private Assembly _containingAssembly;
        private String _containingFileName;
        private ResourceLocation _resourceLocation;

        internal ManifestResourceInfo(Assembly containingAssembly,
                                      String containingFileName,
                                      ResourceLocation resourceLocation)
        {
            _containingAssembly = containingAssembly;
            _containingFileName = containingFileName;
            _resourceLocation = resourceLocation;
        }

        public virtual Assembly ReferencedAssembly
        {
            get {
                return _containingAssembly;
            }
        }

        public virtual String FileName
        {
            get {
                return _containingFileName;
            }
        }

        public virtual ResourceLocation ResourceLocation
        {
            get {
                return _resourceLocation;
            }
        }
    }

    // The ResourceLocation is a combination of these flags, set or not.
    // Linked means not Embedded.
    [Flags, Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum ResourceLocation
    {
        Embedded = 0x1,
        ContainedInAnotherAssembly = 0x2,
        ContainedInManifestFile = 0x4
    }
}
