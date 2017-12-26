using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;
using System.ServiceModel.Channels;
using Microsoft.Samples.Channels.ChunkingChannel;
using System.IO;
namespace TestClient
{
    class Program
    {
        static void Main(string[] args)
        {
            string inputFilePath = "image.jpg"; //replace with path of file to send
            string outputFilePath = "imageout.jpg"; //replace with path to use when saving received file
            Console.WriteLine("Press enter when service is available");
            Console.ReadLine();
            ChannelFactory<TestService.ITestService> factory = new ChannelFactory<TestService.ITestService>(
                new TcpChunkingBinding(),
                new EndpointAddress("net.tcp://localhost:9000/TestService/ep1"));
            TestService.ITestService service=factory.CreateChannel();
            bool success = false;
            try
            {
                FileStream infile = new FileStream(
                    inputFilePath,
                    FileMode.Open,
                    FileAccess.Read);
                Stream echo = service.EchoStream(infile);

                FileStream outfile = new FileStream(
                outputFilePath,
                FileMode.Create,
                FileAccess.Write);

                int count;
                byte[] buffer = new byte[4096];
                while ((count = echo.Read(buffer, 0, buffer.Length)) > 0)
                {
                    outfile.Write(buffer, 0, count);
                    outfile.Flush();
                }
                infile.Close();
                echo.Close();
                outfile.Close();

                ((IClientChannel)service).Close();
                success = true;
            }
            finally
            {
                if (!success)
                {
                    ((IClientChannel)service).Abort();
                }
            }
        }
    }
}
