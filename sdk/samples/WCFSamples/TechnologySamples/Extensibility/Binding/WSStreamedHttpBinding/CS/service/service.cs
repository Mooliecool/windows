using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;
using System.IO;
using System.Configuration;
using System.ServiceModel.Channels;

namespace Microsoft.ServiceModel.Samples
{
    [ServiceContract(Namespace="http://Microsoft.ServiceModel.Samples")]
    public interface IStreamedEchoService
    {
        [OperationContract]
        Stream Echo(Stream data);
    }

    public class StreamedEchoService : IStreamedEchoService
    {
        public Stream Echo(Stream data)
        {
            MemoryStream dataStorage = new MemoryStream();
            byte[] byteArray = new byte[8192];
            int bytesRead = data.Read(byteArray, 0, 8192);
            while (bytesRead > 0)
            {
                dataStorage.Write(byteArray, 0, bytesRead);
                bytesRead = data.Read(byteArray, 0, 8192);
            }
            data.Close();
            dataStorage.Seek(0, SeekOrigin.Begin);

            return dataStorage;
        }
    }
}
