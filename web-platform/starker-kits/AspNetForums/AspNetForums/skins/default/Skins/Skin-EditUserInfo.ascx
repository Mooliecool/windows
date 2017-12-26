<%@ Control Language="C#" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Register TagPrefix="AspNetModeration" Namespace="AspNetForums.Controls.Moderation" Assembly="AspNetForums" %>

<table class="tableBorder" cellSpacing="1" cellPadding="0" width="100%">
    <tr>
        <th class="tableHeaderText" align="left" height="20">
            &nbsp; Edit User Information for:
            <asp:label id="Username" runat="server"></asp:label></th></tr>
    <tr>
        <td class="forumHeaderBackgroundAlternate" align="left" height="20"><span class="forumTitle">&nbsp;Required Information </span></td>
    </tr>
    <tr>
        <td class="forumRow">
            <table cellSpacing="0" cellPadding="2" border="0">
                <tr>
                    <td colSpan="4">&nbsp;</td>
                </tr>
                <!-- Email -->
                <tr>
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <td noWrap align="right"><span class="normalTextSmallBold">Email: </span></td>
                    <td align="left"><asp:textbox id="Email" runat="server" columns="35"></asp:textbox></td>
                    <td align="left"><asp:regularexpressionvalidator id="RegularExpressionValidator3" runat="server" ErrorMessage="You must supply a valid email address." ControlToValidate="Email" CssClass="validationWarningSmall" ValidationExpression="\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*"></asp:regularexpressionvalidator></td>
                </tr>
                <tr>
                    <td colSpan="2"></td>
                    <td vAlign="top" height="20"><span class="normalTextSmaller">&nbsp;Your email 
            address is not publicly available.</span></td>
                    <td vAlign="top"><asp:requiredfieldvalidator id="ValidateEmail" runat="server" ErrorMessage="You must supply an email address." ControlToValidate="Email" CssClass="validationWarningSmall"></asp:requiredfieldvalidator></td>
                </tr>
                <!-- Timezone -->
                <tr>
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <td noWrap align="right"><span class="normalTextSmallBold">Timezone: </span></td>
                    <td align="left"><asp:dropdownlist id="Timezone" runat="server">
                            <asp:listitem Text="Hawaii (GMT -10)" Value="-10" />
                            <asp:listitem Text="Alaska (GMT -9)" Value="-9" />
                            <asp:listitem Text="Pacific Time (GMT -8)" Value="-8" />
                            <asp:listitem Text="Mountain Time (GMT -7)" Value="-7" />
                            <asp:listitem Text="Central Time (GMT -6)" Value="-6" />
                            <asp:listitem Text="Eastern Time (GMT -5)" Value="-5" />
                            <asp:listitem Text="Atlantic Time (GMT -4)" Value="-4" />
                            <asp:listitem Text="Brasilia Time (GMT -3)" Value="-3" />
                            <asp:listitem Text="Greenwich Mean Time (GMT +0)" Value="0" />
                            <asp:listitem Text="Central Europe Time (GMT +1)" Value="1" />
                            <asp:listitem Text="Eastern Europe Time (GMT +2)" Value="2" />
                            <asp:listitem Text="Middle Eastern Time (GMT +3)" Value="3" />
                            <asp:listitem Text="Abu Dhabi Time (GMT +4)" Value="4" />
                            <asp:listitem Text="Indian Time (GMT +5)" Value="5" />
                            <asp:listitem Text="Eastern China Time (GMT +8)" Value="8" />
                            <asp:listitem Text="Japan Time (GMT +9)" Value="9" />
                            <asp:listitem Text="Australian Time (GMT +10)" Value="10" />
                            <asp:listitem Text="Pacific Rim Time (GMT +11)" Value="11" />
                            <asp:listitem Text="New Zealand Time (GMT +12)" Value="12" />
                        </asp:dropdownlist></td>
                </tr>
                <tr>
                    <td colSpan="2"></td>
                    <td vAlign="top" height="20"><span class="normalTextSmaller">&nbsp;Date/Times will 
            be displayed for your timezone.</span></td>
                </tr>
                <!-- Date Format -->
                <tr>
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <td noWrap align="right"><span class="normalTextSmallBold">Date Format: </span></td>
                    <td align="left"><asp:dropdownlist id="DateFormat" runat="server">
                            <asp:listitem Text="Day Month Year (1 May 2002)" Value="dd MMM yyyy" />
                            <asp:listitem Text="Month-Day-Year (5-1-2002)" Value="MM-dd-yyyy" />
                            <asp:listitem Text="Day-Month-Year (1-5-2002)" Value="dd-MM-yyyy" />
                            <asp:listitem Text="Month/Day/Year (5/1/2002)" Value="MM/dd/yyyy" />
                            <asp:listitem Text="Day/Month/Year" Value="dd/MM/yyyy" />
                        </asp:dropdownlist></td>
                <tr>
                    <td colSpan="2"></td>
                    <td vAlign="top" height="20"><span class="normalTextSmaller">&nbsp;Date/Times will 
            be displayed in this format.</span></td>
                </tr>
                <!-- Change Password-->
                <tr>
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <td noWrap align="right"></td>
                    <td align="left"><a Class="linkSmallBold" href="<% =Globals.UrlChangePassword %>">Change 
                            Password</a></td>
                    <td align="left"></td>
                </tr>
            </table>
        </td>
    </tr>
    <tr>
        <td class="forumHeaderBackgroundAlternate" align="left" height="20"><span class="forumTitle">&nbsp;About you (Optional) </span></td>
    </tr>
    <tr>
        <td class="forumRow">
            <table cellSpacing="0" cellPadding="3" width="100%" border="0">
                <tr>
                    <td><span class="normalTextSmaller">&nbsp;</span></td>
                </tr>
                <tr>
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <!-- Occupation -->
                    <td align="right"><span class="normalTextSmallBold">Occupation: </span></td>
                    <td align="left"><asp:textbox id="Occupation" runat="server" columns="35"></asp:textbox></td>
                    <td align="left"></td>
                    <!-- Location -->
                    <td noWrap align="right"><span class="normalTextSmallBold">Location: </span></td>
                    <td noWrap align="left"><asp:textbox id="Location" runat="server" columns="35"></asp:textbox></td>
                </tr>
                <tr>
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <!-- Interests -->
                    <td align="right"><span class="normalTextSmallBold">Interests: </span></td>
                    <td align="left"><asp:textbox id="Interests" runat="server" columns="35"></asp:textbox></td>
                    <td align="left"></td>
                    <!-- MSN IM -->
                    <td noWrap align="right"><span class="normalTextSmallBold">MSN IM: </span></td>
                    <td noWrap align="left"><asp:textbox id="MsnIm" runat="server" columns="35"></asp:textbox></td>
                </tr>
                <tr>
                    <!-- AIM -->
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <td align="right"><span class="normalTextSmallBold">AIM: </span></td>
                    <td align="left"><asp:textbox id="AolIm" runat="server" columns="35"></asp:textbox></td>
                    <td align="left"></td>
                    <!-- Yahhoo Im -->
                    <td noWrap align="right"><span class="normalTextSmallBold">Yahoo IM: </span></td>
                    <td noWrap align="left"><asp:textbox id="YahooIm" runat="server" columns="35"></asp:textbox></td>
                </tr>
                <tr>
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <!-- ICQ -->
                    <td align="right"><span class="normalTextSmallBold">ICQ: </span></td>
                    <td align="left"><asp:textbox id="Icq" runat="server" columns="35"></asp:textbox></td>
                    <td align="left"></td>
                    <!-- Web site -->
                    <td noWrap align="right"><span class="normalTextSmallBold">Website: </span></td>
                    <td noWrap align="left"><asp:textbox id="Website" runat="server" columns="35"></asp:textbox><asp:regularexpressionvalidator id="RegularExpressionValidator1" runat="server" ErrorMessage="Must be valid URL." ControlToValidate="Website" CssClass="validationWarningSmall" ValidationExpression="http://([\w-]+\.)+[\w-]+(/[\w- ./?%&amp;=]*)?"></asp:regularexpressionvalidator></td>
                </tr>
                <tr>
                    <td colSpan="4">&nbsp;</td>
                </tr>
            </table>
        </td>
    </tr>
    <TR>
        <td class="forumHeaderBackgroundAlternate" align="left" height="20"><span class="forumTitle">&nbsp;Display Options </span></td>
    </TR>
    <tr>
        <td class="forumRow">
            <table cellSpacing="0" cellPadding="3" border="0">
                <tr>
                    <td><span class="normalTextSmaller">&nbsp;</span></td>
                </tr>
                <!-- Fake Email -->
                <tr>
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <td noWrap align="right"><span class="normalTextSmallBold">Public Email: 
          </span></td>
                    <td align="left"><asp:textbox id="FakeEmail" runat="server" columns="35"></asp:textbox></td>
                    <td><asp:regularexpressionvalidator id="RegularExpressionValidator2" runat="server" ErrorMessage="You must supply a valid email address." ControlToValidate="FakeEmail" CssClass="validationWarningSmall" ValidationExpression="\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*"></asp:regularexpressionvalidator></td>
                </tr>
                <tr>
                    <td colSpan="2"></td>
                    <td vAlign="top" height="20"><span class="normalTextSmaller">&nbsp;Email address 
            published with your profile.</span></td>
                </tr>
                <!-- Signature -->
                <tr>
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <td vAlign="top" align="right"><span class="normalTextSmallBold">Signature: </span></td>
                    <td align="left" colSpan="2"><asp:textbox id="Signature" runat="server" columns="50" TextMode="MultiLine" rows="5"></asp:textbox></td>
                    <td align="left"></td>
                </tr>
                <tr>
                    <td colSpan="2"></td>
                    <td vAlign="top" height="20"><span class="normalTextSmaller">&nbsp;Message appended 
            to your posts.</span></td>
                </tr>
                <!-- Style -->
                <tr>
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <td vAlign="top" align="right"><span class="normalTextSmallBold">Skin: 
</span></td>
                    <td vAlign="top" align="left"><asp:dropdownlist id="SiteStyle" runat="server"></asp:dropdownlist></td>
                </tr>
                <!-- Post view order -->
                <tr>
                    <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
                    </td>
                    <td vAlign="top" align="right"><span class="normalTextSmallBold">View Posts: 
          </span></td>
                    <td vAlign="top" align="left">
                        <asp:dropdownlist id="PostViewOrder" runat="server"></asp:dropdownlist></td>
                </tr>
                <%-- Temporarily disabled
        <!-- Icon -->
        <tr>
          <td class="forumRow" noWrap width="19">&nbsp; &nbsp;
          </td>
          <td vAlign="top" align="right"><span class="normalTextSmallBold">Icon: </span></td>
          <td vAlign="top" align="left"><input id="Icon" type="file" name="Icon" runat="server"><BR>
            <span class="normalTextSmaller">&nbsp;Upload an image to&nbsp;show next to your posts (must be 80x80 
            pixels).</span>
          </td>
          <td vAlign="top"><span class="validationTextSmall"><asp:label id="validatePostedFile" runat="server"></asp:label>
            </span>
            <P></P>
          </td>
        </tr>
        <!-- Has Icon --><span id="HasIcon" runat="server" Visible="false">
          <tr>
            <td vAlign="top" align="right" colSpan="2"><span class="normalTextSmallBold">Current Icon: 
          </span></td>
            <td>
              <table cellSpacing="0" cellPadding="0" bgColor="white">
                <tr>
                  <td width="80" height="80"><asp:image id="CurrentIcon" Runat="server" Height="80" width="80"></asp:image></td>
                </tr>
              </table>
            </td>
          </tr>
          <!-- Email Tracking -->
          <tr>
            <td colSpan="2">
            <td vAlign="top" align="left"><asp:checkbox id="ShowIcon" runat="server" CssClass="normalTextSmallBold" Text="&nbsp;Display my icon"></asp:checkbox></td>
          </tr>
        </span>
--%>
                <!-- Email Tracking -->
                <tr>
                    <td colSpan="2">
                    <td vAlign="top" align="left"><asp:checkbox id="EmailTracking" runat="server" CssClass="normalTextSmallBold" Text="&nbsp;Enable email tracking"></asp:checkbox></td>
                </tr>
                <!-- Hide read threads -->
                <tr>
                    <td colSpan="2">
                    <td vAlign="top" align="left"><asp:checkbox id="UnreadThreadsOnly" runat="server" CssClass="normalTextSmallBold" Text="&nbsp;Hide read posts"></asp:checkbox></td>
                </tr>
                <tr>
                    <td colSpan="4">&nbsp;</td>
                </tr>
            </table>
        </td>
    </tr>
    <!-- Role Based Security Options -->
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
                            <span class="normalTextSmallBold">&nbsp;&nbsp; Active Roles:</span><br><br>
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
                                                Text="Remove User From Role"
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
                        </td>
                        <td valign="top" width="50%">
                            <span class="normalTextSmallBold">&nbsp;&nbsp; All Roles:</span><br><br>
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
                                                Text="Add User"
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
                        </td>
                    </tr>
                </table>
            </td>
        </tr>
    </span>
    <!-- Administration options -->
    <span id="Administration" runat="server" Visible="false">
        <TR>
            <td class="forumHeaderBackgroundAlternate" align="left" height="20"><span class="forumTitle">&nbsp;Administrator Options </span></td>
        </TR>
        <tr>
            <td class="forumRow">
                <table cellSpacing="0" cellPadding="3" border="0">
                    <tr>
                        <td><span class="normalTextSmaller">&nbsp;</span></td>
                    </tr>
                    <!-- User profile approved -->
                    <tr>
                        <td colSpan="2">
                        <td vAlign="top" align="left"><asp:checkbox id="ProfileApproved" runat="server" CssClass="normalTextSmallBold" Text="&nbsp;User's profile is approved (profile details are public)"></asp:checkbox></td>
                    </tr>
                    <!-- User banned-->
                    <tr>
                        <td colSpan="2">
                        <td vAlign="top" align="left"><asp:checkbox id="Banned" runat="server" CssClass="normalTextSmallBold" Text="&nbsp;User is banned (cannot login)"></asp:checkbox></td>
                    </tr>
                    <!-- User trusted -->
                    <tr>
                        <td colSpan="2">
                        <td vAlign="top" align="left"><asp:checkbox id="Trusted" runat="server" CssClass="normalTextSmallBold" Text="&nbsp;User is trusted (does not require moderation)"></asp:checkbox></td>
                    </tr>
                    <!-- Email user's password -->
                    <tr>
                        <td colSpan="2">
                        <td vAlign="top" align="left"><asp:button Runat="server" ID="EmailUserPassword" Runat="server" Text="Email the user his/her password" /></td>
                    </tr>
                    <tr>
                        <td colSpan="4">&nbsp;</td>
                    </tr>
                </table>
            </td>
        </tr>
    </span>

    <span id="Moderation" runat="server" Visible="false">
		<AspNetModeration:ModeratedForums 
			runat="server" 
			ID="ModeratedForums" 
			Mode="ViewForUser" 
		/>
    </span>

</table>
<p>
    <table width="100%" border="0">
        <tr>
            <td valign="top" align="center">
                <span runat="server" id="PasswordRequired">
                    <span class="normalTextSmallBold">Password (required for update): </span>
                    <asp:textbox id="Password" runat="server" columns="35" TextMode="Password"></asp:textbox>
                </span>
                <asp:button id="Submit" runat="server" Text="Update User Information" type="submit"></asp:button>
            </td>
        </tr>
        <tr>
            <td align="middle" colSpan="2"><asp:requiredfieldvalidator id="ValidatePassword" runat="server" ErrorMessage="You must enter a password to make changes." Display="Dynamic" controltovalidate="Password" CssClass="validationWarningSmall"></asp:requiredfieldvalidator></td>
        </tr>
    </table>
</p>
