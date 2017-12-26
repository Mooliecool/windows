//------------------------------------------------------------------------------
// <copyright file="PropertySourceInfo.cs" company="Microsoft">
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

using System;
using System.Configuration.Internal;
using System.Collections;
using System.Collections.Specialized;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Security.Permissions;
using System.Xml;
using System.Globalization;
using System.ComponentModel;
using System.Security;
using System.Text;

namespace System.Configuration {

    internal class PropertySourceInfo {
        private string _fileName;
        private int _lineNumber;

        internal PropertySourceInfo(XmlReader reader) {
            _fileName = GetFilename(reader);
            _lineNumber = GetLineNumber(reader);
        }

        internal string FileName {
            get {
                //
                // Ensure we return the same string to the caller as the one on which we issued the demand.
                //
                string filename = _fileName;
                try {
                    new FileIOPermission(FileIOPermissionAccess.PathDiscovery, filename).Demand();
                }
                catch (SecurityException) {
                    // don't expose the path to this user but show the filename
                    filename = Path.GetFileName(_fileName);
                    if (filename == null) {
                        filename = String.Empty;
                    }
                }

                return filename;
            }
        }

        internal int LineNumber {
            get {
                return _lineNumber;
            }
        }

        private string GetFilename(XmlReader reader) {
            IConfigErrorInfo err = reader as IConfigErrorInfo;

            if (err != null) {
                return (string)err.Filename;
            }

            return "";
        }

        private int GetLineNumber(XmlReader reader) {
            IConfigErrorInfo err = reader as IConfigErrorInfo;

            if (err != null) {
                return (int)err.LineNumber;
            }
            return 0;
        }
    }
}
