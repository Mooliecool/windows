<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSASPNETTheme._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:Image ID="Image1" SkinId="MainTheme" runat="server" ImageUrl="~/images/Blank.jpg" />
        <br/>
        <asp:Button ID="Button1" SkinId="BlueTheme" 
            runat="server" Text="Blue" onclick="Button1_Click" ForeColor="Blue" BackColor="Azure"/>
        <asp:Button ID="Button2" SkinId="PinkTheme"
            runat="server" Text="Pink" onclick="Button2_Click" ForeColor="Pink" BackColor="Red"/>
    </div>
    </form>
</body>
</html>
