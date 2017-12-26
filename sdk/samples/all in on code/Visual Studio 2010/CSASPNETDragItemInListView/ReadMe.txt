========================================================================
                 CSASPNETDragItemInListView Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The project illustrates how to drag and drop items in ListView using jQuery.
The jQuery library used in this code sample comes from http://jqueryui.com/.
This code-sample includes two ListView controls, user can drag,sort and move
items from one controls to another. this sample can be used in many areas.For 
example, you can create an application of online shopping,it will give customer 
better feelings with your application.



/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Please follow these demonstration steps below.

Step 1: Open the CSASPNETDragItemInListView.sln.

Step 2: Expand the CSASPNETDragItemInListView web application and press 
        Ctrl + F5 to show the Default.aspx page.

Step 3: You will see two ListView controls, use mouse drag items to another 
        ListView or itself, you can also sort the items by drag them to correct
	    position you want.

Step 4: Double click items in ListView controls to remove it.

Step 5: Validation finished.


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1. Create a C# "ASP.NET Empty Web Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "CSASPNETDragItemInListView".

Step 2. Add a web form named "Default.aspx" in root directory.

Step 3. Create a "XmlFile" folder and add two XML files in it, "ListView1.xml",
        "ListView2.xml".

Step 4. Filling some elements in XML file like code-sample and these data will
        bind in ListView controls.
		[code]
        <root>
          <data open="0">element 1</data>
          <data open="1">element 2</data>
          <data open="1">element 3</data>
          <data open="1">element 4</data>
          <data open="1">element 5</data>
          <data open="1">element 6</data>
          <data open="1">element 7</data>
        </root>
	    [/code]
	    Import some Jquery Javascript library in <head> tag like this:
	    [code]
		   <link href="JQuery/jquery-ui.css" rel="stylesheet" type="text/css" />
           <script src="JQuery/jquery-1.4.4.min.js" type="text/javascript"></script>
           <script src="JQuery/jquery-ui.min.js" type="text/javascript"></script>
	    [/code] 
	    Add Jquery functions in Default.aspx page,these two Jquery functions use
		to drag and drop items in ListView control :
	    [code]
	      <script type="text/javascript">
	        $(function () {
	            $("#sortable1, #sortable2").sortable({
	               connectWith: ".connectedSortable"
	            }).disableSelection();
	        });
 
	        $(document).ready(function () {
	            $("li").dblclick(function () {
	                 $(this).closest('li').remove();
	            });
	        });   
	    </script>
	    [/code]

Step 5. Wirte C# code in Default.aspx.cs page to bind XML files data:
        [code]
		    // Bind two xml data file to ListView control, actually you can change the "open" property to "0",
            // In that way, it will not display in ListView control.
            XmlDocument xmlDocument = new XmlDocument();
            using (DataTable tabListView1 = new DataTable())
            {
                tabListView1.Columns.Add("value", Type.GetType("System.String"));
                xmlDocument.Load(AppDomain.CurrentDomain.BaseDirectory + "/XmlFile/ListView1.xml");
                XmlNodeList xmlNodeList = xmlDocument.SelectNodes("root/data[@open='1']");
                foreach (XmlNode xmlNode in xmlNodeList)
                {
                    DataRow dr = tabListView1.NewRow();
                    dr["value"] = xmlNode.InnerText;
                    tabListView1.Rows.Add(dr);
                }
                ListView1.DataSource = tabListView1;
                ListView1.DataBind();
            }
		[/code]

Step 6. Build the application and you can debug it.


/////////////////////////////////////////////////////////////////////////////
References:

ASP.NET: JQuery 
http://wiki.asp.net/page.aspx/1047/jquery/

jQuery UI library
http://jqueryui.com/

MSDN: ListView Web Server Control
http://msdn.microsoft.com/en-us/library/bb398790.aspx


/////////////////////////////////////////////////////////////////////////////