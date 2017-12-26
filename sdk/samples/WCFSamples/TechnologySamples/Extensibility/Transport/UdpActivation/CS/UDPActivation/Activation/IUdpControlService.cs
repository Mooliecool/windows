// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;
using System.Runtime.Serialization;
using System.ServiceModel.Channels;
using Microsoft.ServiceModel.Samples;

namespace Microsoft.ServiceModel.Samples.Activation
{
    [DataContract]
    class ControlRegistrationData
    {
        [DataMember]
        Uri uri;

        [DataMember]
        int instanceId;

        public ControlRegistrationData() { }

        public Uri Uri
        {
            get
            {
                return this.uri;
            }

            set
            {
                this.uri = value;
            }
        }

        public int InstanceId
        {
            get
            {
                return this.instanceId;
            }

            set
            {
                this.instanceId = value;
            }
        }
    }

    [ServiceContract(SessionMode=SessionMode.Required)]
    interface IUdpControlCallback
    {
        [OperationContract]
        void Dispatch(FramingData data);
    }

    [ServiceContract(SessionMode = SessionMode.Required, CallbackContract = typeof(IUdpControlCallback))]
    interface IUdpControlRegistration
    {
        [OperationContract]
        void Register(ControlRegistrationData data);
    }
}
