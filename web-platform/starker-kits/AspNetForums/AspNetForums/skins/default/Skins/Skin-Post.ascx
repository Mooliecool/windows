<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Control %>
<table cellSpacing="0" border="0">
  <tr>
    <td><ASPNETFORUMS:WHEREAMI id="Whereami1" runat="server"></ASPNETFORUMS:WHEREAMI></td>
  </tr>
</table>
<p>
  <table class="tableBorder" cellSpacing="1" cellPadding="3" width="100%">
    <tr>
      <th class="tableHeaderText" align="left" height="25">
        &nbsp;<asp:label id="PostTitle" runat="server"></asp:label>
      </th>
    </tr>
    <span id="ReplyTo" runat="server" visible="false">
      <tr>
        <td class="forumRow">
          <table cellSpacing="1" cellPadding="3">
            <tr>
              <td colSpan="2"><span class="normalTextSmall">The message you are replying to: </span></td>
            </tr>
            <tr>
              <td vAlign="top" noWrap align="right"><span class="normalTextSmallBold">Posted By: </span></td>
              <td vAlign="top" align="left"><asp:hyperlink id="ReplyPostedBy" Runat="server" CssClass="normalTextSmall"></asp:hyperlink><asp:label id="ReplyPostedByDate" Runat="server" CssClass="normalTextSmall"></asp:label></td>
            </tr>
            <tr>
              <td vAlign="top" align="right"><span class="normalTextSmallBold">Subject: </span></td>
              <td vAlign="top" align="left"><asp:hyperlink id="ReplySubject" runat="server" CssClass="normalTextSmall"></asp:hyperlink></td>
            </tr>
            <tr>
              <td vAlign="top" align="right"><span class="normalTextSmallBold">Message: </span></td>
              <td vAlign="top" align="left"><span class="normalTextSmall"><asp:label id="ReplyBody" runat="server"></asp:label>
                </span></td>
            </tr>
          </table>
        </td>
      </tr>
      <tr>
        <td class="forumAlternate">&nbsp;
        </td>
      </tr>
    </span><span id="Preview" runat="server" visible="false">
      <tr>
        <td class="forumRow">
          <table cellSpacing="1" cellPadding="3" width="75%" border="0">
            <tr>
              <td vAlign="top" align="left"><span class="normalTextSmallBold"><asp:label id="PreviewSubject" runat="server"></asp:label>
                </span><br>
                <span class="normalTextSmall">
                  <asp:label id="PreviewBody" runat="server"></asp:label>
                </span></td>
            </tr>
            <tr>
              <td>&nbsp;
              </td>
            </tr>
            <tr>
              <td vAlign="top" align="left"><asp:button id="BackButton" Runat="server" Text=" < Back to Edit Mode "></asp:button>&nbsp;
                <asp:button id="PreviewPostButton" Runat="server" Text=" Post "></asp:button></td>
            </tr>
            <tr>
              <td>&nbsp;
              </td>
            </tr>
          </table>
        </td>
      </tr>
    </span><span id="Post" runat="server" visible="true">
      <tr>
        <td class="forumRow">
          <table cellSpacing="1" cellPadding="3">
            <tr>
              <td vAlign="top" nowrap align="right"><span class="normalTextSmallBold">Author: </span></td>
              <td vAlign="top" align="left" colSpan="2"><span class="normalTextSmall"><asp:label id="PostAuthor" runat="server"></asp:label>
                </span></td>
            </tr>
            <span id="Edit" runat="server" visible="false">
              <tr>
                <td vAlign="top" nowrap align="right"><span class="normalTextSmallBold">Editing Post As: </span></td>
                <td vAlign="top" align="left" colSpan="2"><span class="normalTextSmall"><asp:label id="PostEditor" runat="server"></asp:label>
                  </span></td>
              </tr>
            </span>
            <tr>
              <td nowrap valign="center" align="right"><span class="normalTextSmallBold">Subject: </span></td>
              <td vAlign="top" align="left"><asp:textbox autocomplete="off" id="PostSubject" runat="server" columns="55"></asp:textbox></td>
              <td><asp:requiredfieldvalidator id="RequiredFieldValidator1" runat="server" CssClass="validationWarningSmall" ErrorMessage="RequiredFieldValidator" ControlToValidate="PostSubject">Subject required.</asp:requiredfieldvalidator></td>
            </tr>
            <tr>
              <td vAlign="top" nowrap align="right"><span class="normalTextSmallBold">Message: </span></td>
              <td vAlign="top" align="left"><asp:textbox id="PostBody" runat="server" columns="90" TextMode="MultiLine" rows="20"></asp:textbox></td>
              <td vAlign="top"><asp:requiredfieldvalidator id="RequiredFieldValidator2" runat="server" CssClass="validationWarningSmall" ErrorMessage="RequiredFieldValidator" ControlToValidate="PostBody" EnableClientScript="False">You must supply a message.</asp:requiredfieldvalidator></td>
            </tr>
            <span id="EditNotes" runat="server" visible="false">
              <tr>
                <td vAlign="top" nowrap align="right"><span class="normalTextSmallBold">Edit Notes: </span></td>
                <td vAlign="top" align="left" colSpan="2"><asp:textbox id="EditNotesBody" runat="server" columns="90" TextMode="MultiLine" rows="5"></asp:textbox></td>
              </tr>
            </span>
            <span id="AllowPinnedPosts" runat="server" visible="false">
              <tr>
                <td vAlign="center" align="right" width="91"><span class="normalTextSmallBold">Pinned Post: 
          </span></td>
                <td vAlign="top" align="left"><span class="normalTextSmall"><asp:dropdownlist id="PinnedPost" runat="server"></asp:dropdownlist>
                  </span></td>
              </tr>
            </span>
            <tr>
              <td vAlign="center" align="right" width="93"><span class="normalTextSmallBold">&nbsp;</span></td>
              <td vAlign="top" align="left"><span class="normalTextSmall"><asp:checkbox id="AllowReplies" runat="server" Text=" Do not allow replies to this post."></asp:checkbox>
                </span></td>
            </tr>
            <tr>
              <td vAlign="top" align="right" colSpan="2"><asp:button CausesValidation="false" id="Cancel" Runat="server" Text=" Cancel "></asp:button>&nbsp;
                <asp:button id="PreviewButton" Runat="server" Text=" Preview > "></asp:button></td>
            </tr>
            <tr>
              <td vAlign="top" align="right" colSpan="2">
                <asp:button id="PostButton" Runat="server" Text=" Post "></asp:button></td>
            </tr>
          </table>
        </td>
      </tr>
    </span>
  </table>
</p>
<p>
<table cellSpacing="0" border="0">
  <tr>
    <td><ASPNETFORUMS:WHEREAMI ShowHome="true" id="Whereami2" runat="server"></ASPNETFORUMS:WHEREAMI></td>
  </tr>
</table>

