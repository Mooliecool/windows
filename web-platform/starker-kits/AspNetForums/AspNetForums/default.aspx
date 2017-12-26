<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Register TagPrefix="AspNetForumsSearch" Namespace="AspNetForums.Controls.Search" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<HTML>
  <HEAD>
    <AspNetForums:StyleSkin runat="server" ID="Styleskin1" />
    <META HTTP-EQUIV="Refresh" CONTENT="300">
  </HEAD>
  <body leftmargin="0" bottommargin="0" rightmargin="0" topmargin="0" marginheight="0" marginwidth="0">
    <form runat="server">
      <table width="100%" cellspacing="0" cellpadding="0" border="0">
        <tr valign="bottom">
          <td>
            <table width="100%" height="100%" cellspacing="0" cellpadding="0" border="0">
              <tr valign="top">
                <!-- left column -->
                <td class="LeftColumn">&nbsp;&nbsp;&nbsp;</td>
                <td id="LeftColumn" Visible="true" nowrap width="180" runat="server" class="LeftColumn">
                 <p>
                 &nbsp;
                 <br>
                 <AspNetForums:Login SkinFilename="Skin-LoginSmall.ascx" runat="server" ID="Loginbox1" />
                 <AspNetForumsSearch:SearchRedirect ID="SearchRedirect" runat="server"/>
                 <br>
                 <AspNetForums:SiteStats runat="server" ID="Sitestats1" />
                 <br>
                 <AspNetForums:WhoIsOnline runat="server" ID="Whoisonline1" />
                </td>
                <td class="LeftColumn">&nbsp;&nbsp;&nbsp;</td>
                <!-- center column -->
                <td class="CenterColumn">&nbsp;&nbsp;&nbsp;</td>
                <td id="CenterColumn" Width="95%" runat="server" class="CenterColumn">
                  <br>
                  <AspNetForums:NavigationMenu id="NavigationMenu2" runat="server" />
                  <br>
                  <table Cellpadding="0" Cellspacing="2" width="100%" > 
                    <Tr>
                      <td align="left">
                        <AspNetForums:CurrentTime runat="server" />
                      </td>
                      <td align="right">
<%--                        <AspNetForums:JumpDropDownList runat="server" /> --%>
                      </td>
                    </tr>
                  </table>
                  <AspNetForums:ForumGroupRepeater runat="server" id="ForumGroupRepeater1"></AspNetForums:ForumGroupRepeater>
                  </P>
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
