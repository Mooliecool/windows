<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Control Language="C#" %>
<table cellspacing="1" cellpadding="3" width="100%" Class="tableBorder">
  <tr>
    <th class="tableHeaderText" align="left">
      &nbsp;Admin - Create a new account
    </th>
  </tr>
  <tr>
    <td class="forumRow">
      <table cellpadding="3" cellspacing="1">
        <tr>
          <td colspan="3">
            <span class="normalTextSmall">
            Enter the username, password, and email address of the account to create.
            </span>
          </td>
        </tr>
        <tr>
          <td align="right" nowrap>
            <span class="normalTextSmallBold">
              Desired Username:
            </span>
          </td>
          <td align="left">
            <asp:textbox id="Username" runat="server" columns="55"></asp:textbox>
          </td>
          <td align="left" width="100%">
            <asp:RequiredFieldValidator id="usernameValidator" runat="server" ErrorMessage="RequiredFieldValidator" ControlToValidate="Username" CssClass="validationWarningSmall">Username is required.</asp:RequiredFieldValidator>
            <asp:RegularExpressionValidator id="RegularExpressionValidator1" runat="server" ErrorMessage="RegularExpressionValidator" ControlToValidate="Username" ValidationExpression="^[A-Za-z].*" CssClass="validationWarningSmall">Username must start with a-z/A-Z character.</asp:RegularExpressionValidator>
          </td>
        </tr>
        <span id="InstantSignUp" Visible="true" runat="server">
          <tr>
            <td align="right" nowrap>
              <span class="normalTextSmallBold">
              Password:
            </span>
            </td>
            <td align="left">
              <asp:textbox id="Password" runat="server" columns="55"></asp:textbox>
            </td>
            <td align="left">
              <asp:RequiredFieldValidator id="RequiredFieldValidator2" runat="server" ErrorMessage="RequiredFieldValidator" ControlToValidate="Password" CssClass="validationWarningSmall">Password is required.</asp:RequiredFieldValidator>
            </td>
          </tr>
        </span>
        <tr>
          <td align="right" nowrap>
            <span class="normalTextSmallBold">
              Email Address:
            </span>
          </td>
          <td align="left">
            <asp:textbox id="Email" runat="server" columns="55"></asp:textbox>
          </td>
          <td align="left">
            <asp:RequiredFieldValidator id="emailValidator" runat="server" ErrorMessage="RequiredFieldValidator" ControlToValidate="Email" CssClass="validationWarningSmall">Email is required.</asp:RequiredFieldValidator>
            <asp:RegularExpressionValidator id="RegularExpressionValidator2" runat="server" ErrorMessage="RegularExpressionValidator" ControlToValidate="Email" ValidationExpression="\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*">Must be a valid email address.</asp:RegularExpressionValidator>
          </td>
        </tr>
        <tr>
          <td align="right" nowrap>
          </td>
          <td align="left">
            <asp:button ID="CreateAccount" Runat="server" />
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>
