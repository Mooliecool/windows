<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Page language="c#" %>
<HTML>
    <HEAD>
        <script runat="server">
      public string imagePathForAdminIcon;
      public string imagePathForUserIcon;
  

      /*****************************************************
      // Page_Load
      //
      /// <summary>Event to handle Page Load event</summary>
      //
      //***************************************************/
      public void Page_Load(Object sender, EventArgs e) {
        // Build Image path based on Skin
        imagePathForAdminIcon = Globals.ApplicationVRoot + "/Skins/" + Globals.Skin + "/images/administration_icon.gif";
        imagePathForUserIcon = Globals.ApplicationVRoot + "/Skins/" + Globals.Skin + "/images/users_icon.gif";
      }
        </script>
        <AspNetForums:StyleSkin runat="server" ID="Styleskin1" />
    </HEAD>
    <body>
        <AspNetForums:NavigationMenu id="NavigationMenu2" title="ASP.NET Discussion Forum" runat="server" Description="A free discussion system for ASP.NET"></AspNetForums:NavigationMenu>
        <p></p>
        <table width="100%" cellpadding="3" cellspacing="1" border="0" class="tableBorder">
            <tr>
                <th height="25" class="tableHeaderText" align="left" colspan="2">
                    &nbsp;Administration
                </th>
            </tr>
            <tr>
                <td class="forumHeaderBackgroundAlternate" colspan="2" height="25">
                    <span class="forumTitle">
            User Administration
          </span>
                </td>
            </tr>
            <tr>
                <td width="20" height="45" class="forumRow">
                    <img src="<% =imagePathForUserIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="UserAdmin.aspx">Edit Existing Users</a>
                    <br>
                    <span class="normalTextSmall">
		          View and change user information as well as ban/unban users.
	          </span>
                </td>
            </tr>
            <tr>
                <td width="20" height="45" class="forumRow">
                    <img src="<% =imagePathForUserIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="CreateUser.aspx">Create New Users</a>
                    <br>
                    <span class="normalTextSmall">
		          Create new users.
	          </span>
                </td>
            </tr>
            <tr>
                <td class="forumHeaderBackgroundAlternate" colspan="2" height="25">
                    <span class="forumTitle">
           Forum Administration
          </span>
                </td>
            </tr>
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="Forums.aspx">Edit Forum(s)</a>
                    <br>
                    <span class="normalTextSmall">
		          Edit and delete posts, change forum settings, and assign users to serve as moderators for particular
		          forums.
	          </span>
                </td>
            </tr>
<%--
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="CreateNewForum.aspx">Change forum sort order</a>
                    <br>
                    <span class="normalTextSmall">
            Control the order in which forums are displayed.
	  </span>
                </td>
            </tr>
--%>
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="CreateNewForum.aspx">Create new Forum</a>
                    <br>
                    <span class="normalTextSmall">
            Create new forums.
	  </span>
                </td>
            </tr>
            <tr>
                <td class="forumHeaderBackgroundAlternate" colspan="2" height="25">
                    <span class="forumTitle">
            Forum Group Administration
          </span>
                </td>
            </tr>
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="ForumGroups.aspx">Edit Forum Group(s)</a>
                    <br>
                    <span class="normalTextSmall">
          Edit, delete, and control the sort order of existing forum groups.
          </span>
                </td>
            </tr>
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="CreateEditForumGroup.aspx">Create new Forum Group</a>
                    <br>
                    <span class="normalTextSmall">Create new forum groups (used to categorize forums).</span>
                </td>
            </tr>
            <tr>
                <td class="forumHeaderBackgroundAlternate" colspan="2" height="25">
                    <span class="forumTitle">Role Administration</span>
                </td>
            </tr>
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="CreateEditRole.aspx">Edit/Delete User Role(s)</a>
                    <br>
                    <span class="normalTextSmall">Edit/Delete existing user security roles.</span>
                </td>
            </tr>
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="CreateEditRole.aspx?Action=CreateRole">Create New Role</a>
                    <br>
                    <span class="normalTextSmall">Create a new user security role.  Roles can be used to control forums access, moderation, and access to tools.</span>
                </td>
            </tr>
            <tr>
                <td class="forumHeaderBackgroundAlternate" colspan="2" height="25">
                    <span class="forumTitle">Email Administration</span>
                </td>
            </tr>
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="EmailAdmin.aspx">Email Administration</a>
                    <br>
                    <span class="normalTextSmall">
		          When certain events occur, email messages may be sent off to particular users or groups of
		          users.  These emails that are sent off conform to a specified email template.  Click here
		          to view and edit these various email templates.</span>
                </td>
            </tr>
            <tr>
                <td class="forumHeaderBackgroundAlternate" colspan="2" height="25">
                    <span class="forumTitle">
            Message Administration
          </span>
                </td>
            </tr>
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="MessageAdmin.aspx">Message Administration</a>
                    <br>
                    <span class="normalTextSmall">
            Messages may be generated for errors, such as an invalid Forum or Post ID, or for general information, such as attempting to insert a duplicate post or a new post that requires moderation. This admin tool allows you to edit these messages.		       
          </span>
                </td>
            </tr>
<%--
Next Version

            <tr>
                <td class="forumHeaderBackgroundAlternate" colspan="2" height="25">
                    <span class="forumTitle">
            Reports
          </span>
                </td>
            </tr>
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="../Reports/Users.aspx">User Statistics</a>
                    <br>
                    <span class="normalTextSmall">
            Total number of accounts that have logged in, visitor count in last: day; 2 days; 3 days; 7 days, and top 25 posters.
          </span>
                </td>
            </tr>
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="../Reports/Posts.aspx">Post Statistics</a>
                    <br>
                    <span class="normalTextSmall">
            All topics in last day; 2 days; 3 days that have gone unanswered.
          </span>
                </td>
            </tr>
            <tr>
                <td class="forumRow" width="20" height="45">
                    <img src="<% =imagePathForAdminIcon%>">
                </td>
                <td class="forumRow" align="left">
                    <a class="forumTitle" href="../Reports/Moderation.aspx">Moderation Statistics</a>
                    <br>
                    <span class="normalTextSmall">
            Top moderators, moderation actions.
          </span>
                </td>
            </tr>
--%>
        </table>
    </body>
</HTML>
