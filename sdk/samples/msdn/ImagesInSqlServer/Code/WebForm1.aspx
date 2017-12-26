<%@ Page language="c#" Codebehind="WebForm1.aspx.cs" AutoEventWireup="false" Inherits="UploadImages1.WebForm1" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<title>WebForm1</title>
		<meta name="GENERATOR" Content="Microsoft Visual Studio 7.0">
		<meta name="CODE_LANGUAGE" Content="C#">
		<meta name="vs_defaultClientScript" content="JavaScript">
		<meta name="vs_targetSchema" content="http://schemas.microsoft.com/intellisense/ie5">
	</HEAD>
	<body>
		<form id="Form1" method="post" runat="server" enctype="multipart/form-data">
			<P>&nbsp;
				<TABLE id="Table1" style="HEIGHT: 75px" cellSpacing="1" cellPadding="1" width="100%" border="1" DESIGNTIMEDRAGDROP="32">
					<TR>
						<TD style="WIDTH: 69px"></TD>
						<TD>
							<H1 align="center"><FONT color="#3366ff">Working with Images and SQL Server</FONT></H1>
						</TD>
					</TR>
					<TR>
						<TD style="WIDTH: 69px"></TD>
						<TD>&nbsp;
							<TABLE id="Table2" style="HEIGHT: 75px" cellSpacing="1" cellPadding="1" width="100%" border="1" DESIGNTIMEDRAGDROP="32">
								<TR>
									<TD style="WIDTH: 118px">Image to Upload:</TD>
									<TD>
										<input type="file" id="ImageToUpload" name="ImageToUpload" runat="server" style="WIDTH: 624px; HEIGHT: 22px" size="84"></TD>
								</TR>
								<TR>
									<TD style="WIDTH: 118px">Description:
									</TD>
									<TD>
										<P>
											<asp:TextBox id="imageDescription" runat="server" Width="542px"></asp:TextBox></P>
									</TD>
								</TR>
							</TABLE>
							<P>
								<asp:Button id="UploadImage" runat="server" Text="Upload Image"></asp:Button></P>
						</TD>
					</TR>
					<TR>
						<TD style="WIDTH: 69px"></TD>
						<TD><A href="ViewImages.aspx">View Image List</A></TD>
					</TR>
				</TABLE>
			</P>
		</form>
	</body>
</HTML>
