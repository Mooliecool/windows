//------------------------------------------------------------------------------
// <copyright file="FileVersion.cs" company="Microsoft">
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
    using System.Configuration;
    using System.IO;
    using System.Security.Permissions;
    using System.Reflection;
    using System.Threading;
    using System.Security;
    using System.CodeDom.Compiler;
    using Microsoft.Win32;	

    internal class FileVersion {
        bool        _exists;
        long        _fileSize;
        DateTime    _utcCreationTime;
        DateTime    _utcLastWriteTime;

        internal FileVersion(bool exists, long fileSize, DateTime utcCreationTime, DateTime utcLastWriteTime) {
            _exists = exists;
            _fileSize = fileSize;
            _utcCreationTime = utcCreationTime;
            _utcLastWriteTime = utcLastWriteTime;
        }

        public override bool Equals(Object obj) {
            FileVersion other = obj as FileVersion;
            return
                   other != null
                && _exists == other._exists
                && _fileSize == other._fileSize
                && _utcCreationTime == other._utcCreationTime
                && _utcLastWriteTime == other._utcLastWriteTime;
        }

        public override int GetHashCode() {
            return base.GetHashCode();
        }
    }
}
