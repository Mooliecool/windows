//------------------------------------------------------------------------------
// <copyright file="getresponse.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------


using System;
using System.Net;
using System.IO;
using System.Text;
using System.Threading;

/// <summary>
/// Sample demonstrates:
/// 1) Using a proxy with a WebRequest.
/// 2) Getting a WebResponse 
/// </summary>
class GetResponse
{
    public static ManualResetEvent AllDone = new ManualResetEvent(false);
    const int BUFFER_SIZE = 1024;

    public static void Main(string[] args) 
    {
        try
        {
            Console.WriteLine("Returns the response output from a URL\n");
            Console.WriteLine("   Enter URL format like: http://www.microsoft.com/net/");
            Console.WriteLine("   or simply: www.microsoft.com");
            Console.WriteLine("   SSL is not supported in the SSCLI, do not use a https:// URL");

            Console.Write("\nEnter URL to get: ");
            string url = Console.ReadLine();

            if (!url.StartsWith("http://") && !url.StartsWith("https://"))
            {
                url = "http://" + url;
            }//if

            Uri HttpSite = new Uri(url);
            if (url.StartsWith("https://"))
            {
                Console.WriteLine("\nSSCLI does not support SSL, exiting ...");
                return;
            }//if

            Console.Write("\nInput proxy name or ENTER to not use a proxy: ");
            string Reply = Console.ReadLine();
            if (Reply.Length > 0)
            {
                WebProxy CurrentProxy = new WebProxy(Reply, 80);

                // Disable Proxy use when the host is local.
                CurrentProxy.BypassProxyOnLocal = true;

                //The Select method sets the proxy that all HttpWebRequest instances use.
                GlobalProxySelection.Select = CurrentProxy;
            }//if

            Console.Write("Get page synchronized ('s') or async? ('a') ");
            Reply = Console.ReadLine();
            if (Reply.ToLower() == "s")
            {
                Console.WriteLine("Getting {0} synchronized ...", url);
                GetPage(HttpSite);
            }//if
            else
            {
                if (Reply.ToLower() == "a")
                {
                    Console.WriteLine("Getting {0} asynch ...", url);
                    GetPageAsync(HttpSite);
                }//if
            }//else

            Console.WriteLine("\nPress ENTER to exit...");
            Console.ReadLine();
            return;
        }//try
        catch (Exception e)
        {
            Console.WriteLine("Exception in Main: {0}", e.ToString());
        }//catch

    }//Main

    public static void GetPage(Uri HttpSite) 
    {
        WebResponse result = null;

        try 
        {
            WebRequest CurrentWebRequest = WebRequest.Create(HttpSite);
            result = CurrentWebRequest.GetResponse();
            Stream ReceiveStream = result.GetResponseStream();

            StreamReader SReader = new StreamReader(ReceiveStream, System.Text.Encoding.UTF8);
            while (SReader.Peek() > -1)
            {
                Console.WriteLine(SReader.ReadLine());
            }//while

            Console.WriteLine("");
        }//try
        catch(Exception e) 
        {
            Console.WriteLine("\nThe request URI could not be found or was malformed");
            Console.WriteLine(e.ToString());
        }//catch
        finally 
        {
            if ( result != null ) 
            {
                result.Close();
            }//if
        }//finally
    }//GetPage()


    public static void GetPageAsync(Uri HttpSite)
    {
        // Create the request object
        HttpWebRequest CurrentWebRequest = (HttpWebRequest) WebRequest.Create(HttpSite);

        // Create the state object
        RequestState CurrentRequestState = new RequestState();
        // Add the request into the state so it can be passed around
        CurrentRequestState.Request = CurrentWebRequest;

        // Issue the async request
        IAsyncResult r = 
            (IAsyncResult) CurrentWebRequest.BeginGetResponse(new AsyncCallback(ResponseCallback), 
            CurrentRequestState);

        // Set the ManualResetEvent to Wait so that the app
        // doesn't exit until after the callback is called
        AllDone.WaitOne();

    }//GetPageAsynch()


    /// <summary>
    /// This is the callback handler for the asynch WebRequest.
    /// </summary>
    /// <param name="InResult"></param>
    private static void ResponseCallback(IAsyncResult InResult)
    {
        try {
            // Get the RequestState object from the async result
            RequestState CurrentRequestState = (RequestState)InResult.AsyncState;

            // Get the HttpWebRequest from RequestState
            HttpWebRequest CurrentWebRequest = CurrentRequestState.Request;

            // Calling EndGetResponse produces the HttpWebResponse object
            // which came from the request issued above
            HttpWebResponse CurrentWebResponse = (HttpWebResponse) CurrentWebRequest.EndGetResponse(InResult);

            // Now that we have the response, it is time to start reading
            // data from the response stream
            Stream ResponseStream = CurrentWebResponse.GetResponseStream();

            // The read is also done using async so we'll want
            // to store the stream in RequestState
            CurrentRequestState.ResponseStream = ResponseStream;

            //Now start async read of data. There is only one callback that will occur.
            // Note that CurrentRequestState.BufferRead is passed in to BeginRead.  This is
            // where the data will be read into.
            IAsyncResult iarRead = ResponseStream.BeginRead(CurrentRequestState.BufferRead, 
                0, 
                BUFFER_SIZE, 
                new AsyncCallback(ReadCallBack), 
                CurrentRequestState);
        }
        catch(Exception e) 
        {
            Console.WriteLine(e.ToString());            
            // Set the ManualResetEvent so the main thread can exit
            AllDone.Set();
        }
    }//ResponseCallback()
  
  
    /// <summary>
    /// This is the callback handler for the async stream read.
    /// Calls itself under data from stream is all read.
    /// </summary>
    /// <param name="InResult"></param>
    private static void ReadCallBack(IAsyncResult InResult)
    {
        try {
            // Get the RequestState object from the InResult
            RequestState CurrentRequestState = (RequestState)InResult.AsyncState;

            // Pull out the ResponseStream that was set in ResponseCallback
            Stream RespStream = CurrentRequestState.ResponseStream;
    		
            // At this point CurrentRequestState.BufferRead should have some data in it.
            // ReadCount will tell us if there is any data there
            int ReadCount = RespStream.EndRead(InResult);

            //If ReadCount == -1 then done, if ReadCount == 0 then no data left to read.
            if (ReadCount > 0)
            {
                // Prepare Char array buffer for converting to Unicode
                char[] charBuffer = new Char[BUFFER_SIZE];
    			
                // Convert byte stream to char array and then string
                // len shows how many characters are converted to Unicode
                int len = CurrentRequestState.StreamDecode.GetChars(CurrentRequestState.BufferRead, 
                    0, 
                    ReadCount, 
                    charBuffer, 
                    0);
                string str = new string(charBuffer, 0, len); 			

                // Append the recently read data to the RequestData stringbuilder object
                // contained in RequestState
                CurrentRequestState.RequestData.Append(str);
    			

                // Now fire off another async call to read some more data
                // Note that this will continue to get called until
                // RespStream.EndRead returns -1
                IAsyncResult tmp = RespStream.BeginRead(CurrentRequestState.BufferRead, 
                    0, 
                    BUFFER_SIZE, 
                    new AsyncCallback(ReadCallBack), 
                    CurrentRequestState);
            }//if
            else  //No data left to read.
            {
                if (CurrentRequestState.RequestData.Length > 1)
                {
                    // All of the data has been read, so display it to the console
                    string OutputString;
                    OutputString = CurrentRequestState.RequestData.ToString();
                    Console.WriteLine(OutputString);
                }//if

                // Close down the response stream
                RespStream.Close();

                // Set the ManualResetEvent so the main thread can exit
                AllDone.Set();
            }//else
        }
        catch(Exception e) 
        {
            Console.WriteLine(e.ToString());            
            // Set the ManualResetEvent so the main thread can exit
            AllDone.Set();
        }
        return;
    }//ReadCallBack()
}//class GetResponse

/// <summary>
///The RequestState class is used to pass data across async calls
/// </summary>
public class RequestState
{
    const int BUFFER_SIZE = 1024;
    public StringBuilder RequestData;
    public byte[] BufferRead;
    public HttpWebRequest Request;
    public Stream ResponseStream;
    // Create Decoder for appropriate encoding type
    public Decoder StreamDecode = Encoding.UTF8.GetDecoder();

    public RequestState()
    {
        BufferRead = new byte[BUFFER_SIZE];
        RequestData = new StringBuilder("");
        Request = null;
        ResponseStream = null;
    }//constructor
}//class RequestState
