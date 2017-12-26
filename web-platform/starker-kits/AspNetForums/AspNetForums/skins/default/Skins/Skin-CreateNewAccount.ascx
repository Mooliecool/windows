<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Control Language="C#" %>
<table cellspacing="1" cellpadding="3" width="600" Class="tableBorder">
  <tr>
    <th class="tableHeaderText" align="left">
      &nbsp;Create a new account
    </th>
  </tr>
  <tr>
    <td class="forumRow">
      <table cellpadding="3" cellspacing="1" border="0">
        <tr>
          <td colspan="3">
            <span class="normalTextSmall">
              If you already have a username and password, please <A href="/Forums/login.aspx">login</A>.
              <P>
              Your username will be used as your handle within the site and you will sign on with your username. A randomly generated password for your username will be sent to the email address specified below, which you can then use to login.
              <P>
              Important: The email you enter <b>is not</b> public, shared in anyway, or displayed on this site.
            </span>
          </td>
        </tr>
        <tr>
          <td align="right"nowrap>
            <span class="normalTextSmallBold">
              Desired Username:
            </span>
          </td>
          <td align="left">
            <asp:textbox id="Username" runat="server" columns="20"></asp:textbox>
          </td>
          <td align="left" width="100%">
            <asp:RequiredFieldValidator id="usernameValidator" runat="server" ErrorMessage="" ControlToValidate="Username" CssClass="validationWarningSmall">Username is required.</asp:RequiredFieldValidator>
            <br>
            <asp:RegularExpressionValidator id="RegularExpressionValidator1" ErrorMessage="" runat="server" ControlToValidate="Username" ValidationExpression="^[A-Za-z].[^\<\>]*" CssClass="validationWarningSmall">Username must start with a-z/A-Z character.</asp:RegularExpressionValidator>
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
              <asp:textbox id="Password" runat="server" columns="20"></asp:textbox>
            </td>
            <td align="left">
              <asp:RequiredFieldValidator id="RequiredFieldValidator2" runat="server" ErrorMessage="" ControlToValidate="Password" CssClass="validationWarningSmall">Password is required.</asp:RequiredFieldValidator>
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
            <asp:textbox id="Email" runat="server" columns="20"></asp:textbox>
          </td>
          <td align="left">
            <asp:RequiredFieldValidator id="emailValidator" runat="server" ErrorMessage="" ControlToValidate="Email" CssClass="validationWarningSmall">Email is required.</asp:RequiredFieldValidator>
            <asp:RegularExpressionValidator id="RegularExpressionValidator2" runat="server" ErrorMessage="" ControlToValidate="Email" CssClass="validationWarningSmall" ValidationExpression="\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*">Must be a valid email address.</asp:RegularExpressionValidator>
          </td>
        </tr>
        <tr>
          <td align="right" nowrap>
          </td>
          <td align="left">
            <asp:button ID="CreateAccount" Runat="server" />
          </td>
        </tr>
        <tr>
          <td align="right" nowrap>
          </td>
          <td align="left">
            <span class="normalTextSmall">
            &nbsp;
            <br>
            <a href="<%=Globals.UrlForgotPassword%>">Forget your password?</a>
            </span>
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>
<P>