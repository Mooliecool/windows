========================================================================
   ASP.NET web application project : CSASPNETCacheAPI OverView
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The example shows how to use Cache API. You can open the Default.aspx page 
and then choose the one to view:

1. Simple Cache object.

2. Use Cache with File-Based dependence. 

3. Use Cache with Key-Based dependence. 

4. Use Cache with absolute Time-Based dependence. 

5. Use Cache with sliding Time-Based dependence. 

6. Use Cache with CallBack. 


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CustomDataSource.xml is the data source file, which is used in File-Based
dependence Cache.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Simple Cache object.

  Cache is key/value pair object. We can simple add value to it and save it
  in memory. To remove it, you can use Cache.Remove method. 


2. Use Cache with File-Based dependence. 

  Set file dependence for Cache. In this demo, the cached time will be changed
  when you change the file "CustomDataSource.xml" in App_Data folder.


3. Use Cache with Key-Based dependence. 

   Use another Cache object as dependence.When that Cache is expiration 
   the current Cache will be expiration too.

4. Use Cache with sliding Time-Based dependence.
 
   Set Cache's sliding expiration to 10 seconds. So the cached datetime will 
   be changed when the Cache is not requested within 10 seconds.

5. Use Cache with absolute Time-Based dependence.

   Set Cache's absolute expiration to 10 seconds. So the cached datetime will 
   be changed after 10 seconds.

6. Use Cache with CallBack. 

    Set Cache's absolute time to 10s and call RemovedCallback method when 
    Cache is expiration.	


/////////////////////////////////////////////////////////////////////////////
References:

ASP.NET Caching Overview
http://msdn.microsoft.com/en-us/library/ms178597(VS.80).aspx

Cache Class
http://msdn.microsoft.com/en-us/library/system.web.caching.cache.aspx


/////////////////////////////////////////////////////////////////////////////