using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Globalization;
using System.Data.SqlClient;
using System.ServiceModel.Channels;
using System.ServiceModel;
using System.Collections.Specialized;
using System.Security;
using System.Diagnostics;
using System.IO;
using System.Xml.XPath;

namespace Microsoft.ServiceModel.Samples
{
    [ServiceContract]
    public interface IUniversalContract
    {
        //You must mark up an OperationContract to receive any action because there is no 
        //soap message (and thus no action) defined in the http request
        //Further, we need to work with the Http message properties, so we'll accept and recieve
        //the Message type on our contract
        [OperationContract(Action = "*", ReplyAction = "*")]
        Message ProcessMessage(Message request);
    }


    public class RssService : IUniversalContract
    {
        #region "private members"

        static XmlDocument rssDocument;
        const string startRss =
@"
<rss version='2.0'>
  <channel>
<title>CalcNews</title> 
  <link>http://CalcNews.Com/</link> 
  <description /> 
  <managingEditor>Bernard Reinmann</managingEditor> 
  <generator>.Text Version 0.95.2004.102</generator>
";

        const string endRss =
@"
  </channel>
</rss>
";
        #endregion

        #region "constructor"

        static RssService()
        {
            Type type = typeof(RssService);
            string resource = "rss.Rss.xml";
            using (Stream stream = type.Assembly.GetManifestResourceStream(type, resource))
            {
                rssDocument = new XmlDocument();
                rssDocument.Load(stream);
            }
        }

        #endregion

        #region "Universal Contract"

        public Message ProcessMessage(Message request)
        {
            MessageVersion version = OperationContext.Current.RequestContext.RequestMessage.Version;

            //grab the HttpRequestMessageProperty so we can access the http method and the query string
            HttpRequestMessageProperty httpRequestProperty
                = request.Properties[HttpRequestMessageProperty.Name] as HttpRequestMessageProperty;

            if (httpRequestProperty == null)
            {
                throw new InvalidOperationException("The HttpRequestProperty could not be found.  " +
                    "The RssService depends on the native Indigo Http stack");
            }

            Message response = null;
            HttpResponseMessageProperty httpResponseProperty = new HttpResponseMessageProperty();

            XmlReader result = null;

            if (httpRequestProperty.Method == "GET")
            {
                try
                {
                    //Get the Rss into an XmlReader
                    result = GetRss(httpRequestProperty);
                    //Set the status code for the http response to OK
                    httpResponseProperty.StatusCode = System.Net.HttpStatusCode.OK;
                }
                catch (InvalidQueryStringException queryStringException)
                {
                    //if we get an exception, we need to set an appropriate StatusCode (in this case 400 Bad Request)
                    httpResponseProperty.StatusCode = System.Net.HttpStatusCode.BadRequest;
                    //The QueryStringException has meaningful information for the caller so we'll put it in the StatusDescription
                    httpResponseProperty.StatusDescription = queryStringException.Message;
                    //We don't have anything to send in the http body so we'll supress it
                    httpResponseProperty.SuppressEntityBody = true;
                }
            }
            else
            {
                //Set the status code indicating that this service does not support the method that was used
                httpResponseProperty.StatusCode = System.Net.HttpStatusCode.MethodNotAllowed;
                //We don't have anything to send in the http body so we'll supress it
                httpResponseProperty.SuppressEntityBody = true;
            }

            //create the response message - the RSS in the XmlReader is placed into the message body
            //unless there was an exception in which case it is null
            response = Message.CreateMessage(version, "urn:GetResponse", result);

            //Add the response property so the outbound message will have the
            //HTTP status code we want.
            response.Properties[HttpResponseMessageProperty.Name] = httpResponseProperty;

            return response;
        }

        #endregion

        #region "implementation"

        //expecting something like:  http://localhost/ServiceModelSamples/service.svc or
        //                           http://localhost/ServiceModelSamples/service.svc?categoryName=Geometry
        XmlReader GetRss(HttpRequestMessageProperty request)
        {
            //QueryStringDictionary parses the query string into a name value dictionary
            QueryStringDictionary queryStringDictionary = new QueryStringDictionary(request);

            string categoryName = queryStringDictionary["categoryName"];

            //if the query string has more than 1 entry then it is malformed
            if (queryStringDictionary.Count > 1
                //if the query string  has an entry and it is not categoryName, then it is malformed
                || ((queryStringDictionary.Count > 0) && (string.IsNullOrEmpty(categoryName))))
            {
                throw new InvalidQueryStringException("If a query string is present, the only permissible parameter is categoryName.");
            }

            MemoryStream stream = new MemoryStream();
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.ConformanceLevel = ConformanceLevel.Auto;
            XmlWriter writer = XmlWriter.Create(stream, settings);

            writer.WriteRaw(startRss);
            WriteRssItems(categoryName, writer);
            writer.WriteRaw(endRss);

            writer.Flush();
            stream.Seek(0, SeekOrigin.Begin);

            return XmlReader.Create(stream);
        }

        private void WriteRssItems(string categoryName, XmlWriter writer)
        {
            //if the categoryName is not present, we'll select all the items
            string predicate = string.IsNullOrEmpty(categoryName) ? "root/item" : "root/item[category/text()='" + categoryName + "']";

            //select the item nodes that have a child named category with the text node matching category
            XPathNavigator docNavigator = rssDocument.CreateNavigator();
            XPathNodeIterator items = (XPathNodeIterator)docNavigator.Select(predicate);

            while (items.MoveNext())
            {
                //write them out
                writer.WriteNode(items.Current, true);

            }
        }

        #endregion
    }

    [global::System.Serializable]
    public class InvalidQueryStringException : Exception
    {
        const string defaultMessage = "Excpected query string: team='blah'&statusby='blah'";

        public InvalidQueryStringException() : base(defaultMessage) { }
        public InvalidQueryStringException(string message) : base(message) { }
        public InvalidQueryStringException(string message, Exception inner) : base(message, inner) { }
        protected InvalidQueryStringException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context)
            : base(info, context) { }
    }
}
