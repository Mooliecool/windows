<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="AddArticle.aspx.cs" Inherits="CSASPNETRssFeeds.AddArticle" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:SqlDataSource ID="ArticleDataSource" runat="server" 
            ConnectionString="<%$ ConnectionStrings:ConnStr4Articles %>" 
            DeleteCommand="DELETE FROM [Articles] WHERE [ArticleID] = @ArticleID" 
            InsertCommand="INSERT INTO [Articles] ([Title], [Author], [Link], [Description], [PubDate]) VALUES (@Title, @Author, @Link, @Description, @PubDate)" 
            SelectCommand="SELECT * FROM [Articles]" 
            UpdateCommand="UPDATE [Articles] SET [Title] = @Title, [Author] = @Author, [Link] = @Link, [Description] = @Description, [PubDate] = @PubDate WHERE [ArticleID] = @ArticleID">
            <DeleteParameters>
                <asp:Parameter Name="ArticleID" Type="Int32" />
            </DeleteParameters>
            <UpdateParameters>
                <asp:Parameter Name="Title" Type="String" />
                <asp:Parameter Name="Author" Type="String" />
                <asp:Parameter Name="Link" Type="String" />
                <asp:Parameter Name="Description" Type="String" />
                <asp:Parameter Name="PubDate" Type="DateTime" />
                <asp:Parameter Name="ArticleID" Type="Int32" />
            </UpdateParameters>
            <InsertParameters>
                <asp:Parameter Name="Title" Type="String" />
                <asp:Parameter Name="Author" Type="String" />
                <asp:Parameter Name="Link" Type="String" />
                <asp:Parameter Name="Description" Type="String" />
                <asp:Parameter Name="PubDate" Type="DateTime" />
            </InsertParameters>
        </asp:SqlDataSource>
        <asp:FormView ID="ArticleFormView" runat="server" AllowPaging="True" 
            DataKeyNames="ArticleID" DataSourceID="ArticleDataSource" 
            onprerender="ArticleFormView_PreRender">
            <EditItemTemplate>
                ArticleID:
                <asp:Label ID="ArticleIDLabel1" runat="server" 
                    Text='<%# Eval("ArticleID") %>' />
                <br />
                Title:
                <asp:TextBox ID="TitleTextBox" runat="server" Text='<%# Bind("Title") %>' />
                <br />
                Author:
                <asp:TextBox ID="AuthorTextBox" runat="server" Text='<%# Bind("Author") %>' />
                <br />
                Link:
                <asp:TextBox ID="LinkTextBox" runat="server" Text='<%# Bind("Link") %>' />
                <br />
                Description:
                <asp:TextBox ID="DescriptionTextBox" runat="server" 
                    Text='<%# Bind("Description") %>' Height="120px" TextMode="MultiLine" 
                    Width="200px" />
                <br />
                PubDate:
                <asp:TextBox ID="PubDateTextBox" runat="server" Text='<%# Bind("PubDate") %>' />
                <br />
                <asp:LinkButton ID="UpdateButton" runat="server" CausesValidation="True" 
                    CommandName="Update" Text="Update" />
                &nbsp;<asp:LinkButton ID="UpdateCancelButton" runat="server" 
                    CausesValidation="False" CommandName="Cancel" Text="Cancel" />
            </EditItemTemplate>
            <InsertItemTemplate>
                Title:
                <asp:TextBox ID="TitleTextBox" runat="server" Text='<%# Bind("Title") %>' />
                <br />
                Author:
                <asp:TextBox ID="AuthorTextBox" runat="server" Text='<%# Bind("Author") %>' />
                <br />
                Link:
                <asp:TextBox ID="LinkTextBox" runat="server" Text='<%# Bind("Link") %>' />
                <br />
                Description:
                <asp:TextBox ID="DescriptionTextBox" runat="server" 
                    Text='<%# Bind("Description") %>' Height="120px" TextMode="MultiLine" 
                    Width="200px" />
                <br />
                PubDate:
                <asp:TextBox ID="PubDateTextBox" runat="server" Text='<%# Bind("PubDate") %>' />
                <br />
                <asp:LinkButton ID="InsertButton" runat="server" CausesValidation="True" 
                    CommandName="Insert" Text="Insert" />
                &nbsp;<asp:LinkButton ID="InsertCancelButton" runat="server" 
                    CausesValidation="False" CommandName="Cancel" Text="Cancel" />
            </InsertItemTemplate>
            <ItemTemplate>
                Title:
                <asp:Label ID="TitleLabel" runat="server" Text='<%# Bind("Title") %>' />
                <br />
                Author:
                <asp:Label ID="AuthorLabel" runat="server" Text='<%# Bind("Author") %>' />
                <br />
                Link:
                <asp:Label ID="LinkLabel" runat="server" Text='<%# Bind("Link") %>' />
                <br />
                Description:
                <asp:Label ID="DescriptionLabel" runat="server" 
                    Text='<%# Bind("Description") %>' />
                <br />
                PubDate:
                <asp:Label ID="PubDateLabel" runat="server" Text='<%# Bind("PubDate") %>' />
                <br />
                <asp:LinkButton ID="EditButton" runat="server" CausesValidation="False" 
                    CommandName="Edit" Text="Edit" />
                &nbsp;<asp:LinkButton ID="DeleteButton" runat="server" CausesValidation="False" 
                    CommandName="Delete" Text="Delete" />
                &nbsp;<asp:LinkButton ID="NewButton" runat="server" CausesValidation="False" 
                    CommandName="New" Text="New" />
            </ItemTemplate>
        </asp:FormView>
        <br />
        <br />
    </div>
    </form>
</body>
</html>
