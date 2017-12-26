<%@ Page SmartNavigation="true" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<HTML>
  <HEAD>
    <AspNetForums:StyleSkin runat="server" ID="Styleskin1" />
  </HEAD>
  <body>
    <form runat="server" ID="Form1">
      <AspNetForums:NavigationMenu id="NavigationMenu2" title="ASP.NET Discussion Forum" runat="server" Description="A free discussion system for ASP.NET" />
      <p>
      <table cellpadding="3" cellspacing="1" class="tableBorder" width="100%">
        <tr>
          <th colspan="2" class="forumHeaderBackground" align="left">
            <span class="tableHeaderTextLarge">
            &nbsp;Edit Forums
            </span>
          </th>
        </tr>
        <AspNetForums:ForumAdmin runat="server" CssClass="Normal" HeaderStyle-CssClass="Head" CreateNewForum-BackColor="#cccccc" ID="Forumadmin1" />
      </table>
      <p />
      <a class="normalTextSmall" href="default.aspx">Return to the Administration page</a>
    </form>
  </body>
</HTML>