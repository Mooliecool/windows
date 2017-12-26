<%@ Control Language="C#" %>
<%@ Import Namespace="AspNetForums.Components" %>
<%@ Import Namespace="AspNetForums.Controls" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>

<table class="tableBorder" cellSpacing="1" cellPadding="0" width="100%">
	<asp:Panel id="ModerationHeader" runat="server" Visible="true">
		<tr>
			<td class="forumHeaderBackgroundAlternate" align="left" height="20">
				<span class="forumTitle">&nbsp;User Moderation Options: </span>
			</td>
		</tr>
    </asp:Panel>
		<tr>
			<td class="forumRow">
				<table cellSpacing="0" cellPadding="2" border="0" width="100%">
	<asp:Panel id="ModeratedForumsPanel" runat="server" Visible="true">
					<tr>
						<td colSpan="4">&nbsp;</td>
					</tr>
					<tr>
						<td class="forumRow" noWrap width="19">&nbsp; &nbsp;</td>
						<td noWrap align="right" valign="top" width="100"><span class="normalTextSmallBold">Moderated: </span></td>
						<td align="left" colSpan="2">
							<asp:DataGrid ID="UserModeratedForums" Runat="server" Width="100%" AutoGenerateColumns="False" CssClass="tableBorder">
								<Columns>
									<asp:TemplateColumn HeaderText="" HeaderStyle-Height="20" HeaderStyle-CssClass="forumHeaderBackgroundAlternate" ItemStyle-CssClass="forumRow">
										<ItemTemplate>
											<asp:LinkButton CssClass="linkSmallBold"
												CommandName="Remove"
												CommandArgument='<%# DataBinder.Eval(Container.DataItem, "ForumID") %>'
												Text="Remove"
												runat="server"
												ID="RemoveForumName"/>
										</ItemTemplate>
									</asp:TemplateColumn>
									<asp:TemplateColumn HeaderStyle-Height="20" HeaderStyle-CssClass="forumHeaderBackgroundAlternate" ItemStyle-CssClass="forumRow">
										<HeaderTemplate>
											<span class="forumTitle">Forum Name</span>
										</HeaderTemplate>
										<ItemTemplate>
											<span class="normalTextSmallBold">
												<%# DataBinder.Eval(Container.DataItem, "Name") %>
											</span>
										</ItemTemplate>
									</asp:TemplateColumn>
									<asp:TemplateColumn HeaderStyle-Height="20" HeaderStyle-CssClass="forumHeaderBackgroundAlternate" ItemStyle-CssClass="forumRow">
										<HeaderTemplate>
											<span class="forumTitle">Email Notification</span>
										</HeaderTemplate>
										<ItemTemplate>
											<span class="normalTextSmallBold">
												<%# DataBinder.Eval(Container.DataItem, "EmailNotification") %>
											</span>
										</ItemTemplate>
									</asp:TemplateColumn>
								</Columns>
							</asp:DataGrid>

							<asp:DataGrid ID="ForumModeratedForums" Runat="server" Width="100%" AutoGenerateColumns="False" CssClass="tableBorder">
								<Columns>
									<asp:TemplateColumn HeaderStyle-Height="20" HeaderStyle-CssClass="forumHeaderBackgroundAlternate" ItemStyle-CssClass="forumRow">
										<HeaderTemplate>
											<span class="forumTitle">User Name</span>
										</HeaderTemplate>
										<ItemTemplate>
											<span class="normalTextSmallBold">
												<%# DataBinder.Eval(Container.DataItem, "Username") %>
											</span>
										</ItemTemplate>
									</asp:TemplateColumn>
								</Columns>
							</asp:DataGrid>
						</td>
					</tr>
					<tr>
						<td colSpan="4">&nbsp;</td>
					</tr>
	</asp:Panel>
	<asp:Panel id="UnmoderatedForumsPanel" runat="server" Visible="true">
					<tr>
						<td colSpan="4">&nbsp;</td>
					</tr>
					<tr>
						<td class="forumRow" noWrap width="19">&nbsp; &nbsp;</td>
						<td noWrap align="right" valign="top"><span class="normalTextSmallBold">Unmoderated: </span></td>
						<td align="left" valign="top" colSpan="2">
							<asp:DropDownList
								id="UnmoderatedForums"
								runat="server"
								DataValueField="ForumID"
								DataTextField="Name"
							/>&nbsp;&nbsp;&nbsp;&nbsp;
							<asp:CheckBox
								id="EmailNotification"
								runat="server"
								Checked="true"
							/> Email Notification &nbsp;&nbsp;
							<asp:Button
								id="AddForum"
								runat="server"
								Text="Add Forum"
							/>
						</td>
					</tr>
					<tr>
						<td colSpan="4">&nbsp;</td>
					</tr>
	</asp:Panel>
				</table>
			</td>
		</tr>
</table>