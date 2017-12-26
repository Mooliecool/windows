<%@ Control Language="C#" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums.Controls" %>
<table class="tableBorder" cellSpacing="1" cellPadding="3" width="100%">
  <tr>
    <th height="25" class="tableHeaderText" align="left">
      &nbsp;<asp:label id="title" runat="server"></asp:label>
    </th>
  </tr>
  <tr>
    <td class="forumRow">
      <table cellSpacing="1" cellPadding="3" border="0">
        <tr>
          <td colSpan="3"><span class="normalTextSmall">Forum groups are used to categorize various 
            forums. For example, a 'moderated' forum group may exist to group 
            forums that are moderated. </span></td>
        </tr>
        <span id="EditMode" runat="server" visible="false">
          <tr>
            <td noWrap valign="top" align="right"><span class="normalTextSmallBold">Forum Groups: 
          </span></td>
            <td align="left"><asp:ListBox id="ForumGroups" runat="server"></asp:ListBox></td>
            <td align="left" colspan="2" width="100%"><asp:ImageButton id="MoveUpButton" runat="server"/><br><span class="normalTextSmall">Change Sort Order</span><br><asp:ImageButton id="MoveDownButton" runat="server"/></td>
          </tr>
        </span>
        <tr>
          <td noWrap align="right"><span class="normalTextSmallBold">Forum Group Name: </span></td>
          <td colspan="2" align="left"><asp:textbox id="forumGroupName" columns="55" runat="server" /></td>
        </tr>
        <tr>
        <tr>
          <td />
          <td colspan="2">
            <asp:button id="CreateForumGroup" Runat="server" />&nbsp;<asp:button id="DeleteForumGroup" runat="server" />
          </td>     
        </tr>
        <tr>
          <td />
          <td><asp:requiredfieldvalidator id="forumGroupNameValidator" runat="server" CssClass="validationWarningSmall" ControlToValidate="forumGroupName" ErrorMessage="RequiredFieldValidator">A forum group name is required.</asp:requiredfieldvalidator></td>
        </tr>
      </table>
    </td>
  </tr>
</table>
