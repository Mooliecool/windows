<%@ Control Language="c#" CodeBehind="_AlsoBought.ascx.cs" AutoEventWireup="false" Inherits="IBuySpy.C_AlsoBought" %>

<%--
        This user controls lists other products that
        customers who purchased a product "also bought".
--%>

<table width="95%" cellpadding="0" cellspacing="0" border="0">
    <tr>
        <td>
            <asp:Repeater ID="alsoBoughtList" runat="server">
                <HeaderTemplate>
                    <tr>
                        <td class="MostPopularHead">
                            &nbsp;Customers who bought this also bought
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
        </td>
    </tr>
</table>
