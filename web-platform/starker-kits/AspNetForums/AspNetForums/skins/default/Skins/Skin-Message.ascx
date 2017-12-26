<%@ Control Language="C#" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<table width="100%">
  <tr>
    <td align="center">

      <table cellspacing="1" cellpadding="0" width="50%" Class="tableBorder">
        <tr>
          <th align="left">
            &nbsp;<asp:label CssClass="tableHeaderText" id="MessageTitle" runat="server"/>
          </th>
        </tr>
        <tr>
          <td Class="forumRow">
            <table cellpadding="3" cellspacing="0">
              <tr>
                <td>
                  &nbsp;
                </td>
                <td> 
                 <asp:label CssClass="normalTextSmall" id="MessageBody" runat="server"/>
                </td>
              </tr>
            </table>
          </td>
        </tr>
      </table>
    </td>
  </tr>
  <tr>
    <td align="center">
      <br>
<%--
      <AspNetForums:JumpDropDownList runat="server" ID="Jumpdropdownlist1" />
--%>
    </td>
  </tr>
</table>
