//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------
using System;
using System.Configuration;
using System.ServiceModel.Configuration;

namespace Microsoft.Samples.TextFileTracking
{
    public class TextFileTrackingExtensionElement : BehaviorExtensionElement
    {
        [ConfigurationProperty("path", DefaultValue = "", IsKey = false, IsRequired = true)]
        public string Path
        {
            get { return (string)this["path"]; }
            set { this["path"] = value; }
        }

        [ConfigurationProperty("profileName", DefaultValue = null, IsKey = false, IsRequired = false)]
        public string ProfileName
        {
            get { return (string)this["profileName"]; }
            set { this["profileName"] = value; }
        }

        public override Type BehaviorType { get { return typeof(TextFileTrackingBehavior); } }
        protected override object CreateBehavior() { return new TextFileTrackingBehavior(Path, ProfileName); }
    }
}