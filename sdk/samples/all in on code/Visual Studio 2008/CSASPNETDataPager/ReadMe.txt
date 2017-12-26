========================================================================
    ASP.NET APPLICATION : CSASPNETDataPager Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSASPNETDataPager sample describes how to use ASP.NET DataPager to 
render a paging interface and communicate to the corresponding data-bound 
control.

DataPager control Provides paging functionality for data-bound controls that 
implement the IPageableItemContainer interface, such as the ListView control.

In this sample, the ListView control is populated with data from SQL Server 
database in ADO.NET way. The sample uses the SQLServer2005DB sample database.  


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a C# ASP.NET Web Application named CSASPNETDataPager in 
Visual Studio 2008 / Visual Web Developer.


Step2. Drag a ListView control and a DataPager control into an 
ASP.NET Web Form page. And then rename the controls as follows:
	
	ListView1  -> lvPerson
	DataPager1 -> dpPerson
	
	
Step3. Add the following three templates in lvPerson: LayoutTemplate, 
ItemTemplate and AlternatingItemTemplate.

For a ListView control, in order to display content, the LayoutTemplate and 
ItemTemplate are required. In this sample, we use the following three 
templates.

	(1) LayoutTemplate: The root template that defines a container object, 
	such as a table, div, or span element, that will contain the content 
	defined in the ItemTemplate or GroupTemplate template. 

	The LayoutTemplate content must include a placeholder control, which has 
	the runat attribute set to "server" and the ID attribute set to the 
	value of the ItemPlaceholderID or the GroupPlaceholderID property. 
	
	We display the title of the person list in this template.

	(2) ItemTemplate: Defines the data-bound content to display for 
	individual items.
	
	In this template, we display the retrieved person information from the 
	SQL Server table.
	
	(3) AlternatingItemTemplate: Defines the content to render for 
	alternating items to make it easier to distinguish between consecutive 
	items.
	
	We specify a different background color from ItemTemplate to make the 
	ListView look better.
	 
Related references:	
	
MSDN: ListView Class
MSDN: ListView Web Server Control Overview
MSDN: PlaceHolder Class


Step4. Copy the BindListView method from this sample and paste it 
in code-behind. We can use a SqlDataReader here since it's forward-only and 
cannot go backwards.


Step5. Navigate to the Property panel of dpPerson, set the property 
PagedControlID to lvPerson, which measn lvPerson will be paged by dpPerson.

Related reference:
MSDN: DataPager.PagedControlID Property 


Step6. Navigate to the Property panel of dpPerson and then switch to Event. 
Double-click on the event PreRender to generate the event handler 
in code-behind.

/////////////////////////////////////////////////////////////////////////////
    protected void dpPerson_PreRender(object sender, EventArgs e)
    {
        BindListView();
    }
/////////////////////////////////////////////////////////////////////////////
    
This event occurs after control object is loaded but prior to rendering. 
We use it to populate the ListView lvPerson.

Related reference:
MSDN: Control.PreRender Event


Step7. Right-click on dpPerson, select Show Smart Tag. On the Smart Tag, 
click Edit Pager Fields and add a Next/Previous Pager Field and 
three Template Pager Fields to create the custom paging UI.

Select the Next/Previous Pager Field and enable the following properties 
in the right panel:

	ShowFirstPageButton
	ShowLastPageButton

Related references:

MSDN: NextPreviousPagerField Class
MSDN: TemplatePagerField Class


Step8. In the Source view, copy and paste the following markup:

	(1) This TemplatePagerField is used to display the current page number,
	 the total number of pages and the total number of rows.

    /////////////////////////////////////////////////////////////////////////	
	<asp:TemplatePagerField>
        <PagerTemplate>
            Page
            <asp:Label runat="server" ID="lbCurrentPage" 
              Text="<%# Container.TotalRowCount>0 ? (Container.StartRowIndex / Container.PageSize) + 1 : 0 %>" />
            of
            <asp:Label runat="server" ID="lbTotalPages" 
              Text="<%# Math.Ceiling ((double)Container.TotalRowCount / Container.PageSize) %>" />
            (Total:
            <asp:Label runat="server" ID="lbTotalItems" 
              Text="<%# Container.TotalRowCount%>" />
            records)                      
        </PagerTemplate>
    </asp:TemplatePagerField>
    /////////////////////////////////////////////////////////////////////////
    
    (2) This TemplatePagerField shows the pager number in intervals, 
    like 1-10, 11-20 and so on.
    
    We also need to add a PagerCommand method in code-behind. So that when a 
    button is clicked in this TemplatePagerField,we can handle its behavior. 
    
    We will add the method in following steps.
    
    /////////////////////////////////////////////////////////////////////////   
    <asp:TemplatePagerField OnPagerCommand="TemplateNextPrevious_OnPagerCommand">
        <PagerTemplate>                     
            <asp:LinkButton ID="lbtnFirst" runat="server" CommandName="First" 
            Text="First" Visible='<%# Container.StartRowIndex > 0 %>' />

            <asp:LinkButton ID="lbtnPrevious" runat="server" CommandName="Previous" 
            Text='<%# (Container.StartRowIndex - Container.PageSize + 1) + " - " + (Container.StartRowIndex) %>'
            Visible='<%# Container.StartRowIndex > 0 %>' />
            
            <asp:Label ID="lbtnCurrent" runat="server"
            Text='<%# (Container.StartRowIndex + 1) + "-" + (Container.StartRowIndex + Container.PageSize > Container.TotalRowCount ? Container.TotalRowCount : Container.StartRowIndex + Container.PageSize) %>' />
           
            <asp:LinkButton ID="lbtnNext" runat="server" CommandName="Next"
            Text='<%# (Container.StartRowIndex + Container.PageSize + 1) + " - " + (Container.StartRowIndex + Container.PageSize*2 > Container.TotalRowCount ? Container.TotalRowCount : Container.StartRowIndex + Container.PageSize*2) %>' 
            Visible='<%# (Container.StartRowIndex + Container.PageSize) < Container.TotalRowCount %>' />

            <asp:LinkButton ID="lbtnLast" runat="server" CommandName="Last" 
            Text="Last" Visible='<%# (Container.StartRowIndex + Container.PageSize) < Container.TotalRowCount %>' />                        
        </PagerTemplate>
    </asp:TemplatePagerField>    
    /////////////////////////////////////////////////////////////////////////
    
    (3) In this TemplatePagerField, we can type a page number and click the 
    GoTo button to jump to that page.
    
    This TemplatePagerField also requires a PagerCommand method in code-behind.    
    
    /////////////////////////////////////////////////////////////////////////    
    <asp:TemplatePagerField OnPagerCommand = "TemplateGoTo_OnPagerCommand">            
        <PagerTemplate>
            <br /><br />
            <asp:TextBox ID="tbPageNumber" runat="server" Width="30px"
            Text="<%# Container.TotalRowCount>0 ? (Container.StartRowIndex / Container.PageSize) + 1 : 0 %>" ></asp:TextBox>
            <asp:Button ID="btnGoTo" runat="server" Text="GoTo" />
        </PagerTemplate>            
    </asp:TemplatePagerField>    
    /////////////////////////////////////////////////////////////////////////

Related references:
 
MSDN: TemplatePagerField.PagerCommand Event
MSDN: TemplatePagerField.OnPagerCommand Method
   
We can also editing the page fields as follows:

Right-click on dpPerson, select Show Smart Tag. On the Smart Tag, click 
Edit Templates to edit the added three Template Pager Fields. We can switch 
between them via the Display dropdownlist.


Step9. Copy the following methods from this sample and paste them in 
code-behind.

	TemplateNextPrevious_OnPagerCommand
	TemplateGoTo_OnPagerCommand




/////////////////////////////////////////////////////////////////////////////
References:

MSDN: ListView Class
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.listview.aspx

MSDN: ListView Web Server Control Overview
http://msdn.microsoft.com/en-us/library/bb398790.aspx

MSDN: PlaceHolder Class
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.placeholder.aspx

MSDN: DataPager.PagedControlID Property
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.datapager.pagedcontrolid.aspx 

MSDN: Control.PreRender Event
http://msdn.microsoft.com/en-us/library/system.web.ui.control.prerender.aspx

MSDN: NextPreviousPagerField Class
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.nextpreviouspagerfield.aspx

MSDN: TemplatePagerField Class
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.templatepagerfield.aspx

MSDN: TemplatePagerField.PagerCommand Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.templatepagerfield.pagercommand.aspx

MSDN: TemplatePagerField.OnPagerCommand Method
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.templatepagerfield.onpagercommand.aspx

/////////////////////////////////////////////////////////////////////////////
