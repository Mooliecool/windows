<%@ Control Language="c#" CodeBehind="_PopularItems.ascx.cs" AutoEventWireup="false" Inherits="IBuySpy.C_PopularItems" %>
<%@ OutputCache Duration="3600" VaryByParam="None" %>

<%--

    This user control displays a list of the most popular items this week.

--%>

<table width="95%" cellpadding="0" cellspacing="0" border="0">
    <asp:Repeater ID="productList" runat="server">
        <HeaderTemplate>
            <tr>
                <td class="MostPopularHead">
                    &nbsp;Our most popular items this week
                </td>
            </tr>
        </HeaderTemplate>
        <ItemTemplate>
            <tr>
                <td bgcolor="#d3d3d3">
                    &nbsp;
                    <asp:HyperLink class="MostPopularItemText" NavigateUrl='<%# "ProductDetails.aspx?ProductID=" + DataBinder.Eval(Container.DataItem, "ProductID")%>' Text='<%#DataBinder.Eval(Container.DataItem, "ModelName")%>' runat="server" />
                    <br>
                </td>
            </tr>
        </ItemTemplate>
        <FooterTemplate>
            <tr>
                <td bgcolor="#d3d3d3">
                    &nbsp;
                </td>
            </tr>
        </FooterTemplate>
    </asp:Repeater>
</table>
