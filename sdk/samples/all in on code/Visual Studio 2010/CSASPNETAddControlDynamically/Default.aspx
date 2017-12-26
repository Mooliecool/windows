<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSASPNETAddControlDynamically.Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:Button ID="btnAddAddress" runat="server" Text="Add a New Address" OnClick="btnAddAddress_Click" />
        <asp:Button ID="btnSave" runat="server" Text="Save These Addresses" OnClick="btnSave_Click" />
        <br />
        <br />
        <asp:Panel ID="pnlAddressContainer" runat="server">
        </asp:Panel>
        <br />
    </div>
    </form>
</body>
</html>
