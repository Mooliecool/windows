<%@ Page Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage" %>

<asp:Content ID="aboutTitle" ContentPlaceHolderID="TitleContent" runat="server">
    About Us
</asp:Content>

<asp:Content ID="aboutContent" ContentPlaceHolderID="MainContent" runat="server">
    <h2>Data</h2>
   <%if (ViewData.Model as MvcWebRole.Models.CustomersSet != null && ((MvcWebRole.Models.CustomersSet)ViewData.Model).ReadyToShowUI) 
    {%>
    
    <%= Html.ActionLink("Previous", "Previous", "Home")%>
    <%= Html.ActionLink("Next", "Next", "Home")%>
     <table>     
      <tr><th>Name</th><th>Age</th></tr>
        <%for (int i = 0; i < ((MvcWebRole.Models.CustomersSet)ViewData.Model).Customers.Count; i++)
         { %>
        <tr>
        <td>
        <%=((MvcWebRole.Models.CustomersSet)ViewData.Model).Customers[i].Name%>
        </td>
        <td>
        <%=((MvcWebRole.Models.CustomersSet)ViewData.Model).Customers[i].Age%>
        </td>
        </tr>
         <%}%>
         </table>
         <%}else {%>
    <p>
        <%= Html.ActionLink("Add data to test first", "AddDataToTest", "Home")%>
        <%}%>
    </p>
         
</asp:Content>
