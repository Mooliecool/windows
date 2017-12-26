<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="Client._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:FormView ID="FormView1" DefaultMode="Insert" runat="server" DataSourceID="ObjectDataSource1">
        <InsertItemTemplate>
           CustomerID: <asp:TextBox MaxLength="5" ID="TextBox1" runat="server" Text='<%#Bind("CustomerID") %>'></asp:TextBox>
           CompanyName: <asp:TextBox ID="TextBox2" runat="server" Text='<%#Bind("CompanyName") %>'></asp:TextBox>
                    <asp:Button ID="InsertButton"
                runat="server" Text="Insert" CommandName="Insert" />
        </InsertItemTemplate>
        </asp:FormView>
        <asp:GridView DataSourceID="ObjectDataSource1" 
            ID="GridView1" runat="server" BackColor="White" BorderColor="White" 
            BorderStyle="Ridge" BorderWidth="2px" CellPadding="3" CellSpacing="1" 
            GridLines="None">
            <RowStyle BackColor="#DEDFDE" ForeColor="Black" />
            <FooterStyle BackColor="#C6C3C6" ForeColor="Black" />
            <PagerStyle BackColor="#C6C3C6" ForeColor="Black" HorizontalAlign="Right" />
            <SelectedRowStyle BackColor="#9471DE" Font-Bold="True" ForeColor="White" />
            <HeaderStyle BackColor="#4A3C8C" Font-Bold="True" ForeColor="#E7E7FF" />
        </asp:GridView>
        <asp:ObjectDataSource ID="ObjectDataSource1" runat="server" 
            SelectMethod="Select" InsertMethod="Insert" TypeName="Client.BLL"></asp:ObjectDataSource>
    </div>
    </form>
</body>
</html>
