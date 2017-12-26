<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSASPNETOutputCache._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Default Page</title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
    </div>
    <asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="~/OutputCacheWithDuration.aspx">OutPutCacheWithDuration</asp:HyperLink>
    <br />
    <br />
    <asp:HyperLink ID="HyperLink2" runat="server" NavigateUrl="~/OutputCacheWithControl.aspx">OutPutCacheWithVaryByControl</asp:HyperLink>
    <br />
    <br />
    <asp:HyperLink ID="HyperLink3" runat="server" NavigateUrl="~/OutputCacheWithCustom.aspx">OutPutCacheWithVaryByCustom</asp:HyperLink>
    <br />
    <br />
    <asp:HyperLink ID="HyperLink4" runat="server" NavigateUrl="~/OutputCacheWithParam.aspx?id=1">OutPutCacheWithVaryByParam(The QueryString value is 1.)</asp:HyperLink>
    <br />
    <asp:HyperLink ID="HyperLink5" runat="server" NavigateUrl="~/OutputCacheWithParam.aspx?id=2">OutPutCacheWithVaryByParam(The QueryString value is 2.)</asp:HyperLink>
    </form>
</body>
</html>
