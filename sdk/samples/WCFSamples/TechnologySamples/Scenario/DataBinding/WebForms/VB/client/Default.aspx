<%@ Page Language="VB" Async="true" AutoEventWireup="true" CodeFile="Default.aspx.vb" Inherits="_Default" %>
<%@ Import Namespace="System.Data" %>
<%@ Import Namespace="System.ServiceModel" %>
<%@ Import Namespace="Microsoft.ServiceModel.Samples" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Untitled Page</title>
</head>
<body>
    <h3><font face="Verdana">Weather Service</font></h3>
    <form id="Form2" runat="server">
        <asp:DataGrid ID="dataGrid1" runat="server" 
            BorderColor="black" 
            BorderWidth="1" 
            GridLines="Both"
            CellPadding="3" 
            CellSpacing="0" 
            HeaderStyle-BackColor="#aaaadd" />
    </form>
</body>
</html>
