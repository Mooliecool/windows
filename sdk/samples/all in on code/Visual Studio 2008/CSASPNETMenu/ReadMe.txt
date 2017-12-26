========================================================================
    Web APPLICATION : CSASPNETMenu Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Introduction:
The sample demonstrates how to bind the ASP.NET Menu Control to the Database.
All the contents of the Menu control are generated dynamically, if we want to 
add some new navigation items into the website, we only need to insert some 
data to the database instead of modifying the source code. It is more 
convenient for us to finish a navigation module.
 
/////////////////////////////////////////////////////////////////////////////
The structure of the tables in Database.
Main table:
mainId int key
mainName varchar
mainUrl varchar

Child table:
childId int key
childName varchar
MainId int foreign key reference to Main table
childUrl varchar

/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
References:

ASP.NET Menu Control Overview 
http://msdn.microsoft.com/en-us/library/ecs0x9w5(VS.80).aspx


/////////////////////////////////////////////////////////////////////////////