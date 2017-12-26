//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Net;
using System.Net.Mime;
using System.ServiceModel.Channels;
using System.ServiceModel.Syndication;
using System.ServiceModel.Web;

namespace Microsoft.Samples.AdvancedFormatSelection
{
    public class Service : IService
    {
        public Message EchoListWithGet(string list)
        {
            List<string> returnList = new List<string>(list.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries));

            // If the format query string is present, use it to determine the response format
            string formatQueryStringValue = WebOperationContext.Current.IncomingRequest.UriTemplateMatch.QueryParameters["format"];
            if (!string.IsNullOrEmpty(formatQueryStringValue))
            {
                string normalizedFormatQueryStringValue = formatQueryStringValue.ToUpperInvariant();
                switch (normalizedFormatQueryStringValue)
                {
                    case "JPEG": return CreateJpegResponse(returnList);
                    case "XHTML": return CreateXhtmlResponse(returnList);
                    case "ATOM": return CreateAtom10Response(returnList);
                    case "XML": return CreateXmlResponse(returnList);
                    case "JSON": return CreateJsonResponse(returnList);
                    default:
                        throw new WebFaultException<string>(string.Format("Unsupported format '{0}'", formatQueryStringValue), HttpStatusCode.BadRequest);
                }
            }

            // Otherwise, iterate through the Accept header elements
            IList<ContentType> acceptHeaderElements = WebOperationContext.Current.IncomingRequest.GetAcceptHeaderElements();
            for (int x = 0; x < acceptHeaderElements.Count; x++)
            {
                string normalizedMediaType = acceptHeaderElements[x].MediaType.ToLowerInvariant();
                switch (normalizedMediaType)
                {
                    case "image/jpeg": return CreateJpegResponse(returnList);
                    case "application/xhtml+xml": return CreateXhtmlResponse(returnList);
                    case "application/atom+xml": return CreateAtom10Response(returnList);
                    case "application/xml": return CreateXmlResponse(returnList);
                    case "application/json": return CreateJsonResponse(returnList);
                }
            }

            // Default response format will be Xml
            return CreateXmlResponse(returnList);
        }

        private Message CreateJsonResponse(List<string> returnList)
        {
            return WebOperationContext.Current.CreateJsonResponse(returnList);
        }

        private Message CreateXmlResponse(List<string> returnList)
        {
            return WebOperationContext.Current.CreateXmlResponse(returnList);
        }

        private Message CreateAtom10Response(List<string> returnList)
        {
            SyndicationFeed feed = new SyndicationFeed(returnList.Select((s, i) => new SyndicationItem("Item " + i.ToString(), s, null)).ToArray());
            return WebOperationContext.Current.CreateAtom10Response(feed);
        }

        private Message CreateXhtmlResponse(List<string> returnList)
        {
            XhtmlTemplate template = new XhtmlTemplate();
            template.ReturnList = returnList;
            return WebOperationContext.Current.CreateTextResponse(template.TransformText());
        }

        private Message CreateJpegResponse(List<string> returnList)
        {
            int width = 500;
            int height = (returnList.Count * 15) + 15;

            Bitmap bitmap = new Bitmap(width, height);
            Graphics graphic = Graphics.FromImage(bitmap);
            
            using (Brush brush = new SolidBrush(Color.Indigo))
            {
                graphic.FillRectangle(brush, 0, 0, width, height);
            }

            using (Brush brush = new SolidBrush(Color.WhiteSmoke))
            {
                using (Font font = new Font("Consolas", 13))
                {
                    for (int x = 0; x < returnList.Count; x++)
                    {
                        graphic.DrawString(returnList[x], font, brush, new PointF(5, 5 + (x * 15)));
                    }
                }
            }

            return WebOperationContext.Current.CreateStreamResponse(stream => bitmap.Save(stream, ImageFormat.Jpeg), "image/jpeg");
        }
    }
}
