//  Copyright (c) Microsoft Corporation. All rights reserved.

using System.ServiceModel;
using System.ServiceModel.Syndication;
using System.ServiceModel.Web;

namespace Microsoft.Syndication.Samples
{
    [ServiceContract]
    interface IStreamingFeedService
    {
        [OperationContract]
        [WebGet(BodyStyle = WebMessageBodyStyle.Bare)]
        Atom10FeedFormatter StreamedFeed();
    }
}
