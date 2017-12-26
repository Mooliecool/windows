<%@ Page Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="System.Data" %>
<%@ Import Namespace="System.Data.SqlClient" %>
<%@ Register TagPrefix="calypso" TagName="Banner" Src="~/SiteTabsTop.ascx" %>
<%@ Register TagPrefix="calypso" TagName="Spacer" Src="~/Modules/Spacer.ascx" %>

<script runat="server">
  SqlConnection connection;
  SqlCommand command;

  public void Page_Load() {
    connection = new SqlConnection(Globals.DatabaseConnectionString);

    // Get uservisitsbyday
    GetUserVisitsByDay();

    // Get top posters
    GetTop50Posters();
  }

  public void GetTop50Posters() {
    UserList.DataSource = Users.GetAllUsers(0, 25, Users.SortUsersBy.Posts, 1, null);
    UserList.DataBind();
  }

  public void GetUserVisitsByDay() {
    command = new SqlCommand("Reports_UserVisitsByDay", connection);
    command.CommandType = CommandType.StoredProcedure;

    // Add Parameters to SPROC
    command.Parameters.Add("@DaysBack", SqlDbType.Int).Value = 1;

    // Add Parameters to SPROC
    command.Parameters.Add("@SumDayCount", SqlDbType.Bit).Value = false;

    // Open the connection
    connection.Open();

    UserActivity.DataSource = command.ExecuteReader();
    UserActivity.DataBind();

    // Close the connection
    connection.Close();

  }

</script>

<HTML>
  <HEAD>
    <link rel="stylesheet" href='<%= Context.Items["AppPath"] + "AspNetWeb.css" %>' type="text/css" >
    <AspNetForums:StyleSkin runat="server" ID="Styleskin1" />
    <META HTTP-EQUIV="Refresh" CONTENT="300">
  </HEAD>
  <body leftmargin="0" bottommargin="0" rightmargin="0" topmargin="0" marginheight="0" marginwidth="0">
    <form runat="server">
      <table width="100%" cellspacing="0" cellpadding="0" border="0">
        <tr valign="top">
          <td>
            <calypso:Banner id="Banner" SelectedTabIndex="0" runat="server" />
          </td>
        </tr>
        <tr valign="bottom">
          <td>
            <table width="100%" height="100%" cellspacing="0" cellpadding="0" border="0">
              <tr valign="top">
                <!-- left column -->
                <td>&nbsp; &nbsp; &nbsp;</td>
                <!-- center column -->
                <td id="CenterColumn" Width="95%" runat="server" class="CenterColumn">
                  <br>
                  <AspNetForums:NavigationMenu id="Navigationmenu1" runat="server" />
                  <br>
                  <span class="forumName">User Statistics</span>
                  <p>
                  <span class="normalTextSmallBold">Total users that have logged in: </span><asp:Label CssClass="normalTextSmall" id="activeUsers" runat="server"/>
                  <p>
                  <span class="normalTextSmallBold">Post Activity: </span>
                  <asp:DataList width="100%" ExtractTemplateRows="true" CellPadding="3" CellSpacing="1" Class="tableBorder" runat="server" id="UserActivity">
                    <HeaderTemplate>
                     <asp:Table runat="Server">
                       <asp:TableRow>
                         <asp:TableHeaderCell class="tableHeaderText">Date</asp:TableHeaderCell>
                         <asp:TableHeaderCell class="tableHeaderText">Active Users</asp:TableHeaderCell>
                         <asp:TableHeaderCell class="tableHeaderText">Total Posts</asp:TableHeaderCell>
                         <asp:TableHeaderCell class="tableHeaderText">% Posts/User</asp:TableHeaderCell>
                         <asp:TableHeaderCell class="tableHeaderText">Total Posts for ASP.NET Team</asp:TableHeaderCell>
                         <asp:TableHeaderCell class="tableHeaderText">% Posts by ASP.NET Team</asp:TableHeaderCell>
                       </asp:TableRow>
                     </asp:Table>
                    </HeaderTemplate>
  
                    <ItemTemplate>
                     <asp:Table runat="Server">
                       <asp:TableRow>
                         <asp:TableCell class="forumRow"><span class="normalTextSmall"><%# DataBinder.Eval(Container.DataItem, "StatDate") %></span></asp:TableCell>
                         <asp:TableCell class="forumRow"><span class="normalTextSmall"><%# DataBinder.Eval(Container.DataItem, "UserCount") %></span></asp:TableCell>
                         <asp:TableCell class="forumRow"><span class="normalTextSmall"><%# DataBinder.Eval(Container.DataItem, "PostCount") %></span></asp:TableCell>
                         <asp:TableCell class="forumRow"><span class="normalTextSmall"><%# DataBinder.Eval(Container.DataItem, "AvgPostPerUser") %></span></asp:TableCell>
                         <asp:TableCell class="forumRow"><span class="normalTextSmall"><%# DataBinder.Eval(Container.DataItem, "PostCountAspNetTeam") %></span></asp:TableCell>
                         <asp:TableCell class="forumRow"><span class="normalTextSmall"><%# DataBinder.Eval(Container.DataItem, "PercentagePostsAspNetTeam") %></span></asp:TableCell>
                       </asp:TableRow>
                     </asp:Table>
                    </ItemTemplate>
                  </asp:DataList>
                  <P>
                  <span class="normalTextSmallBold">Top 25 Most Active Users: </span>
                  <AspNetForums:UserList width="100%" id="UserList" CssClass="tableBorder" CellPadding="3" CellSpacing="1" runat="server" />
                </td>
                <td class="CenterColumn">&nbsp;&nbsp;&nbsp;</td>
                <!-- right margin -->
                <td class="RightColumn">&nbsp;&nbsp;&nbsp;</td>
                <td id="RightColumn" Visible="false" nowrap Width="230" runat="server" class="RightColumn">
                </td>
                <td class="RightColumn"><calypso:spacer height="800" width="10" runat="server" /></td>
              </tr>
            </table>
          </td>
        </tr>
      </table>
    </form>
  </body>
</HTML>
