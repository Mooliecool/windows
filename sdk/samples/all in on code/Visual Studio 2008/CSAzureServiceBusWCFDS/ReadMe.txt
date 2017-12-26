========================================================================
    APPFABRIC : CSAzureServiceBusWCFDS Solution Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

It's a common scenario that we want to expose local database to internet and cloud access.
However it involves a lot of security problems if we directly allow TCP connections
to local SQL Server. 

In this scenario, another difficulty is to scale the entire system. 

Windows Azure platform AppFabric can help us resolve both of above issues.

The solution uses the following techniques/products:

*Service Bus in Windows Azure platform AppFabric
*WCF Data Services (formerly named ADO.NET Data Services)
*SQL Server

Service Bus in Windows Azure platform AppFabric provides us the flexibility and the scalability
of the entire solution.(You can refer to the LoadBalance sample in AppFabric SDK to learn details
about how to use Service Bus to do load balance for your service. We don't cover it in this sample
for the simplicity purpose)

Service Bus also allows you to expose the local data to the intenet, so you can consume the data
in your cloud applications. While this sample ships with a standard ASP.NET client, you can easily
convert the client to a Web Role for a Windows Azure solution.

WCF Data Services works as an intermediate node that can provide additional access control and other
business logic to meet your needs.You may refer to CSADONETDataService/VBADONETDataService samples to
learn more about it. WCF Data Services has more to do with WCF than ADO.NET. The sample uses a WCF
message inspector to workaround a potential issue in Service Bus where POST/PUT verbs do not work properly.

SQL Server is the backend database.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Windows Azure platform AppFabric SDK V1.0
http://www.microsoft.com/downloads/details.aspx?familyid=39856A03-1490-4283-908F-C8BF0BFAD8A5&displaylang=en


/////////////////////////////////////////////////////////////////////////////
Demo:

A. Change code and settings to meet your environment

1.Press Control+H to open "Find and Replace" window.Look in "Entire Solution".

2.Replace [Your Connection String of Northwind Database] with your connection string of 
Northwind Database.

3.Replace [Your Secret] with Current Management Key of your service. 
You can find Current Management Key from our web portal (replace [Your Service Namespace] 
with your service namespace):
https://appfabric.azure.com/ServiceNamespace.aspx?ServiceNamespace=[Your Service Namespace]

4.Replace [Your Service Namespace] with your service namespace.


B. Run Service

1.Start debugging CSAzureServiceBusWCFDS project. Input your service namespace and press enter.

2.Wait several seconds for service start and after you see message "Press [Enter] to exit"
the service has been started.

C. Test Service in browser

1.Stop debugging service and modify Program.cs to test in browser.Replace:

            WebHttpRelayBinding binding = new WebHttpRelayBinding(EndToEndWebHttpSecurityMode.Transport, RelayClientAuthenticationType.RelayAccessToken);
            with:
            WebHttpRelayBinding binding = new WebHttpRelayBinding(EndToEndWebHttpSecurityMode.Transport, RelayClientAuthenticationType.None);
            
2.Repeat step B.

3.Follow the instruction shown on the console window to access the following URI in browser to test
(replace [Your Service Namespace] with your service namespace):
https://[Your Service Namespace].servicebus.windows.net/DataService/Customers

2.View the source of the page you'll be able to see all records in Customers table in Northwind DB.

D.Consume Service in ASP.NET

1. Stop debugging service and modify Program.cs to change code back to:

            WebHttpRelayBinding binding = new WebHttpRelayBinding(EndToEndWebHttpSecurityMode.Transport, RelayClientAuthenticationType.RelayAccessToken);
            
2. Repeat step B.

3. Start debugging Client project. You'll be able to view the records in database and insert records.
To keep it as concise as possible we only use simple select and insert queries. After insert
you can check your database to view the records or you can insert a record with CustomerID="abc" so that
it will be returned by the show-first-10-records query and shown on the page.If you want to know how to 
do other queries please refer to WCF Data Services samples in AIO Framework.


/////////////////////////////////////////////////////////////////////////////
References:

WCF Data Services
http://msdn.microsoft.com/en-us/data/bb931106.aspx

Message Inspectors
http://msdn.microsoft.com/en-us/library/aa717047.aspx

Creating a Custom AppFabric Service Bus Binding
http://msdn.microsoft.com/en-us/library/dd582769.aspx


/////////////////////////////////////////////////////////////////////////////