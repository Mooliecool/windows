<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Control Language="C#" %>

<table Class="tableBorder" width="100%" cellpadding="3" cellspacing="0">
  <tr>
    <td class="forumRowHighlight">
      <asp:LinkButton runat="server" id="ApprovePost" CssClass="linkSmallBold">Approve Post</asp:LinkButton> | 
      <asp:LinkButton runat="server" id="ApprovePostAndTurnOffModeration" CssClass="linkSmallBold">Approve Post and turn off moderation for <asp:Label id="Username" runat="server"/></asp:LinkButton> |
      <asp:LinkButton runat="server" id="ApprovePostAndReply" CssClass="linkSmallBold">Approve Post and reply</asp:LinkButton> |
      <asp:LinkButton runat="server" id="ApprovePostAndEdit" CssClass="linkSmallBold">Approve Post and edit</asp:LinkButton> |
      <asp:HyperLink runat="server" id="DeletePost" CssClass="linkSmallBold">Delete Post</asp:HyperLink> |
      <asp:HyperLink runat="server" id="MovePost" CssClass="linkSmallBold">Move Post</asp:HyperLink>
      <br>
      <span class="normalTextSmallBold">PostID: </span><span class="normalTextSmall"><Asp:Label runat="server" id="PostID" /></span>
    </td>
  </tr>
</table>