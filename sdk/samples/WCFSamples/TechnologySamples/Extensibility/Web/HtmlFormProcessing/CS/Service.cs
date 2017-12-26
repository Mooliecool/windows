//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Specialized;
using System.IO;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Web;

namespace Microsoft.WebProgrammingModel.Samples
{
    [ServiceContract]
    class Service
    {
        [OperationContract]
        [WebGet(UriTemplate = "*")]
        public Message DisplayForm()
        {
            return StreamMessageHelper.CreateMessage(MessageVersion.None, "", "text/html",
                delegate(Stream output)
            {
                TextWriter writer = new StreamWriter(output);
                writer.WriteLine(
                    @"<!DOCTYPE HTML PUBLIC ""-//W3C//DTD HTML 4.0 Transitional//EN"">
                            <html>
	<head>
		<title></title>
	</head>
	<body>
	    <form action=""http://localhost:8000/FormTest/ProcessForm/X/Y"" method=POST>
	        Foo: <input name=""Foo"" type=""text"" /><br />
	        Bar: <input name=""Bar"" type=""text"" /><br />
            <input name=""Submit1"" type=""submit"" value=""submit"" />
	    </form>
	</body>
</html>");
                writer.Flush();

            });
        }


        [OperationContract]
        [WebInvoke(UriTemplate = "ProcessForm/{templateParam1}/{templateParam2}")]
        public Message ProcessForm(string templateParam1, string templateParam2, NameValueCollection formData)
        {
            DumpValues(Console.Out, templateParam1, templateParam2, formData);

            return StreamMessageHelper.CreateMessage(MessageVersion.None, "", "text/plain",
                delegate(Stream output)
            {
                TextWriter writer = new StreamWriter(output);
                DumpValues(writer, templateParam1, templateParam2, formData);
            });
        }

        private void DumpValues(TextWriter writer, string templateParam1, string templateParam2, NameValueCollection formData)
        {
            writer.WriteLine("Form processed!");
            writer.WriteLine("templateParam1: {0}", templateParam1);
            writer.WriteLine("templateParam2: {0}", templateParam2);

            foreach (string key in formData.Keys)
            {
                writer.WriteLine("{0}: {1}", key, formData[key]);
            }

            writer.Flush();
        }
    }
}
