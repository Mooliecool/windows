//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Xml;

namespace Microsoft.Samples.ByteStreamEncoder
{
    class Client
    {
        const string TestFileName = "smallsuccess.gif";

        public static void Main()
        {
            // Typically this request would be constructed by a web browser or non-WCF application instead of using WCF

            Console.WriteLine("Starting client with ByteStreamHttpBinding");

            using (ChannelFactory<IHttpHandler> cf = new ChannelFactory<IHttpHandler>("byteStreamHttpBinding"))
            {
                IHttpHandler channel = cf.CreateChannel();
                Console.WriteLine("Client channel created");

                Message byteStream = Message.CreateMessage(MessageVersion.None, "*", new ByteStreamBodyWriter(TestFileName));
                HttpRequestMessageProperty httpRequestProperty = new HttpRequestMessageProperty();
                httpRequestProperty.Headers.Add("Content-Type", "application/octet-stream");
                byteStream.Properties.Add(HttpRequestMessageProperty.Name, httpRequestProperty);

                Console.WriteLine("Client calling service");
                Message reply = channel.ProcessRequest(byteStream);

                //Get bytes from the reply 
                XmlDictionaryReader reader = reply.GetReaderAtBodyContents();
                reader.MoveToElement();
                String name = reader.Name;
                Console.WriteLine("First element in the byteStream message is : <" + name + ">");
                byte[] array = reader.ReadElementContentAsBase64();
                String replyMessage = System.Text.Encoding.UTF8.GetString(array);
                Console.WriteLine("Client received a reply from service of length :" + replyMessage.Length);
            }

            Console.WriteLine("Done");
            Console.WriteLine("Press <ENTER> to exit client");
            Console.ReadLine();
        }
    }
}
