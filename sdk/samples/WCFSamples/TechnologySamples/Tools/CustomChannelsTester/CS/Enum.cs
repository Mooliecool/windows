namespace System.ServiceModel.Samples.CustomChannelsTester
{
    using System;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.Diagnostics;
    using System.ServiceModel;
    using System.ServiceModel.Channels;
    using System.Text;   

    //Contract options possible through the Test Spec
    public enum ContractOption
    {
        True,
        False,
        Both,
    }

    //All possible types of service contracts specified in the Interfaces.cs
    public enum ServiceContract
    {
        IAsyncSessionOneWay,
        IAsyncSessionTwoWay,
        IAsyncOneWay,
        IAsyncTwoWay,
        ISyncSessionOneWay,
        ISyncSessionTwoWay,
        ISyncOneWay,
        ISyncTwoWay,
        IDuplexContract,
        IDuplexSessionContract
    }
    
}
