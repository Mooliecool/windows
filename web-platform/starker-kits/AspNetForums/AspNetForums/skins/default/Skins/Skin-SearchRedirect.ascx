<%@ Control Language="C#" %>
<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>

<table Class="tableBorder" cellSpacing="1" cellPadding="3" width="100%">
  <tr>
    <th class="tableHeaderText" align="left" colspan="2">
      &nbsp;Search ASP.NET Forums
    </th>
  </tr>
  <tr>
    <td class="forumRow" align="left" valign="top" colspan="2">
      <table cellspacing="1" border="0" cellpadding="2">
        <tr>
          <td>
            <asp:TextBox id="SearchText" runat="server" size="10" maxlength="50" />
          </td>

          <td align="right" colspan="2">
            <asp:Button id="SearchButton" runat="server" text="Search" />
          </td>
        </tr>
      </table>
      <span class="normalTextSmall">
      <Br>
      <a href="<%=Globals.UrlSearch%>">More search options</a>
      </span>
    </td>
  </tr>
</table>



