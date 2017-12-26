<%@ Page language="c#" Codebehind="SendingMail.aspx.cs" AutoEventWireup="false" Inherits="SendMail.SendingMail" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<title>SendingMail</title>
		<meta content="Microsoft Visual Studio 7.0" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
	</HEAD>
	<body>
		<form id="SendingMail" method="post" runat="server">
			<TABLE id="Table1" style="Z-INDEX: 101; LEFT: 8px; POSITION: absolute; TOP: 8px; HEIGHT: 75px" cellSpacing="1" cellPadding="1" width="100%" border="1" DESIGNTIMEDRAGDROP="32">
				<TR>
					<TD style="WIDTH: 69px"></TD>
					<TD>
						<H1 align="center"><FONT color="#0066ff">Sending Mail using SMTP Mail</FONT></H1>
					</TD>
				</TR>
				<TR>
					<TD style="WIDTH: 69px"></TD>
					<TD>
						<TABLE id="Table2" style="HEIGHT: 75px" cellSpacing="1" cellPadding="1" width="100%" border="1" DESIGNTIMEDRAGDROP="32">
							<TR>
								<TD style="WIDTH: 69px">From</TD>
								<TD>
									<asp:TextBox id="fromAddress" runat="server" Width="409px">me@myco.com</asp:TextBox></TD>
							</TR>
							<TR>
								<TD style="WIDTH: 69px">To</TD>
								<TD>
									<asp:TextBox id="sendToAddress" runat="server" Width="409px">you@myco.com</asp:TextBox></TD>
							</TR>
							<TR>
								<TD style="WIDTH: 69px">Subject</TD>
								<TD>
									<asp:TextBox id="messageSubject" runat="server" Width="852px">Test Message</asp:TextBox></TD>
							</TR>
							<TR>
								<TD style="WIDTH: 69px">Message Body</TD>
								<TD>
									<asp:TextBox id="messageBody" runat="server" Width="851px" TextMode="MultiLine" Height="211px">This is a dummy message</asp:TextBox></TD>
							</TR>
							<TR>
								<TD style="WIDTH: 69px">File to attach</TD>
								<TD>
									<asp:TextBox id="fileNameToAttach" runat="server" Width="409px"></asp:TextBox></TD>
							</TR>
						</TABLE>
						<asp:Button id="SendMessage" runat="server" Text="Send Message"></asp:Button>
						<asp:Button id="SendMessageNoMessageObj" runat="server" Text="Send Message 2"></asp:Button>
						<asp:Button id="SendMessageWithFile" runat="server" Text="Send Message w/ File"></asp:Button></TD>
				</TR>
				<TR>
					<TD style="WIDTH: 69px"></TD>
					<TD>
						<asp:Label id="statusLabel" runat="server"></asp:Label></TD>
				</TR>
				<TR>
					<TD style="WIDTH: 69px"></TD>
					<TD>
						<P align="right">Server:
							<asp:TextBox id="SMTPServerName" runat="server" Width="291px">localhost</asp:TextBox></P>
					</TD>
				</TR>
			</TABLE>
		</form>
	</body>
</HTML>
