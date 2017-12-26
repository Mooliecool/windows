//  Copyright (c) Microsoft Corporation. All rights reserved.

using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples
{
	[ServiceContractAttribute(Namespace = "http://Microsoft.ServiceModel.Samples")]
    interface IAlbumService
    {
        [OperationContract]
        Album[] GetAlbumList();

		[OperationContract]
        void AddAlbum(string title);
    }
}
