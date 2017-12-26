using System;
using System.ServiceModel;
using System.IO;
using Microsoft.Samples.Channels.ChunkingChannel;
namespace TestService
{
    [ServiceContract]
    interface ITestService
    {
        [OperationContract]
        [ChunkingBehavior(ChunkingAppliesTo.Both)]
        Stream EchoStream(Stream stream);

        [OperationContract]
        Stream DownloadStream();

        [OperationContract(IsOneWay=true)]
        [ChunkingBehavior(ChunkingAppliesTo.InMessage)]
        void UploadStream(Stream stream);
    }
}
