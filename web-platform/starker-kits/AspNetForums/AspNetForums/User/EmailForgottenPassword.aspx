<%@ Page Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums" %>
<script language="C#" runat="server">

	void Page_Load(Object sender, EventArgs e)
	{
		// if the user is already logged in, just send her the info
		if (User.Identity.Name.Length > 0)
		{
			panelNotLoggedIn.Visible = false;
			lblEmailSent.Text = "Your password has been sent to the email address on file, " + User.Identity.Name;
			lblEmailSent.Visible = true;
			
			// actually send the email
			Emails.SendEmail(User.Identity.Name, EmailTypeEnum.ForgottenPassword);
		}
		else
		{
			panelNotLoggedIn.Visible = true;
		}
	}


  public void btnSubmit_Click(Object sender, EventArgs e) {

      string username;

      // Lookup the username
      username = Users.GetUsernameByEmail(txtUsername.Text);

      if (username != null) {

        // See if the email was sent correctly
        if (Emails.SendEmail(username, EmailTypeEnum.ForgottenPassword)) {
          panelNotLoggedIn.Visible = false;
          lblEmailSent.Text = "Your password has been sent to the email address on file, " + txtUsername.Text;
          lblEmailSent.Visible = true;
        } else {
          // display a message letting the user know the Username entered was invalid
          lblInvalidUsername.Visible = true;
        }
      } else {
        // display a message letting the user know the Username entered was invalid
        lblInvalidUsername.Visible = true;
      }
  }

</script>
<HTML>
  <HEAD>
    <AspNetForums:StyleSkin runat="server" ID="Styleskin1" />
  </HEAD>
  <body leftmargin="0" bottommargin="0" rightmargin="0" topmargin="0" marginheight="0" marginwidth="0">
    <form runat="server">
      <table width="100%" cellspacing="0" cellpadding="0" border="0">
        <tr valign="bottom">
          <td>
            <table width="100%" height="100%" cellspacing="0" cellpadding="0" border="0">
              <tr valign="top">
                <!-- left column -->
                <td>&nbsp; &nbsp; &nbsp;</td>
                <!-- center column -->
                <td id="CenterColumn" Width="95%" runat="server" class="CenterColumn">
                  <br>
                  <AspNetForums:NavigationMenu DisplayTitle="false" id="Navigationmenu1" runat="server" />
                  <P>
                  <br>
                  <center>
                    <table Class="tableBorder" CellSpacing="1" CellPadding="3" Width="500">
                      <tr>
                        <th align="left" Class="tableHeaderText">
                          &nbsp;Receive your Forgotten Password via Email
                        </th>
                      </tr>
                      <tr>
                        <td Class="forumRow">
                    <asp:panel id="panelNotLoggedIn" runat="server">
                        <span class="normalTextSmall">
                                <br>
				Enter your email address to receive your password. Once you receive this
				password, visit the <a href="<%=Globals.UrlLogin%>">logon page</a>.
			</span>
                      <P>
                      <asp:label id="lblInvalidUsername" CssClass="validationWarningSmall" runat="server" Visible="False" Text="The username or email address you entered does not exist in the database.<p>" />
                      <span class="normalTextSmallBold">Enter your email address:</span>
                      <asp:textbox id="txtUsername" runat="server" Cssclass="NormalTextBox" MaxLength="50" Columns="25" />
                      <asp:button id="btnSubmit" Text="Send Password" Cssclass="normalButton" runat="server" OnClick="btnSubmit_Click" />
                    </asp:panel>
                  <p>
                    <asp:label id="lblEmailSent" runat="server" Visible="False" Cssclass="normalTextSmall" />
                        </td>
                      </tr>
                    </table>
                  </center>

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
