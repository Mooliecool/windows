<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSASPNETCacheAPI._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<head id="Head1" runat="server">
    <title>Please choose which demo to view</title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
    </div>
    <asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="~/CacheObject.aspx">View Simple Cache Object</asp:HyperLink>
    <br />
    <br />
    <asp:HyperLink ID="HyperLink2" runat="server" 
        NavigateUrl="~/CacheWithFileBasedDependence.aspx">View Cache with File-based Dependency</asp:HyperLink>
    <br />
    <br />
    <asp:HyperLink ID="HyperLink3" runat="server" 
        NavigateUrl="~/CacheWithKeyBasedDependence.aspx">View Cache with Key-based Dependency</asp:HyperLink>
    <br />
    <br />
    <asp:HyperLink ID="HyperLink4" runat="server" 
        NavigateUrl="~/CacheWithAbsoluteTimeBasedDependence.aspx">View Cache with Absolute Time-based Dependency</asp:HyperLink>
    <br />
    <br />
        <asp:HyperLink ID="HyperLink6" runat="server" 
        NavigateUrl="~/CacheWithSlidingTimeBasedDependence.aspx">View Cache with Sliding Time-based Dependency</asp:HyperLink>
    <br />
    <br />
    <asp:HyperLink ID="HyperLink5" runat="server" 
        NavigateUrl="~/CacheWithCallBack.aspx">View Cache with CallBack</asp:HyperLink>
    </form>
</body>
</html>