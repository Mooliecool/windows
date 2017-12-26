<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSASPNETUserControlEventExpose._Default" %>
<%@ Register src="MyUserControl.ascx" tagname="MyUserControl" tagprefix="uc1" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title>CSASPNETUserControlEventExpose</title>
</head>
<body>
    <form id="form1" runat="server">
   <div>
        <asp:Label ID="lblText" Text="I am On Main Page." runat="server"></asp:Label>
        <asp:DropDownList ID="ddlTemp" runat="server">
            <asp:ListItem>America</asp:ListItem>
            <asp:ListItem>China</asp:ListItem>
            <asp:ListItem>Germany </asp:ListItem>
            <asp:ListItem>Japan </asp:ListItem>    
        </asp:DropDownList>
        <br />
        <br />
  
       <asp:PlaceHolder ID="PlaceHolder1" runat="server"></asp:PlaceHolder>
    </div>
    </form>
</body>
</html>
