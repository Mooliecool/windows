<%@ Control Language="c#" CodeBehind="_Menu.ascx.cs" AutoEventWireup="false" Inherits="IBuySpy.C_Menu" %>
<%@ OutputCache Duration="3600" VaryByParam="selection" %>

<%--

    This user control creates a menu of all product categories
    in the database.  This forms the left-hand navigation of the
    product catalog pages.

--%>

<table cellspacing="0" cellpadding="0" width="145" border="0">
    <tr valign="top">
        <td colspan="2">
            <a href="default.aspx"><img src="images/logo.gif" border="0"></a>
        </td>
    </tr>
    <tr valign="top">
        <td colspan="2">
            <asp:DataList id="MyList" runat="server" cellpadding="3" cellspacing="0" width="145" SelectedItemStyle-BackColor="dimgray" EnableViewState="false">
                <ItemTemplate>
                    <asp:HyperLink cssclass="MenuUnselected" id="HyperLink1" Text='<%# DataBinder.Eval(Container.DataItem, "CategoryName") %>' NavigateUrl='<%# "productslist.aspx?CategoryID=" + DataBinder.Eval(Container.DataItem, "CategoryID") + "&selection=" + Container.ItemIndex %>' runat="server" />
                </ItemTemplate>
                <SelectedItemTemplate>
                    <asp:HyperLink cssclass="MenuSelected" id="HyperLink2" Text='<%# DataBinder.Eval(Container.DataItem, "CategoryName") %>' NavigateUrl='<%# "productslist.aspx?CategoryID=" + DataBinder.Eval(Container.DataItem, "CategoryID") + "&selection=" + Container.ItemIndex %>' runat="server" />
                </SelectedItemTemplate>
            </asp:DataList>
        </td>
    </tr>
    <tr>
        <td width="10">
            &nbsp;
        </td>
        <td>
            <br><br><br><br><br><br>
            <a href="docs/docs.htm" target="_blank" class="SiteLink">IBuySpy Store<br>Documentation</a>
        </td>
    </tr>
</table>
