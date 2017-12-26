<%@ Page Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>

<HTML>
  <HEAD>
    <link rel="stylesheet" href='<%= Context.Items["AppPath"] + "AspNetWeb.css" %>' type="text/css" >
      <AspNetForums:StyleSkin runat="server" ID="Styleskin1" />
  </HEAD>
  <body>
    <form runat="server" ID="Form1">
      <AspNetForums:NavigationMenu id="NavigationMenu2" title="ASP.NET Discussion Forum" runat="server" Description="A free discussion system for ASP.NET" />
      <SPAN class=normalTextSmall>Edit and delete posts, change forum settings, and assign users to serve as moderators for particular forums.</SPAN>
      <P>
      <AspNetForums:ForumGroupRepeater ShowAllForumGroups="true" runat="server" id="ForumGroupRepeater1">
        <HeaderTemplate>
          <table cellpadding="3" cellspacing="1" class="tableBorder" width="100%">
            <tr>
              <th height="25" class="tableHeaderText" align="left">
                &nbsp;Edit Forums
              </th>
            </tr>
        </HeaderTemplate>
        <ItemTemplate>
            <tr>
              <td class="forumHeaderBackgroundAlternate" height="20">
                <span class="forumTitle">
                  <%# DataBinder.Eval(Container.DataItem, "Name") %>
                </span>
              </td>
            </tr>
            
            <AspNetForums:ForumRepeater ShowAllForums="true" ForumGroupID='<%# DataBinder.Eval(Container.DataItem, "ForumGroupID" ) %>' runat="server" ID="Forumrepeater1">
              <ItemTemplate>
              <tr>
                <td class="forumRow">
                  <span class="forumTitle">
                    <a href="EditForum.aspx?ForumId=<%# DataBinder.Eval(Container.DataItem, "ForumID") %>"><%# DataBinder.Eval(Container.DataItem, "Name") %></a>
                  </span>
                </td>
              </tr>
              </ItemTemplate>
            </AspNetForums:ForumRepeater>
        </ItemTemplate>
        
        <FooterTemplate>
          </table>  
        </FooterTemplate>
      </AspNetForums:ForumGroupRepeater>
      
      <p />
      <a class="normalTextSmall" href="default.aspx">Return to the Administration page</a>
    </form>
  </body>
</HTML>
