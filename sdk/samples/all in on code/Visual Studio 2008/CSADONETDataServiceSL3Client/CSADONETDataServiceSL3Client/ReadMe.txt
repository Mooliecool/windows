====================================================================================
     SILVERLIGHT APPLICATION : CSADONETDataServiceSL3Client Project Overview
====================================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSADONETDataServiceSL3Client example demonstrates how to access ADO.NET 
Data Services in Silverlight. It creates three proxies that maps three different
services at server side, which respectively uses ADO.NET Entity Data Model, 
Linq to SQL Data Classes, and non-relational in-memory data as data source.

This sample demonstrates how to select, update, delete and insert data via REST
calls to ADO.NET Data Services. These REST messages will be automatically created
by the proxy class. We can also create/send/receive messages on our own but using 
proxy is much more easier.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSADONETDataServiceSL3Client -> CSADONETDataService -> SQLServer2005DB
CSADONETDataServiceSL3Client sends async REST call to CSADONETDataService to
retrieve data.
CSADONETDataService accesses the database created by SQLServer2005DB.


/////////////////////////////////////////////////////////////////////////////
Note:

Before running this project please make sure you've deployed CSADONETDataService
and changed the URL of the service (such as "http://localhost/SchoolLinqToEntities.svc"
in SchoolLinqToEntitiesUpdate.xaml.cs) to your own one. Also make sure the web
page that hosts Silverlight application are in the same domain of ADO.NET Data Service.
For more details about this requirement, please check out "Known Issue" section
of this file.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Select.
   (1) Initialize a DataServiceContext object.
   
   (2) Create a DataServiceQuery<T> object.
   
   (3) Call the DataServiceQuery<T>.BeginExecute() method to begin an
	   async REST call. Hook up a callback method which will be called after
	   query complete.
	   
   (4) Call DataServiceQuery<T>.EndExecute() method to end the query
       and retrieve data.
       
2. Update.
   
   (1) Maintain a reference of DataServiceContext object that is retrieved by
	   select operation. 
	   
   (2) Call DataServiceContext.UpdateObject() method.
   
   (3) Call DataServiceContext.BeginSaveChanges() method to begin an async REST
       call. Hook up a callback method which will be called after query complete.
   
   (4) Call DataServiceContext.EndSaveChanges() method to end the query.
   
3. Delete.

   (1) Maintain a reference of DataServiceContext object that is retrieved by
	   select operation. 
	   
   (2) Call DataServiceContext.DeleteObject() method.
   
   (3) Call DataServiceContext.BeginSaveChanges() method to begin an async REST
       call. Hook up a callback method which will be called after query complete.
   
   (4) Call DataServiceContext.EndSaveChanges() method to end the query.
   
4. Insert.

   (1) Maintain a reference of DataServiceContext object that is retrieved by
	   select operation. 
	   
   (2) Call DataServiceContext.AddObject() method. (Or a more friendly method 
       such as AddToCategories() in this sample)
   
   (3) Call DataServiceContext.BeginSaveChanges() method to begin an async REST
       call. Hook up a callback method which will be called after query complete.
   
   (4) Call DataServiceContext.EndSaveChanges() method to end the query.
   
   
/////////////////////////////////////////////////////////////////////////////
Known Issue:

If the web page that hosts Silverlight application and the ADO.NET Data Services 
are from different domains, the sample will not work in FireFox. Because IE 8 
allows you to do cross-domain access from XmlHttpRequest if have the option set 
or brings up a prompt if you don’t. Firefox silently fails when you attempt 
Cross-domain XmlHttpRequest.

To work around this issue, you have two options:

1. Use WebClient or HttpWebRequest instead of the generated proxy. Below is the 
sample code to update a CourseGrade record:

bool httpResult = WebRequest.RegisterPrefix("http://", WebRequestCreator.ClientHttp);          
decimal grade = 2;
string s = string.Format(@"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<entry xmlns:d=""http://schemas.microsoft.com/ado/2007/08/dataservices"" xmlns:m=""http://schemas.microsoft.com/ado/2007/08/dataservices/metadata"" xmlns=""http://www.w3.org/2005/Atom"">
  <category scheme=""http://schemas.microsoft.com/ado/2007/08/dataservices/scheme"" term=""SQLServer2005DBModel.CourseGrade"" />
  <title />
  <updated>{0}</updated>
  <author>
    <name />
  </author>
  <id>http://yourdomain/SchoolLinqToEntities.svc/CourseGrade(7)</id>
  <content type=""application/xml"">
    <m:properties>
      <d:EnrollmentID m:type=""Edm.Int32"">7</d:EnrollmentID>
      <d:Grade m:type=""Edm.Decimal"">{1}</d:Grade>
    </m:properties>
  </content>
</entry>",DateTime.Now.ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ"),grade);
            byte[] buffer = System.Text.Encoding.UTF8.GetBytes(s);
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(new Uri("http://yourdomain/SchoolLinqToEntities.svc/CourseGrade(7)"));
            request.Method = "POST";
            request.Headers["x-http-method"] = "PUT";
            request.ContentType = "application/atom+xml";
            request.Headers["dataserviceversion"] = "1.0;Silverlight";
            request.Headers["maxdataserviceversion"] = "1.0;Silverlight";
            request.BeginGetRequestStream((asynchronousResult) => {
                HttpWebRequest r = (HttpWebRequest)asynchronousResult.AsyncState;
                // End the operation.
                Stream postStream = r.EndGetRequestStream(asynchronousResult);
                postStream.Write(buffer, 0, buffer.Length);
                postStream.Close();

                request.BeginGetResponse((asynchronousResult2) => {
                    HttpWebRequest r2 = (HttpWebRequest)asynchronousResult2.AsyncState;
                    HttpWebResponse resp = (HttpWebResponse)r2.EndGetResponse(asynchronousResult2);
                    Stream streamResponse = resp.GetResponseStream();
                    StreamReader streamRead = new StreamReader(streamResponse);
                    string responseString = streamRead.ReadToEnd();
                    // Close the stream object.
                    streamResponse.Close();
                    streamRead.Close();

                    // Release the HttpWebResponse.
                    resp.Close();
                }, request);
            }, request);
        }

For more details about the REST HTTP messages, please refer to:
http://msdn.microsoft.com/en-us/library/dd672595(VS.100).aspx

Or you can create a test client of ADO.NET Data Service and use tools like Fiddler to view the 
correct messages directly. 

This approach is a little overwhelming because we have to do a lot of stuffs to 
request/receive/parse messages.

2. Add a web service in the same domain of the web site that hosts Silverlight application.
The web service acts as a proxy. Silverlight application access the web service and web service
retrieve data from ADO.NET Data Service on behalf of Silverlight application.

For more details please check out:
http://blogs.msdn.com/phaniraj/archive/2008/10/21/accessing-cross-domain-ado-net-data-services-from-the-silverlight-client-library.aspx
http://blogs.msdn.com/tom_laird-mcconnell/archive/2009/03/25/creating-an-ado-net-data-service-proxy-as-workaround-for-silverlight-ado-net-cross-domain-issue.aspx


/////////////////////////////////////////////////////////////////////////////
References:

ADO.NET Data Services (Silverlight)
http://msdn.microsoft.com/en-us/library/cc838234(VS.95).aspx

How to: Specify Browser or Client HTTP Handling
http://blogs.msdn.com/silverlight_sdk/archive/2009/08/12/new-networking-stack-in-silverlight-3.aspx