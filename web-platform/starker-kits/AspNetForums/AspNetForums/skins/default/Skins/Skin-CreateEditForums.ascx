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
                <!-- Forum Name -->
                <tr>
                    <td class="forumRow" nowrap>
                        &nbsp; &nbsp;
                    </td>
                    <td align="right" nowrap>
                        <span class="normalTextSmallBold">Forum Name:</span>
                    </td>
                    <td align="left"><asp:TextBox id="ForumName" runat="server" Columns="45"></asp:TextBox>
                    </td>
                    <td align="left" width="100%">
                        <span class="normalTextSmall"></span>
                    </td>
                </tr>
                <!-- Forum Group -->
                <tr>
                    <td class="forumRow" nowrap>
                        &nbsp; &nbsp;
                    </td>
                    <td align="right" nowrap>
                        <span class="normalTextSmallBold">Forum Group:</span>
                    </td>
                    <td align="left">
                        <asp:DropDownList id="ForumGroups" runat="server"></asp:DropDownList>
                    </td>
                    <td align="left" width="100%">
                        <span class="normalTextSmall"></span>
                    </td>
                </tr>
                <!-- Forum Description -->
                <tr>
                    <td class="forumRow" nowrap>
                        &nbsp; &nbsp;
                    </td>
                    <td align="right" valign="top">
                        <span class="normalTextSmallBold">Description:</span>
                    </td>
                    <td align="left">
                        <asp:textbox rows="10" columns="60" TextMode="MultiLine" id="Description" runat="server" MaxLength="3500" />
                    </td>
                    <td align="left">
                    </td>
                </tr>
                <!-- Moderated -->
                <tr>
                    <td class="forumRow" nowrap>
                        &nbsp; &nbsp;
                    </td>
                    <td align="right" nowrap>
                        <span class="normalTextSmallBold"> Moderated:</span>
                    </td>
                    <td align="left">
                        <asp:checkbox id="Moderated" runat="server" Checked="True" />
                    </td>
                    <td align="left" width="100%">
                    </td>
                </tr>
                <!-- Active -->
                <tr>
                    <td class="forumRow" nowrap>
                        &nbsp; &nbsp;
                    </td>
                    <td align="right" nowrap>
                        <span class="normalTextSmallBold"> Active:</span>
                    </td>
                    <td align="left">
                        <asp:checkbox id="Active" runat="server" Checked="True" />
                    </td>
                    <td align="left" width="100%">
                    </td>
                </tr>
                <!-- Button -->
                <tr>
                    <td class="forumRow" nowrap>
                        &nbsp; &nbsp;
                    </td>
                    <td align="right" nowrap colspan="2">
                        <asp:button type="submit" id="CreateUpdate" runat="server" />
                    </td>
                    <td align="left" width="100%">
                        <span class="normalTextSmall"></span>
                    </td>
                </tr>
            </table>
        </td>
    </tr>
    <span id="RoleBasedSecurity" runat="server" Visible="false">
        <tr>
            <td class="forumHeaderBackgroundAlternate" align="left" height="20"><span class="forumTitle">&nbsp;Role Based Security</span></td>
        </tr>
        <tr>
            <td class="forumRow">
                <table cellSpacing="0" cellPadding="3" border="0" width="100%">
                    <tr>
                        <td><span class="normalTextSmaller">&nbsp;</span></td>
                    </tr>
                    <tr>
                        <td valign="top" width="50%">
                            <span class="normalTextSmallBold">&nbsp;&nbsp; Available to users in the following roles:</span><br><br>
                            <asp:DataGrid ID="ActiveRoles" Runat="server" Width="100%" AutoGenerateColumns="False" CssClass="tableBorder">
                                <Columns>
                                    <asp:TemplateColumn HeaderText=""
                                        HeaderStyle-Height="20"
                                        HeaderStyle-CssClass="forumHeaderBackgroundAlternate"
                                        ItemStyle-CssClass="forumRow">
                                        <ItemTemplate>
                                            <asp:LinkButton CssClass="linkSmallBold"
                                                CommandName="RemoveRole"
                                                CommandArgument='<%# Container.DataItem %>'
                                                Text="Remove"
                                                runat="server"
                                                ID="RemoveRoleButton"/>
                                        </ItemTemplate>
                                    </asp:TemplateColumn>
                                    <asp:TemplateColumn
                                        HeaderStyle-Height="20"
                                        HeaderStyle-CssClass="forumHeaderBackgroundAlternate"
                                        ItemStyle-CssClass="forumRow">
                                        <HeaderTemplate>
                                            <span class="forumTitle">Role</span>
                                        </HeaderTemplate>
                                        <ItemTemplate>
                                            <span class="normalTextSmallBold"><%# Container.DataItem %></span>
                                        </ItemTemplate>
                                    </asp:TemplateColumn>
                                </Columns>
                            </asp:DataGrid>
                            <asp:Label ID="NoActiveRoles" Runat="server" CssClass="normalTextSmallBold" Text="Public Forum" Visible="false" />
                        </td>
                        <td valign="top" width="50%">
                            <span class="normalTextSmallBold">&nbsp;&nbsp; Available Roles:</span><br><br>
                            <asp:DataGrid ID="AllRoles" Runat="server" Width="100%" AutoGenerateColumns="False" CssClass="tableBorder">
                                <Columns>
                                    <asp:TemplateColumn HeaderText="" 
                                        HeaderStyle-Height="20"
                                        HeaderStyle-CssClass="forumHeaderBackgroundAlternate"
                                        ItemStyle-CssClass="forumRow">
                                        <ItemTemplate>
                                            <asp:LinkButton CssClass="linkSmallBold"
                                                CommandName="AddRole"
                                                CommandArgument='<%# Container.DataItem %>'
                                                Text="Add"
                                                runat="server"
                                                ID="AddRoleButton"/>
                                        </ItemTemplate>
                                    </asp:TemplateColumn>
                                    <asp:TemplateColumn
                                        HeaderStyle-Height="20"
                                        HeaderStyle-CssClass="forumHeaderBackgroundAlternate"
                                        ItemStyle-CssClass="forumRow">
                                        <HeaderTemplate>
                                            <span class="forumTitle">Role</span>
                                        </HeaderTemplate>
                                        <ItemTemplate>
                                            <span class="normalTextSmallBold"><%# Container.DataItem %></span>
                                        </ItemTemplate>
                                    </asp:TemplateColumn>
                                </Columns>
                            </asp:DataGrid>
                            <asp:Label ID="NoAllRoles" Runat="server" CssClass="normalTextSmallBold" Text="No Defined Roles" Visible="false" />
                        </td>
                    </tr>
                </table>
            </td>
        </tr>
    </span>
</table>
