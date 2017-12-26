<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="CacheWithKeyBasedDependence.aspx.cs"
    Inherits="CSASPNETCacheAPI.CacheWithKeyBasedDependence" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Key-Based Cache</title>
</head>
<body>
    <form id="form1" runat="server">
    The current time is
    <asp:Label ID="lblCurrentTime" runat="server"></asp:Label>
    <br />
    <br />
    The cached time is
    <asp:Label ID="lblCacheTime" runat="server"></asp:Label>
    <br />
    <br />
    <asp:Button ID="btnRefreshPage" runat="server" Text="Refresh Page" />
    <br />
    <br />
    <br />
    <br />
    Note: Use another Cache object as dependence.The cached time will be changed after 10s.
    <br />
    <br />
    <asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="~/Default.aspx">Go Back To Default Page</asp:HyperLink>
    </form>
</body>
</html>
