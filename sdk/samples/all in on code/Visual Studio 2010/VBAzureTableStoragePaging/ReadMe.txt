========================================================================
    WINDOWSAZURE : VBAzureTableStoragePaging Solution Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

It's a common scenario that we want to use paging for table storage. However,
due to current limitation of Table storage it only supports continuation token
that can help us implement a simple previous/next paging. This sample demonstrates
how to implement previous/next paging in a MVC application. The classes in the 
TableStoragePagingUtility.vb file can be reused for other applications. If you want 
to reuse the code, what you need to do is to implement custom ICachedDataProvider<T> 
class to store data required by TableStoragePagingUtility<T>.

Please note due to the limitation of the Table storage we could only do forward-only
reading, which means if we want to get the latest data we have to read from the begining.
In this sample we just fetch data from Table storage and save it in cache. All data
is read from cache.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Windows Azure Tools for Microsoft Visual Studio
http://www.microsoft.com/downloads/en/details.aspx?FamilyID=7a1089b6-4050-4307-86c4-9dadaa5ed018


/////////////////////////////////////////////////////////////////////////////
Demo:

A. Make sure CloudService is set as start up project. Press F5 to start debugging.
B. Click "Add data to test first" link on the page.
C. View the table shown on the page.
D. Click "Next" or "Previous" link to test the paging function.


/////////////////////////////////////////////////////////////////////////////
References:

Query Timeout and Pagination
http://msdn.microsoft.com/en-us/library/dd135718.aspx


/////////////////////////////////////////////////////////////////////////////