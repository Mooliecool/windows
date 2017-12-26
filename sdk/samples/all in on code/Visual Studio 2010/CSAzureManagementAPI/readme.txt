=============================================================================
          APPLICATION : CSAzureManagementAPI Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary: 

This project shows how to create a new hosted service on Azure via Management API.


/////////////////////////////////////////////////////////////////////////////
Demo:

1.	Fill in your subscription ID to below:
       string subscriptionId = "";

2.	Fill in the management certificate thumbprint to below:

        // This is the thumbprint of the management certificate
        // make sure the certificate exists in your personal/Certificates. Please verify it by certmgr.msc
        // this certificate must be uploaded to Azure Management Portal, in Management Certificates
		    // http://msdn.microsoft.com/en-us/library/gg551726.aspx
        string certThumbprint = "";
  
3.	Fill in the ServiceName and the labelName:
 
        // This will be use as the DNS prefix, so if it exists(someone else might took it) then you
		    // will receive 409 conflict error
        string hostServiceName = "";
            
        // The Name of the hosted service
        string labelName = "";

4.	Run the program.

5.  You will see a service with name you defined in 
       // The Name of the hosted service
       string labelName = "";
    created on your Azure account.


/////////////////////////////////////////////////////////////////////////////
Implementation:

1. Sending the request xml string asynchronously.

            byte[] formData =
                   UTF8Encoding.UTF8.GetBytes(sbRequestXML.ToString());
            request.ContentLength = formData.Length;
        
            using (Stream post = request.GetRequestStream())
            {
                post.Write(formData, 0, formData.Length);
            }

            Console.WriteLine("Creating Hosted Service: " + hostServiceName);

            try
            {
                RequestState state = new RequestState();
                state.Request = request;
                IAsyncResult result = request.BeginGetResponse(new AsyncCallback(RespCallback), state);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: " + ex.Message);
            }

2. Response callback.

        private static void RespCallback(IAsyncResult result)
        {
            RequestState state = (RequestState)result.AsyncState;   // Grab the custom state object
            WebRequest request = (WebRequest)state.Request;

            HttpWebResponse response =  (HttpWebResponse)request.EndGetResponse(result); // Get the Response
         
            string statusCode = response.StatusCode.ToString();

            // A value that uniquely identifies a request made against the Management service. For an asynchronous operation, 
		        // you can call get operation status with the value of the header to determine whether the operation is complete, 
			      // has failed, or is still in progress.
            string reqId = response.GetResponseHeader("x-ms-request-id");  

            Console.WriteLine("Creation Return Value: " + statusCode);
            Console.WriteLine("RequestId: " + reqId);    
        }


/////////////////////////////////////////////////////////////////////////////
References:

Windows Azure Service Management REST API Reference
http://msdn.microsoft.com/en-us/library/ee460799.aspx

How to Add a Management Certificate to a Windows Azure Subscription
http://msdn.microsoft.com/en-us/library/gg551726.aspx


/////////////////////////////////////////////////////////////////////////////
