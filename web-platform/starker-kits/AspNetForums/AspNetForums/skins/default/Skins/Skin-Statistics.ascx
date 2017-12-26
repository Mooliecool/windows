<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Control Language="C#" %>
<script runat="server">
  int i = 1;
</script>
<table class="tableBorder" cellSpacing="1" cellPadding="3" width="100%">
  <tr>
    <th class="tableHeaderText" align="left" colSpan="2">
      &nbsp;Forum Statistics
    </th>
  </tr>
  <tr>
    <td class="forumRow" vAlign="top">
      <table CellPadding="2" CellSpacing="2">
        <tr>
          <td>
            <span class="normalTextSmaller">
            <b><asp:label id="TotalUsers" runat="server"></asp:label>&nbsp;</b>users 
            have contributed to <b><asp:label id="TotalThreads" runat="server"></asp:label>&nbsp;</b>threads 
            and <b><asp:label id="TotalPosts" runat="server"></asp:label>&nbsp;</b>posts.
            </span>
          </td>
        </tr>
        <tr>
          <td>
            <span class="normalTextSmaller">
              In the past 24 hours:
              <br>
              - <b><asp:label id="NewThreadsInPast24Hours" runat="server"></asp:label>&nbsp;</b>new threads
              <br>
              - <b><asp:label id="NewPostsInPast24Hours" runat="server"></asp:label></b>&nbsp;new posts
              <br>
              - <b><asp:label id="NewUsersInPast24Hours" runat="server"></asp:label></b>&nbsp;new users
            </span>
          </td>
        </tr>
        <tr>
          <td>
            <span class="normalTextSmaller">
              In the past 3 days the most viewed&nbsp;thread is <asp:hyperlink id="MostViewedThread" runat="server"></asp:hyperlink>.
            </span>
          </td>
        </tr>
        <tr>
          <td>
            <span class="normalTextSmaller">
              In the past 3 days the thread with the most posts is <asp:hyperlink id="MostActiveThread" runat="server"></asp:hyperlink>.
            </span>
          </td>
        </tr>
        <tr>
          <td>
            <span class="normalTextSmaller">
              In the past 3 days the most read thread is <asp:hyperlink id="MostReadThread" runat="server"></asp:hyperlink>. 
            </span>
          </td>
        </tr>
        <tr>
          <td>
            <span class="normalTextSmaller">
              Top 10 most active users in past 24 hours:<br> 
              <asp:Repeater id="TopUsers" runat="server">
                <ItemTemplate>
                 &nbsp;<%# (i++).ToString() %>. <a href='<%# Globals.UrlUserProfile + DataBinder.Eval(Container.DataItem, "Username") %>'><%# DataBinder.Eval(Container.DataItem, "Username") %></a> (<%# ((int)DataBinder.Eval(Container.DataItem, "TotalPosts")).ToString("n0") %>)<br>
                </ItemTemplate>
              </asp:Repeater>
            </span>
          </td>
        </tr>
        <tr>
          <td>
            <span class="normalTextSmaller">
              The newest user is <asp:hyperlink id="NewestUser" runat="server"></asp:hyperlink>.</span></td>
            </span>
          </td>
        </tr>
      </table>
  </tr>
</table>
