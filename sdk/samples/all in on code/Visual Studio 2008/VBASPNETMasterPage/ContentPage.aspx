<%@ Page Title="" Language="vb" AutoEventWireup="false" MasterPageFile="~/MasterPage.master" CodeBehind="ContentPage.aspx.vb" Inherits="VBASPNETMasterPage.ContentPage" %>
<asp:Content ID="Content1" ContentPlaceHolderID="MainContentHolder" runat="server">
    <div style="background-color: #D4D0C8; width: 500px;">
        <p>
            <h3>Content Page</h3>
            Enter Your Name Please:<br />
            <asp:TextBox ID="txtName" runat="server"></asp:TextBox>
            <asp:Button ID="Button1" runat="server" Text="Button" />
        </p>
    </div>
</asp:Content>
