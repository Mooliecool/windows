<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Control Language="C#" %>
<table cellPadding="0" width="100%">
  <tr>
    <td align="left" valign="top">
      <span class="forumName">Member List</span>
    </td>
  </tr>
  <tr>
    <td align="center">
      <AspNetForums:AlphaPicker id="AlphaPicker" runat="server" />
    </td>
    <td vAlign="bottom" align="right">
      &nbsp;<span class="normalTextSmall">Sort by: </span>
      <asp:DropDownList id="SortBy" runat="server"></asp:DropDownList>
      &nbsp;<span class="normalTextSmall">Order: </span>
      <asp:DropDownList id="SortDirection" runat="server"></asp:DropDownList>
    </td>
  </tr>
  <tr>
    <td vAlign="top" align="right" colspan="2">
      <AspNetForums:UserList width="100%" id="UserList" CssClass="tableBorder" CellPadding="3" CellSpacing="1" runat="server" />
      <AspNetForums:Paging id="Pager" runat="server" />
    </td>
  </tr>
  <tr>
    <td>
    &nbsp;
    </td>
  </tr>
  <tr>
    <td colspan="2" align="right">
      &nbsp;<span class="normalTextSmall">Find User: </span>
      <asp:TextBox id="SeachForUser" runat="server"/>
      <asp:Button id="SearchButton" Text=" Search " runat="server"/>
    </td>
  </tr>
</table>

<p>
<%--
  <AspNetForums:JumpDropDownList runat="server" ID="Jumpdropdownlist1" />
--%>
</p>