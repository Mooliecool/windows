<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="OutputCacheWithDuration.aspx.cs"
    Inherits="CSASPNETOutputCache.OutputCacheWithDuration" %>

<%@ OutputCache Duration="10" VaryByParam="none" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>OutPutCacheWithDuration</title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:Label ID="lblResult" runat="server"></asp:Label>
        <br />
        <br />
        <asp:Button ID="btnPostBack" runat="server" Text="Post Back"  />
        <p>
            The page will be cached 10s, and then you can click Button to update datetime.
        </p>
    </div>
    <asp:HyperLink ID="HyperLink5" runat="server" NavigateUrl="~/Default.aspx">Back To Default Page</asp:HyperLink>
    </form>
</body>
</html>
