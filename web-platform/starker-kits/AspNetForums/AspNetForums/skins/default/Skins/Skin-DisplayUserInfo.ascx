<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Control %>

<table cellspacing="1" cellpadding="0" width="100%" Class="tableBorder">
  <tr>
    <th height="25" class="tableHeaderText" align="left" colspan="2">
      &nbsp; Viewing User Profile for:
      <asp:label id="Username" runat="server" />
    </th>
  </tr>
  <tr>
    <td height="20" class="forumHeaderBackgroundAlternate">
      <span class="forumTitle">
        &nbsp;About
      </span>
    </td>
    <td class="forumHeaderBackgroundAlternate">
      <span class="forumTitle">
        &nbsp;Contact
      </span>
    </td>
  </tr>
  <tr>
    <td align="left" valign="top" class="forumRow" width="50%">
      <table cellpadding="4">
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              Joined:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:label id="Joined" runat="server" />
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              Last Login:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:label id="LastLogin" runat="server" />
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              Website:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:HyperLink id="WebSite" runat="server" Target="_blank"></asp:HyperLink>
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              Location:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:label id="Location" runat="server" />
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              Occupation:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:label id="Occupation" runat="server" />
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              Interests:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:label id="Interests" runat="server" />
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              Signature:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:label id="Signature" runat="server" />
            </span>
          </td>
        </tr>
        <span id="HasIcon" Visible="false" runat="server">
          <tr>
            <td valign="top" align="right">
              <span class="normalTextSmallBold">
              Icon:
            </span>
            </td>
            <td valign="top" align="left">
              <span class="normalTextSmall">
                <asp:image id="Icon" runat="server" />
              </span>
            </td>
          </tr>
        </span>
      </table>
    </td>
    <td valign="top" class="forumRow" width="50%">
      <table cellpadding="4">
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              Email:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:HyperLink id="Email" runat="server" Target="_blank"></asp:HyperLink>
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              MSN IM:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:label id="MsnIm" runat="server" />
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              AIM:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:label id="AolIm" runat="server" />
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              Yahoo IM:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:label id="YahooIm" runat="server" />
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="right">
            <span class="normalTextSmallBold">
              ICQ:
            </span>
          </td>
          <td valign="top" align="left">
            <span class="normalTextSmall">
              <asp:label id="ICQ" runat="server" />
            </span>
          </td>
        </tr>
      </table>
    </td>
  </tr>
  <tr>
    <td height="20" class="forumHeaderBackgroundAlternate" colspan="2">
      <span class="forumTitle">
        &nbsp;Post Statistics
      </span>
    </td>
  </tr>
  <tr>
    <td class="forumRow" valign="top" colspan="2">
      <table width="100%" cellpadding="4">
        <tr>
          <td valign="top" align="left">
            <span class="normalTextSmallBold">
              <asp:label id="PostStats" runat="server" />
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="left">
            <span class="normalTextSmallBold">
              Most Recent Posts:
            </span>
          </td>
        </tr>
        <tr>
          <td valign="top" align="left" colspan="2">
            <AspNetForums:PostList id="PostList" Cellpadding="3" cellspacing="1" width="100%" runat="server">
              <ItemTemplate>
                <table width="100%" cellpadding="0" cellspacing="0">
                  <tr>
                    <td class="forumAlternate" >
                      <a class="linkSmallBold" href="<% =AspNetForums.Components.Globals.UrlShowPost%><%# DataBinder.Eval(Container.DataItem, "ThreadID") %>#<%# DataBinder.Eval(Container.DataItem, "PostID") %>"><%# DataBinder.Eval(Container.DataItem, "Subject") %></a>
                      <span class="normalTextSmall"><i><%# DataBinder.Eval(Container.DataItem, "PostDate") %></i></span>
                      &nbsp;
                      <span class="normalTextSmall">(Total replies: <%# DataBinder.Eval(Container.DataItem, "Replies") %>)</span>
                    </td>
                  </tr>
                  <tr>
                    <td class="forumAlternate" ><span class="normalTextSmall"><%# DataBinder.Eval(Container.DataItem, "BodySummary") %></span></td>
                  </tr>
                </table>
              </ItemTemplate>
              <AlternatingItemTemplate>
                <table width="100%" cellpadding="0" cellspacing="0">
                  <tr>
                    <td>
                      <a class="linkSmallBold" href="<% =AspNetForums.Components.Globals.UrlShowPost%><%# DataBinder.Eval(Container.DataItem, "ThreadID") %>#<%# DataBinder.Eval(Container.DataItem, "PostID") %>"><%# DataBinder.Eval(Container.DataItem, "Subject") %></a>
                      <span class="normalTextSmall"><i><%# DataBinder.Eval(Container.DataItem, "PostDate") %></i></span>
                      &nbsp;
                      <span class="normalTextSmall">(Total replies: <%# DataBinder.Eval(Container.DataItem, "Replies") %>)</span>
                    </td>
                  </tr>
                  <tr>
                    <td><span class="normalTextSmall"><%# DataBinder.Eval(Container.DataItem, "BodySummary") %></span></td>
                  </tr>
                </table>
              </AlternatingItemTemplate>
              
              <SeparatorTemplate>
                <hr size="1">
              </SeparatorTemplate>
            </AspNetForums:PostList>
            
            <p>
            <asp:HyperLink CssClass="linkSmallBold" Runat="server" ID="MorePosts">Search for more...</asp:HyperLink>
            </p>
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>

<p>
<%--
<AspNetForums:JumpDropDownList runat="server" ID="Jumpdropdownlist1" />
--%>
</p>
