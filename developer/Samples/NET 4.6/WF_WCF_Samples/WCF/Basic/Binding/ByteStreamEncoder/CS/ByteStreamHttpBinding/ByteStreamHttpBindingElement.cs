//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Configuration;
using System.Globalization;
using System.ServiceModel.Channels;
using System.ServiceModel.Configuration;

namespace Microsoft.Samples.ByteStreamEncoder
{

    public class ByteStreamHttpBindingElement : StandardBindingElement
    {
        public ByteStreamHttpBindingElement(string configurationName) :
            base(configurationName)
        {
        }

        public ByteStreamHttpBindingElement() :
            this(null)
        {
        }

        [ConfigurationProperty(ByteStreamHttpConfigurationStrings.BypassProxyOnLocal, DefaultValue = ByteStreamHttpDefaults.DefaultBypassProxyOnLocal)]
        public bool BypassProxyOnLocal
        {
            get
            {
                return ((bool)(base[ByteStreamHttpConfigurationStrings.BypassProxyOnLocal]));
            }
            set
            {
                base[ByteStreamHttpConfigurationStrings.BypassProxyOnLocal] = value;
            }
        }

        [ConfigurationProperty(ByteStreamHttpConfigurationStrings.HostNameComparisonMode, DefaultValue = ByteStreamHttpDefaults.DefaultHostNameComparisonMode)]
        [ServiceModelEnumValidator(typeof(HostNameComparisonModeHelper))]
        public System.ServiceModel.HostNameComparisonMode HostNameComparisonMode
        {
            get
            {
                return ((System.ServiceModel.HostNameComparisonMode)(base[ByteStreamHttpConfigurationStrings.HostNameComparisonMode]));
            }
            set
            {
                base[ByteStreamHttpConfigurationStrings.HostNameComparisonMode] = value;
            }
        }

        [ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxBufferPoolSize, DefaultValue = ByteStreamHttpDefaults.DefaultMaxBufferPoolSize)]
        [LongValidator(MinValue = 0)]
        public long MaxBufferPoolSize
        {
            get
            {
                return ((long)(base[ByteStreamHttpConfigurationStrings.MaxBufferPoolSize]));
            }
            set
            {
                base[ByteStreamHttpConfigurationStrings.MaxBufferPoolSize] = value;
            }
        }

        [ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxBufferSize, DefaultValue = ByteStreamHttpDefaults.DefaultMaxBufferSize)]
        [IntegerValidator(MinValue = 1)]
        public int MaxBufferSize
        {
            get
            {
                return ((int)(base[ByteStreamHttpConfigurationStrings.MaxBufferSize]));
            }
            set
            {
                base[ByteStreamHttpConfigurationStrings.MaxBufferSize] = value;
            }
        }

        [ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxReceivedMessageSize, DefaultValue = ByteStreamHttpDefaults.DefaultMaxReceivedMessageSize)]
        [LongValidator(MinValue = 1)]
        public long MaxReceivedMessageSize
        {
            get
            {
                return ((long)(base[ByteStreamHttpConfigurationStrings.MaxReceivedMessageSize]));
            }
            set
            {
                base[ByteStreamHttpConfigurationStrings.MaxReceivedMessageSize] = value;
            }
        }

        [ConfigurationProperty(ByteStreamHttpConfigurationStrings.ProxyAddress, DefaultValue = ByteStreamHttpDefaults.DefaultProxyAddress)]
        [AddressValidator]
        public System.Uri ProxyAddress
        {
            get
            {
                return ((System.Uri)(base[ByteStreamHttpConfigurationStrings.ProxyAddress]));
            }
            set
            {
                base[ByteStreamHttpConfigurationStrings.ProxyAddress] = value;
            }
        }

        [ConfigurationProperty(ByteStreamHttpConfigurationStrings.ReaderQuotas, DefaultValue = ByteStreamHttpDefaults.DefaultReaderQuotas)]
        public XmlDictionaryReaderQuotasElement ReaderQuotas
        {
            get
            {
                return (XmlDictionaryReaderQuotasElement)base[ByteStreamHttpConfigurationStrings.ReaderQuotas];
            }
        }

        [ConfigurationProperty(ByteStreamHttpConfigurationStrings.TransferMode, DefaultValue = ByteStreamHttpDefaults.DefaultTransferMode)]
        [ServiceModelEnumValidator(typeof(TransferModeHelper))]
        public System.ServiceModel.TransferMode TransferMode
        {
            get
            {
                return ((System.ServiceModel.TransferMode)(base[ByteStreamHttpConfigurationStrings.TransferMode]));
            }
            set
            {
                base[ByteStreamHttpConfigurationStrings.TransferMode] = value;
            }
        }

        [ConfigurationProperty(ByteStreamHttpConfigurationStrings.UseDefaultWebProxy, DefaultValue = ByteStreamHttpDefaults.DefaultUseDefaultWebProxy)]
        public bool UseDefaultWebProxy
        {
            get
            {
                return ((bool)(base[ByteStreamHttpConfigurationStrings.UseDefaultWebProxy]));
            }
            set
            {
                base[ByteStreamHttpConfigurationStrings.UseDefaultWebProxy] = value;
            }
        }

        protected override Type BindingElementType
        {
            get
            {
                return typeof(ByteStreamHttpBinding);
            }
        }

        protected override ConfigurationPropertyCollection Properties
        {
            get
            {
                ConfigurationPropertyCollection properties = base.Properties;
                properties.Add(new ConfigurationProperty(ByteStreamHttpConfigurationStrings.HostNameComparisonMode, typeof(System.ServiceModel.HostNameComparisonMode), ByteStreamHttpDefaults.DefaultHostNameComparisonMode));
                properties.Add(new ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxBufferSize, typeof(int), ByteStreamHttpDefaults.DefaultMaxBufferSize));
                properties.Add(new ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxBufferPoolSize, typeof(long), ByteStreamHttpDefaults.DefaultMaxBufferPoolSize));
                properties.Add(new ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxReceivedMessageSize, typeof(long), ByteStreamHttpDefaults.DefaultMaxReceivedMessageSize));
                properties.Add(new ConfigurationProperty(ByteStreamHttpConfigurationStrings.TransferMode, typeof(System.ServiceModel.TransferMode), ByteStreamHttpDefaults.DefaultTransferMode));
                properties.Add(new ConfigurationProperty(ByteStreamHttpConfigurationStrings.ReaderQuotas, typeof(XmlDictionaryReaderQuotasElement), ByteStreamHttpDefaults.DefaultReaderQuotas));
                return properties;
            }
        }

        protected override void InitializeFrom(Binding binding)
        {
            base.InitializeFrom(binding);
            ByteStreamHttpBinding ByteStreamHttpBinding = ((ByteStreamHttpBinding)(binding));
            this.HostNameComparisonMode = ByteStreamHttpBinding.HostNameComparisonMode;
            this.MaxBufferSize = ByteStreamHttpBinding.MaxBufferSize;
            this.MaxBufferPoolSize = ByteStreamHttpBinding.MaxBufferPoolSize;
            this.MaxReceivedMessageSize = ByteStreamHttpBinding.MaxReceivedMessageSize;
            this.TransferMode = ByteStreamHttpBinding.TransferMode;

            // Copy reader quotas over.
            this.ReaderQuotas.MaxDepth = ByteStreamHttpBinding.ReaderQuotas.MaxDepth;
            this.ReaderQuotas.MaxStringContentLength = ByteStreamHttpBinding.ReaderQuotas.MaxStringContentLength;
            this.ReaderQuotas.MaxArrayLength = ByteStreamHttpBinding.ReaderQuotas.MaxArrayLength;
            this.ReaderQuotas.MaxBytesPerRead = ByteStreamHttpBinding.ReaderQuotas.MaxBytesPerRead;
            this.ReaderQuotas.MaxNameTableCharCount = ByteStreamHttpBinding.ReaderQuotas.MaxNameTableCharCount;
        }

        protected override void OnApplyConfiguration(Binding binding)
        {
            if ((binding == null))
            {
                throw new System.ArgumentNullException("binding");
            }
            if ((binding.GetType() != typeof(ByteStreamHttpBinding)))
            {
                throw new System.ArgumentException(string.Format(CultureInfo.CurrentCulture, "Invalid type for binding. Expected type: {0}. Type passed in: {1}.", typeof(ByteStreamHttpBinding).AssemblyQualifiedName, binding.GetType().AssemblyQualifiedName));
            }
            ByteStreamHttpBinding ByteStreamHttpBinding = ((ByteStreamHttpBinding)(binding));
            ByteStreamHttpBinding.HostNameComparisonMode = this.HostNameComparisonMode;
            ByteStreamHttpBinding.MaxBufferSize = this.MaxBufferSize;
            ByteStreamHttpBinding.MaxBufferPoolSize = this.MaxBufferPoolSize;
            ByteStreamHttpBinding.MaxReceivedMessageSize = this.MaxReceivedMessageSize;
            ByteStreamHttpBinding.TransferMode = this.TransferMode;

            // Copy reader quotas over if set from config.
            if (this.ReaderQuotas.MaxDepth != 0)
            {
                ByteStreamHttpBinding.ReaderQuotas.MaxDepth = this.ReaderQuotas.MaxDepth;
            }
            if (this.ReaderQuotas.MaxStringContentLength != 0)
            {
                ByteStreamHttpBinding.ReaderQuotas.MaxStringContentLength = this.ReaderQuotas.MaxStringContentLength;
            }
            if (this.ReaderQuotas.MaxArrayLength != 0)
            {
                ByteStreamHttpBinding.ReaderQuotas.MaxArrayLength = this.ReaderQuotas.MaxArrayLength;
            }
            if (this.ReaderQuotas.MaxBytesPerRead != 0)
            {
                ByteStreamHttpBinding.ReaderQuotas.MaxBytesPerRead = this.ReaderQuotas.MaxBytesPerRead;
            }
            if (this.ReaderQuotas.MaxNameTableCharCount != 0)
            {
                ByteStreamHttpBinding.ReaderQuotas.MaxNameTableCharCount = this.ReaderQuotas.MaxNameTableCharCount;
            }
        }
    }
}
