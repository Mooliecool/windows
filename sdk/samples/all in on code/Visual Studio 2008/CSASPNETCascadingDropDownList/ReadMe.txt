========================================================================
   ASP.NET web application project : CSASPNETCascadingDropDownList OverView
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The example shows how to create Cascading DropDownList in two ways,and you can 
view the Default.aspx page and then choose the one to view:

1. Use ASP.NET DropDownList control with postback.
2. Use ASP.NET DropDownList control with callback. 

The Demo has been tested in IE and FireFox.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CustomDataSource.xml is the data source file, which saves the demo data.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Use ASP.NET DropDownList control with postback:

The demo is in the CascadingDropDownList.aspx page. 

In this demo, we bind top level DropDownList on Page Load event and bind child 
DropDownList basing on previous DropDownList's selected item 
in SelectedIndexChanged event of DropDownList. 


2. Use ASP.NET DropDownList control with callback.

The demo uses CascadingDropDownListWithCallBack.aspx page and
GetDataForCallBack.aspx. The former one is the container of Three 
ASP.NET DropDownList controls;the later one is used to retrieve data from 
data source and write response to client.

In this demo, we primarily use XMLHttpRequest to get data from server and
update DropDownList using JavaScript in client. 

Please keep in mind, ASP.NET page is statusless. We can use hide field to save 
DropDownList status and restore it when page is rendering in client.  


/////////////////////////////////////////////////////////////////////////////
References:

ASP.NET DropDownList Control
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.dropdownlist.aspx

XMLHttpRequest Object
http://msdn.microsoft.com/en-us/library/ms535874(VS.85).aspx

XmlDocument Class
http://msdn.microsoft.com/en-us/library/system.xml.xmldocument.aspx


/////////////////////////////////////////////////////////////////////////////