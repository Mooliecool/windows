// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;
using System.ServiceModel.Channels;
using Microsoft.ServiceModel.Samples;

namespace Microsoft.ServiceModel.Samples.Activation
{
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.PerSession,
        ConcurrencyMode=ConcurrencyMode.Multiple)]
    class UdpControlService : IUdpControlRegistration
    {
        App app;
        IUdpControlCallback controlCallback;
        ControlRegistrationData data;
        public void Register(ControlRegistrationData data)
        {
            this.data = data;
            UdpListenerAdapter.Register(data, this);
            controlCallback = OperationContext.Current.GetCallbackChannel<IUdpControlCallback>();
        }

        internal void SetApp(App app)
        {
            this.app = app;
        }

        internal void Dispatch()
        {
            FramingData framingData = app.EndDequeue(app.BeginDequeue());
            controlCallback.Dispatch(framingData);

            UdpListenerAdapter.Dispatch(data, this);
        }
    }
}
