//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.IO;
using System.ServiceModel.Channels;
using System.Xml;

namespace Microsoft.Samples.ByteStreamEncoder
{
    class ByteStreamBodyWriter : BodyWriter
    {
        string testFileName;

        public ByteStreamBodyWriter(string testFileName)
            : base(false)
        {
            this.testFileName = testFileName;
        }

        protected override void OnWriteBodyContents(XmlDictionaryWriter writer)
        {
            writer.WriteStartElement("Binary");

            FileStream fs = new FileStream(this.testFileName, FileMode.Open);
            byte[] tmp = new byte[fs.Length];
            fs.Read(tmp, 0, tmp.Length);
            writer.WriteBase64(tmp, 0, (int)tmp.Length);

            writer.WriteEndElement();
            fs.Close();
        }
    }
}
