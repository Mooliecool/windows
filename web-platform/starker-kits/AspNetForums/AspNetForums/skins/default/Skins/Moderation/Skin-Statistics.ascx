<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Control Language="C#" %>
<script runat="server">
  int i = 1;
</script>
<table class="tableBorder" cellSpacing="1" cellPadding="3" width="100%">
  <tr>
    <th class="tableHeaderText" align="left" colSpan="2">
      &nbsp;Moderation Statistics
    </th>
  </tr>
  <tr>
    <td class="forumRow" vAlign="top">
      <table CellPadding="2" CellSpacing="2">
        <tr>
          <td>
            <span class="normalTextSmaller">
            <b><asp:label id="TotalModerators" runat="server" />&nbsp;</b>moderators 
            have moderated <b><asp:label id="TotalModeratedPosts" runat="server" />&nbsp;</b>threads 
            and posts.
            </span>
          </td>
        </tr>
        <tr>
          <td>
            <span class="normalTextSmaller">
              10 most active moderators:<br> 
              <asp:Repeater id="TopModerators" runat="server">
                <ItemTemplate>
                 <%# (i++).ToString() %>. <a href='<%# Globals.UrlUserProfile + DataBinder.Eval(Container.DataItem, "Username") %>'><%# DataBinder.Eval(Container.DataItem, "Username") %></a> (<%# ((Int32)DataBinder.Eval(Container.DataItem, "TotalPostsModerated")).ToString("n0") %>)<br>
                </ItemTemplate>
              </asp:Repeater>
            </span>
          </td>
        </tr>
        <tr>
          <td>
            <span class="normalTextSmaller">
              Moderator actions:<br> 
              <asp:Repeater id="ModerationAction" runat="server">
                <ItemTemplate>
                 &nbsp;<%# DataBinder.Eval(Container.DataItem, "Action") %> (<%# ((Int32)DataBinder.Eval(Container.DataItem, "ActionSummary")).ToString("n0") %>)<br>
                </ItemTemplate>
              </asp:Repeater>
            </span>
          </td>
        </tr>
      </table>
  </tr>
</table>
