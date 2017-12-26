<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Control Language="C#" %>
<table class="tableBorder" cellSpacing="1" cellPadding="3" width="100%">
  <tr>
    <th class="tableHeaderText" align="left" colspan="2">
      &nbsp;Moderators Online
    </th>
  </tr>
  <tr>
    <td class="forumRow" valign="top">
      <P>
        <span class="normalTextSmaller">
      There are currently:
      <br>
      <b><asp:Label ID="Moderators" Runat="server" /></b> moderators online.
      <br>
      <br>
      Moderators online<asp:label id="ModeratorList" runat="server"></asp:label>
      </span>
    </td>
  </tr>
</table>
