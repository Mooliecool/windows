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
<table Class="tableBorder" CellPadding="3" Cellspacing="1">
  <tr>
    <th class="tableHeaderText" align="left" height="25">
      &nbsp;Delete Post/Thread
    </th>
  </tr>
  <tr>
    <td class="forumRow">
      <table cellSpacing="1" cellPadding="3">
        <tr>
          <td vAlign="top" nowrap align="left"><span class="normalTextSmall">Please provide a reason for why this post is being deleted. Note, this is a final action - the post/thread cannot be recovered.</span></td>
        </tr>
        <tr>
          <td align="left" colspan="2">
            <table>
              <tr> 
                <td align="right">
                  <span class="normalTextSmallBold">Has replies: </span>
                </td>
                <td align="left">
                  <asp:Label CssClass="normalTextSmallBold" runat="server" id="HasReplies" />
                </td>
              </tr>
            </table>
          </td>
        </tr>
        <tr>
          <td vAlign="top" colspan="2" nowrap align="left"><span class="normalTextSmallBold"><asp:CheckBox Checked="true" id="SendUserEmail" runat="server" text=" Send user email (thread owner only) why post was deleted"/></span></td>
        </tr>
        <tr>
          <td align="left">
            <table>
              <tr>
                <td vAlign="top" colspan="2" nowrap align="right"><span class="normalTextSmallBold">Reason: </span></td>
                <td vAlign="top" align="left"><asp:textbox id="DeleteReason" runat="server" columns="90" TextMode="MultiLine" rows="8"></asp:textbox></td>
                <td vAlign="top"><asp:requiredfieldvalidator id="ValidateReason" runat="server" CssClass="validationWarningSmall" ErrorMessage="RequiredFieldValidator" ControlToValidate="DeleteReason" EnableClientScript="False">You must supply a reason.</asp:requiredfieldvalidator></td>
              </tr>
            </table>
          </td>
        </tr>
        <tr>
          <td vAlign="top" colspan="2" nowrap align="right"><asp:HyperLink CssClass="linkSmallBold" id="CancelDelete" runat="server" Text="Cancel" /> &nbsp; <asp:LinkButton CssClass="linkSmallBold" id="DeletePost" runat="server" Text="Delete Post" /></td>
        </tr>
      </table>
    </td>
  </tr>
</table>
</center>
