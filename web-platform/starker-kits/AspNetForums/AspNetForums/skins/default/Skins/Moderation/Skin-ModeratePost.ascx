<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Control Language="C#" %>

<table Class="tableBorder" width="100%" cellpadding="3" cellspacing="0">
  <tr>
    <td class="forumRowHighlight" align="center">
      <span class="normalTextSmallBold">Moderate Post: <Asp:Label runat="server" id="PostID" /> [</span>
      <asp:HyperLink runat="server" id="DeletePost" CssClass="menuTextLink">Delete Post</asp:HyperLink> |
      <asp:HyperLink runat="server" id="EditPost" CssClass="menuTextLink">Edit Post</asp:HyperLink> |
      <asp:HyperLink runat="server" id="ModerationHistory" CssClass="menuTextLink">Moderation History</asp:HyperLink> | 
      <asp:HyperLink runat="server" id="MovePost" CssClass="menuTextLink">Move Post</asp:HyperLink>
<%--
      <asp:LinkButton runat="server" id="TurnOffModeration" CssClass="menuTextLink">Unmoderate <asp:Label id="Username" runat="server"/></asp:LinkButton> |
      <asp:HyperLink runat="server" id="DiscussPost" CssClass="menuTextLink">Discuss Post</asp:HyperLink>
--%>
      <span class="normalTextSmallBold">]</span>
    </td>
  </tr>
</table>
