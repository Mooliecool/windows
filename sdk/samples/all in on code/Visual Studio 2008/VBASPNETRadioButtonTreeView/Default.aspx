<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="Default.aspx.vb" Inherits="VBASPNETRadioButtonTreeView._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:TreeView ID="RadioButtonTreeView" runat="server" ShowLines="True">
            <Nodes>
                <asp:TreeNode SelectAction="Expand" Text="Operating System" Value="OS">
                    <asp:TreeNode Text="Windows XP SP1" Value="Windows XP SP1" ImageUrl="unchecked.gif" />
                    <asp:TreeNode Text="Windows XP SP2" Value="Windows XP SP2" ImageUrl="unchecked.gif" />
                    <asp:TreeNode Text="Windows 2003" Value="Windows 2003" ImageUrl="unchecked.gif" />
                    <asp:TreeNode Text="Windows Vista" Value="Windows Vista" ImageUrl="unchecked.gif" />
                </asp:TreeNode>
                <asp:TreeNode SelectAction="Expand" Text="Application" Value="App">
                    <asp:TreeNode Text="Office XP" Value="Office XP" ImageUrl="unchecked.gif" />
                    <asp:TreeNode Text="Office 2003" Value="Office 2003" ImageUrl="unchecked.gif" />
                    <asp:TreeNode Text="Office 2007" Value="Office 2007" ImageUrl="unchecked.gif" />
                </asp:TreeNode>
            </Nodes>
        </asp:TreeView>
        <p />
        <asp:Label ID="lbSelectInfo" runat="server"></asp:Label>
        <p />
        <asp:Button ID="btnSubmit" runat="server" Text="Click here to Check the items you choose" />
    </div>
    </form>
</body>
</html>
