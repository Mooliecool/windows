<%@ Control Language="C#" %>
<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Register TagPrefix="AspNetForumsModeration" Namespace="AspNetForums.Controls.Moderation" Assembly="AspNetForums" %>
<table cellPadding="0" width="100%">
  <tr>
    <td align="left" colSpan="2"><ASPNETFORUMS:WHEREAMI id="Whereami1" runat="server" NAME="Whereami1"></ASPNETFORUMS:WHEREAMI></td>
  </tr>
  <tr>
    <td align="left" colSpan="2">&nbsp;
    </td>
  </tr>
  <tr>
    <td vAlign="top" align="left">
	<span class="normalTextSmallBold"><asp:CheckBox runat="server" id="TrackThread" Text="Email me when someone replies to this thread"/></span>
    </td>
    <td vAlign="bottom" align="right"><span class="normalTextSmallBold">Display using: </span><asp:dropdownlist id="DisplayMode" Runat="server">
        <asp:ListItem Value="Flat">Flat View</asp:ListItem>
        <asp:ListItem Value="Threaded">Threaded View</asp:ListItem>
      </asp:dropdownlist>&nbsp;<asp:dropdownlist id="SortOrder" Runat="server">
        <asp:ListItem Value="0">Oldest to newest</asp:ListItem>
        <asp:ListItem Value="1">Newest to oldest</asp:ListItem>
      </asp:dropdownlist>
    </td>
  </tr>
  <tr>
    <td colSpan="2"><ASPNETFORUMS:POSTLIST id="PostList" CssClass="tableBorder" runat="server" CellSpacing="1" CellPadding="0" Width="100%"></ASPNETFORUMS:POSTLIST><ASPNETFORUMS:PAGING id="Pager" runat="server"></ASPNETFORUMS:PAGING></td>
  </tr>
  <tr>
    <td colSpan="2">&nbsp;</td>
  </tr>
  <tr>
    <td align="left" colSpan="2">
<%--
      <AspNetForums:JumpDropDownList runat="server" id="JumpDropDownList1" />
--%>
    </td>
  </tr>
  <tr>
    <td align="left" colSpan="2">
      <AspNetForums:WhereAmI ShowHome="true" DynamicMenu="false" id="Whereami2" runat="server" NAME="Whereami2" />
    </td>
  </tr>
</table>
