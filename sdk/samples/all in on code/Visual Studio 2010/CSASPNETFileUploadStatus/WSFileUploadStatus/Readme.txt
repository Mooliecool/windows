========================================================================
                 CSASPNETFileUploadStatus Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The project illustrates how to show the file upload status based on AJAX
without a third part component like ActiveX control, Flash or Silverlight.
It's also a solution for big file uploading.

/////////////////////////////////////////////////////////////////////////////
Principle:

When a file is uploading, the server will get the request data like below.
(P.S. we can get this part when we upload a file by a tool like Fiddler.)

/*---------Sample reference Start------*/
POST http://jerrywengserver/UploadControls.aspx HTTP/1.1
Accept: application/x-ms-application, image/jpeg, application/xaml+xml, 
        image/gif, image/pjpeg, application/x-ms-xbap,
        application/x-shockwave-flash, */*
Referer: http://jerrywengserver/UploadControls.aspx
Accept-Language: en-US
User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.1; WOW64;
            Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729;
            .NET CLR 3.0.30729; Media Center PC 6.0; InfoPath.3; MS-RTC LM 8;
            .NET4.0C; .NET4.0E)
Content-Type: multipart/form-data; boundary=---------------------------7da106f207ba
Accept-Encoding: gzip, deflate
Host: jerrywengserver
Content-Length: 1488
Connection: Keep-Alive
Pragma: no-cache

-----------------------------7da106f207ba
Content-Disposition: form-data; name="__VIEWSTATE"

/wEPDwUKMTI3MTMxMTcxNw9kFgICAw8WAh4HZW5jdHlwZQUTbXVsdGlwYXJ0L2Zvcm0tZGF0YWRkcrWP136t6D4d+g8BDfyR5WF+aP/yi4YARRyuOuRsO1M=
-----------------------------7da106f207ba
Content-Disposition: form-data; name="__EVENTVALIDATION"

/wEWAgL5mtyRBALt3oXMA9W4TniGaEKs/xcWf28H93S+wRcfLHr35wNo+N1v9gQ5
-----------------------------7da106f207ba
Content-Disposition: form-data; name="fuFile"; filename="C:\******\FileUploadTest.txt"
Content-Type: text/plain

*****This part is the content of the uploaed file!*****
-----------------------------7da106f207ba
Content-Disposition: form-data; name="btnUpload"

Upload
-----------------------------7da106f207ba--
/*---------Sample reference End-----*/

There are some useful information in the head part, for example,
  The content-Type of this request.
  The boundary which seperate the body part.
  The content-length of this request.
  Some request variables.
  The filename of the uploaded file and its content-type.

If we analyze the data, we can find some tips like below.
  1. All the request data is sperated by a boundary which is defined in 
     the content-type of the head part.
  2. The name and the value of one parameter is seperated by a newline.
  3. If the parameter is a file, we can get the filename and
     content-type of the file
  4. The data of the file followed the content-type of the file.

So when the server has got all these data, the uploading will be finished.
The prolem left here is how can we get to know that how much data 
the server had read and is there a way that we can control the length 
which the server read by one time.

For IIS and .Net Framework, we can control this by a HTTPModule.
Data reading will be started in BeginRequest event.
And the HttpWorkerRequest class could control the reading process.

We can use HttpWorkerRequest.GetPreloadedEntityBody() to get 
the first part of the request data which the server read. 
If the data is too large, HttpWorkerRequest.IsEntireEntityBodyIsPreloaded 
will return false, we can use HttpWorkerRequest.ReadEntityBody() to read the
left data. By this way, we can know how much data loaded and how much left.
At last, we need to send the status back to the server,
here I store the status to the Cache.

Another important issue is how the client side get the status from 
the server side without postback to the server.
The answer is to use AJAX feature. Here we use ICallBackEventHandler, 
because it is easy to handle and clear enough for us to understand the process.
We can learn how to use it from the reference at the bottom of this readme file.
We can also use jQuery ajax to call back a web service or generic handler to
get the status.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Step 1. Build the website "WSFileUploadStatus".

Step 2. View the Default.aspx in the browser.

Step 3. Select a file which you want to test the sample.

Step 4. Click Upload button. And you will see the upload status in details.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1.  Create a C# Code Project in Visual Studio 2010 or Visual Web
Developer 2010. Name it as UploadStatus.

Step 2.  Add the two references into the project, System.Web.Extension and 
System.Web.

Step 3.  Copy the code from the UploadStatus.cs from the sample folder, 
"FileUploadStatus".

Step 4.  Add an ASP.NET Module and named it as UploadProcessModule.

Step 5.  Copy the code from the UploadProcessModule.cs in the sample folder, 
"FileUploadStatus".

Step 6.  Also create the files: BinaryHelper.cs, UploadFile.cs, 
UploadFileCollection.cs and FileUploadDataManager.cs, and copy the codes.

Step 7.  Save and Build the project.
[Caution] Be sure the project is released on "ANY CPU".
We can't add this project reference which is x86 version to the website 
deployed on an x64 system platform.

Step 8.  Add a new Empty ASP.NET WebSite into the solution,
name it as WSFileUploadStatus.

Step 9.  Add the project reference, "UploadStatus", which we created at first.

Step 10.  Create a new Web User Control named as UploadStatusWindow.ascx.
We will use this user control to hold a Popup window to show 
the status information. Copy the markups from the UploadStatusWindow.ascx
in the sample folder, "WSFileUploadStatus".

Step 11. Create an ASP.NET web page named as UploadControls.aspx. Add one
FileUpload web control and one Button web control into the page. 
Copy the markups from UploadControls.aspx in the sample folder, 
"WSFileUploadStatus".

Step 12. Create an ASP.NET web page named as Deafult.aspx.
Add an iframe into the page. Set the src to "UploadControls.aspx".
Copy the javascript function and markups from the Default.aspx 
in the sample folder, "WSFileUploadStatus".

Step 13. Copy the follow two folders in the sapmle folder into the website,
"styles" and "scripts".

Step 14. Modify the web.config. Register the HttpModule. 
Set the maxRequestLength to 1048576, means max request data will be 
limited to 1GB. If we delpoy the website to IIS7, we need set 
the requestLimits in the system.webServer block.

Step 15. Make sure we have made the page same as the sample.
Build the solution.

Step 16. Test the site.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: HttpModules
http://msdn.microsoft.com/en-us/library/zec9k340(VS.71).aspx

MSDN: HttpWorkerRequest Class
http://msdn.microsoft.com/en-us/library/system.web.httpworkerrequest.aspx

MSDN: ICallBackEventHandler Interface
http://msdn.microsoft.com/en-us/library/system.web.ui.icallbackeventhandler.aspx

MSDN: An Introduction to JavaScript Object Notation (JSON) in JavaScript and .NET
http://msdn.microsoft.com/en-us/library/bb299886.aspx

MSDN: JavaScriptSerializer Class
http://msdn.microsoft.com/en-us/library/system.web.script.serialization.javascriptserializer.aspx

/////////////////////////////////////////////////////////////////////////////