<%@ Control Language="C#" %>
<%@ Import Namespace="AspNetForums.Components" %>

<tr>
  <td class="forumRow">
    <asp:HyperLink class="forumTitle" runat="server" NavigateUrl='<%# Globals.UrlShowForum + DataBinder.Eval(((RepeaterItem)Container).DataItem, "ForumID") %>' Text='<%# DataBinder.Eval(((RepeaterItem)Container).DataItem, "Name") %>' />
    <br>
    <asp:Label runat="server" class="normalTextSmaller" Text='<%# DataBinder.Eval(((RepeaterItem)Container).DataItem, "Description") %>' />
  </td>
  <td class="forumRow" width="30" align="center">
    <asp:Label runat="server" class="normalTextSmall" Text='<%# ((Int32)DataBinder.Eval(((RepeaterItem)Container).DataItem, "TotalPosts")).ToString("n0") %>' />
  </td>
  <td class="forumRow" width="150" align="center">
    <span class="normalTextSmall"><asp:Label runat="Server" Text='<%# ((DateTime)DataBinder.Eval( ((RepeaterItem) Container).DataItem, "MostRecentPostDate")).ToString("MMMM dd yyyy hh:mm") %>' /> by <asp:HyperLink runat="Server" NavigateUrl='<%# Globals.UrlUserProfile + DataBinder.Eval( ((RepeaterItem) Container).DataItem, "MostRecentPostAuthor") %>' Text='<%# DataBinder.Eval( ((RepeaterItem) Container).DataItem, "MostRecentPostAuthor") %>' />
  </td>
</tr>