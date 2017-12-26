<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Register TagPrefix="AspNetForumsSearch" Namespace="AspNetForums.Controls.Search" Assembly="AspNetForums" %>
<table Class="tableBorder" CellPadding="3" CellSpacing="1" width="100%">
  <tr>
    <th Align="Left" Class="tableHeaderText">
     &nbsp;Search ASP.NET Forums
    </th>
  </tr>
  <tr>
    <td Align="Center" Class="forumRow">
      <asp:DropDownList id="forumList" runat="server" />&nbsp;<asp:DropDownList id="SearchType" runat="server"/>&nbsp;<asp:DropDownList id="MatchType" runat="server" />
    </td>
  </tr>
  <tr>
    <td Align="Center" Class="forumRow">
      <asp:TextBox id="SearchText" runat="server" Columns="50" />&nbsp;<asp:Button id="Search_Button" Text=" Search " runat="server" />
    </td>
  </tr>
</table>