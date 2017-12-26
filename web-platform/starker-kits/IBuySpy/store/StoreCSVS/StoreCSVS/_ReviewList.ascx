<%@ Control Language="c#" CodeBehind="_ReviewList.ascx.cs" AutoEventWireup="false" Inherits="IBuySpy.C_ReviewList" %>

<%--

    This user control display a list of review for a specific product.

--%>

<br>
<br>

<table cellspacing="0" cellpadding="0" width="100%" border="0">
    <tr>
        <td class="SubContentHead">
            &nbsp;Reviews
            <br>
        </td>
    </tr>
    <tr>
        <td>
            &nbsp;
        </td>
    </tr>
    <tr>
        <td>
            <asp:Hyperlink id="AddReview" runat="server">
                <img align="absbottom" src="images/review_this_product.gif" border="0">
            </asp:Hyperlink>
            <br>
            <br>
        </td>
    </tr>
    <tr>
        <td>
            <asp:DataList ID="MyList" runat="server" width="500" cellpadding="0" cellspacing="0">
                <ItemTemplate>
                    <asp:Label class="NormalBold" Text='<%# DataBinder.Eval(Container.DataItem, "CustomerName") %>' runat="server" />
                    <span class="Normal">says... </span><img src='images/ReviewRating<%# DataBinder.Eval(Container.DataItem, "Rating") %>.gif'>
                    <br>
                    <asp:Label class="Normal" Text='<%# DataBinder.Eval(Container.DataItem, "Comments") %>' runat="server" />
                </ItemTemplate>
                <SeparatorTemplate>
                    <br>
                </SeparatorTemplate>
            </asp:DataList>
        </td>
    </tr>
</table>
