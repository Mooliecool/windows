<%@ Control Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums.Controls" %>
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
    <td vAlign="bottom" align="right"><span class="normalTextSmallBold">Display using </span><asp:dropdownlist id="DisplayMode" Runat="server">
        <asp:ListItem Value="Flat">Flat View</asp:ListItem>
        <asp:ListItem Selected="true" Value="Threaded">Threaded View</asp:ListItem>
      </asp:dropdownlist>
    </td>
  </tr>
  <tr>
    <td colSpan="2"><AspNetForums:PostThreaded id="ThreadView" runat="server" CellSpacing="1" CellPadding="0" Width="100%"></AspNetForums:PostThreaded><ASPNETFORUMS:PAGING id="Pager" runat="server"></ASPNETFORUMS:PAGING></td>
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
    <td align="left" colSpan="2"><ASPNETFORUMS:WHEREAMI ShowHome="true" id="Whereami2" runat="server" NAME="Whereami1"></ASPNETFORUMS:WHEREAMI></td>
  </tr>
</table>
