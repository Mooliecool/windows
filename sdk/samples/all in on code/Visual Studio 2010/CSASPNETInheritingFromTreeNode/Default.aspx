<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSASPNETInheritingFromTreeNode.Default" %>

<%@ Register Assembly="CSASPNETInheritingFromTreeNode" Namespace="CSASPNETInheritingFromTreeNode"
    TagPrefix="cc1" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Demo page of Inheriting TreeNode</title>
</head>
<body>
    <form id="form1" runat="server">
    <p>
        Each tree node has been assgined a custom object. Please select a node to show the
        value.</p>
    <!-- The TreeView control -->
    <cc1:CustomTreeView ID="CustomTreeView1" runat="server" OnSelectedNodeChanged="CustomTreeView1_SelectedNodeChanged">
        <SelectedNodeStyle Font-Bold="True" />
    </cc1:CustomTreeView>
    <br />
    <asp:Label ID="lbMessage" runat="server" Text=""></asp:Label>
    </form>
</body>
</html>
