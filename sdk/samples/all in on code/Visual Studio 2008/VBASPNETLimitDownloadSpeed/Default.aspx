<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="Default.aspx.vb" Inherits="VBASPNETLimitDownloadSpeed._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:DropDownList ID="ddlDownloadSpeed" runat="server">
            <asp:ListItem Value="20">20 Kb/s</asp:ListItem>
            <asp:ListItem Value="50">50 Kb/s</asp:ListItem>
            <asp:ListItem Value="80">80 Kb/s</asp:ListItem>
        </asp:DropDownList>
        <asp:Button ID="btnDownload" runat="server" Text="Download" />
    </div>
    </form>
</body>
</html>
