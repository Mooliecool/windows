<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Control Language="C#" %>

<script runat="server">
  int userCounter = 1;
</script>

<table class="tableBorder" cellSpacing="1" cellPadding="3" width="100%">
  <tr>
    <th class="tableHeaderText" align="left">
      &nbsp;Edit a User's Information
    </th>
  </tr>
  <tr>
    <td class="forumRow">
      <table width="100%">
        <tr>
          <td>
            <span class="normalTextSmall">View user whose username begins with:</span>
            <br>
            <AspNetForums:AlphaPicker id="AlphaPicker" runat="server" />
          </td>
        </tr>
        <tr>
          <td align="left">
            <AspNetForums:UserList runat="server" ID="UserList" Width="100%" ExtractTemplateRows=true CellPadding="3" Cellspacing="1">
              <HeaderTemplate>
                <asp:Table Runat="server">
                  <asp:TableRow Runat="server">
                    <asp:TableCell width="10" CssClass="forumHeaderBackgroundAlternate" Runat="server">
                      <span class="normalTextSmallBold">#</span>
                    </asp:TableCell>

                    <asp:TableCell CssClass="forumHeaderBackgroundAlternate" Runat="server" ID="Tablecell2" NAME="Tablecell2">
                      <span class="normalTextSmallBold">Username</span>
                    </asp:TableCell>

                    <asp:TableCell CssClass="forumHeaderBackgroundAlternate" Runat="server" ID="Tablecell3" NAME="Tablecell3">
                      <span class="normalTextSmallBold">Joined</span>
                    </asp:TableCell>

                    <asp:TableCell CssClass="forumHeaderBackgroundAlternate" Runat="server" ID="Tablecell4" NAME="Tablecell4">
                      <span class="normalTextSmallBold">Last Active</span>
                    </asp:TableCell>

                    <asp:TableCell CssClass="forumHeaderBackgroundAlternate" Runat="server" ID="Tablecell5" NAME="Tablecell5">
                      <span class="normalTextSmallBold">Posts</span>
                    </asp:TableCell>
                  </asp:TableRow>
                </asp:Table>
              </HeaderTemplate>
              
              <ItemTemplate>
                <asp:Table Runat="server" ID="Table1" NAME="Table1">
                  <asp:TableRow Runat="server" ID="Tablerow1" NAME="Tablerow1">
                    <asp:TableCell Runat="server" ID="Tablecell1" NAME="Tablecell1" ><span class="normalTextSmallBold"><%# userCounter++ %></span></asp:TableCell>

                    <asp:TableCell Runat="server" ID="Tablecell6" NAME="Tablecell2" >
                      <asp:HyperLink runat="server" NavigateUrl="test" CssClass="normalLinkSmallBold" Text='<%# DataBinder.Eval(Container.DataItem, "Username") %>' />
                    </asp:TableCell>

                    <asp:TableCell Runat="server" ID="Tablecell7" NAME="Tablecell2" ><span class="normalTextSmall"><%# ((DateTime) DataBinder.Eval(Container.DataItem, "DateCreated")).ToString(Globals.DateFormat) %></span></asp:TableCell>

                    <asp:TableCell Runat="server" ID="Tablecell8" NAME="Tablecell2" ><span class="normalTextSmall"><%# ((DateTime) DataBinder.Eval(Container.DataItem, "LastActivity")).ToString(Globals.DateFormat) %></span></asp:TableCell>
                    
                    <asp:TableCell Runat="server" ID="Tablecell9" NAME="Tablecell2" >
                      <asp:HyperLink runat="server" NavigateUrl='<%# Globals.UrlSearch + "?searchtext=" + DataBinder.Eval(Container.DataItem, "Username") + "&SearchFor=1" %>' CssClass="normalLinkSmallBold" Text='<%# DataBinder.Eval(Container.DataItem, "TotalPosts") %>' ID="Hyperlink1" NAME="Hyperlink1"/> 
                    </asp:TableCell>
                  </asp:TableRow>
                </asp:Table>              
              </ItemTemplate>

            </AspNetForums:UserList>
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>
