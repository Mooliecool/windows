<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Vertical Partitioning.aspx.cs" Inherits="WebRole1.Vertical_Partitioning" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
Students:
    <asp:GridView ID="GridView1" runat="server" AutoGenerateColumns="False" 
    DataKeyNames="StudentId" DataSourceID="SqlDataSource1">
        <Columns>
            <asp:BoundField DataField="StudentId" HeaderText="StudentId" 
                InsertVisible="False" ReadOnly="True" SortExpression="StudentId" />
            <asp:BoundField DataField="StudentName" HeaderText="StudentName" 
                SortExpression="StudentName" />
        </Columns>
    </asp:GridView>
<br />
Courses:
    <asp:GridView ID="GridView2" runat="server" AutoGenerateColumns="False" 
    DataSourceID="SqlDataSource2">
        <Columns>
            <asp:BoundField DataField="CourseName" HeaderText="CourseName" 
                SortExpression="CourseName" />
            <asp:BoundField DataField="StudentId" HeaderText="StudentId" 
                SortExpression="StudentId" />
        </Columns>
    </asp:GridView>
 <br />
 Vertical Partitioning: <br />
 This sample shows how to join two tables on different SQL Azure databases using LINQ. This technique vertically partitions your data in SQL Azure.
    <asp:GridView ID="GridView3" runat="server">
    </asp:GridView>
<asp:SqlDataSource ID="SqlDataSource2" runat="server" 
    ConnectionString="<%$ ConnectionStrings:CoursesConnectionString %>" 
    SelectCommand="SELECT [CourseName], [StudentId] FROM [Course]">
</asp:SqlDataSource>
<asp:SqlDataSource ID="SqlDataSource1" runat="server" 
    ConnectionString="<%$ ConnectionStrings:StudentsConnectionString %>" 
    SelectCommand="SELECT * FROM [student]"></asp:SqlDataSource>    
    </div>
    </form>
</body>
</html>
