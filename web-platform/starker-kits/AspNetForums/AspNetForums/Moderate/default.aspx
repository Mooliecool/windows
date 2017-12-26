<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Register TagPrefix="Moderation" Namespace="AspNetForums.Controls.Moderation" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<HTML>
  <HEAD>
    <AspNetForums:StyleSkin runat="server" ID="Styleskin1" />
    <META HTTP-EQUIV="Refresh" CONTENT="10">
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
                 <br>
                 <AspNetForums:SiteStats runat="server" ID="Sitestats1" />
                 <br>
                 <Moderation:ModerationStats runat="server" ID="ModerationStats" />
                 <br>
                 <AspNetForums:WhoIsOnline SkinFilename="Moderation/Skin-WhoIsOnline.ascx" runat="server" ID="Whoisonline1" />
                </td>
                <td class="LeftColumn">&nbsp;&nbsp;&nbsp;</td>
                <!-- center column -->
                <td class="CenterColumn">&nbsp;&nbsp;&nbsp;</td>
                <td id="CenterColumn" Width="95%" runat="server" class="CenterColumn">
                  <br>
                  <AspNetForums:NavigationMenu id="NavigationMenu2" SkinFilename="Moderation/Skin-Navigation.ascx" title="ASP.NET Discussion Forum" runat="server" Description="A free discussion system for ASP.NET" />
                  <P>
                  <Moderation:ModeratedForumGroupRepeater runat="server" />
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