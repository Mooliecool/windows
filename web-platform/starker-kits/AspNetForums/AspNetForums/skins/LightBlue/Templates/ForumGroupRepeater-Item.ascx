<%@ Control Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>

<tr>
  <AspNetForums:ForumRepeater ForumGroupId='<%# DataBinder.Eval(Container.DataItem, "ForumGroupID") %>' id="ForumGroupRepeater" runat="server">
  </AspNetForums:ForumRepeater>
</tr>