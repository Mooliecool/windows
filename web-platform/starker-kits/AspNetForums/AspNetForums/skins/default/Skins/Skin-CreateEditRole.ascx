<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Control Language="C#" %>
<table cellspacing="1" cellpadding="0" width="100%" Class="tableBorder">
    <tr>
        <th height="25" class="tableHeaderText" align="left">
            &nbsp;
            <asp:Label ID="Title" Runat="server" />
        </th>
    </tr>
    <tr>
        <td class="forumRow" align="left">
            <table border="0" cellpadding="3" cellspacing="0">
                <!-- Role Name -->
                <tr>
                    <td class="forumRow" nowrap>
                        &nbsp; &nbsp;
                    </td>
                    <td align="right" nowrap>
                        <span class="normalTextSmallBold">Role Name:</span>
                    </td>
                    <td align="left">
                    	<asp:TextBox id="roleName" runat="server" Columns="45"></asp:TextBox>
                    	<asp:DropDownList id="roleNames" runat="server" />
                    </td>
                    <td align="left" width="100%">
                        <span class="normalTextSmall"></span>
                    </td>
                </tr>
                <!-- Role Description -->
                <tr>
                    <td class="forumRow" nowrap>
                        &nbsp; &nbsp;
                    </td>
                    <td align="right" valign="top">
                        <span class="normalTextSmallBold">Description:</span>
                    </td>
                    <td align="left">
                        <asp:textbox rows="10" columns="60" TextMode="MultiLine" id="roleDescription" runat="server" MaxLength="3500" />
                    </td>
                    <td align="left">
                    </td>
                </tr>
                <!-- Button -->
                <tr>
                    <td class="forumRow" nowrap>
                        &nbsp; &nbsp;
                    </td>
                    <td align="right" nowrap colspan="2">
                        <asp:Button type="submit" id="CreateRole" runat="server" />
                        <asp:Button type="submit" ID="DeleteRole" Runat="server" />
                    </td>
                    <td align="left" width="100%">
                        <span class="normalTextSmall"></span>
                    </td>
                </tr>
            </table>
        </td>
    </tr>
</table>
