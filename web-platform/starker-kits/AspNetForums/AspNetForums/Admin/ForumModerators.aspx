<%@ Page %>
<% @Register TagPrefix="WebForums" TagName="EditForumModerators" Src="~/Controls/EditForumModerators.ascx" %>
<% @Register TagPrefix="WebForums" TagName="Header" Src="~/Controls/Header.ascx" %>
<% @Register TagPrefix="WebForums" TagName="Footer" Src="~/Controls/Footer.ascx" %>
<% @Import Namespace="WebForums" %>
<script language="c#" runat="server">

	void Page_Load(Object sender, EventArgs e)
	{
		// read in the ForumID from the querystring
		if (Request.Params["ForumID"] == null)
		{
			Response.Write("Did not get passed in a ForumID!");
			Response.End();
		}
		 else
		{
			objForumModerators.ForumID = Convert.ToInt32(Request.Params["ForumID"]);
		}
	}

</script>

<WebForums:Header Title="WebForums.NET Administration : Forum Moderator Administration" ShowAdminBox="True"
			runat="server" />

	<li><a class="Normal" href="default.aspx">Return to the Administration page...</a>
	<p>
	<WebForums:EditForumModerators runat="server" id="objForumModerators" />
	<p>
	<li><a class="Normal" href="default.aspx">Return to the Administration page...</a>
<WebForums:Footer runat="server" />