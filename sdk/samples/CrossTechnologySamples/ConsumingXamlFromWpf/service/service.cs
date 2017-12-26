//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.ServiceModel;
using System.IO;
using System.Runtime.Serialization;

namespace Microsoft.ServiceModel.Samples
{
    // Define a service contract.
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public interface IProvideFruit
    {
        [OperationContract]
        string GetFruit(FruitType fruitType);
    }

    [DataContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public enum FruitType
    {
        [EnumMember]
        Banana,
        [EnumMember]
        Lemon,
        [EnumMember]
        Cherry,
        [EnumMember]
        Lime,
    }

    // Service class which implements the service contract.
    public class FruitService : IProvideFruit
    {
        public string GetFruit(FruitType fruitType)
        {
            Type type = this.GetType();
            string resource = "xaml." + fruitType.ToString() + ".xaml";
            using (Stream stream = type.Assembly.GetManifestResourceStream(type, resource))
            using (StreamReader reader = new StreamReader(stream))
            {
                return reader.ReadToEnd();
            }
        }
    }
}
