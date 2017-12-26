<%@ Page Language="c#" CodeBehind="CheckOut.aspx.cs" AutoEventWireup="false" Inherits="IBuySpy.CheckOut" %>
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
                                                <img align="left" height="32" width="60" src="images/1x1.gif"><asp:Label ID="Header" runat="server">Review and Submit Your Order</asp:Label>
                                                <br>
                                            </td>
                                        </tr>
                                    </table>
                                    <img align="left" height="1" width="92" src="images/1x1.gif">
                                    <table height="100%" cellspacing="0" cellpadding="0" width="550" border="0">
                                        <tr valign="top">
                                            <td width="100%" class="Normal">
                                                <br>
                                                <asp:Label ID="Message" runat="server">Please check all the information below to be sure it's correct.</asp:Label>
                                                <br>
                                                <br>
                                                <asp:DataGrid id="MyDataGrid" width="90%" BorderColor="black" GridLines="Vertical" cellpadding="4" cellspacing="0" Font-Name="Verdana" Font-Size="8pt" ShowFooter="true" HeaderStyle-CssClass="CartListHead" FooterStyle-CssClass="cartlistfooter" ItemStyle-CssClass="CartListItem" AlternatingItemStyle-CssClass="CartListItemAlt" AutoGenerateColumns="false" runat="server">
                                                    <Columns>
                                                        <asp:BoundColumn HeaderText="Product Name" DataField="ModelName" />
                                                        <asp:BoundColumn HeaderText="Model Number" DataField="ModelNumber" />
                                                        <asp:BoundColumn HeaderText="Quantity" DataField="Quantity" />
                                                        <asp:BoundColumn HeaderText="Price" DataField="UnitCost" DataFormatString="{0:c}" />
                                                        <asp:BoundColumn HeaderText="Subtotal" DataField="ExtendedAmount" DataFormatString="{0:c}" />
                                                    </Columns>
                                                </asp:DataGrid>
                                                <br>
                                                <br>
                                                <b>Total: </b>
                                                <asp:Label ID="TotalLbl" runat="server" />
                                                <p>
                                                    <asp:ImageButton id="SubmitBtn" ImageURL="images/submit.gif" runat="server" />
                                                </p>
                                            </td>
                                        </tr>
                                    </table>
                                </form>
                            </td>
                        </tr>
                    </table>
                </td>
            </tr>
        </table>
    </body>
</HTML>
