<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="ConfigurationEncryption.aspx.vb"
    Inherits="VBASPNETEncryptAndDecryptConfiguration.ConfigurationEncryption_aspx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:Label ID="lbSection" runat="server" Text="Choose a section:"></asp:Label>
        <asp:DropDownList ID="ddlSection" runat="server">
            <asp:ListItem>connectionStrings</asp:ListItem>
            <asp:ListItem>appSettings</asp:ListItem>
            <asp:ListItem>system.web/machineKey</asp:ListItem>
            <asp:ListItem>system.web/sessionState</asp:ListItem>
        </asp:DropDownList>
        <br />
        <br />
        <asp:Button ID="btnEncrypt" runat="server" Text="Encrypt it" OnClick="btnEncrypt_Click"
            Height="30px" Width="120px" />
        <asp:Button ID="btnDecrypt" runat="server" Text="Decrypt it" OnClick="btnDecrypt_Click"
            Height="30px" Width="120px" />
    </div>
    </form>
</body>
</html>
