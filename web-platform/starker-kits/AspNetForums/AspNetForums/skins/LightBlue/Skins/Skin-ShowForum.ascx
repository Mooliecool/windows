<%@ Control Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums.Controls" %>
<table cellPadding="0" width="100%">
  <tr>
    <td colspan="2" align="left"><AspNetForums:WhereAmI id="Whereami1" runat="server" NAME="Whereami1" /><br><asp:Label CssClass="normalTextSmall" id="ForumDescription" runat="server"/></td>
  </tr>
  <tr>
    <td>
      &nbsp;
    </td>
  </tr>
  <tr>
    <TD align="left"><SPAN class="normalTextSmallBold">Search: </SPAN>
      <asp:TextBox id="Search" runat="server"></asp:TextBox>
      <asp:Button id="SearchButton" runat="server" Text=" Go "></asp:Button>
    </TD>
    <TD align="right">
      <asp:HyperLink CssClass="linkSmallBold" ID="NewThreadLinkTop" runat="server">Create New Thread</asp:HyperLink>&nbsp;
    </TD>
  </tr>
  <tr>
    <td vAlign="top" colSpan="2">
<AspNetForums:ThreadList id="ThreadList" bordercolorlight="#3399FF" bordercolordark="#FFFFFF" ExtractTemplateRows="true" border=1 Width="100%" CellPadding="3" Cellspacing="1" runat="server">
  <HeaderTemplate>
    <asp:Table runat="server">
      <asp:TableRow>
        <asp:TableHeaderCell CssClass="tableHeaderText">
          Thread
        </asp:TableHeaderCell>
        <asp:TableHeaderCell CssClass="tableHeaderText">
          Started By
        </asp:TableHeaderCell>
        <asp:TableHeaderCell CssClass="tableHeaderText">
          Replies
        </asp:TableHeaderCell>
        <asp:TableHeaderCell CssClass="tableHeaderText">
          Last Post (GMT -06:00)
        </asp:TableHeaderCell>
      </asp:TableRow>
    </asp:Table>
  </HeaderTemplate>

  <ItemTemplate>
    <asp:Table runat="server">
      <asp:TableRow>
        <asp:TableCell>
          <asp:HyperLink CssClass="linkSmallBold" runat="Server" NavigateUrl='<%# Globals.UrlShowPost + DataBinder.Eval( ((DataListItem) Container).DataItem, "PostID") %>' Text='<%# DataBinder.Eval( ((DataListItem) Container).DataItem, "Subject") %>' />
        </asp:TableCell>
        <asp:TableCell align=center>
          <asp:HyperLink CssClass="linkSmallBold" runat="Server" NavigateUrl='<%# Globals.UrlUserProfile + DataBinder.Eval( ((DataListItem) Container).DataItem, "Username") %>' Text='<%# DataBinder.Eval( ((DataListItem) Container).DataItem, "Username") %>' />
        </asp:TableCell>
        <asp:TableCell align=center>
          <asp:Label runat="Server" CssClass="normalTextSmall" Text='<%# ((Int32)DataBinder.Eval( ((DataListItem) Container).DataItem, "Replies")).ToString("n0") %>' />
        </asp:TableCell>
        <asp:TableCell width="300">
          <span class="normalTextSmall"><asp:Label runat="Server" Text='<%# ((DateTime)DataBinder.Eval( ((DataListItem) Container).DataItem, "ThreadDate")).ToString("MMMM dd yyyy hh:mm") %>' /> by <asp:HyperLink runat="Server" NavigateUrl='<%# Globals.UrlUserProfile + DataBinder.Eval( ((DataListItem) Container).DataItem, "MostRecentPostAuthor") %>' Text='<%# DataBinder.Eval( ((DataListItem) Container).DataItem, "MostRecentPostAuthor") %>' />
        </asp:TableCell>
      </asp:TableRow>
    </asp:Table>
  </ItemTemplate>

  <AlternatingItemTemplate>
    <asp:Table runat="server">
      <asp:TableRow>
        <asp:TableCell class="forumRow">
          <asp:HyperLink CssClass="linkSmallBold" runat="Server" NavigateUrl='<%# Globals.UrlShowPost + DataBinder.Eval( ((DataListItem) Container).DataItem, "PostID") %>' Text='<%# DataBinder.Eval( ((DataListItem) Container).DataItem, "Subject") %>' />
        </asp:TableCell>
        <asp:TableCell align=center class="forumRow">
          <asp:HyperLink CssClass="linkSmallBold" runat="Server" NavigateUrl='<%# Globals.UrlUserProfile + DataBinder.Eval( ((DataListItem) Container).DataItem, "Username") %>' Text='<%# DataBinder.Eval( ((DataListItem) Container).DataItem, "Username") %>' />
        </asp:TableCell>
        <asp:TableCell align=center class="forumRow">
          <asp:Label runat="Server" CssClass="normalTextSmall" Text='<%# ((Int32)DataBinder.Eval( ((DataListItem) Container).DataItem, "Replies")).ToString("n0") %>' />
        </asp:TableCell>
        <asp:TableCell class="forumRow" width="300">
          <span class="normalTextSmall"><asp:Label runat="Server" Text='<%# ((DateTime)DataBinder.Eval( ((DataListItem) Container).DataItem, "ThreadDate")).ToString("MMMM dd yyyy hh:mm") %>' /> by <asp:HyperLink runat="Server" NavigateUrl='<%# Globals.UrlUserProfile + DataBinder.Eval( ((DataListItem) Container).DataItem, "MostRecentPostAuthor") %>' Text='<%# DataBinder.Eval( ((DataListItem) Container).DataItem, "MostRecentPostAuthor") %>' />
        </asp:TableCell>
      </asp:TableRow>
    </asp:Table>
  </AlternatingItemTemplate>

  <FooterTemplate>
    <asp:Table runat="server">
    </asp:Table>
  </FooterTemplate>
</AspNetForums:ThreadList>

<ASPNETFORUMS:Paging id="Pager" runat="server" /><asp:Label CssClass="normalTextSmall" ID="NoThreads" Runat="server" Visible="False">Sorry, there are no posts in this forum yet.</asp:Label><asp:Label CssClass="normalTextSmall" id="NoPostsDueToFilter" runat="server" Visible="false">There are posts available, but your current display options prevent them from being displayed. Please change the settings below to display these posts. Most likely you have 'Hide Read Topics' selected which hides posts you've already read.</asp:Label></td>
  </tr>
  <tr>
    <td colspan="2">
      &nbsp;
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
     <AspNetForums:WhereAmI id="Whereami2" runat="server" NAME="Whereami2" />
    </td>
    <TD align="right">
      <span class="normalTextSmallBold">Display topics for: </span><asp:dropdownlist id="DisplayByDays" runat="server"></asp:dropdownlist>
      <br>
      <asp:linkbutton id="MarkAllRead" CssClass="linkSmallBold" Runat="server">Mark all topics as read</asp:linkbutton>
      <br>
      <span class="normalTextSmallBold"><asp:checkbox id="HideReadThreads" runat="server" Text="Hide read threads"></asp:checkbox></span>
    </td>
  </tr>
  <tr>
    <td colSpan="2">&nbsp;
    </td>
  </tr>
</table>
