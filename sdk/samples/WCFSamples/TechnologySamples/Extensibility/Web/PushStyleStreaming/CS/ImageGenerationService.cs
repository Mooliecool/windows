//  Copyright (c) Microsoft Corporation. All rights reserved.

using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Web;

namespace Microsoft.WebProgrammingModel.Samples
{
    [ServiceContract]
    public class ImageGenerationService
    {

        [OperationContract]
        [WebGet(UriTemplate = "images")]
        public Message GetDynamicImage()
        {
            string text = WebOperationContext.Current.IncomingRequest.UriTemplateMatch.QueryParameters["text"];

            Bitmap theBitmap = GenerateImage(text);

            Message response = StreamMessageHelper.CreateMessage(MessageVersion.None, "", "image/jpeg",
                delegate(Stream outputStream)
                {
                    theBitmap.Save(outputStream, ImageFormat.Jpeg);
                });

            return response;
        }

        [OperationContract]
        [WebGet(UriTemplate = "text")]
        public Message GetDynamicText()
        {
            string text = WebOperationContext.Current.IncomingRequest.UriTemplateMatch.QueryParameters["text"];
            Message response = StreamMessageHelper.CreateMessage(MessageVersion.None, "", "text/plain",

                delegate(Stream s)
                {
                    TextWriter writer = new StreamWriter(s);

                    writer.WriteLine("You said: ");
                    writer.WriteLine(text);
                    writer.WriteLine("Didn't you?");
                    writer.Flush();
                });

            return response;
        }

        private Bitmap GenerateImage(string text)
        {
            Bitmap bitmap = new Bitmap(468, 60);

            Graphics g = Graphics.FromImage(bitmap);
            Brush brush = new SolidBrush(Color.Indigo);

            g.FillRectangle(brush, 0, 0, 468, 60);

            brush = new SolidBrush(Color.WhiteSmoke);
            g.DrawString(text, new Font("Consolas", 13), brush, new PointF(5, 5));

            return bitmap;
        }
    }
}
