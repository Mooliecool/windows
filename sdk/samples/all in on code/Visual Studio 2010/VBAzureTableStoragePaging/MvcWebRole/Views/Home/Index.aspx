<%@ Page Language="VB" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage" %>

<asp:Content ID="aboutTitle" ContentPlaceHolderID="TitleContent" runat="server">
    About Us
</asp:Content>

<asp:Content ID="aboutContent" ContentPlaceHolderID="MainContent" runat="server">
    <h2>Data</h2>
   <%  If Not ViewData.Model Is Nothing AndAlso ViewData.Model.ReadyToShowUI Then
    %>
    
    <%= Html.ActionLink("Previous", "Previous", "Home")%>
    <%= Html.ActionLink("Next", "Next", "Home")%>
     <table>     
      <tr><th>Name</th><th>Age</th></tr>
        <%  For i As Integer = 0 To (CType(ViewData.Model, MvcWebRole.Models.CustomersSet)).Customers.Count - 1
                 %>
        <tr>
        <td>
        <%= ViewData.Model.Customers(i).Name%>
        </td>
        <td>
        <%= ViewData.Model.Customers(i).Age%>
        </td>
        </tr>
         <% Next i%>
         </table>
         <%Else%>
    <p>
        <%= Html.ActionLink("Add data to test first", "AddDataToTest", "Home")%>
        <%End If%>
    </p>
         
</asp:Content>

