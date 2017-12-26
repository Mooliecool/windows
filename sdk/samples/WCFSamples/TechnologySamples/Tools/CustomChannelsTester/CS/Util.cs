namespace System.ServiceModel.Samples.CustomChannelsTester
{
    using System;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.Diagnostics;
    using System.Reflection;
    using System.ServiceModel;
    using System.ServiceModel.Channels;
    using System.Text;
    using System.Threading;
    using System.Xml;
    using System.Xml.Schema;
    using System.Xml.XPath;
    

    public static class Util
    {
        public static Binding GetBinding(string bindingName)
        {
            Binding binding = null;
            Type bindingType;
            Assembly bindingAssembly;
            if (Parameters.BindingAssembly != null)
            {
                bindingType = Parameters.BindingType;
            }
            else
            {
                bindingType = typeof(Binding);
                bindingAssembly = bindingType.Assembly;
                bindingType = bindingAssembly.GetType("System.ServiceModel." + bindingName);
            }            
            binding = Activator.CreateInstance(bindingType, true) as Binding;

            if (Parameters.IsCallBack != ContractOption.False)
            {
                if (Parameters.ClientCallBackAddress.ToString() != null)
                {
                    if (binding.CreateBindingElements().Find<CompositeDuplexBindingElement>() != null)
                    {
                        BindingElementCollection bec = binding.CreateBindingElements();
                        bec.Find<CompositeDuplexBindingElement>().ClientBaseAddress = Parameters.ClientCallBackAddress;
                        binding = new CustomBinding(bec);
                    }
                }
            }

            return binding;
        }

        public static Uri GetUri(Binding binding)
        {
            Uri uri = null;
            UriBuilder uriBuilder = null;
            if(Parameters.ServerPortNumber != 0)
                uriBuilder = new UriBuilder(binding.Scheme, Parameters.ServerMachineName, Parameters.ServerPortNumber);
            else
                uriBuilder = new UriBuilder(binding.Scheme, Parameters.ServerMachineName);
            uri = uriBuilder.Uri;            
            return uri;
        }
    }
}
