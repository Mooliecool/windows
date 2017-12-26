<%@ Page Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums" %>
<script language="C#" runat="server">

	public void Page_Load(Object sender, EventArgs e) {
		// log the user out
		FormsAuthentication.SignOut();
		
		// Nuke the roles cookie
		UserRoles.SignOut();
	}
	
</script>
<HTML>
  <HEAD>
    <AspNetForums:StyleSkin runat="server" ID="Styleskin2" />
  </HEAD>
  <body leftmargin="0" bottommargin="0" rightmargin="0" topmargin="0" marginheight="0" marginwidth="0">
    <form runat="server" ID="Form1">
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
                  <br>
                  <table width="100%">
                    <tr>
                      <td align="center">
                  
                        <table cellspacing="1" cellpadding="0" width="50%" Class="tableBorder">
                          <tr>
                            <th align="left">
                              &nbsp;<span class="tableHeaderText">Log Out Complete</span>
                            </th>
                          </tr>
                          <tr>
                            <td Class="forumRow">
                              <table cellpadding="3" cellspacing="0">
                                <tr>
                                  <td>
                                    &nbsp;
                                  </td>
                                  <td> 
                                   <span class="normalTextSmall">
                                   You have been logged out.  
                                   <P>
                                   To log back in, please visit the <a href="login.aspx">Login page</a>.
                                   <P>
                                   <a href="<%=Globals.UrlHome%>">Return to forum home</a>
                                   </span>
                                  </td>
                                </tr>
                              </table>
                            </td>
                          </tr>
                        </table>
                      </td>
                    </tr>
                    <tr>
                      <td align="center">
                        <br>
                        <AspNetForums:JumpDropDownList runat="server" ID="Jumpdropdownlist1" />
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
