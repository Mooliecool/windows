<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="Default.aspx.vb" Inherits="VBASPNETControlPermissionForFiles._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>   
         <asp:ListView ID="ListView1" runat="server">
         <LayoutTemplate>
         <table>
         <tr>
         <td>ID</td>
         <td>Name</td>
         <td>Path</td>
         </tr>
         <tr id="itemPlaceholder" runat="server"></tr>
         </table>
         </LayoutTemplate>
         <ItemTemplate>
         <tr>
         <td>
         <%#Eval("ID") %>
         </td>
         <td>
         <%#Eval("Name") %>
         </td>
         <td>
         <a href='<%#Eval("Path") %>' ><%#Eval("Path") %></a>
         </td>
         </tr>
         </ItemTemplate>
         </asp:ListView>
      
     </div>   
    </form>
</body>
</html>
