<%@ Page SmartNavigation="true" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<script language="C#" runat="server">

	void Page_Load(Object sender, EventArgs e)
	{
		if (Request.QueryString["ForumID"] != null)
		{
			objPostListing.ForumID = Convert.ToInt32(Request.QueryString["ForumID"]);

			// set the hyperlink NavigateUrl
			lnkBackToForum.NavigateUrl = Globals.UrlEditForum + Request.QueryString["ForumID"];
		}
		else
		{
			// TODO: Handle case of blank querystring
			Response.Write("NO PARAMETERS PASSED THROUGH QUERYSTRING");
			Response.End();
		}

	}
	
</script>
<HTML>
  <HEAD>
    <AspNetForums:StyleSkin runat="server" ID="Styleskin1" />
  </HEAD>
  <body>
    <form runat="server" ID="Form1">
      <table border="0" cellpadding="10" cellspacing="0" width="100%" class="tableForum">
        <tr>
          <td valign="top" width="100%">
            <AspNetForums:NavigationMenu id="NavigationMenu2" title="ASP.NET Discussion Forum" runat="server" Description="A free discussion system for ASP.NET" />
            <p>
              <AspNetForums:AdminForumPostListing runat="server" id="objPostListing" ForumTitleStyle-CssClass="Head" WarningStyle-CssClass="Warning"></AspNetForums:AdminForumPostListing>
            <p />
            <asp:hyperlink id="lnkBackToForum" runat="server" CssClass="warning" Text="Return to the Forum" />
          </td>
        </tr>
      </table>
    </form>
  </body>
</HTML>
