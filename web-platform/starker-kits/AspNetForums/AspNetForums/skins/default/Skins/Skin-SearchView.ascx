<%@ Control Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Register TagPrefix="AspNetForumsSearch" Namespace="AspNetForums.Controls.Search" Assembly="AspNetForums" %>
<table cellPadding="0" width="100%">
  <tr>
    <td>
      <AspNetForumsSearch:SearchOptions id="SearchOptions" runat="server" />
    </td>
  </tr>
  <tr>
    <td vAlign="top" colSpan="2">
      <AspNetForums:ThreadList id="SearchResults" CssClass="tableBorder" Width="100%" CellPadding="3" Cellspacing="1" runat="server" />
      <ASPNETFORUMS:Paging id="Pager" runat="server" />
    </td>
  </tr>
  <tr>
    <td colspan="2">
      &nbsp;
    </td>
  </tr>
</table>