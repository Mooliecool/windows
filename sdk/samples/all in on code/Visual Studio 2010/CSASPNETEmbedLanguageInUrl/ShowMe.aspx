<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ShowMe.aspx.cs" Inherits="CSASPNETEmbedLanguageInUrl.Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <asp:Literal ID="litTitle" runat="server" Text='<%$ Resources:Resource,Title %>'></asp:Literal>
    <asp:Label ID="lbTitleContent" runat="server"></asp:Label>
    <br />
    <br />
    <asp:Literal ID="litText" runat="server" Text='<%$ Resources:Resource,Text %>'></asp:Literal>
    <asp:TextBox ID="lbTextContent" runat="server" Height="121px" TextMode="MultiLine"
        Width="338px"></asp:TextBox>
    <br />
    <br />
    <asp:Literal ID="litTime" runat="server" Text='<%$ Resources:Resource,Time %>'></asp:Literal>
    <asp:Label ID="lbTimeContent" runat="server"></asp:Label>
    <br />
    <br />
    <asp:Literal ID="litComment" runat="server" Text='<%$ Resources:Resource,End %>'></asp:Literal>
    <asp:Label ID="lbCommentContent" runat="server"></asp:Label>
    </form>
</body>
</html>
