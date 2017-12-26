<%@ Page Title="Home Page" Language="C#" MasterPageFile="~/Site.master" AutoEventWireup="true"
    CodeBehind="Default.aspx.cs" Inherits="WebRole1._Default" %>

<asp:Content ID="HeaderContent" runat="server" ContentPlaceHolderID="HeadContent">
</asp:Content>
<asp:Content ID="BodyContent" runat="server" ContentPlaceHolderID="MainContent">
    <br />
    <asp:HyperLink ID="HyperLink1" NavigateUrl="~/Vertical Partitioning.aspx" runat="server">SQL Azure Vertical Partitioning Demo</asp:HyperLink> 
    <br />
    <br />
    <asp:HyperLink ID="HyperLink2" NavigateUrl="~/Horizontal Partitioning.aspx" runat="server">SQL Azure Horizontal Partitioning Demo</asp:HyperLink>
</asp:Content>
