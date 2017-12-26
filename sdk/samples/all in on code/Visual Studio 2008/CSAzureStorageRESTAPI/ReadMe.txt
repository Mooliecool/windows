========================================================================
    AZURESTORAGE : CSAzureStorageRESTAPI Solution Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Sometimes we need to use raw REST API instead of the StorageClient class
provided by SDK. i.e. inserting an entity to table storage without schema, writing
a "StorageClient" library in other programming languages, etc.This sample shows
how to generate an HTTP message that uses the List Blobs API. You may reuse the
code to add authentication header to call other REST APIs.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Windows Azure Tools for Microsoft Visual Studio
http://www.microsoft.com/downloads/en/details.aspx?FamilyID=7a1089b6-4050-4307-86c4-9dadaa5ed018


/////////////////////////////////////////////////////////////////////////////
Demo:

A. Make sure Storage Emulator is running.

B. Start debugging.

C. Input the name of a container in Blob storage and press <ENTER>.

D. Observe the output that lists all blobs information in that container.



/////////////////////////////////////////////////////////////////////////////
References:

Differences Between the Storage Emulator and Windows Azure Storage Services
http://msdn.microsoft.com/en-us/library/gg433135.aspx

List Blobs
http://msdn.microsoft.com/en-us/library/dd135734.aspx

Authentication Schemes
http://msdn.microsoft.com/en-us/library/dd179428.aspx


/////////////////////////////////////////////////////////////////////////////