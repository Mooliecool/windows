<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="OutputCacheWithParam.aspx.cs"
    Inherits="CSASPNETOutputCache.OutputCacheWithParam" %>

<%@ OutputCache Duration="1000" VaryByParam="id" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>OutPutCacheWithVaryByParam</title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:Label ID="lblResult" runat="server"></asp:Label>
        <p>
            The page will be rendered from cache until the value of QueryString named "id" is
            changed or Duration is expiration.
        </p>
    </div>
    <asp:HyperLink ID="HyperLink5" runat="server" NavigateUrl="~/Default.aspx">Back To Default Page</asp:HyperLink>
    </form>
</body>
</html>
