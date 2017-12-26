//------------------------------------------------------------------------------
// <copyright file="CodeChecksumPragma.cs" company="Microsoft">
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

namespace System.CodeDom {

    using System.Diagnostics;
    using System;
    using Microsoft.Win32;
    using System.Collections;
    using System.Runtime.InteropServices;

    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeChecksumPragma: CodeDirective {
        private string fileName;
        private byte[] checksumData;
        private Guid checksumAlgorithmId;

        public CodeChecksumPragma() {
        }
        
        public CodeChecksumPragma(string fileName, Guid checksumAlgorithmId, byte[] checksumData) {
            this.fileName = fileName;
            this.checksumAlgorithmId = checksumAlgorithmId;
            this.checksumData = checksumData;
        }

        public string FileName {
            get {
                return (fileName == null) ? string.Empty : fileName;
            }
            set {
                fileName = value;
            }
        }
        
        public Guid ChecksumAlgorithmId {
            get {
                return checksumAlgorithmId;
            }
            set {
                checksumAlgorithmId = value;
            }
        }
        
        public byte[] ChecksumData {
            get {
                return checksumData;
            }
            set {
                checksumData = value;
            }
        }
    }
}
