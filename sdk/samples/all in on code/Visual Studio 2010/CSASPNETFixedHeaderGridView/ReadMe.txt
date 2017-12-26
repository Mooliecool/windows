=============================================================================
                  CSASPNETFixedHeaderGridView Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The GridView control in ASP.NET usually loads many rows of data with a 
vertical scroll bar.  When users scroll using vertical bar, the header of the 
GridView will move and disappear. This sample illustrates how to fix the 
position of the GridView header by using JQuery.  The approach demoed here 
works for various web browsers on the client side (IE, Chrome, FireFox, 
Safari, etc).


/////////////////////////////////////////////////////////////////////////////
Demo:

Please follow these demonstration steps below.

Step 1: Open the CSASPNETFixedHeaderGridView.sln.

Step 2: Expand the CSASPNETFixedHeaderGridView web application and press 
        Ctrl + F5 to show the Default.aspx page.

Step 3: You will see a GridView control on the page, please scroll the 
        vertical scroll bar, the header line of the GridView not move 
		with the scroll bar.

Step 5: Please browser this page with some other browsers, such as Chrome,
        FireFox, Safari, etc.

Step 6: Validation finished.

/////////////////////////////////////////////////////////////////////////////
Implementation:

Step 1. Create a C# "ASP.NET Empty Web Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "CSASPNETFixedHeaderGridView".

Step 2. Add one web form and named it as "Default.aspx".

Step 3. Create a folder named "JScript" and copy the code of sample file in
        your JS files. You can also download of them from these websites:
		http://docs.jquery.com/Downloading_jQuery
		http://plugins.jquery.com/project/fixedtableheader

Step 4. Add a GridView on Default.aspx page, and add create a data table as 
        GridView's data source. Here we give the code of Default.aspx.cs file.

		[code]
            // Define a data table as GridView's data source.
            DataTable tab = new DataTable();
            tab.Columns.Add("a", Type.GetType("System.String"));
            tab.Columns.Add("b", Type.GetType("System.String"));
            tab.Columns.Add("c", Type.GetType("System.String"));
            tab.Columns.Add("d", Type.GetType("System.String"));
            tab.Columns.Add("e", Type.GetType("System.String"));
            tab.Columns.Add("f", Type.GetType("System.String"));
            tab.Columns.Add("g", Type.GetType("System.String"));
            tab.Columns.Add("h", Type.GetType("System.String"));
            tab.Columns.Add("i", Type.GetType("System.String"));
            tab.Columns.Add("j", Type.GetType("System.String"));
            for (int i = 0; i < 35; i++)
            {
                DataRow dr = tab.NewRow();
                dr["a"] = string.Format("row:{0} columns:a", i.ToString().PadLeft(2,'0'));
                dr["b"] = string.Format("row:{0} columns:b", i.ToString().PadLeft(2, '0'));
                dr["c"] = string.Format("row:{0} columns:c", i.ToString().PadLeft(2, '0'));
                dr["d"] = string.Format("row:{0} columns:d", i.ToString().PadLeft(2, '0'));
                dr["e"] = string.Format("row:{0} columns:e", i.ToString().PadLeft(2, '0'));
                dr["f"] = string.Format("row:{0} columns:f", i.ToString().PadLeft(2, '0'));
                dr["g"] = string.Format("row:{0} columns:g", i.ToString().PadLeft(2, '0'));
                dr["h"] = string.Format("row:{0} columns:h", i.ToString().PadLeft(2, '0'));
                dr["i"] = string.Format("row:{0} columns:i", i.ToString().PadLeft(2, '0'));
                dr["j"] = string.Format("row:{0} columns:j", i.ToString().PadLeft(2, '0'));
                tab.Rows.Add(dr);
            }

            // Data bind method.
            gvwList.DataSource = tab;
            gvwList.DataBind();
	    [/code]

Step 5. Now we need bind the specifically columns of data table to GridView's columns,
        The code of Default.aspx page will be like this:

        [code]
		    <Columns>
                <asp:BoundField DataField="a" FooterText="titile a" 
            HeaderText="titile a" />
                <asp:BoundField DataField="b" FooterText="titile b" 
            HeaderText="titile b" />
                <asp:BoundField DataField="c" FooterText="titile c" 
            HeaderText="titile c" />
                <asp:BoundField DataField="d" FooterText="titile d" 
            HeaderText="titile d" />
                <asp:BoundField DataField="e" FooterText="titile e" 
            HeaderText="titile e" />
                <asp:BoundField DataField="f" FooterText="titile f" 
            HeaderText="titile f" />
                <asp:BoundField DataField="g" FooterText="titile g" 
            HeaderText="titile g" />
                <asp:BoundField DataField="h" FooterText="titile h" 
            HeaderText="titile h" />
                <asp:BoundField DataField="i" FooterText="titile i" 
            HeaderText="titile i" />
                <asp:BoundField DataField="j" FooterText="titile j" 
            HeaderText="titile j" />
            </Columns>
		[/code]

Step 6. The last of step, involve the JQuery libraries and write the JQuery 
        function to fixed the header of GridView control.
		The JQuery code will be like this:

		[code]
		    <script src="JScript/jquery-1.4.4.min.js" type="text/javascript"></script>
            <script src="JScript/ScrollableGridPlugin.js" type="text/javascript"></script>
            <script type = "text/javascript">
                $(document).ready(function () {
                    //Invoke Scrollable function.
                    $('#<%=gvwList.ClientID %>').Scrollable({
                    ScrollHeight: 600,        
                    });
                });

            </script>
		[/code]

Step 7. Build the application and you can debug it.

/////////////////////////////////////////////////////////////////////////////
References:

MSDN: JQuery
http://wiki.asp.net/page.aspx/1047/jquery/

MSDN: GridView Class
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.gridview.aspx


/////////////////////////////////////////////////////////////////////////////