<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="OutputCacheWithControl.aspx.cs"
    Inherits="CSASPNETOutputCache.OutputCacheWithControl" %>

<%@ OutputCache Duration="1000" VaryByControl="ddlOption" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>OutPutCacheWithVaryByControl</title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:Label ID="lblResult" runat="server"></asp:Label>
        <br />
        <br />
        <asp:DropDownList ID="ddlOption" runat="server" AutoPostBack="True" OnSelectedIndexChanged="ddlOption_SelectedIndexChanged">
            <asp:ListItem Selected="True">Option One</asp:ListItem>
            <asp:ListItem>Option Two</asp:ListItem>
            <asp:ListItem>Option Three</asp:ListItem>
        </asp:DropDownList>
        <p>
            The page will be rendered from cache basing on the selected item of DropDownList. 
            The different item has corresponding cache. 
        </p>
    </div>
    <asp:HyperLink ID="HyperLink5" runat="server" NavigateUrl="~/Default.aspx">Back To Default Page</asp:HyperLink>
    </form>
</body>
</html>
