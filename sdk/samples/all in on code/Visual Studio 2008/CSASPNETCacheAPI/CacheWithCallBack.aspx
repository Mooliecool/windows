<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="CacheWithCallBack.aspx.cs"
    Inherits="CSASPNETCacheAPI.CacheWithCallBack" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Cache with Call Back</title>
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
    Note: Set Cache&#39;s absolute time to 10s and call RemovedCallback method when
    Cache is expiration.<br />
    <br />
    <br />
    <asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="~/Default.aspx">Go Back To Default Page</asp:HyperLink>
    </form>
</body>
</html>
