<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Control Language="C#" %>
<table cellSpacing="0" border="0">
  <tr>
    <td><ASPNETFORUMS:WHEREAMI id="Whereami2" runat="server"></ASPNETFORUMS:WHEREAMI></td>
  </tr>
</table>
<p>
<center>
<table Class="tableBorder" width="75%" CellPadding="3" Cellspacing="1">
  <tr>
    <th class="tableHeaderText" align="left" height="25">
      &nbsp; Move Thread
    </th>
  </tr>
  <tr>
    <td class="forumRow">
      <table cellSpacing="1" cellPadding="3">
        <tr>
          <td vAlign="top" nowrap align="left"><span class="normalTextSmall">Please choose the appropriate forum to move this thread to.</span></td>
        </tr>
        <tr>
          <td align="left" colspan="2">
            <table>
              <tr> 
                <td align="right">
                  <span class="normalTextSmallBold">Subject: </span>
                </td>
                <td align="left">
                  <asp:Label CssClass="normalTextSmall" runat="server" id="Subject" />
                </td>
              </tr>
              <tr> 
                <td valign="top" align="right">
                  <span class="normalTextSmallBold">Has Replies: </span>
                </td>
                <td align="left">
                  <asp:Label CssClass="normalTextSmall" runat="server" id="HasReplies" />
                </td>
              </tr>
              <tr> 
                <td valign="top" align="right">
                  <span class="normalTextSmallBold">Posted By: </span>
                </td>
                <td align="left">
                  <asp:Label CssClass="normalTextSmall" runat="server" id="PostedBy" />
                </td>
              </tr>
              <tr> 
                <td valign="top" align="right">
                  <span class="normalTextSmallBold">Body: </span>
                </td>
                <td align="left">
                  <asp:Label CssClass="normalTextSmall" runat="server" id="Body" />
                </td>
              </tr>
              <tr> 
                <td valign="top" align="right">
                  <span class="normalTextSmallBold">Move to: </span>
                </td>
                <td align="left">
                  <asp:DropDownList runat="server" id="MoveTo" />
                </td>
              </tr>
            </table>
          </td>
        </tr>
        <tr> 
          <td colspan="2" valign="top" align="left">
            <span class="normalTextSmallBold"><asp:CheckBox Checked="true" id="SendUserEmail" runat="server" text=" Send user email (thread owner only) that the post was moved."/></span>
          </td>
        </tr>
        <tr>
          <td vAlign="top" colspan="2" nowrap align="right"><asp:HyperLink CssClass="linkSmallBold" id="CancelMove" runat="server" Text="Cancel" /> &nbsp; <asp:LinkButton CssClass="linkSmallBold" id="MovePost" runat="server" Text="Move Post" /></td>
        </tr>
      </table>
    </td>
  </tr>
</table>
</center>
