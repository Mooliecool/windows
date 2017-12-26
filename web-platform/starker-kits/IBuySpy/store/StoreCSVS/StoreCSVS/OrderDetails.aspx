<%@ Page Language="c#" CodeBehind="OrderDetails.aspx.cs" AutoEventWireup="false" Inherits="IBuySpy.OrderDetailsPage" %>
<%@ Register TagPrefix="IBuySpy" TagName="Header" Src="_Header.ascx" %>
<%@ Register TagPrefix="IBuySpy" TagName="Menu" Src="_Menu.ascx" %>

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
                                <img align="left" width="24" src="images/1x1.gif">
                                <table cellspacing="0" cellpadding="0" width="100%" border="0">
                                    <tr>
                                        <td class="ContentHead">
                                            <img align="left" height="32" width="60" src="images/1x1.gif">Order Details
                                            <br>
                                        </td>
                                    </tr>
                                </table>
                                <img align="left" height="15" width="86" src="images/1x1.gif" border="0">
                                <asp:Label id="MyError" CssClass="ErrorText" EnableViewState="false" runat="Server" />
                                <table id="detailsTable" height="100%" cellspacing="0" cellpadding="0" width="550" border="0" EnableViewState="false" runat="server">
                                    <tr valign="top">
                                        <td width="100%" class="Normal">
                                            <br>
                                            <b>Your Order Number Is: </b>
                                            <asp:Label ID="lblOrderNumber" EnableViewState="false" runat="server" />
                                            <br>
                                            <b>Order Date: </b>
                                            <asp:Label ID="lblOrderDate" EnableViewState="false" runat="server" />
                                            <br>
                                            <b>Ship Date: </b>
                                            <asp:Label ID="lblShipDate" EnableViewState="false" runat="server" />
                                            <br>
                                            <br>
                                            <asp:DataGrid id="GridControl1" width="90%" BorderColor="black" GridLines="Vertical" cellpadding="4" cellspacing="0" Font-Name="Verdana" Font-Size="8pt" ShowFooter="true" HeaderStyle-CssClass="CartListHead" FooterStyle-CssClass="cartlistfooter" ItemStyle-CssClass="CartListItem" AlternatingItemStyle-CssClass="CartListItemAlt" AutoGenerateColumns="false" runat="server">
                                                <Columns>
                                                    <asp:BoundColumn HeaderText="Product Name" DataField="ModelName" />
                                                    <asp:BoundColumn HeaderText="Model Number" DataField="ModelNumber" />
                                                    <asp:BoundColumn HeaderText="Quantity" DataField="Quantity" />
                                                    <asp:BoundColumn HeaderText="Price" DataField="UnitCost" DataFormatString="{0:c}" />
                                                    <asp:BoundColumn HeaderText="Subtotal" DataField="ExtendedAmount" DataFormatString="{0:c}" />
                                                </Columns>
                                            </asp:DataGrid>
                                            <br>
                                            <b>Total: </b>
                                            <asp:Label ID="lblTotal" EnableViewState="false" runat="server" />
                                        </td>
                                    </tr>
                                </table>
                            </td>
                        </tr>
                    </table>
                </td>
            </tr>
        </table>
    </body>
</HTML>
