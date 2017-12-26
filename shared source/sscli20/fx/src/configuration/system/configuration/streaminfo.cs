//------------------------------------------------------------------------------
// <copyright file="StreamInfo.cs" company="Microsoft">
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

namespace System.Configuration {
    using System.Configuration.Internal;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Configuration;
    using System.Globalization;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Security.Permissions;
    using System.Security;
    using System.Text;
    using System.Xml;

    // Information about a stream used in configuration
    class StreamInfo {
        private string  _sectionName;   // the section the stream contains for configSource, null for a full config file
        private string  _configSource;  // the configSource directive that generated this stream, null for a full config file
        private string  _streamName;    // name of the stream
        private bool    _isMonitored;   // is the stream currently monitored?
        private object  _version;       // version that we're monitoring

        internal StreamInfo(string sectionName, string configSource, string streamName) {
            _sectionName = sectionName;
            _configSource = configSource;
            _streamName = streamName;
        }

        private StreamInfo() {
        }

        internal StreamInfo Clone() {
            StreamInfo clone = new StreamInfo();

            clone._sectionName  = this._sectionName;
            clone._configSource = this._configSource;
            clone._streamName   = this._streamName;
            clone._isMonitored  = this._isMonitored;
            clone._version      = this._version;

            return clone;
        }

        internal string SectionName {
            get { return _sectionName; }
        }

        internal string ConfigSource {
            get { return _configSource; }
        }

        internal string StreamName {
            get { return _streamName; }
        }

        internal bool IsMonitored {
            get { return _isMonitored; }
            set { _isMonitored = value;}
        }

        internal object Version {
            get { return _version; }
            set { _version = value; }
        }
    }
}
