<%@ Control Language="C#" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums.Controls" %>
<table cellspacing="1" cellpadding="3" width="100%" Class="tableBorder">
  <tr>
    <th class="tableHeaderText" align="left">
      &nbsp;Edit message template&nbsp;
    </th>
  </tr>
  <tr>
    <td class="forumRow">
      <table cellpadding="3" cellspacing="1">
        <tr>
          <td colspan="3">
            <span class="normalTextSmall"> Select a message template to edit.&nbsp;
            </span>
          </td>
        </tr>
        <tr>
          <td align="right" nowrap>
            <asp:DropDownList id="messageTemplateList" runat="server"></asp:DropDownList> &nbsp; <asp:HyperLink CssClass="linkSmallBold" id="PreviewMessage" runat="server">Preview Message</asp:HyperLink>
          </td>
          <td align="left">
          </td>
          <td align="left" width="100%">
          </td>
        </tr>
      </table>
      <table cellpadding="3" cellspacing="1">
        <tr>
          <td align="right" valign="top" nowrap>
            <span class="normalTextSmall"> Subject:</span>
          </td>
          <td align="left">
            <asp:TextBox id="Title" runat="server" Columns="45"></asp:TextBox>
          </td>
          <td align="left" width="100%">
          </td>
        </tr>
        <tr>
          <td align="right" nowrap vAlign="top">
            <span class="normalTextSmall"> Body:</span>
          </td>
          <td align="left">
            <asp:TextBox id="Body" runat="server" TextMode="MultiLine" Width="524px" Height="220px" Columns="60" Rows="10"></asp:TextBox>
          </td>
          <td align="left" width="100%" vAlign="top">
            <asp:Label CssClass="validationWarningSmall" id="Status" runat="server"></asp:Label>
          </td>
        </tr>
        <tr>
          <td align="right" nowrap vAlign="top">&nbsp;
          </td>
          <td align="right">
            <asp:Button id="UpdateTemplate" runat="server"></asp:Button>
          </td>
          <td align="left" width="100%">
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>
