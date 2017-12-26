<%@ Page Language="c#" CodeBehind="ShoppingCart.aspx.cs" AutoEventWireup="false" Inherits="IBuySpy.ShoppingCart" %>
<%@ Register TagPrefix="IBuySpy" TagName="Menu" Src="_Menu.ascx" %>
<%@ Register TagPrefix="IBuySpy" TagName="Header" Src="_Header.ascx" %>

<HTML>
  <HEAD>
        <link rel="stylesheet" type="text/css" href="IBuySpy.css">
  </HEAD>
    <body background="images/sitebkgrd.gif" leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginheight="0" marginwidth="0">
        <table cellspacing="0" cellpadding="0" width="100%" border="0">
            <tr>
                <td colspan="2">
                    <IBuySpy:Header ID="Header1" runat="server" />
                </td>
            </tr>
            <tr>
                <td valign="top">
                    <IBuySpy:Menu id="Menu1" runat="server" />
                    <img height="1" src="images/1x1.gif" width="145">
                </td>
                <td align="left" valign="top" width="100%" nowrap>
                    <table height="100%" align="left" cellspacing="0" cellpadding="0" width="100%" border="0">
                        <tr valign="top">
                            <td nowrap>
                                <br>
                                <form runat="server">
                                    <img align="left" width="24" src="images/1x1.gif">
                                    <table cellspacing="0" cellpadding="0" width="100%" border="0">
                                        <tr>
                                            <td class="ContentHead">
                                                <img align="left" height="32" width="60" src="images/1x1.gif">Shopping Cart
                                                <br>
                                            </td>
                                        </tr>
                                    </table>
                                    <img align="left" height="4" width="110" src="images/1x1.gif"> <font color="red">
                                        <asp:Label id="MyError" class="ErrorText" EnableViewState="false" runat="Server" />
                                    </font>
                                    <br>
                                    <img align="left" height="15" width="24" src="images/1x1.gif" border="0">
                                    <asp:panel id="DetailsPanel" runat="server">
                                    <img height=1 src="images/1x1.gif" width=50 align=left>
                                        <table height="100%" cellSpacing=0 cellPadding=0 width=550 border=0>
                                            <tr vAlign=top>
                                                <td width=550>
                                                    <asp:DataGrid id=MyList runat="server" BorderColor="black" GridLines="Vertical" cellpadding="4" cellspacing="0" Font-Name="Verdana" Font-Size="8pt" ShowFooter="true" HeaderStyle-CssClass="CartListHead" FooterStyle-CssClass="CartListFooter" ItemStyle-CssClass="CartListItem" AlternatingItemStyle-CssClass="CartListItemAlt" DataKeyField="Quantity" AutoGenerateColumns="false">
                                                        <Columns>
                                                            <asp:TemplateColumn HeaderText="Product&nbsp;ID">
                                                                <ItemTemplate>
                                                                    <asp:Label id="ProductID" runat="server" Text='<%# DataBinder.Eval(Container.DataItem, "ProductID") %>' />
                                                                </ItemTemplate>
                                                            </asp:TemplateColumn>
                                                            <asp:BoundColumn HeaderText="Product Name" DataField="ModelName" />
                                                            <asp:BoundColumn HeaderText="Model" DataField="ModelNumber" />
                                                            <asp:TemplateColumn HeaderText="Quantity">
                                                                <ItemTemplate>
                                                                    <asp:TextBox id="Quantity" runat="server" Columns="4" MaxLength="3" Text='<%# DataBinder.Eval(Container.DataItem, "Quantity") %>' width="40px" />
                                                                </ItemTemplate>
                                                            </asp:TemplateColumn>
                                                            <asp:BoundColumn HeaderText="Price" DataField="UnitCost" DataFormatString="{0:c}" />
                                                            <asp:BoundColumn HeaderText="Subtotal" DataField="ExtendedAmount" DataFormatString="{0:c}" />
                                                            <asp:TemplateColumn HeaderText="Remove">
                                                                <ItemTemplate>
                                                                    <center>
                                                                        <asp:CheckBox id="Remove" runat="server" />
                                                                    </center>
                                                                </ItemTemplate>
                                                            </asp:TemplateColumn>
                                                        </Columns>
                                                    </asp:DataGrid>
                                                    <img height=1 src="Images/1x1.gif" width=350>
                                                    <span class=NormalBold>Total: </span>
                                                    <asp:Label class=NormalBold id=lblTotal runat="server" EnableViewState="false"></asp:Label>
                                                    <br><br>
                                                    <img height=1 src="Images/1x1.gif" width=60>
                                                    <asp:imagebutton id=UpdateBtn runat="server" ImageURL="images/update_cart.gif"></asp:imagebutton>
                                                    <img height=1 src="Images/1x1.gif" width=15>
                                                    <asp:imagebutton id=CheckoutBtn runat="server" ImageURL="images/final_checkout.gif"></asp:imagebutton>
                                                    <br>
                                                </td>
                                            </tr>
                                        </table>
                                    </asp:panel>
                                </form>
                            </td>
                        </tr>
                    </table>
                </td>
            </tr>
        </table>
    </body>
</HTML>
