=============================================================================
  ASP.NET APPLICATION : CSASPNETDataListImageGallery Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

  The project illustrates how to implement an image gallery with DataList

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1. Create a C# ASP.NET Web Application in Visual Studio 2008 and name it 
as CSASPNETDataListImageGallery.

Step2. Drag a DataList onto page and set RepeatColumns="5" 
RepeatDirection="Horizontal".

<asp:DataList ID="DataList1" runat="server" RepeatColumns="5" 
RepeatDirection="Horizontal" >

Step3. Set template in DataList, which binds to Url field.

<ItemTemplate>
  <asp:ImageButton ID="imgBtn" runat="server" 
   ImageUrl='<%# "/Image/" + Eval("Url") %>' Width="100px" Height="100px" 
   OnClick="imgBtn_Click" CommandArgument='<%# Container.ItemIndex %>' />
</ItemTemplate>

Step4. Add four buttons for page navigation.

<asp:LinkButton ID="lbnFirstPage" Text="First" CommandName="first" 
  OnCommand="Page_OnClick" runat="server" Width="125px" />
<asp:LinkButton ID="lbnPrevPage" Text="Prev" CommandName="prev" 
  OnCommand="Page_OnClick" runat="server" Width="125px" />
<asp:LinkButton ID="lbnNextPage" Text="Next" CommandName="next" 
  OnCommand="Page_OnClick" runat="server" Width="125px" />
<asp:LinkButton ID="lbnLastPage" Text="Last" CommandName="last" 
  OnCommand="Page_OnClick" runat="server" Width="125px" />

Step5: Open the code file of page (Default.aspx.cs)

Step6: Import System.Data and System.IO namespace to page.

using System.Data;
using System.IO;

Step7: Create two properties for page index and page count.

        // Property for current page index.
        public int Page_Index
        {
            get { return (int)ViewState["_Page_Index"]; }
            set { ViewState["_Page_Index"] = value; }
        }

        // Property for total page count.
        public int Page_Count
        {
            get { return (int)ViewState["_Page_Count"]; }
            set { ViewState["_Page_Count"] = value; }
        }

Step8: Return number of images.

        // Return total number of images.
        protected int ImageCount()
        {
            DirectoryInfo di = new DirectoryInfo(Server.MapPath("/Image/"));
            FileInfo[] fi = di.GetFiles();
            return fi.GetLength(0);
        }

Step9: Bind DataList

        // Return the data source for DataList.
        protected DataTable BindGrid()
        {
            // Get all image paths.             
            DirectoryInfo di = new DirectoryInfo(Server.MapPath("/Image/"));
            FileInfo[] fi = di.GetFiles();

            // Save all paths to the DataTable as the data source.
            DataTable dt = new DataTable();
            DataColumn dc = new DataColumn("Url", typeof(System.String));
            dt.Columns.Add(dc);
            int lastindex = 0;
            if (Page_Count == 0 || Page_Index == Page_Count - 1)
            {
                lastindex = ImageCount();
            }
            else
            {
                lastindex = Page_Index * _pageSize + 5;
            }
            for (int i = Page_Index * _pageSize; i < lastindex; i++)
            {
                DataRow dro = dt.NewRow();
                dro[0] = fi[i].Name;
                dt.Rows.Add(dro);
            }
            return dt;
        }

Step10: Handle paging buttons.

        // Handle the navigation button event.
        public void Page_OnClick(Object sender, CommandEventArgs e)
        {
            if (e.CommandName.Equals("first"))
            {
                Page_Index = 0;
                lbnFirstPage.Enabled = false;
                lbnPrevPage.Enabled = false;
                lbnNextPage.Enabled = true;
                lbnLastPage.Enabled = true;
            }
            else if (e.CommandName.Equals("prev"))
            {
                Page_Index -= 1;
                if (Page_Index == 0)
                {
                    lbnFirstPage.Enabled = false;
                    lbnPrevPage.Enabled = false;
                    lbnNextPage.Enabled = true;
                    lbnLastPage.Enabled = true;
                }
                else
                {
                    lbnFirstPage.Enabled = true;
                    lbnPrevPage.Enabled = true;
                    lbnNextPage.Enabled = true;
                    lbnLastPage.Enabled = true;
                }
            }
            else if (e.CommandName.Equals("next"))
            {
                Page_Index += 1;
                if (Page_Index == Page_Count - 1)
                {
                    lbnFirstPage.Enabled = true;
                    lbnPrevPage.Enabled = true;
                    lbnNextPage.Enabled = false;
                    lbnLastPage.Enabled = false;
                }
                else
                {
                    lbnFirstPage.Enabled = true;
                    lbnPrevPage.Enabled = true;
                    lbnNextPage.Enabled = true;
                    lbnLastPage.Enabled = true;
                }
            }
            else if (e.CommandName.Equals("last"))
            {
                Page_Index = Page_Count - 1;
                lbnFirstPage.Enabled = true;
                lbnPrevPage.Enabled = true;
                lbnNextPage.Enabled = false;
                lbnLastPage.Enabled = false;
            }

            DataList1.SelectedIndex = 0;
            DataList1.DataSource = BindGrid();
            DataList1.DataBind();
            Image1.ImageUrl 
                = ((Image)DataList1.Items[0].FindControl("imgBtn")).ImageUrl;
        }

Step11: Handle image click event.

        // Handle the thumbnail image selecting event.
        protected void imgBtn_Click(object sender, EventArgs e)
        {
            ImageButton ib = (ImageButton)sender;
            Image1.ImageUrl = ib.ImageUrl;
            DataList1.SelectedIndex = Convert.ToInt32(ib.CommandArgument);
        }
        
/////////////////////////////////////////////////////////////////////////////
References:

MSDN: 
#DataList Server Control
http://msdn.microsoft.com/en-us/library/9cx2f3ks(VS.85).aspx

MSDN: 
#Desiding When to Use the DataGrid, DataList and Repeater
http://msdn.microsoft.com/en-us/library/aa479015.aspx

ASP.NET: 
#Efficient Data Paging with the ASP.NET 2.0 DataList Control and ObjectDataSource 
http://weblogs.asp.net/scottgu/archive/2006/01/07/434787.aspx

/////////////////////////////////////////////////////////////////////////////