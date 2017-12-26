<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Horizontal Partitioning.aspx.cs" Inherits="WebRole1.Horizontal_Partitioning" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
    Add an account name please:
        <asp:TextBox ID="tbAccountName" runat="server"></asp:TextBox>
        <asp:Button ID="btnAdd" runat="server" Text="Add" onclick="btnAdd_Click" />
        <br />
        <br />
    After adding some data, you may notice that the data was evenly divided across all the databases.
        <br />
        [NOTE: The number of databases is hard coded in this sample.]
    <br />
    <br />
    Accounts In Database001:
    <asp:GridView ID="GridView1" runat="server" AutoGenerateColumns="False" 
            DataKeyNames="Id" DataSourceID="SqlDataSource1">
        <Columns>
            <asp:BoundField DataField="Id" HeaderText="Id" ReadOnly="True" 
                SortExpression="Id" />
            <asp:BoundField DataField="Name" HeaderText="Name" SortExpression="Name" />
            <asp:BoundField DataField="Date" HeaderText="Date" SortExpression="Date" />
        </Columns>
    </asp:GridView>
        <asp:SqlDataSource ID="SqlDataSource1" runat="server" 
            ConnectionString="<%$ ConnectionStrings:Database001ConnectionString %>" 
            SelectCommand="SELECT * FROM [Accounts]"></asp:SqlDataSource>
    <br />
    Accounts In Database002:
     <asp:GridView ID="GridView2" runat="server" AutoGenerateColumns="False" 
            DataKeyNames="Id" DataSourceID="SqlDataSource2">
         <Columns>
             <asp:BoundField DataField="Id" HeaderText="Id" ReadOnly="True" 
                 SortExpression="Id" />
             <asp:BoundField DataField="Name" HeaderText="Name" SortExpression="Name" />
             <asp:BoundField DataField="Date" HeaderText="Date" SortExpression="Date" />
         </Columns>
    </asp:GridView>
        <asp:SqlDataSource ID="SqlDataSource2" runat="server" 
            ConnectionString="<%$ ConnectionStrings:Database002ConnectionString %>" 
            SelectCommand="SELECT * FROM [Accounts]"></asp:SqlDataSource>
    </div>
    </form>
</body>
</html>
