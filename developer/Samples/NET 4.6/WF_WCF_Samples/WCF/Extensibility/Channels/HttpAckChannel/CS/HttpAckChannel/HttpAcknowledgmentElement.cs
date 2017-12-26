//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.ServiceModel.Channels;
using System.ServiceModel.Configuration;

namespace Microsoft.Samples.HttpAckChannel 
{
    public class HttpAcknowledgmentElement : BindingElementExtensionElement 
    {
        public HttpAcknowledgmentElement() 
        {
        }
        
        public override Type BindingElementType 
        {
            get 
            {
                return typeof(HttpAcknowledgmentBindingElement);
            }
        }
        
        protected override BindingElement CreateBindingElement() 
        {
            HttpAcknowledgmentBindingElement httpAcknowledgmentBindingElement 
                = new HttpAcknowledgmentBindingElement();
            this.ApplyConfiguration(httpAcknowledgmentBindingElement);
            return httpAcknowledgmentBindingElement;
        }
        
        public override void ApplyConfiguration(BindingElement bindingElement) 
        {
            base.ApplyConfiguration(bindingElement);
            HttpAcknowledgmentBindingElement httpAcknowledgmentBindingElement 
                = ((HttpAcknowledgmentBindingElement)(bindingElement));
        }
        
        protected override void InitializeFrom(BindingElement bindingElement) 
        {
            base.InitializeFrom(bindingElement);
            HttpAcknowledgmentBindingElement httpAcknowledgmentBindingElement 
                = ((HttpAcknowledgmentBindingElement)(bindingElement));
        }
        
        public override void CopyFrom(ServiceModelExtensionElement from) 
        {
            base.CopyFrom(from);
            HttpAcknowledgmentElement httpAcknowledgmentBindingElement
                = ((HttpAcknowledgmentElement)(from));
        }
    }
}
