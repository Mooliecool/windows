<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="CacheObject.aspx.cs" Inherits="CSASPNETCacheAPI.CacheObject" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Simple Cache Object</title>
</head>
<body>
    <form id="form1" runat="server">
    The current time is:
    <asp:Label ID="lblCurrentTime" runat="server"></asp:Label>
    <br />
    The cached time is
    <asp:Label ID="lblCacheTime" runat="server"></asp:Label>
    <br />
    <br />
    <br />
    <asp:Button ID="btnRefreshPage" runat="server" Text="Refresh Page" />
    <br />
    <br />
    <br />
    <br />
    <asp:Button ID="btnClearCache" runat="server" Text="Remove Cache" OnClick="btnClearCache_Click" />
    <br />
    <br />
    Note: Cache data is saved in memory. You can click Remove Cache button to remove
    it.<br />
    <br />
    <br />
    <asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="~/Default.aspx">Go Back To Default Page</asp:HyperLink>
    </form>
</body>
</html>
