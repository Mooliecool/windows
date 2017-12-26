<%@ Page Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums" %>
<script runat="server">

  public void ChangePassword_Click(object sender, EventArgs e) {

     bool status = false;

     if (!Page.IsValid)
       return;

     // Ensure NewPassword1 and NewPassword2 are the same
     if (NewPassword1.Text != NewPassword2.Text) {
       ValidatePassword2.IsValid = false;
       ValidatePassword2.Text = "The re-entered password and the new password did not match.";
       return;
     }

     // Change the user's password
     status = Users.ChangePasswordForLoggedOnUser(Password.Text, NewPassword1.Text);

     if (status) {
       Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UserPasswordChangeSuccess));
     } else {
       Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UserPasswordChangeFailed));
     }
     Response.End();

  }

</script>
<HTML>
  <HEAD>
    <AspNetForums:StyleSkin runat="server" ID="Styleskin2" />
  </HEAD>
  <body leftmargin="0" bottommargin="0" rightmargin="0" topmargin="0" marginheight="0" marginwidth="0">
    <form enctype="multipart/form-data" runat="server" ID="Form1">
      <table width="100%" cellspacing="0" cellpadding="0" border="0">
        <tr valign="bottom">
          <td>
            <table width="100%" height="100%" cellspacing="0" cellpadding="0" border="0">
              <tr valign="top">
                <!-- left column -->
                <td>&nbsp; &nbsp; &nbsp;</td>
                <!-- center column -->
                <td id="CenterColumn" Width="95%" runat="server" class="CenterColumn">
                  &nbsp;
                  <br>
                  <AspNetForums:NavigationMenu id="Navigationmenu1" runat="server" />
                  <P>
                    <table width="100%" CellPadding="3" CellSpacing="1" Class="tableBorder">
                      <tr>
                        <th Align="left" Class="tableHeaderText">
                          &nbsp; Change Password
                        </th>
                      </tr>
                      <tr>
                        <td Class="forumRow">
                          <table>
                            <tr>
                              <td colspan="3">
                                <span class="normalTextSmall">
                              Use this form to change your password.
                              </span>
                              </td>
                            </tr>
                            <tr>
                              <td align="right">
                                <span class="normalTextSmallBold">
                              Current Password:
                              </span>
                              </td>
                              <td align="left">
                                <asp:textbox TextMode="Password" runat="server" id="Password" Columns="40" />
                              </td>
                              <td>
                                <asp:requiredfieldvalidator id="ValidatePassword" runat="server" ErrorMessage="You must supply your current password." ControlToValidate="Password" CssClass="validationWarningSmall"></asp:requiredfieldvalidator>
                              </td>
                            </tr>
                            <tr>
                              <td align="right">
                                <span class="normalTextSmallBold">
                              New Password:
                              </span>
                              </td>
                              <td align="left">
                                <asp:textbox TextMode="Password" runat="server" id="NewPassword1" Columns="40" />
                              </td>
                              <td>
                                <asp:requiredfieldvalidator id="ValidatePassword2" runat="server" ErrorMessage="You must supply a new password." ControlToValidate="NewPassword1" CssClass="validationWarningSmall"></asp:requiredfieldvalidator>
                              </td>
                            </tr>
                            <tr>
                              <td align="right">
                                <span class="normalTextSmallBold">
                              Re-Enter New Password:
                              </span>
                              </td>
                              <td align="left">
                                <asp:textbox TextMode="Password" runat="server" id="NewPassword2" Columns="40" />
                              </td>
                              <td>
                                <asp:requiredfieldvalidator id="ValidatePassword3" runat="server" ErrorMessage="You must re-enter your new password." ControlToValidate="NewPassword2" CssClass="validationWarningSmall"></asp:requiredfieldvalidator>
                              </td>
                            </tr>
                            <tr>
                              <td align="right">
                              </td>
                              <td align="left">
                                <asp:button id="Update" OnClick="ChangePassword_Click" Text=" Change Password " runat="server" />
                              </td>
                            </tr>
                          </table>
                        </td>
                      </tr>
                    </table>
                </td>
                <td class="CenterColumn">&nbsp;&nbsp;&nbsp;</td>
                <!-- right margin -->
                <td class="RightColumn">&nbsp;&nbsp;&nbsp;</td>
                <td id="RightColumn" Visible="false" nowrap Width="230" runat="server" class="RightColumn">
                </td>
              </tr>
            </table>
          </td>
        </tr>
      </table>
    </form>
  </body>
</HTML>
