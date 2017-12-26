========================================================================
    Web APPLICATION : VBASPNETMenu Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Introduction:

The sample demonstrates how to bind the ASP.NET Menu Control to the Database.
All the contents of the Menu control are generated dynamically, if we want to 
add some new navigation items into the website, we only need to insert some 
data to the database instead of modifying the source code. It is more 
convenient for us to finish a navigation module.
 

/////////////////////////////////////////////////////////////////////////////
Code principle:

1. In order to simplify the sample, we create a function "GetData" that 
includes a dataset to hold the memory data instead of using a database. 
2. Create a function "GenerateMenuItem" to generate the menu item. 
   The detailed logic is shown below: 
   a. Get the data from the "GetData" function.
   b. Use two foreach loops to generate menu items for the menu control. The first
      one is used to generate master items. And the second one is used to generate
      the child items based on the master items. 
   c. Set the NavigateUrl and Name for each menu item.



/////////////////////////////////////////////////////////////////////////////
Menu control style:

ASP.NET menu control also provides some style templates for us to modify the appearance
of it like the other controls.
Goto design panel>right-click menu control>select AutoFormat, then we can choose
some style template for the menu control.

/////////////////////////////////////////////////////////////////////////////
The structure of the tables in Database:

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
References:

ASP.NET Menu Control Overview 
http://msdn.microsoft.com/en-us/library/ecs0x9w5(VS.80).aspx


/////////////////////////////////////////////////////////////////////////////