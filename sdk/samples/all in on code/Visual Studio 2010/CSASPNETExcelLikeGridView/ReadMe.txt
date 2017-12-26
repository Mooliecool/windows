=============================================================================
         ASP.NET APPLICATION : CSASPNETExcelLikeGridView Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

  The project illustrates how to do a batch insert,delete and update instead
  of inserting,delting,updating row by row.


/////////////////////////////////////////////////////////////////////////////
Demo:

 1) Open the project and right click the "Default.aspx", choose "View In Browser";
 2) When you check a checkbox inside a GridView to mark the row to be deleted,
  the Cell will change to red.
 3) When you add a new row by clicking the Add button, the new row is green
  by default.
 4) When you change a value from a cell inside the GridView, the background
  of the cell will turn blue.
 5) When you click the Save button, all the changes (including modified,
  deleted as well as added data) will be batch executed.


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1. Create a Web Application by referring “New”-->“Project…”-->“Visual C#”-->“ASPNET Web Application”, 
name it as “CSASPNETExcelLikeGridView”.

Step2. Right click your project tag, choose “Add…”-->“New Item…”. 
Expand the “Visual Studio C#” tag and select “Sql Server Database”. Name it as “db_Persons.mdf”.
 Then right click the newly-created database in the “App_Data” folder, choose “Open”. In the left 
“Server Explorer” panel please expand the tag “db_Persons.mdf”, and right click the folder called “Tables”, 
choose “Add New Table” to create a table as what you can in the App_Data folder's structure and save it as
"tb_personInfo".


[ NOTE: You can download the free Web Developer from:
 http://www.microsoft.com/express/Web/ ]

[ NOTE: You can also download the free SQL 2008 from:
 http://www.microsoft.com/express/Database/  ]

Step3. Delete the following default folders and files created automatically 
by Visual Studio.

Account folder
Script folder
Style folder
About.aspx file
Default.aspx file
Global.asax file
Site.Master file

Step4. Add a connection string in web.config file:
<connectionStrings>
    <add name="MyConn" 
	connectionString="Data Source=.\SQLEXPRESS;AttachDbFileName=|DataDirectory|db_Persons.mdf;Integrated Security=True;User Instance=True"/>
</connectionStrings>

Step5. Right click the mouse button to create a class named "DBProcess.cs". 
Create a class like what you can see in the file to process with DB 
(For more functions please see the detail comments).

Step6. Drag and drop a GridView, add some template fields, some checkboxes as well
as an "Add" button, a "Save" button and set all properties as what I've mentioned
in the aspx markups.

Step7. In order to implement non-refresh modification symbol (cell backcolors).
We should write some JQuery functions. You can find them in the same Default.aspx
HTML markups with detail comments.


/////////////////////////////////////////////////////////////////////////////
References:

ASP.NET QuickStart Torturial:

1）http://www.asp.net/data-access/tutorials/batch-deleting-cs

2）http://www.asp.net/data-access/tutorials/batch-updating-vb

3）http://www.asp.net/data-access/tutorials/batch-inserting-vb


/////////////////////////////////////////////////////////////////////////////