<%@ Page %>
<% @Register TagPrefix="AspNetForums" TagName="Header" Src="~/Controls/Header.ascx" %>
<% @Register TagPrefix="AspNetForums" TagName="Footer" Src="~/Controls/Footer.ascx" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<script language="C#" runat="server">

	void Page_Load(Object sender, EventArgs e)
	{
		User user = Users.GetUserInfo(User.Identity.Name);
		if (!user.Administrator)
			Response.Redirect(Globals.MessagesPath + "CannotAdminister.aspx");


		if (Request.QueryString["PostID"] != null)
		{
			// we are editing a post from the admin page
			objEditPost.Mode = CreateEditPostMode.EditPost;
			objEditPost.PostID = Convert.ToInt32(Request.QueryString["PostID"].ToString());
		}
		else
		{
			// TODO: Handle case of blank querystring
			Response.Write("NO PARAMETERS PASSED THROUGH QUERYSTRING");
			Response.End();
		}

		if (Request.QueryString["ForumID"] != null)
			objEditPost.RedirectUrl = "EditForum.aspx?ForumID=" + Request.QueryString["ForumID"].ToString();
	
	}
	
</script>

<AspNetForums:Header runat="server" ShowAdminBox="True" Title="AspNetForums.NET Administration : Editing a Post" />

<form runat="server">
  <span class="Head">Edit this Post</span><br />
  <span class="Normal">
			You have chosen to edit a post that's still waiting moderation.
			Please make the changes needed.
		</span>
  <br />
  <AspNetForums:CreateEditPost runat="server" id="objEditPost" HeaderStyle-CssClass="Head" WarningStyle-CssClass="Warning" ButtonStyle-CssClass="CommandButton">

    <Post CssClass="Normal" />
    <PreviewPost CssClass="Normal" BackColor="#cccccc" />
    <ReplyToPost CssClass="Normal" HorizontalAlign="center" BackColor="#dddddd" />


  </AspNetForums:CreateEditPost>
</form>

<AspNetForums:Footer runat="server" />